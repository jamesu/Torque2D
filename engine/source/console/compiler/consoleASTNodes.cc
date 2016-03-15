//-----------------------------------------------------------------------------
// Copyright (c) 2012 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "console/console.h"
#include "platform/event.h"

#include "console/compiler/consoleAST.h"
#include "collection/findIterator.h"
#include "io/resource/resourceManager.h"

#include "string/findMatch.h"
#include "console/consoleInternal.h"
#include "io/fileStream.h"
#include "console/compiler/consoleCompiler.h"

#include "sim/simBase.h"

template< typename T >
struct Token
{
    T value;
    S32 lineNumber;
};
#include "console/compiler/cmdgram.h"

ReferencedVariableNode *gCurrentLocalVariables = NULL;
ReferencedVariableNode *gLocalVariableStack[16];
U32 gLocalVariableStackIdx = 0;
U32 gAnonFunctionID = 0;
DocBlockNode *gCurrentDocBlock = NULL;

namespace Compiler
{
    void compileBlock(StmtNode *block, CodeStream &codeStream)
    {
#ifdef DEBUG_COMPILER
        U32 startTarget = codeStream.mTargetList.size();
#endif
        for(StmtNode *walk = block; walk; walk = walk->getNext())
        {
           walk->compileStmt(codeStream);
#ifdef DEBUG_COMPILER
           AssertFatal(codeStream.mTargetList.size() == startTarget, "Target stack change not allowed between statements!");
           
           if (codeStream.mTargetList.size() != startTarget)
           {
              walk->compileStmt(codeStream);
           }
#endif
        }
    }
}

using namespace Compiler;

//-----------------------------------------------------------------------------

void StmtNode::addBreakLine(CodeStream &code)
{
    code.addBreakLine(dbgLineNumber, code.tell());
}

//------------------------------------------------------------

extern const char *CMDgetFileLine(int &lineNumber);
extern const char *CMDGetCurrentFile();
extern S32 CMDGetCurrentLine();

StmtNode::StmtNode()
{
   next = NULL;
   dbgFileName = CMDGetCurrentFile();
   dbgLineNumber = CMDGetCurrentLine();
}

void StmtNode::setPackage(StringTableEntry)
{
}

void StmtNode::append(StmtNode *next)
{
    StmtNode *walk = this;
    while(walk->next)
        walk = walk->next;
    walk->next = next;
}


void FunctionDeclStmtNode::setPackage(StringTableEntry packageName)
{
    package = packageName;
}

//------------------------------------------------------------
//
// Console language compilers
//
//------------------------------------------------------------

void BreakStmtNode::compileStmt(CodeStream &codeStream)
{
    if(codeStream.inLoop())
    {
        addBreakLine(codeStream);
        codeStream.emitFix(CodeStream::FIXTYPE_BREAK);
    }
    else
    {
        Con::warnf(ConsoleLogEntry::General, "%s (%d): break outside of loop... ignoring.", dbgFileName, dbgLineNumber);
    }
}

//------------------------------------------------------------

void ContinueStmtNode::compileStmt(CodeStream &codeStream)
{
    if(codeStream.inLoop())
    {
        addBreakLine(codeStream);
        codeStream.emitFix(CodeStream::FIXTYPE_CONTINUE);
    }
    else
    {
        Con::warnf(ConsoleLogEntry::General, "%s (%d): continue outside of loop... ignoring.", dbgFileName, dbgLineNumber);
    }
}

//------------------------------------------------------------

void ExprNode::compileStmt(CodeStream &codeStream)
{
    addBreakLine(codeStream);
    return compile(codeStream, TypeReqNone);
}

//------------------------------------------------------------

void ReturnStmtNode::compileStmt(CodeStream &codeStream)
{
    addBreakLine(codeStream);
    if(!expr)
       codeStream.emitOpcodeABC(Compiler::OP_RETURN, 0, 0, 0);
    else
    {
       expr->compile(codeStream, TypeReqString);
       CodeStream::RegisterTarget returnTarget = codeStream.popTarget();
       codeStream.emitOpcodeABC(Compiler::OP_RETURN, 1, codeStream.emitTargetRef(returnTarget), 0);
    }
}

//------------------------------------------------------------

ExprNode *IfStmtNode::getSwitchOR(ExprNode *left, ExprNode *list, bool string)
{
    ExprNode *nextExpr = (ExprNode *) list->getNext();
    ExprNode *test;
    if(string)
        test = StreqExprNode::alloc( left->dbgLineNumber, left, list, true );
    else
        test = IntBinaryExprNode::alloc( left->dbgLineNumber, opEQ, left, list );
    if(!nextExpr)
        return test;
    return IntBinaryExprNode::alloc( test->dbgLineNumber, opOR, test, getSwitchOR( left, nextExpr, string ) );
}

void IfStmtNode::propagateSwitchExpr(ExprNode *left, bool string)
{
    testExpr = getSwitchOR(left, testExpr, string);
    if(propagate && elseBlock)
        ((IfStmtNode *) elseBlock)->propagateSwitchExpr(left, string);
}

void IfStmtNode::compileStmt(CodeStream &codeStream)
{
    addBreakLine(codeStream);
    
    if(testExpr->getPreferredType() == TypeReqUInt)
    {
        integer = true;
    }
    else
    {
        integer = false;
    }
   
    // jamesu - conditionals work as follows:
    // JMP_IFEQ
    // JMP [else]
    // [if]
    // Note: as with c, true is anything != 0
   
    // We need the test expression to emit an conditional instruction so we can position our blocks
    testExpr->compile(codeStream, TypeReqConditional);
   
    U32 savedConstPage = codeStream.mLastKonstPage;
    U32 elseJmpIp = codeStream.emitOpcodeABx(Compiler::OP_JMP, 0, TS2_OP_MAKE_sBX(0));
   
    if(elseBlock)
    {
        compileBlock(ifBlock, codeStream);
        elseOffset = codeStream.tell();
       
        // Restore original page at the end of the if block
        if (codeStream.mLastKonstPage != savedConstPage)
        {
           codeStream.setKonstPage(savedConstPage);
           codeStream.emitOpcodeABx(Compiler::OP_PAGEK, 0, savedConstPage);
           elseOffset++;
        }
       
        codeStream.emitOpcodeABx(Compiler::OP_JMP, 0, TS2_OP_MAKE_sBX(0));
       
        compileBlock(elseBlock, codeStream);
        endifOffset = codeStream.tell();
       
        // Restore original page at the end of the else block
        if (codeStream.mLastKonstPage != savedConstPage)
        {
           codeStream.setKonstPage(savedConstPage);
           codeStream.emitOpcodeABx(Compiler::OP_PAGEK, 0, savedConstPage);
           endifOffset++;
        }
       
        // Need to jump past else block at the end of if
       //U32 relOffs = TS2_OP_MAKE_sBX(endifOffset - elseOffset - 1);
       codeStream.patch(elseOffset, TS2_OP_ENC_A_Bx(Compiler::OP_JMP, 0, TS2_OP_MAKE_sBX(endifOffset - elseOffset - 1)));
       
        codeStream.patch(elseJmpIp, TS2_OP_ENC_A_Bx(Compiler::OP_JMP, 0, TS2_OP_MAKE_sBX((elseOffset) - elseJmpIp)));
    }
    else
    {
        compileBlock(ifBlock, codeStream);
        endifOffset = codeStream.tell();
       
        // Restore original page at the end of the if block
        if (codeStream.mLastKonstPage != savedConstPage)
        {
           codeStream.setKonstPage(savedConstPage);
           codeStream.emitOpcodeABx(Compiler::OP_PAGEK, 0, savedConstPage);
           endifOffset++;
        }
       
        codeStream.patch(elseJmpIp, TS2_OP_ENC_A_Bx(Compiler::OP_JMP, 0, TS2_OP_MAKE_sBX(endifOffset - elseJmpIp - 1)));
    }
}

//------------------------------------------------------------

void LoopStmtNode::compileStmt(CodeStream &codeStream)
{
    if(testExpr->getPreferredType() == TypeReqUInt)
    {
        integer = true;
    }
    else
    {
        integer = false;
    }
   
    addBreakLine(codeStream);
    codeStream.pushFixScope(true);
    
    if(initExpr)
        initExpr->compile(codeStream, TypeReqNone);
   
    // Konst page to restore at end of loop
    U32 originalKonstVersion = codeStream.mKonstPageVersion;
   
    if(!isDoLoop)
    {
        testExpr->compile(codeStream, TypeReqConditional);
        codeStream.emitFix(CodeStream::FIXTYPE_BREAK);
    }
    
    // Compile internals of loop.
    loopBlockStartOffset = codeStream.tell();
    Compiler:CodeStream::CodeData* startLoopData = codeStream.allocCodeData();
    compileBlock(loopBlock, codeStream);
    continueOffset = codeStream.tell();
   
    if(endLoopExpr)
        endLoopExpr->compile(codeStream, TypeReqNone);
    
    testExpr->compile(codeStream, TypeReqFalseConditional);
    codeStream.emitFix(CodeStream::FIXTYPE_LOOPBLOCKSTART);
   
   // Reset constant page if required for wrapping around the loop
   if (codeStream.mKonstPageVersion != originalKonstVersion)
   {
      // We need to emit code to reset the initial constant page at the start of the loop
      codeStream.setKonstPage(-1);
      U8* data = (U8*)dMalloc(4);
      *data = TS2_OP_ENC_A_Bx(Compiler::OP_LOADK, 0, originalKonstVersion);
      startLoopData->data = data;
      startLoopData->size = 4;
   }
   else
   {
      startLoopData->size = 0;
   }
   
    breakOffset = codeStream.tell(); // exit loop
    
    codeStream.fixLoop(
                       loopBlockStartOffset,
                       breakOffset,
                       continueOffset
                       );
    codeStream.popFixScope();
}

//------------------------------------------------------------

void IterStmtNode::compileStmt(CodeStream &codeStream)
{
   addBreakLine(codeStream);
   
   codeStream.pushFixScope(true);
   
   // init expr
   
   Compiler::CompilerConstantRef nullConst = codeStream.getConstantsTable()->addNull();
   CodeStream::RegisterTarget iteratorValueTarget = CodeStream::RegisterTarget(codeStream.getLocalVariable(varName));
   CodeStream::RegisterTarget iteratorTarget = codeStream.pushTarget(CodeStream::RegisterTarget());
   
   AssertFatal(iteratorValueTarget.regNum != -1, "Cant find iterator var");
   
   // Init iterator
   codeStream.emitOpcodeABx(Compiler::OP_LOADK, iteratorTarget.regNum, codeStream.emitKonstRef(nullConst));
   
   // Konst page to restore at end of loop
   U32 originalKonstVersion = codeStream.mKonstPageVersion;
   
   containerExpr->compile(codeStream, TypeReqVar);
   CodeStream::RegisterTarget containerTarget = codeStream.topTarget();
   
   // Compile internals of loop.
   U32 loopBlockStartOffset = codeStream.tell();
   
   codeStream.emitOpcodeABCRef(isStringIter ? Compiler::OP_ITR_SGET : Compiler::OP_ITR_GET, iteratorValueTarget.regNum, containerTarget, iteratorTarget);
   codeStream.emitFix(CodeStream::FIXTYPE_BREAK);
   
   Compiler:CodeStream::CodeData* startLoopData = codeStream.allocCodeData();
   compileBlock(body, codeStream);
   U32 continueOffset = codeStream.tell();
   
   // Back to start of loop
   codeStream.emitFix(CodeStream::FIXTYPE_LOOPBLOCKSTART);
   
   // Reset constant page if required for wrapping around the loop
   if (codeStream.mKonstPageVersion != originalKonstVersion)
   {
      // We need to emit code to reset the initial constant page at the start of the loop
      codeStream.setKonstPage(-1);
      U8* data = (U8*)dMalloc(4);
      *data = TS2_OP_ENC_A_Bx(Compiler::OP_LOADK, 0, originalKonstVersion);
      startLoopData->data = data;
      startLoopData->size = 4;
   }
   else
   {
      startLoopData->size = 0;
   }
   
   codeStream.popTarget(); // containerExpr
   codeStream.popTarget(); // iteratorTarget
   //codeStream.popTarget(); // iteratorValueTarget // jamesu - not actually pushed to stack
   
   U32 breakOffset = codeStream.tell(); // exit loop
   
   codeStream.fixLoop(
                      loopBlockStartOffset,
                      breakOffset,
                      continueOffset
                      );
   codeStream.popFixScope();
}

//------------------------------------------------------------

void ConditionalExprNode::compile(CodeStream &codeStream, TypeReq type)
{
   U32 startTarget = codeStream.mTargetList.size();
   U32 ip = codeStream.tell();
   
   AssertFatal(type != TypeReqFalseConditional, "Not tested");
   
   int cmpValue = 0;
   
   if (type == TypeReqFalseConditional)
   {
      type = TypeReqConditional;
      cmpValue = 1;
   }
   
   if(testExpr->getPreferredType() == TypeReqUInt)
   {
      integer = true;
   }
   else
   {
      integer = false;
   }
   
   CodeStream::RegisterTarget targetRegister;
   
   if (type == TypeReqVar)
   {
      targetRegister = codeStream.pushTarget(CodeStream::RegisterTarget());
   }
   else if (type == TypeReqTargetRegister)
   {
      targetRegister = codeStream.topTarget();
   }
   else if (type == TypeReqConditional)
   {
      targetRegister = codeStream.pushTarget(CodeStream::RegisterTarget());
   }
   
   bool firstBranchDirty = false;
   bool secondBranchDirty = false;
   
   testExpr->compile(codeStream, TypeReqConditional);
   U32 elseJmpIp = codeStream.emitOpcodeABx(Compiler::OP_JMP, 0, TS2_OP_MAKE_sBX(0));
   
   U32 savedConstPage = codeStream.mLastKonstPage;
   
   // If we are assigning a target register we need to
   // emit it for each branch, otherwise store the result in our own temp target.
   
   codeStream.pushTargetReference(targetRegister);
   trueExpr->compile(codeStream, TypeReqTargetRegister);
   ip = codeStream.tell();
   
   // Restore konst page
   if (codeStream.mLastKonstPage != savedConstPage)
   {
      codeStream.setKonstPage(savedConstPage);
      firstBranchDirty = true;
   }
   
   U32 endBlockJmpIp = codeStream.emitOpcodeABx(Compiler::OP_JMP, 0, TS2_OP_MAKE_sBX(0));
   codeStream.patch(elseJmpIp, TS2_OP_ENC_A_Bx(Compiler::OP_JMP, 0, TS2_OP_MAKE_sBX(ip - elseJmpIp)));
   
   // Need to push the target again
   codeStream.pushTargetReference(targetRegister);
   falseExpr->compile(codeStream, TypeReqTargetRegister);
   ip = codeStream.tell();
   
   // Restore konst page
   if (codeStream.mLastKonstPage != savedConstPage)
   {
      codeStream.setKonstPage(savedConstPage);
      secondBranchDirty = true;
   }
   
   codeStream.patch(endBlockJmpIp, TS2_OP_ENC_A_Bx(Compiler::OP_JMP, 0, TS2_OP_MAKE_sBX(ip - endBlockJmpIp - 1)));
   
   // Correct konst here
   if (firstBranchDirty && secondBranchDirty)
   {
      codeStream.setKonstPage(-1);
   }
   
   // Final push for result
   if (type == TypeReqConditional)
   {
      Compiler::CompilerConstantRef trueConst = codeStream.getConstantsTable()->addInt(0);
      codeStream.emitOpcodeABCRef(Compiler::OP_EQ, cmpValue, targetRegister, CodeStream::RegisterTarget(trueConst));
      codeStream.popTarget();
      AssertFatal(codeStream.mTargetList.size() == startTarget, "Target size misbalance");
   }
   else if (type == TypeReqTargetRegister)
   {
      codeStream.popTarget();
      AssertFatal(codeStream.mTargetList.size() == startTarget-1, "Target size misbalance");
   }
   else
   {
      AssertFatal(codeStream.mTargetList.size() == startTarget+1, "Target size misbalance");
   }
}

TypeReq ConditionalExprNode::getPreferredType()
{
    return trueExpr->getPreferredType();
}

//------------------------------------------------------------

void FloatBinaryExprNode::compile(CodeStream &codeStream, TypeReq type)
{
#ifdef DEBUG_COMPILER
    Con::printf("FloatBinaryExprNode [%s]", type == TypeReqTargetRegister ? "direct" : "temp");
#endif
   
   int cmpValue = 1;
   
   if (type == TypeReqFalseConditional)
   {
      type = TypeReqConditional;
      cmpValue = 0;
   }
   
    CodeStream::RegisterTarget targetRegister;
    if (type == TypeReqTargetRegister)
    {
       targetRegister = codeStream.popTarget();
    }
    
    right->compile(codeStream, TypeReqFloat); // 2 -> 3
    CodeStream::RegisterTarget rightTarget = codeStream.topTarget();
    left->compile(codeStream, TypeReqFloat);
    CodeStream::RegisterTarget leftTarget = codeStream.topTarget();
    
    CodeStream::RegisterTarget leftReg = codeStream.popTarget();
    CodeStream::RegisterTarget rightReg = codeStream.popTarget();
    if (targetRegister.regNum < 0)
    {
       targetRegister = codeStream.pushTarget(CodeStream::RegisterTarget());
    }
   
#ifdef DEBUG_COMPILER
    Con::printf("FloatBinaryExprNode::Add %s + %s -> %s", leftReg.toString(), rightReg.toString(), targetRegister.toString());
#endif
   
    switch(op)
    {
        case '+':
          codeStream.emitOpcodeABCRef(Compiler::OP_ADD, targetRegister.regNum, (leftReg), (rightReg));
            break;
       case '-':
          codeStream.emitOpcodeABCRef(Compiler::OP_SUB, targetRegister.regNum, (leftReg), (rightReg));
            break;
       case '/':
          codeStream.emitOpcodeABCRef(Compiler::OP_DIV, targetRegister.regNum, (leftReg), (rightReg));
            break;
       case '*':
          codeStream.emitOpcodeABCRef(Compiler::OP_MUL, targetRegister.regNum, (leftReg), (rightReg));
            break;
       default:
          Con::errorf("Unknown operand %c", op);
          break;
    }
   
    if (type == TypeReqConditional)
    {
       Compiler::CompilerConstantRef trueConst = codeStream.getConstantsTable()->addInt(0);
       codeStream.emitOpcodeABCRef(Compiler::OP_EQ, cmpValue, targetRegister, CodeStream::RegisterTarget(trueConst));
       codeStream.popTarget();
    }
}

TypeReq FloatBinaryExprNode::getPreferredType()
{
    return TypeReqFloat;
}

//------------------------------------------------------------

void IntBinaryExprNode::getSubTypeOperand()
{
    subType = TypeReqUInt;
    conditionalValue = 0;
    operand = 0;
    cmpValue = 1;
    switch(op)
    {
        case '^':
          operand = Compiler::OP_XOR;
            break;
        case '%':
            operand = Compiler::OP_MOD;
            break;
        case '&':
            operand = Compiler::OP_BITAND;
            break;
        case '|':
            operand = Compiler::OP_BITOR;
            break;
        case '<':
            conditionalValue = Compiler::OP_LT;
            cmpValue = 0;
            subType = TypeReqFloat;
            break;
        case '>':
            conditionalValue = Compiler::OP_LE;
            cmpValue = 1;
            break;
        case opGE:
           conditionalValue = Compiler::OP_LT;
            cmpValue = 1;
            break;
       case opLE:
           conditionalValue = Compiler::OP_LE;
           cmpValue = 0;
            subType = TypeReqFloat;
            break;
       case opEQ:
            conditionalValue = Compiler::OP_EQ;
            subType = TypeReqFloat;
            cmpValue = 0;
            break;
       case opNE:
          conditionalValue = Compiler::OP_EQ;
          subType = TypeReqFloat;
          cmpValue = 1;
          break;
        case opOR:
          operand = Compiler::COND_OR;
          break;
        case opAND:
          operand = Compiler::COND_AND;
          break;
        case opSHR:
          operand = Compiler::OP_SHR;
          break;
       case opSHL:
          operand = Compiler::OP_SHL;
          break;
    }
}

void IntBinaryExprNode::compile(CodeStream &codeStream, TypeReq type)
{
   getSubTypeOperand();
   
   if (type == TypeReqFalseConditional)
   {
      type = TypeReqConditional;
      cmpValue = cmpValue == 1 ? 0 : 1;
   }
   
   if (type == TypeReqConditional)
   {
      CodeStream::RegisterTarget leftReg;
      CodeStream::RegisterTarget rightReg;
      
      if(operand == Compiler::COND_OR || operand == Compiler::COND_AND)
      {
         // For this type of conditional,
         // left == right != false AND
         // left != false, right != false OR
         
         if (operand == Compiler::COND_AND)
         {
            left->compile(codeStream, TypeReqConditional);
            //leftReg = codeStream.popTarget();
            
            U32 fixIp = codeStream.emitOpcodeABx(Compiler::OP_JMP, 0, TS2_OP_MAKE_sBX(2)); // false path
            
            // Following true path here
            right->compile(codeStream, TypeReqConditional);
            //rightReg = codeStream.popTarget();
            
            codeStream.patch(fixIp, TS2_OP_ENC_A_Bx(Compiler::OP_JMP, 0, TS2_OP_MAKE_sBX(codeStream.tell() - fixIp - 1)));
            
            // <conditional JMP for false>
            // <true branch>
         }
         else if (operand == Compiler::COND_OR)
         {
            left->compile(codeStream, TypeReqFalseConditional);
            //leftReg = codeStream.popTarget();
            
            U32 fixIp = codeStream.emitOpcodeABx(Compiler::OP_JMP, 0, TS2_OP_MAKE_sBX(2)); // false path
            
            // Following false path here
            right->compile(codeStream, TypeReqConditional);
            //rightReg = codeStream.popTarget();
            
            // NOTE: true path needs to go to the instruction AFTER the false JMP
            codeStream.patch(fixIp, TS2_OP_ENC_A_Bx(Compiler::OP_JMP, 0, TS2_OP_MAKE_sBX(codeStream.tell()+1 - fixIp - 1)));
            
            // <conditional JMP for false>
            // <true branch>
         }
      }
      else if (conditionalValue != 0)
      {
         right->compile(codeStream, subType);
         left->compile(codeStream, subType);
         
         leftReg = codeStream.popTarget();
         rightReg = codeStream.popTarget();
         
         // We can just emit a conditional op
         codeStream.emitOpcodeABCRef(conditionalValue, cmpValue, (leftReg), (rightReg));
      }
      else
      {
         right->compile(codeStream, subType); // right
         left->compile(codeStream, subType); // left
         
         leftReg = codeStream.popTarget();
         rightReg = codeStream.popTarget();
         
         // We need a temp target
         CodeStream::RegisterTarget tempTarget = codeStream.pushTarget(CodeStream::RegisterTarget());
         
         codeStream.emitOpcodeABCRef(operand, tempTarget.regNum, (leftReg), (rightReg));
         
         Compiler::CompilerConstantRef trueValue = codeStream.getConstantsTable()->addInt(1);
         
         codeStream.emitOpcodeABCRef(Compiler::OP_EQ, 0, tempTarget, (CodeStream::RegisterTarget(trueValue)));
         
         codeStream.popTarget();
      }
   }
   else
   {
      CodeStream::RegisterTarget targetRegister;
      if (type == TypeReqTargetRegister)
      {
         targetRegister = codeStream.popTarget();
      }
      
      CodeStream::RegisterTarget leftReg;
      CodeStream::RegisterTarget rightReg;
      
      if (targetRegister.regNum < 0)
      {
         targetRegister = codeStream.pushTarget(CodeStream::RegisterTarget());
      }
      
      if(operand == Compiler::COND_OR || operand == Compiler::COND_AND)
      {
         // For this type of conditional,
         // left == right != false AND
         // left != false, right != false OR
         
         Compiler::CompilerConstantRef trueValue = codeStream.getConstantsTable()->addInt(1);
         Compiler::CompilerConstantRef falseValue = codeStream.getConstantsTable()->addInt(0);
         
         if (operand == Compiler::COND_AND)
         {
            left->compile(codeStream, TypeReqFalseConditional);
            //leftReg = codeStream.popTarget();
            
            U32 fixIp = codeStream.emitOpcodeABx(Compiler::OP_JMP, 0, TS2_OP_MAKE_sBX(2)); // false path
            
            // Following true path here
            right->compile(codeStream, TypeReqFalseConditional);
            //rightReg = codeStream.popTarget();
            
            codeStream.patch(fixIp, TS2_OP_ENC_A_Bx(Compiler::OP_JMP, 0, TS2_OP_MAKE_sBX(codeStream.tell() - fixIp - 1)));
            
            // <conditional JMP for false>
            // <true branch>
         }
         else if (operand == Compiler::COND_OR)
         {
            left->compile(codeStream, TypeReqFalseConditional);
            //leftReg = codeStream.popTarget();
            
            U32 fixIp = codeStream.emitOpcodeABx(Compiler::OP_JMP, 0, TS2_OP_MAKE_sBX(2)); // false path
            
            // Following false path here
            right->compile(codeStream, TypeReqConditional);
            //rightReg = codeStream.popTarget();
            
            // NOTE: true path needs to go to the instruction AFTER the false JMP
            codeStream.patch(fixIp, TS2_OP_ENC_A_Bx(Compiler::OP_JMP, 0, TS2_OP_MAKE_sBX(codeStream.tell()+1 - fixIp - 1)));
            
            // <conditional JMP for flase>
            // <true branch>
         }
         
         // <conditional JMP for false>
         // <true branch>
         
         codeStream.emitOpcodeABx(Compiler::OP_JMP, 0, TS2_OP_MAKE_sBX(2));
         codeStream.emitOpcodeABx(Compiler::OP_LOADK, targetRegister.regNum, codeStream.emitKonstRef(trueValue));
         codeStream.emitOpcodeABx(Compiler::OP_JMP, 0, TS2_OP_MAKE_sBX(1));
         codeStream.emitOpcodeABx(Compiler::OP_LOADK, targetRegister.regNum, codeStream.emitKonstRef(falseValue));
      }
      else if (conditionalValue != 0)
      {
         right->compile(codeStream, subType);
         left->compile(codeStream, subType);
         
         leftReg = codeStream.popTarget();
         rightReg = codeStream.popTarget();
         
         // Gotta convert the conditional
         codeStream.emitOpcodeABCRef(conditionalValue, cmpValue, (leftReg), (rightReg));
         
         // jamesu - conditionals work as follows:
         // JMP_IFEQ
         // JMP [else]
         // [if]
         // Note: as with c, true is anything != 0
         
         Compiler::CompilerConstantRef trueValue = codeStream.getConstantsTable()->addInt(1);
         Compiler::CompilerConstantRef falseValue = codeStream.getConstantsTable()->addInt(0);
         
         codeStream.emitOpcodeABx(Compiler::OP_JMP, 0, TS2_OP_MAKE_sBX(2));
         codeStream.emitOpcodeABx(Compiler::OP_LOADK, targetRegister.regNum, codeStream.emitKonstRef(trueValue));
         codeStream.emitOpcodeABx(Compiler::OP_JMP, 0, TS2_OP_MAKE_sBX(1));
         codeStream.emitOpcodeABx(Compiler::OP_LOADK, targetRegister.regNum, codeStream.emitKonstRef(falseValue));
      }
      else
      {
         right->compile(codeStream, subType);
         left->compile(codeStream, subType);
         
         leftReg = codeStream.popTarget();
         rightReg = codeStream.popTarget();
         
         codeStream.emitOpcodeABCRef(operand, targetRegister.regNum, (leftReg), (rightReg));
      }
   }
}

TypeReq IntBinaryExprNode::getPreferredType()
{
    return TypeReqUInt;
}

//------------------------------------------------------------

void StreqExprNode::compile(CodeStream &codeStream, TypeReq type)
{
   CodeStream::RegisterTarget leftReg;
   CodeStream::RegisterTarget rightReg;
   CodeStream::RegisterTarget targetRegister;
   
   if (type == TypeReqNone)
   {
      return;
   }
   
   U32 cmpValue = eq ? 0 : 1;
   if (type == TypeReqFalseConditional)
   {
      cmpValue = eq ? 1 : 0;
      type = TypeReqConditional;
   }
   
   if (type == TypeReqTargetRegister)
   {
      targetRegister = codeStream.topTarget();
   }
   else
   {
      targetRegister = codeStream.pushTarget(CodeStream::RegisterTarget(targetRegister));
   }
   
   right->compile(codeStream, TypeReqVar);
   left->compile(codeStream, TypeReqVar);
   
   leftReg = codeStream.popTarget();
   rightReg = codeStream.popTarget();
   
   codeStream.emitOpcodeABCRef(Compiler::OP_EQ_STR, cmpValue, leftReg, rightReg);
   
   if (type != TypeReqConditional)
   {
      // Need this as a number
      Compiler::CompilerConstantRef trueValue = codeStream.getConstantsTable()->addInt(1);
      Compiler::CompilerConstantRef falseValue = codeStream.getConstantsTable()->addInt(0);
      
      codeStream.emitOpcodeABx(Compiler::OP_JMP, 0, TS2_OP_MAKE_sBX(2));
      codeStream.emitOpcodeABx(Compiler::OP_LOADK, targetRegister.regNum, codeStream.emitKonstRef(trueValue));
      codeStream.emitOpcodeABx(Compiler::OP_JMP, 0, TS2_OP_MAKE_sBX(1));
      codeStream.emitOpcodeABx(Compiler::OP_LOADK, targetRegister.regNum, codeStream.emitKonstRef(falseValue));
      
      if (type == TypeReqTargetRegister)
      {
         codeStream.popTarget();
      }
   }
   else
   {
      codeStream.popTarget();
   }
}

TypeReq StreqExprNode::getPreferredType()
{
    return TypeReqUInt;
}

//------------------------------------------------------------

void StrcatExprNode::compile(CodeStream &codeStream, TypeReq type)
{
   AssertFatal(!(type == TypeReqConditional || type == TypeReqFalseConditional), "Unhandled case");
   
   // Destination override (otherwise we use startReg)
   CodeStream::RegisterTarget destRegister;
   if (type == TypeReqTargetRegister)
   {
      destRegister = codeStream.popTarget();
   }
   
   CodeStream::RegisterTarget startReg = codeStream.pushTarget(CodeStream::RegisterTarget());
   CodeStream::RegisterTarget charReg;
   CodeStream::RegisterTarget endReg;
   
   if (type != TypeReqTargetRegister)
   {
      destRegister = startReg;
   }
 
   if (appendChar)
   {
      charReg = codeStream.pushTarget(CodeStream::RegisterTarget());
   }
   
   endReg = codeStream.pushTarget(CodeStream::RegisterTarget());
   
   right->compile(codeStream, TypeReqString);
   left->compile(codeStream, TypeReqString);
   
    // OP_CONCAT
   // A := B...C
   
   CodeStream::RegisterTarget leftReg = codeStream.popTarget();
   CodeStream::RegisterTarget rightReg = codeStream.popTarget();
   
   if (leftReg.constRef.idx >= 0)
   {
      codeStream.emitOpcodeABx(Compiler::OP_LOADK, startReg.regNum, codeStream.emitKonstRef(leftReg.constRef));
   }
   else
   {
      codeStream.emitOpcodeABC(Compiler::OP_MOVE, startReg.regNum, leftReg.regNum, 0);
   }
   
   char str[2];
   str[0] = appendChar;
   str[1] = '\0';
   Compiler::CompilerConstantRef charConst = codeStream.getConstantsTable()->addString(str);
   
   if (appendChar)
   {
      codeStream.emitOpcodeABx(Compiler::OP_LOADK, charReg.regNum, codeStream.emitKonstRef(charConst));
   }
   
   
   if (rightReg.constRef.idx >= 0)
   {
      codeStream.emitOpcodeABx(Compiler::OP_LOADK, endReg.regNum, codeStream.emitKonstRef(rightReg.constRef));
   }
   else
   {
      codeStream.emitOpcodeABC(Compiler::OP_MOVE, endReg.regNum, rightReg.regNum, 0);
   }
   
   
   // Concat from left to right
   codeStream.emitOpcodeABC(Compiler::OP_CONCAT, destRegister.regNum, startReg.regNum, endReg.regNum);
   
   codeStream.popTarget();
   if (appendChar) codeStream.popTarget();
   
   if (type == TypeReqTargetRegister)
   {
      codeStream.popTarget(); // startReg
   }
}

TypeReq StrcatExprNode::getPreferredType()
{
    return TypeReqString;
}

//------------------------------------------------------------

void CommaCatExprNode::compile(CodeStream &codeStream, TypeReq type)
{
   AssertFatal(false, "Not handled");
   
   /* TODO FIX
    left->compile(codeStream, TypeReqString);
    codeStream.emit(OP_ADVANCE_STR_COMMA);
    right->compile(codeStream, TypeReqString);
    codeStream.emit(OP_REWIND_STR);
    
    // At this point the stack has the concatenated string.
    
    // But we're paranoid, so accept (but whine) if we get an oddity...
    if(type == TypeReqUInt || type == TypeReqFloat)
        Con::warnf(ConsoleLogEntry::General, "%s (%d): converting comma string to a number... probably wrong.", dbgFileName, dbgLineNumber);
    if(type == TypeReqUInt)
        codeStream.emit(OP_STR_TO_UINT);
    else if(type == TypeReqFloat)
        codeStream.emit(OP_STR_TO_FLT);*/
}

TypeReq CommaCatExprNode::getPreferredType()
{
    return TypeReqString;
}

//------------------------------------------------------------

void IntUnaryExprNode::compile(CodeStream &codeStream, TypeReq type)
{
    integer = true;
   U32 cmpValue = 1;
   if (type == TypeReqFalseConditional)
   {
      type = TypeReqConditional;
      cmpValue = 0;
   }
   
   CodeStream::RegisterTarget targetRegister;
   if (type == TypeReqTargetRegister)
   {
      targetRegister = codeStream.topTarget();
   }
   else
   {
      targetRegister = codeStream.pushTarget(CodeStream::RegisterTarget(targetRegister));
      if (!(type == TypeReqNone || type == TypeReqConditional))
      {
         codeStream.pushTargetReference(targetRegister); // need to keep this around
      }
   }
   
   expr->compile(codeStream, TypeReqTargetRegister);
   if(op == '!')
      codeStream.emitOpcodeABCRef(Compiler::OP_NOT, targetRegister.regNum, targetRegister, CodeStream::RegisterTarget());
   else if(op == '~')
      codeStream.emitOpcodeABCRef(Compiler::OP_ONESCOMPLEMENT, targetRegister.regNum, targetRegister, CodeStream::RegisterTarget());
   
   if (type == TypeReqConditional)
   {
      // We compare with false since true is anything not != 0
      Compiler::CompilerConstantRef trueConst = codeStream.getConstantsTable()->addInt(0);
      codeStream.emitOpcodeABCRef(Compiler::OP_EQ, cmpValue, targetRegister, (CodeStream::RegisterTarget(trueConst)));
#ifdef DEBUG_COMPILER
      Con::printf("IntUnaryExpr conditional check");
#endif
   }
}

TypeReq IntUnaryExprNode::getPreferredType()
{
    return TypeReqUInt;
}

//------------------------------------------------------------

void FloatUnaryExprNode::compile(CodeStream &codeStream, TypeReq type)
{
   if (type == TypeReqNone)
   {
      return;
   }
   
   CodeStream::RegisterTarget targetRegister;
   Compiler::CompilerConstantRef numRef = codeStream.getConstantsTable()->addFloat(-1);
   
   U32 cmpValue = 1;
   if (type == TypeReqFalseConditional)
   {
      type = TypeReqConditional;
      cmpValue = 0;
   }

   if (type == TypeReqTargetRegister)
   {
      targetRegister = codeStream.topTarget();
   }
   else
   {
      targetRegister = codeStream.pushTarget(CodeStream::RegisterTarget(targetRegister));
   }
   
   if (type != TypeReqTargetRegister)
   {
      codeStream.pushTargetReference(targetRegister);
   }
   expr->compile(codeStream, TypeReqTargetRegister);
   codeStream.emitOpcodeABCRef(Compiler::OP_MUL, targetRegister.regNum, targetRegister, numRef);
   
   if (type == TypeReqConditional)
   {
      // Compare to true
      Compiler::CompilerConstantRef trueConst = codeStream.getConstantsTable()->addInt(0);
      codeStream.emitOpcodeABCRef(Compiler::OP_EQ, cmpValue, targetRegister, trueConst);
      codeStream.popTarget(); // targetRegister
   }
}

TypeReq FloatUnaryExprNode::getPreferredType()
{
    return TypeReqFloat;
}

//------------------------------------------------------------

void VarNode::compile(CodeStream &codeStream, TypeReq type)
{
    if(type == TypeReqNone)
    {
        return;
    }
   
   
   U32 cmpValue = 1;
   if (type == TypeReqFalseConditional)
   {
      type = TypeReqConditional;
      cmpValue = 0;
   }

    S32 variableRegister = -1;
   
   // Firstly lets get the local variable case out of the way. All other cases require
   // using a temporary register or the target.
   if (varName[0] != '$')
   {
      variableRegister = codeStream.getLocalVariable(varName);
      AssertFatal(variableRegister != -1, "Local variable not registered");
      
      if (!arrayIndex)
      {
         if (type == TypeReqTargetRegister)
         {
            codeStream.emitOpcodeABC(Compiler::OP_MOVE, codeStream.topTarget().regNum, variableRegister, 0);
            codeStream.popTarget();
         }
         else if (type == TypeReqConditional)
         {
            // Compare to true
            Compiler::CompilerConstantRef trueConst = codeStream.getConstantsTable()->addInt(0);
            codeStream.emitOpcodeABCRef(Compiler::OP_EQ, cmpValue, (CodeStream::RegisterTarget(variableRegister)), trueConst);
         }
         else
         {
            codeStream.pushTarget(CodeStream::RegisterTarget(variableRegister));
         }
         
         return;
      }
   }
   
   CodeStream::RegisterTarget tempRegister;
   if (type == TypeReqTargetRegister)
   {
      tempRegister = codeStream.topTarget();
   }
   else
   {
      tempRegister = codeStream.pushTarget(CodeStream::RegisterTarget());
   }
   
   // Handle arrays and global vars separately since arrays may require an additional 2
   // temp registers.
   if (arrayIndex)
   {
      CodeStream::RegisterTarget arrayTarget = codeStream.pushTarget(CodeStream::RegisterTarget());
      CodeStream::RegisterTarget arrayValueTarget;
      Compiler::CompilerConstantRef nullRef = codeStream.getConstantsTable()->addNull();
      bool arrayIndexPushed = false;
      
      // Registers in stack need to go VALUE INDEX
      if (arrayTarget.regNum == tempRegister.regNum+1)
      {
         arrayValueTarget = tempRegister;
      }
      else
      {
         CodeStream::RegisterTarget newTarget = codeStream.pushTarget(CodeStream::RegisterTarget());
         arrayValueTarget = arrayTarget;
         arrayTarget = newTarget;
         arrayIndexPushed = true;
      }
      
      // Load global if applicable
      if (varName[0] == '$')
      {
         Compiler::CompilerConstantRef varNameIdx = codeStream.getConstantsTable()->addString(varName);
         codeStream.emitOpcodeABCRef(Compiler::OP_LOADVAR, arrayValueTarget.regNum, varNameIdx, 0);
      }
      else
      {
         codeStream.emitOpcodeABC(Compiler::OP_MOVE, arrayValueTarget.regNum, variableRegister, 0);
      }
      
      // Load array index
      arrayIndex->compile(codeStream, TypeReqTargetRegister);
      // Load field, replacing the array copy
      codeStream.emitOpcodeABCRef(Compiler::OP_GETFIELDA, arrayValueTarget.regNum, arrayValueTarget, nullRef);
      
      if (arrayIndexPushed)
      {
         codeStream.popTarget();
      }
      
      // Move instruction to target if applicable
      if (type == TypeReqTargetRegister && arrayValueTarget.regNum != tempRegister.regNum)
      {
         codeStream.emitOpcodeABC(Compiler::OP_MOVE, tempRegister.regNum, arrayValueTarget.regNum, 0);
      }
      
      // Do conditional check if applicable
      if (type == TypeReqConditional)
      {
         // Compare to true
         Compiler::CompilerConstantRef trueConst = codeStream.getConstantsTable()->addInt(0);
         codeStream.emitOpcodeABCRef(Compiler::OP_EQ, cmpValue, arrayValueTarget, trueConst);
      }
      else if (type == TypeReqTargetRegister)
      {
         codeStream.popTarget(); // absorb target
      }
   }
   else
   {
      // Must be a global
      AssertFatal(varName[0] == '$', "Not a global");
      Compiler::CompilerConstantRef varNameIdx = codeStream.getConstantsTable()->addString(varName);
      codeStream.emitOpcodeABCRef(Compiler::OP_LOADVAR, tempRegister.regNum, varNameIdx, 0);
      
      if (type == TypeReqNone || type == TypeReqTargetRegister)
      {
         codeStream.popTarget(); // tempRegister
      }
      else if (type == TypeReqConditional)
      {
         // Compare to true
         Compiler::CompilerConstantRef trueConst = codeStream.getConstantsTable()->addInt(0);
         codeStream.emitOpcodeABCRef(Compiler::OP_EQ, cmpValue, tempRegister, trueConst);
         codeStream.popTarget(); // tempRegister
      }
   }
}

TypeReq VarNode::getPreferredType()
{
    return TypeReqNone; // no preferred type
}

//------------------------------------------------------------

void IntNode::compile(CodeStream &codeStream, TypeReq type)
{
   Compiler::CompilerConstantRef ref;
   if (value < 0)
      ref = codeStream.getConstantsTable()->addInt(value);
   else
      ref = codeStream.getConstantsTable()->addFloat(value);
   
   U32 cmpValue = 1;
   if (type == TypeReqFalseConditional)
   {
      type = TypeReqConditional;
      cmpValue = 0;
   }
   
    if (type == TypeReqConditional)
    {
       // Compare to true
       Compiler::CompilerConstantRef trueConst = codeStream.getConstantsTable()->addInt(0);
       codeStream.emitOpcodeABCRef(Compiler::OP_EQ, cmpValue, (CodeStream::RegisterTarget(ref)), (CodeStream::RegisterTarget(trueConst)));
#ifdef DEBUG_COMPILER
       Con::printf("IntNode conditional check [TODO: should be optimized out]");
#endif
    }
    else if (type == TypeReqTargetRegister)
    {
       CodeStream::RegisterTarget targetRegister = codeStream.popTarget();
#ifdef DEBUG_COMPILER
       Con::printf("IntNode target == %s [direct]", targetRegister.toString());
#endif
       codeStream.emitOpcodeABx(Compiler::OP_LOADK, targetRegister.regNum, codeStream.emitKonstRef(ref));
    }
    else
    {
       CodeStream::RegisterTarget targetRegister = codeStream.pushTarget(CodeStream::RegisterTarget(ref));
#ifdef DEBUG_COMPILER
       Con::printf("IntNode target == %s", targetRegister.toString());
#endif
    }
}

TypeReq IntNode::getPreferredType()
{
    return TypeReqUInt;
}

//------------------------------------------------------------

void FloatNode::compile(CodeStream &codeStream, TypeReq type)
{
   Compiler::CompilerConstantRef ref = codeStream.getConstantsTable()->addFloat(value);
   
   U32 cmpValue = 1;
   if (type == TypeReqFalseConditional)
   {
      type = TypeReqConditional;
      cmpValue = 0;
   }
   
   if (type == TypeReqConditional)
   {
      // Compare to true
      Compiler::CompilerConstantRef trueConst = codeStream.getConstantsTable()->addInt(0);
      codeStream.emitOpcodeABCRef(Compiler::OP_EQ, cmpValue, (CodeStream::RegisterTarget(ref)), (CodeStream::RegisterTarget(trueConst)));
#ifdef DEBUG_COMPILER
      Con::printf("FloatNode conditional check [TODO: should be optimized out]");
#endif
   }
   else if (type == TypeReqTargetRegister)
   {
      CodeStream::RegisterTarget targetRegister = codeStream.popTarget();
#ifdef DEBUG_COMPILER
      Con::printf("FloatNode target == %s [direct]", targetRegister.toString());
#endif
      codeStream.emitOpcodeABx(Compiler::OP_LOADK, targetRegister.regNum, codeStream.emitKonstRef(ref));
   }
   else
   {
      CodeStream::RegisterTarget targetRegister = codeStream.pushTarget(CodeStream::RegisterTarget(ref));
#ifdef DEBUG_COMPILER
      Con::printf("FloatNode target == %s", targetRegister.toString());
#endif
   }
}

TypeReq FloatNode::getPreferredType()
{
    return TypeReqFloat;
}

//------------------------------------------------------------

void StrConstNode::compile(CodeStream &codeStream, TypeReq type)
{
   if (type == TypeReqNone)
   {
      return;
   }
   
   CompilerConstantRef ref = codeStream.getConstantsTable()->addString(str, true, tag);
   
   U32 cmpValue = 1;
   if (type == TypeReqFalseConditional)
   {
      type = TypeReqConditional;
      cmpValue = 0;
   }
   
   if (type == TypeReqConditional)
   {
      // Compare to true
      Compiler::CompilerConstantRef trueConst = codeStream.getConstantsTable()->addInt(0);
      codeStream.emitOpcodeABCRef(Compiler::OP_EQ, cmpValue, (CodeStream::RegisterTarget(ref)), (CodeStream::RegisterTarget(trueConst)));
#ifdef DEBUG_COMPILER
      Con::printf("StrConstNode conditional check [TODO: should be optimized out]");
#endif
   }
   else if (type == TypeReqTargetRegister)
   {
      CodeStream::RegisterTarget targetRegister = codeStream.popTarget();
#ifdef DEBUG_COMPILER
      Con::printf("StrConstNode target == %s [direct]", targetRegister.toString());
#endif
      codeStream.emitOpcodeABx(Compiler::OP_LOADK, targetRegister.regNum, codeStream.emitKonstRef(ref));
   }
   else
   {
      CodeStream::RegisterTarget targetRegister = codeStream.pushTarget(CodeStream::RegisterTarget(ref));
#ifdef DEBUG_COMPILER
      Con::printf("StrConstNode target == %s", targetRegister.toString());
#endif
   }
}

TypeReq StrConstNode::getPreferredType()
{
    return TypeReqString;
}

//------------------------------------------------------------

void DocBlockNode::compile(CodeStream &codeStream, TypeReq type)
{
   AssertFatal(type == TypeReqNone, "Unhandled case");
   
   // NOTE: we only emit code here if we have a class doc
   if (isClassDoc())
   {
      char nsDocBlockClass[128];
      char* sansClass = dStrstr( doc, "@class" );
      if( !sansClass )
         sansClass = dStrstr( doc, "\\class" );
      
      // Don't save the class declaration. Scan past the 'class'
      // keyword and up to the first whitespace.
      sansClass += 7;
      S32 index = 0;
      while( ( *sansClass != ' ' ) && ( *sansClass != '\n' ) && *sansClass && ( index < ( sizeof(nsDocBlockClass) - 1 ) ) )
      {
         nsDocBlockClass[index++] = *sansClass;
         sansClass++;
      }
      nsDocBlockClass[index] = '\0';
      sansClass++;
      
      Compiler::CompilerConstantRef nsIdx = codeStream.getConstantsTable()->addNamespace(nsDocBlockClass);
      Compiler::CompilerConstantRef docIdx = codeStream.getConstantsTable()->addString(sansClass);
      codeStream.emitOpcodeABCRef(Compiler::OP_DOCNSFUNC, 0, nsIdx, docIdx);
   }
}

TypeReq DocBlockNode::getPreferredType()
{
   return TypeReqNone;
}

bool DocBlockNode::isClassDoc()
{
   if (!doc)
      return false;
   
   char* sansClass = dStrstr( doc, "@class" );
   if( !sansClass )
      sansClass = dStrstr( doc, "\\class" );
   
   return sansClass != NULL;
}

//------------------------------------------------------------

void ConstantNode::compile(CodeStream &codeStream, TypeReq type)
{
   Compiler::CompilerConstantRef ref;
   ref = codeStream.getConstantsTable()->addString(value);
   
   AssertFatal(!(type == TypeReqConditional || type == TypeReqFalseConditional), "Unhandled case");
   
    if (type == TypeReqTargetRegister)
    {
       CodeStream::RegisterTarget targetRegister = codeStream.popTarget();
       codeStream.emitOpcodeABx(Compiler::OP_LOADK, targetRegister.regNum, codeStream.emitKonstRef(ref));
    }
    else
    {
       codeStream.pushTarget(CodeStream::RegisterTarget(ref));
    }
}

TypeReq ConstantNode::getPreferredType()
{
    return TypeReqString;
}

//------------------------------------------------------------

void AssignExprNode::compile(CodeStream &codeStream, TypeReq type)
{
   S32 variableRegister = -1;
   subType = expr->getPreferredType();
   if(subType == TypeReqNone)
      subType = type;
   if(subType == TypeReqNone)
   {
      subType = TypeReqVar;
   }
   AssertFatal(!(type == TypeReqConditional || type == TypeReqFalseConditional), "Unhandled case");
   
   CodeStream::RegisterTarget outRegister;
 
   if (varName[0] != '$')
   {
      variableRegister = codeStream.getLocalVariable(varName);
      
      if (variableRegister == -1)
      {
         AssertFatal(false, "Local variable not registered");
      }
   }
   
   // We either store to a target or emit it.
   if (type == TypeReqTargetRegister)
   {
      outRegister = codeStream.topTarget();
   }
   
   // Handle array indexing
   if (arrayIndex)
   {
      // For an array we need to do the following:
      // - If it's a global variable, grab it to a temp register, setting it as an array
      // - If it's a local variable, create a temp register
      // - Evaluate expression into temp register
      // - Use OP_SETFIELDA on the temp register
      // object(a).slot(b).array(c+1)=value(c)
      
      Compiler::CompilerConstantRef nullRef = codeStream.getConstantsTable()->addNull();
      CodeStream::RegisterTarget getTarget;
      CodeStream::RegisterTarget valueTarget;
      CodeStream::RegisterTarget getArrayTarget; // actually used
      
      valueTarget = codeStream.pushTarget(CodeStream::RegisterTarget());
      codeStream.pushTargetReference(valueTarget);
      expr->compile(codeStream, TypeReqTargetRegister); // everything goes into valueTarget
      getArrayTarget = codeStream.pushTarget(CodeStream::RegisterTarget());
      codeStream.pushTargetReference(getArrayTarget);
      arrayIndex->compile(codeStream, TypeReqTargetRegister);
      
      if (variableRegister == -1)
      {
         // global var
         getTarget = codeStream.pushTarget(CodeStream::RegisterTarget());
         Compiler::CompilerConstantRef varNameIdx = codeStream.getConstantsTable()->addString(varName);
         codeStream.emitOpcodeABCRef(Compiler::OP_LOADVAR, getTarget.regNum, varNameIdx, CodeStream::RegisterTarget(0));
      }
      else
      {
         getTarget = CodeStream::RegisterTarget(variableRegister);
      }
      
      // Now we can get the field, either in the designated register or reuse the getTarget register
      codeStream.emitOpcodeABCRef(Compiler::OP_SETFIELDA, getTarget.regNum, nullRef, valueTarget);
      
      codeStream.popTarget(); // getArrayTarget
      
      if (variableRegister == -1)
      {
         codeStream.popTarget(); // getTarget
      }
      
      if (type == TypeReqNone)
      {
         // Don't need valueTarget anymore
         codeStream.popTarget();
      }
      else if (type == TypeReqTargetRegister)
      {
         // Need to move to right register
         codeStream.emitOpcodeABC(Compiler::OP_MOVE, outRegister.regNum, valueTarget.regNum, 0);
         codeStream.popTarget();
      }
      // else we return a value
   }
   else
   {
      // Normal handler
      // We either store to the target register or the variable register
      CodeStream::RegisterTarget registerTarget = (type == TypeReqTargetRegister) ? outRegister : codeStream.pushTarget(variableRegister);
      codeStream.pushTargetReference(registerTarget);
      expr->compile(codeStream, TypeReqTargetRegister); // everything goes into target
      
      // Assign global
      if (variableRegister == -1)
      {
         // global var
         Compiler::CompilerConstantRef varNameIdx = codeStream.getConstantsTable()->addString(varName);
         codeStream.emitOpcodeABCRef(Compiler::OP_SETVAR, 0, varNameIdx, registerTarget);
      }
      
      if (type == TypeReqNone || type == TypeReqTargetRegister)
      {
         // Don't need outRegister anymore
         codeStream.popTarget();
      }
   }
}

TypeReq AssignExprNode::getPreferredType()
{
    return expr->getPreferredType();
}

//------------------------------------------------------------

static void getAssignOpTypeOp(S32 op, TypeReq &type, U32 &operand)
{
    switch(op)
    {
        case '+':
            type = TypeReqFloat;
            operand = Compiler::OP_ADD;
            break;
        case '-':
            type = TypeReqFloat;
            operand = Compiler::OP_SUB;
            break;
        case '*':
            type = TypeReqFloat;
            operand = Compiler::OP_MUL;
            break;
        case '/':
            type = TypeReqFloat;
            operand = Compiler::OP_DIV;
            break;
        case '%':
            type = TypeReqUInt;
            operand = Compiler::OP_MOD;
            break;
        case '&':
            type = TypeReqUInt;
            operand = Compiler::OP_BITAND;
            break;
        case '^':
            type = TypeReqUInt;
            operand = Compiler::OP_XOR;
            break;
        case '|':
            type = TypeReqUInt;
            operand = Compiler::OP_BITOR;
            break;
        case opSHL:
            type = TypeReqUInt;
            operand = Compiler::OP_SHL;
            break;
        case opSHR:
            type = TypeReqUInt;
            operand = Compiler::OP_SHR;
            break;
    }
}
void AssignOpExprNode::compile(CodeStream &codeStream, TypeReq type)
{
   getAssignOpTypeOp(op, subType, operand);
   AssertFatal(!(type == TypeReqConditional || type == TypeReqFalseConditional), "Unhandled case");
   
   S32 variableRegister = -1;
   subType = expr->getPreferredType();
   if(subType == TypeReqNone)
      subType = type;
   if(subType == TypeReqNone)
   {
      subType = TypeReqVar;
   }
   
   CodeStream::RegisterTarget outRegister;
 
   if (varName[0] != '$')
   {
      variableRegister = codeStream.getLocalVariable(varName);
      
      if (variableRegister == -1)
      {
         AssertFatal(false, "Local variable not registered");
      }
   }
   
   // We either store to a target or emit it.
   if (type == TypeReqTargetRegister)
   {
      outRegister = codeStream.topTarget();
   }
   
   // Handle array indexing
   if (arrayIndex)
   {
      // For an array we need to do the following:
      // - If it's a global variable, grab it to a temp register, setting it as an array
      // - If it's a local variable, create a temp register
      // - Evaluate expression into temp register
      // - Use OP_SETFIELDA on the temp register
      // object(a).slot(b).array(c+1)=value(c)
      
      Compiler::CompilerConstantRef nullRef = codeStream.getConstantsTable()->addNull();
      CodeStream::RegisterTarget getTarget;
      CodeStream::RegisterTarget valueTarget;
      CodeStream::RegisterTarget getArrayTarget; // actually used
      
      valueTarget = codeStream.pushTarget(CodeStream::RegisterTarget());
      getTarget = codeStream.pushTarget(CodeStream::RegisterTarget());
      getArrayTarget = codeStream.pushTarget(CodeStream::RegisterTarget());
      codeStream.pushTargetReference(getArrayTarget);
      arrayIndex->compile(codeStream, TypeReqTargetRegister);
      
      if (variableRegister == -1)
      {
         // global var
         Compiler::CompilerConstantRef varNameIdx = codeStream.getConstantsTable()->addString(varName);
         codeStream.emitOpcodeABCRef(Compiler::OP_LOADVAR, getTarget.regNum, varNameIdx, CodeStream::RegisterTarget(0));
      }
      else
      {
         codeStream.emitOpcodeABC(Compiler::OP_MOVE, getTarget.regNum, variableRegister, 0);
      }
      
      // Stack should now be (value) TARGET ARRAY_INDEX, so grab field
      codeStream.emitOpcodeABCRef(Compiler::OP_GETFIELDA, valueTarget.regNum, getTarget, nullRef);
      
      // Grab the field value. We'll perform a temp calc on the end
      expr->compile(codeStream, TypeReqVar);
      CodeStream::RegisterTarget tmpCalc = codeStream.popTarget();
      codeStream.emitOpcodeABCRef(operand, valueTarget.regNum, valueTarget, tmpCalc);
      
      // Move stack so we're like VALUE ARRAY_INDEX
      tmpCalc = codeStream.pushTarget(CodeStream::RegisterTarget());
      codeStream.emitOpcodeABC(Compiler::OP_MOVE, tmpCalc.regNum, getTarget.regNum, 0); // 13 = tmp
      codeStream.emitOpcodeABC(Compiler::OP_MOVE, getTarget.regNum, getArrayTarget.regNum, 0); // 11 = getarray(12)
      
      // Store result back in field. We'll
      codeStream.emitOpcodeABCRef(Compiler::OP_SETFIELDA, tmpCalc.regNum, nullRef, valueTarget.regNum);
      
      codeStream.popTarget(); // tmpCalc
      codeStream.popTarget(); // getArrayTarget
      codeStream.popTarget(); // getTarget
      
      if (type == TypeReqNone)
      {
         // Don't need valueTarget anymore
         codeStream.popTarget();
      }
      else if (type == TypeReqTargetRegister)
      {
         // Need to move to right register
         codeStream.emitOpcodeABC(Compiler::OP_MOVE, outRegister.regNum, valueTarget.regNum, 0);
         codeStream.popTarget();
      }
      // else we return a value
   }
   else
   {
      // Normal handler
      CodeStream::RegisterTarget getTarget;
      CodeStream::RegisterTarget setTarget;
      Compiler::CompilerConstantRef varNameIdx;
      
      if (variableRegister == -1)
      {
         // global var
         if (type == TypeReqTargetRegister)
         {
            getTarget = setTarget = outRegister;
         }
         else
         {
            getTarget = setTarget = codeStream.pushTarget(CodeStream::RegisterTarget());
         }
         varNameIdx = codeStream.getConstantsTable()->addString(varName);
         codeStream.emitOpcodeABCRef(Compiler::OP_LOADVAR, getTarget.regNum, varNameIdx, CodeStream::RegisterTarget(0));
      }
      else
      {
         getTarget = setTarget = CodeStream::RegisterTarget(variableRegister);
      }
      
      // Perform calc
      expr->compile(codeStream, TypeReqVar);
      CodeStream::RegisterTarget tmpCalc = codeStream.popTarget();
      codeStream.emitOpcodeABCRef(operand, setTarget.regNum, getTarget, tmpCalc);
      
      // Assign global
      if (variableRegister == -1)
      {
         // global var
         codeStream.emitOpcodeABCRef(Compiler::OP_SETVAR, 0, varNameIdx, setTarget);
         
         if (type == TypeReqNone)
         {
            codeStream.popTarget(); // temp reg no longer needed
         }
      }
      else
      {
         if (type == TypeReqTargetRegister)
         {
            // Need to copy to the target
            codeStream.emitOpcodeABC(Compiler::OP_MOVE, outRegister.regNum, setTarget.regNum, 0);
         }
      }
   }
}

TypeReq AssignOpExprNode::getPreferredType()
{
    getAssignOpTypeOp(op, subType, operand);
    return subType;
}

//------------------------------------------------------------

void FuncCallExprNode::compile(CodeStream &codeStream, TypeReq type)
{
    // How this works:
    // - lookup function with OP_GETFUNC
    // - OP_CALL(funcStart, params) -> sets returnStart to result
   
   // [abc] [a := a(a[...b params])[...c return params]]

#ifdef DEBUG_COMPILER
   Con::printf("func[%s]", funcName);
#endif
   
   CodeStream::RegisterTarget targetRegister;
   if (type == TypeReqTargetRegister)
   {
      targetRegister = codeStream.popTarget();
#ifdef DEBUG_COMPILER
      Con::printf("FuncCallExprNode direct target == %s", targetRegister.toString());
#endif
   }
   
   ExprNode *funcArgs = args;
   
   // First get function
   {
      CodeStream::RegisterTarget nsRegister = codeStream.pushTarget(CodeStream::RegisterTarget());
      CodeStream::RegisterTarget namespaceTarget;
      Compiler::CompilerConstantRef funcIdx = codeStream.getConstantsTable()->addString(funcName);
      
      if (callType == ParentCall)
      {
         namespaceTarget = codeStream.getConstantsTable()->addString("$parent");
      }
      else if (callType == MethodCall)
      {
         args->compile(codeStream, TypeReqVar);
         namespaceTarget = codeStream.popTarget();
         funcArgs = static_cast<ExprNode*>(args);
      }
      else
      {
         namespaceTarget = nameSpace == NULL ? codeStream.getConstantsTable()->addNamespace(StringTable->EmptyString) : codeStream.getConstantsTable()->addNamespace(nameSpace);
      }
      
      // Grab first param: ns entry
      codeStream.emitOpcodeABCRef(Compiler::OP_GETFUNC, nsRegister.regNum, namespaceTarget, funcIdx);
      
      // Add on extra params
      
      U32 numArgs = 0;
      for(ExprNode *walk = funcArgs; walk; walk = (ExprNode *) walk->getNext())
      {
         walk->compile(codeStream, TypeReqVar);
         
         CodeStream::RegisterTarget funcArg = codeStream.popTarget();
         
         if (funcArg.constRef.idx >= 0)
         {
            // Need to move const -> reg
            CodeStream::RegisterTarget regArg = codeStream.pushTarget(CodeStream::RegisterTarget());
            codeStream.emitOpcodeABx(Compiler::OP_LOADK, regArg.regNum, codeStream.emitKonstRef(funcArg.constRef));
         }
         else if (funcArg.regNum >= 0)
         {
            // We need to move the register to the preallocated register
            CodeStream::RegisterTarget regArg = codeStream.pushTarget(CodeStream::RegisterTarget());
            if (funcArg.regNum != regArg.regNum)
            {
               codeStream.emitOpcodeABC(Compiler::OP_MOVE, regArg.regNum, funcArg.regNum, 0);
               
            }
         }
         
         numArgs++;
      }
      
      // Reset target stack
      for(ExprNode *walk = funcArgs; walk; walk = (ExprNode *) walk->getNext())
      {
         codeStream.popTarget();
      }
      
      codeStream.emitOpcodeABC(Compiler::OP_CALL, nsRegister.regNum, numArgs, type == TypeReqNone ? 0 : 1);
      
      if (type == TypeReqNone)
      {
         codeStream.popTarget();
      }
      else if (type == TypeReqTargetRegister)
      {
         codeStream.emitOpcodeABC(Compiler::OP_MOVE, targetRegister.regNum, nsRegister.regNum, 0);
         codeStream.popTarget();
      }
      else if (type == TypeReqConditional || type == TypeReqFalseConditional)
      {
         int cmpValue = 1;
         if (type == TypeReqFalseConditional)
         {
            cmpValue = 0;
            type = TypeReqConditional;
         }
         
         Compiler::CompilerConstantRef trueConst = codeStream.getConstantsTable()->addInt(0);
         codeStream.emitOpcodeABCRef(Compiler::OP_EQ, cmpValue, codeStream.topTarget(), trueConst);

         codeStream.popTarget();
      }
   }
}

TypeReq FuncCallExprNode::getPreferredType()
{
    return TypeReqString;
}


//------------------------------------------------------------

void AssertCallExprNode::compile( CodeStream &codeStream, TypeReq type )
{
    CompilerConstantRef messageConstant = codeStream.getConstantsTable()->addString( message, true, false );
    
    testExpr->compile( codeStream, TypeReqUInt );
    CodeStream::RegisterTarget targetExpr = codeStream.popTarget();
   
    codeStream.emitOpcodeABCRef(Compiler::OP_ASSERT, targetExpr.regNum, messageConstant, messageConstant);
}

TypeReq AssertCallExprNode::getPreferredType()
{
    return TypeReqNone;
}

//------------------------------------------------------------

void SlotAccessNode::compile(CodeStream &codeStream, TypeReq type)
{
    if(type == TypeReqNone)
        return;
   
   CodeStream::RegisterTarget objectTarget;
   CodeStream::RegisterTarget arrayTarget;
   CodeStream::RegisterTarget targetRegister;
   
   if (type == TypeReqTargetRegister)
   {
      targetRegister = codeStream.topTarget();
#ifdef DEBUG_COMPILER
      Con::printf("SlotAcessNode direct target == %s", targetRegister.toString());
#endif
   }
   else
   {
      targetRegister = codeStream.pushTarget(CodeStream::RegisterTarget());
   }
   
   /*
    OBJ_GETFIELD
    a = object(b).slot(c)
    OBJ_SETFIELDA
    a = object(b).slot(c).array(b+1)
    */
   
   objectTarget = codeStream.pushTarget(CodeStream::RegisterTarget());
   codeStream.pushTargetReference(objectTarget);
   objectExpr->compile(codeStream, TypeReqTargetRegister);
   if (arrayExpr) {
      arrayTarget = codeStream.pushTarget(CodeStream::RegisterTarget());
      codeStream.pushTargetReference(arrayTarget);
      arrayExpr->compile(codeStream, TypeReqTargetRegister);
   }
   
   // Grab results from registers
   if (arrayExpr) arrayTarget = codeStream.popTarget();
   objectTarget = codeStream.popTarget();
   CompilerConstantRef slotNameConst = codeStream.getConstantsTable()->addString(slotName);
   
   if (arrayExpr)
   {
      codeStream.emitOpcodeABCRef(Compiler::OP_GETFIELDA, targetRegister.regNum, objectTarget, slotNameConst);
   }
   else
   {
      codeStream.emitOpcodeABCRef(Compiler::OP_GETFIELD, targetRegister.regNum, objectTarget, slotNameConst);
   }
   
   int cmpValue = 1;
   if (type == TypeReqFalseConditional)
   {
      cmpValue = 0;
      type = TypeReqConditional;
   }
   
   if (type == TypeReqConditional)
   {
      Compiler::CompilerConstantRef trueConst = codeStream.getConstantsTable()->addInt(0);
      codeStream.emitOpcodeABCRef(Compiler::OP_EQ, cmpValue, targetRegister, CodeStream::RegisterTarget(trueConst));
      codeStream.popTarget();
   }
   else if (type == TypeReqTargetRegister || type == TypeReqNone)
   {
      codeStream.popTarget();
   }
}

TypeReq SlotAccessNode::getPreferredType()
{
    return TypeReqNone;
}

//-----------------------------------------------------------------------------

void InternalSlotAccessNode::compile(CodeStream &codeStream, TypeReq type)
{
    if(type == TypeReqNone)
       return;
   
   AssertFatal(type != TypeReqConditional && type != TypeReqFalseConditional, "Unsupported conditional types");
   
    CodeStream::RegisterTarget objectTarget;
    CodeStream::RegisterTarget slotTarget;
   
   CodeStream::RegisterTarget targetRegister;
   if (type == TypeReqTargetRegister)
   {
      targetRegister = codeStream.popTarget();
#ifdef DEBUG_COMPILER
      Con::printf("InternalSlotAccessNode direct target == %s", targetRegister.toString());
#endif
   }
   else
   {
      targetRegister = codeStream.pushTarget(CodeStream::RegisterTarget());
   }
   
    // grabs slotExpr from objectExpr
    objectExpr->compile(codeStream, TypeReqString);
    slotExpr->compile(codeStream, TypeReqString);
   
    slotTarget = codeStream.popTarget();
    objectTarget = codeStream.popTarget();
   
    codeStream.emitOpcodeABCRef(recurse ? Compiler::OP_GETINTERNAL_N : Compiler::OP_GETINTERNAL, targetRegister.regNum, objectTarget, slotTarget);
}

TypeReq InternalSlotAccessNode::getPreferredType()
{
    return TypeReqUInt;
}

//-----------------------------------------------------------------------------

void SlotAssignNode::compile(CodeStream &codeStream, TypeReq type)
{
   CodeStream::RegisterTarget objectTarget;
   CodeStream::RegisterTarget arrayTarget;
   CodeStream::RegisterTarget valueTarget;
   CodeStream::RegisterTarget targetRegister;
   AssertFatal(!(type == TypeReqConditional || type == TypeReqFalseConditional), "Unhandled case");
   
   //
   if (type != TypeReqTargetRegister)
   {
      AssertFatal(objectExpr != NULL, "No object specified for target");
   }
   else
   {
      targetRegister = codeStream.topTarget();
   }
   
   if (objectExpr == NULL) objectTarget = codeStream.topTarget(); // if objectExpr is NULL
   
   /*
    
    OBJ_SETFIELD
    object(a).slot(b) = value(c)
    OBJ_SETFIELDA
    object(a).slot(b).array(c+1)=value(c)
    
    */
   CompilerConstantRef slotNameConst = codeStream.getConstantsTable()->addString(slotName);
   
   if (arrayExpr)
   {
      CodeStream::RegisterTarget tempReg = codeStream.pushTarget(CodeStream::RegisterTarget());
      codeStream.pushTargetReference(tempReg);
      valueExpr->compile(codeStream, TypeReqTargetRegister); // slot value
      valueTarget = codeStream.topTarget();
      
      arrayTarget = codeStream.pushTarget(CodeStream::RegisterTarget());
      codeStream.pushTargetReference(arrayTarget);
      arrayExpr->compile(codeStream, TypeReqTargetRegister); // []
      
      // We need the object after because value needs to be the lowest reg
      if (objectExpr)
      {
         objectTarget = codeStream.pushTarget(CodeStream::RegisterTarget());
         objectExpr->compile(codeStream, TypeReqTargetRegister); // obj prefix
      }
      
      codeStream.emitOpcodeABCRef(Compiler::OP_SETFIELDA, objectTarget.regNum, slotNameConst, valueTarget);
      codeStream.popTarget(); // array
   }
   else
   {
      valueExpr->compile(codeStream, TypeReqVar); // slot value
      valueTarget = codeStream.topTarget();
      
      if (objectExpr)
      {
         objectTarget = codeStream.pushTarget(CodeStream::RegisterTarget());
         objectExpr->compile(codeStream, TypeReqTargetRegister); // obj prefix
      }
      
      codeStream.emitOpcodeABCRef(Compiler::OP_SETFIELD, objectTarget.regNum, slotNameConst, valueTarget);
   }
   
   if (objectExpr)
   {
      if (type == TypeReqNone)
      {
         codeStream.popTarget(); // value
      }
      else if (type == TypeReqTargetRegister)
      {
         // Need to move to right register
         codeStream.emitOpcodeABC(Compiler::OP_MOVE, targetRegister.regNum, valueTarget.regNum, 0);
         codeStream.popTarget(); // value
         codeStream.popTarget(); // target
      }
      // else we'll return a value
   }
   else
   {
      AssertFatal(type == TypeReqTargetRegister, "Invalid state");
      codeStream.popTarget(); // value
      codeStream.popTarget(); // object
   }
}

TypeReq SlotAssignNode::getPreferredType()
{
    return TypeReqString;
}

//------------------------------------------------------------

void SlotAssignOpNode::compile(CodeStream &codeStream, TypeReq type)
{
    getAssignOpTypeOp(op, subType, operand);
   
   int cmpValue = 1;
   
   if (type == TypeReqFalseConditional)
   {
      type = TypeReqConditional;
      cmpValue = 0;
   }
   
   CodeStream::RegisterTarget objectTarget;
   CodeStream::RegisterTarget arrayTarget;
   CodeStream::RegisterTarget valueTarget;
   
   AssertFatal(objectExpr != NULL, "No object specified for target");
   
   /*
    
    OBJ_SETFIELD
    object(a).slot(b) = value(c)
    OBJ_SETFIELDA
    object(a).slot(b).array(c+1)=value(c)
    
    OBJ_GETFIELD
    a := object(b).slot(c)
    OBJ_GETFIELDA
    a := object(b).slot(c).array(b+1)
    
    */
   CompilerConstantRef slotNameConst = codeStream.getConstantsTable()->addString(slotName);
   
   // Allocate register to store result and value
   if (type == TypeReqTargetRegister)
   {
      valueTarget = codeStream.topTarget();
   }
   else
   {
      valueTarget = codeStream.pushTarget(CodeStream::RegisterTarget());
   }
   
   codeStream.pushTargetReference(valueTarget);
   valueExpr->compile(codeStream, TypeReqTargetRegister); // slot value
   
   if (arrayExpr)
   {
      // Need to set to OBJECT ARRAY for OBJ_GETFIELDA
      if (objectExpr)
      {
         objectTarget = codeStream.pushTarget(CodeStream::RegisterTarget());
         codeStream.pushTargetReference(objectTarget);
         objectExpr->compile(codeStream, TypeReqTargetRegister); // obj prefix
      }
      
      arrayTarget = codeStream.pushTarget(CodeStream::RegisterTarget());
      codeStream.pushTargetReference(arrayTarget);
      arrayExpr->compile(codeStream, TypeReqTargetRegister); // []
      
      // Grab field (OBJECT ARRAY)
      codeStream.emitOpcodeABCRef(Compiler::OP_GETFIELDA, valueTarget.regNum, objectTarget, slotNameConst);
      
      // Move array to the end
      CodeStream::RegisterTarget tempTarget = codeStream.pushTarget(CodeStream::RegisterTarget());
      codeStream.emitOpcodeABC(Compiler::OP_MOVE, tempTarget.regNum, arrayTarget.regNum, 0);
      
      // Now we need to perform the operation on it (stored in arrayTarget which is now the value)
      codeStream.emitOpcodeABCRef(operand, arrayTarget.regNum, tempTarget.regNum, valueTarget.regNum);
      
      // Now we can to set the result (VALUE ARRAY)
      codeStream.emitOpcodeABCRef(Compiler::OP_SETFIELDA, objectTarget.regNum, slotNameConst, arrayTarget);
      
      // If we need to keep the value, store it in the target
      if (type != TypeReqNone)
      {
         codeStream.emitOpcodeABC(Compiler::OP_MOVE, valueTarget.regNum, arrayTarget.regNum, 0);
      }
      
      codeStream.popTarget(); // temp
      codeStream.popTarget(); // array
      codeStream.popTarget(); // object
   }
   else
   {
      if (objectExpr)
      {
         objectTarget = codeStream.pushTarget(CodeStream::RegisterTarget());
         codeStream.pushTargetReference(objectTarget);
         objectExpr->compile(codeStream, TypeReqTargetRegister); // obj prefix
      }
      
      CodeStream::RegisterTarget tempTarget = codeStream.pushTarget(CodeStream::RegisterTarget());
      
      // Grab field
      codeStream.emitOpcodeABCRef(Compiler::OP_GETFIELD, tempTarget.regNum, objectTarget, slotNameConst);
      // Perform op
      codeStream.emitOpcodeABCRef(operand, valueTarget.regNum, tempTarget.regNum, valueTarget.regNum);
      // Save in field
      codeStream.emitOpcodeABCRef(Compiler::OP_SETFIELD, objectTarget.regNum, slotNameConst, valueTarget);
      
      codeStream.popTarget(); // temp
      codeStream.popTarget(); // object
   }
   
   // Pop target if required
   if (type == TypeReqTargetRegister || type == TypeReqNone)
   {
      codeStream.popTarget();
   }
   else if (type == TypeReqConditional)
   {
      Compiler::CompilerConstantRef trueConst = codeStream.getConstantsTable()->addInt(0);
      codeStream.emitOpcodeABCRef(Compiler::OP_EQ, cmpValue, valueTarget, CodeStream::RegisterTarget(trueConst));
      codeStream.popTarget();
   }
}

TypeReq SlotAssignOpNode::getPreferredType()
{
    getAssignOpTypeOp(op, subType, operand);
    return subType;
}

//------------------------------------------------------------

void ObjectDeclNode::compileSubObject(CodeStream &codeStream, U32 targetRegisterId, S32 parentObjectId)
{
   SET_VERIFY_TARGET_REG_SIZE()
   
   // target is either the destination object or the destination register
   CodeStream::RegisterTarget targetRegister = CodeStream::RegisterTarget(targetRegisterId);
   CodeStream::RegisterTarget parentRegister = CodeStream::RegisterTarget(parentObjectId);
   
   if (parentObjectId < 0)
   {
      parentRegister = codeStream.getConstantsTable()->addNull();
   }
   
   // Get constructor args
   CodeStream::RegisterTarget classNameTarget = codeStream.pushTarget(CodeStream::RegisterTarget());
   CodeStream::RegisterTarget objectNameTarget = codeStream.pushTarget(CodeStream::RegisterTarget());
   U32 argCount = 0;
   
   codeStream.pushTargetReference(classNameTarget);
   classNameExpr->compile(codeStream, TypeReqTargetRegister);
   
   codeStream.pushTargetReference(objectNameTarget);
   objectNameExpr->compile(codeStream, TypeReqTargetRegister);
   
   for(ExprNode *exprWalk = argList; exprWalk; exprWalk = (ExprNode *) exprWalk->getNext())
   {
      CodeStream::RegisterTarget arg = codeStream.pushTarget(CodeStream::RegisterTarget());
      codeStream.pushTargetReference(arg);
      exprWalk->compile(codeStream, TypeReqTargetRegister);
      argCount++;
   }
   
   U32 failJmpIp;
   U32 flags = 0;
   if (isSingleton) flags |= Compiler::CREATEOBJECT_ISSINGLETON;
   if (isDatablock) flags |= Compiler::CREATEOBJECT_ISDATABLOCK;
   if (isClassNameInternal) flags |= Compiler::CREATEOBJECT_ISINTERNAL;
   U32 flagArgs = argCount | (flags << 8);
   
   codeStream.emitOpcodeABC(Compiler::OP_CREATE_OBJECT, targetRegister.regNum, classNameTarget.regNum, flagArgs);
   failJmpIp = codeStream.emitOpcodeABx(Compiler::OP_JMP, 0, 0);
   
   // pop constructor args so we can reuse registers
   for (U32 i=0; i<argCount; i++)
   {
      codeStream.popTarget();
   }
   codeStream.popTarget(); // object name
   codeStream.popTarget(); // class name
   
   // Copy parent fields
   if (parentObject && parentObject != StringTable->EmptyString)
   {
      CompilerConstantRef parentRefKonst = codeStream.getConstantsTable()->addString(parentObject);
      codeStream.emitOpcodeABCRef(Compiler::OP_COPYFIELDS, targetRegister.regNum, parentRefKonst, CodeStream::RegisterTarget(0));
   }
   
   // Now emit all the slot assignments
   for(SlotAssignNode *slotWalk = slotDecls; slotWalk; slotWalk = (SlotAssignNode *) slotWalk->getNext())
   {
      codeStream.pushTargetReference(targetRegister);
      slotWalk->compile(codeStream, TypeReqTargetRegister);
   }
   
   // Now we can register the object and add it to the right location
   // registerObject(A), B.addObject(A), [flags(c)]
   codeStream.emitOpcodeABC(Compiler::OP_FINISH_OBJECT, targetRegister.regNum, codeStream.emitTargetRef(parentRegister), 0);
   
   // Emit sub objects
   for(ObjectDeclNode *objectWalk = subObjects; objectWalk; objectWalk = (ObjectDeclNode *) objectWalk->getNext())
   {
      CodeStream::RegisterTarget subObjectRegister = codeStream.pushTarget(CodeStream::RegisterTarget());
      objectWalk->compileSubObject(codeStream, subObjectRegister.regNum, targetRegisterId);
      codeStream.popTarget();
   }
   
   // Patch fail point
   codeStream.patch(failJmpIp, TS2_OP_ENC_A_Bx(Compiler::OP_JMP, 0, TS2_OP_MAKE_sBX(codeStream.tell() - failJmpIp - 1)));
   
   VERIFY_TARGET_REG_SIZE(0)
   
   // At the end we should just have a new object reference
}

void ObjectDeclNode::compile(CodeStream &codeStream, TypeReq type)
{
    AssertFatal(type != TypeReqConditional && type != TypeReqFalseConditional, "Unsupported conditional types");
   
    CodeStream::RegisterTarget objectTarget;
    if (type != TypeReqTargetRegister)
    {
       objectTarget = codeStream.pushTarget(CodeStream::RegisterTarget());
    }
    else
    {
       objectTarget = codeStream.topTarget();
    }
   
    compileSubObject(codeStream, objectTarget.regNum, -1);
   
    if (type == TypeReqTargetRegister || type == TypeReqNone)
    {
       codeStream.popTarget();
    }
}

TypeReq ObjectDeclNode::getPreferredType()
{
    return TypeReqUInt;
}

//------------------------------------------------------------

void FunctionDeclStmtNode::compileStmt(CodeStream &codeStream)
{
    // Defined with
    // OP_BINDNSFUNC <ns> <pkg> <name> <entry idx>
   CodeBlockFunction* func = new CodeBlockFunction;
   func->name = fnName;
   CodeBlock::smCurrentCodeBlock->mFunctions.push_back(func);
   func->stmt = this;
   
   // Add a declare instruction so things are linked in at the right time
   
   // Emit code to assign the function to the namespace
   
   
   Compiler::CompilerConstantRef nsIdx = nameSpace == NULL ? codeStream.getConstantsTable()->addNamespace(StringTable->EmptyString) : codeStream.getConstantsTable()->addNamespace(nameSpace);
   Compiler::CompilerConstantRef pkgIdx = package == NULL ? codeStream.getConstantsTable()->addNull() : codeStream.getConstantsTable()->addString(package);
   Compiler::CompilerConstantRef funcIdx = codeStream.getConstantsTable()->addString(fnName);
   
   CodeStream::RegisterTarget r1 = codeStream.pushTarget(CodeStream::RegisterTarget());
   CodeStream::RegisterTarget r2 = codeStream.pushTarget(CodeStream::RegisterTarget());
   CodeStream::RegisterTarget r3 = codeStream.pushTarget(CodeStream::RegisterTarget());
   
   codeStream.emitOpcodeABx(Compiler::OP_LOADK, r1.regNum, codeStream.emitKonstRef(pkgIdx));
   codeStream.emitOpcodeABx(Compiler::OP_LOADK, r2.regNum, codeStream.emitKonstRef(nsIdx));
   codeStream.emitOpcodeABx(Compiler::OP_LOADK, r3.regNum, codeStream.emitKonstRef(funcIdx));
   
   codeStream.emitOpcodeABC(Compiler::OP_BINDNSFUNC, r1.regNum, CodeBlock::smCurrentCodeBlock->mFunctions.size()-1, 0);
   
   codeStream.popTarget();
   codeStream.popTarget();
   codeStream.popTarget();
   
   // assign the doc if there is one
   if (doc)
   {
      // NOTE: we only emit function docs here
      if (!doc->isClassDoc())
      {
         Compiler::CompilerConstantRef docIdx = codeStream.getConstantsTable()->addString(doc->doc);
         codeStream.emitOpcodeABCRef(Compiler::OP_GETFUNC, r1.regNum, nsIdx, funcIdx);
         codeStream.emitOpcodeABCRef(Compiler::OP_DOCNSFUNC, 0, r1, docIdx);
      }
   }
}

void FunctionDeclStmtNode::compileFunction(CodeStream& codeStream)
{
   // Begin code
   codeStream.pushFunctionState();
	
   CodeBlock::smCurrentFunction->numArgs = 0;
   
   // Count args
   for(VarNode *walk = args; walk; walk = (VarNode *)((StmtNode*)walk)->getNext())
   {
      CodeBlock::smCurrentFunction->numArgs++;
   }
   
   // Add actual local variables
   Vector<StringTableEntry> walkVarList;
   for (ReferencedVariableNode* walk = localVars; walk; walk = walk->next)
   {
      walkVarList.push_back(walk->varName);
      //codeStream.addLocalVariable(walk->varName);
   }
   
   for (S32 i=walkVarList.size()-1; i>=0; i--)
   {
      codeStream.addLocalVariable(walkVarList[i]);
   }
   
   CodeBlock::smInFunction = true;
   compileBlock(stmts, codeStream);
   
   // Add break so breakpoint can be set at closing brace or
   // in empty function.
   addBreakLine(codeStream);
   
   CodeBlock::smInFunction = false;
   codeStream.emitOpcodeABC(Compiler::OP_RETURN, 0, 0, 0);
   
   CodeBlock::smCurrentFunction->maxStack = codeStream.getMaxStack();
   
   for (U32 i=codeStream.mCurrentFunctionState.startVar; i<codeStream.mVarInfo.size(); i++)
   {
      CodeBlockFunction::Symbol sym;
      sym.registerIdx = codeStream.mVarInfo[i].index;
      if (codeStream.mVarInfo[i].name)
      {
         sym.varName = codeStream.mVarInfo[i].name;
      }
      else
      {
         sym.varName = NULL;
         continue;
      }
      CodeBlock::smCurrentFunction->vars.push_back(sym);
   }
   
   CodeBlock::smCurrentFunction = CodeBlock::smCurrentCodeblockFunction;
   codeStream.popFunctionState();
}

void FunctionDeclStmtNode::referenceLocalVariable(StringTableEntry name)
{
   // Don't use locals for global vars
   if (name && name[0] == '$')
      return;
   
   //Con::printf("  referenceLocalVariable(%s)", name);
   
   // Check if it already exists
   for (ReferencedVariableNode* walk = gCurrentLocalVariables; walk; walk = walk->next)
   {
      if (walk->varName == name)
         return;
   }
   
   ReferencedVariableNode* node = (ReferencedVariableNode*)consoleAlloc(sizeof(ReferencedVariableNode));
   node->next = NULL;
   node->varName = name;
   
   if (!gCurrentLocalVariables)
   {
      gCurrentLocalVariables = node;
   }
   else
   {
      node->next = gCurrentLocalVariables;
      gCurrentLocalVariables = node;
   }
}

void FunctionDeclStmtNode::pushVariableScope()
{
#ifdef DEBUG_COMPILER
   Con::printf("pushVariableScope");
#endif
   
   if (gLocalVariableStackIdx >= 15)
   {
      Con::errorf("Too many nested local variable scopes!");
      gLocalVariableStackIdx = 14;
   }
   gLocalVariableStack[gLocalVariableStackIdx++] = gCurrentLocalVariables;
   gCurrentLocalVariables = NULL;
}

void FunctionDeclStmtNode::popVariableScope()
{
#ifdef DEBUG_COMPILER
   Con::printf("popVariableScope");
#endif
   if (gLocalVariableStackIdx == 0)
   {
      Con::errorf("Local variable scope stack underflow");
      gLocalVariableStackIdx = 0;
   }
   else
   {
      gLocalVariableStackIdx--;
   }
   gCurrentLocalVariables = gLocalVariableStack[gLocalVariableStackIdx];
}

