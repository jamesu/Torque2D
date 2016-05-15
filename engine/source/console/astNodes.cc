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

#include "console/ast.h"
#include "collection/findIterator.h"
#include "io/resource/resourceManager.h"

#include "string/findMatch.h"
#include "console/consoleInternal.h"
#include "io/fileStream.h"
#include "console/compiler.h"

#include "sim/simBase.h"

template< typename T >
struct Token
{
    T value;
    S32 lineNumber;
};
#include "console/cmdgram.h"


bool gNLAfterText = false;

namespace Compiler
{
    U32 compileBlock(StmtNode *block, CodeStream &codeStream, U32 ip)
    {
        for(StmtNode *walk = block; walk; walk = walk->getNext())
		  {
            ip = walk->compileStmt(codeStream, ip);
			   if (gNLAfterText) codeStream.emitText("\n");
		  }
        return codeStream.tell();
    }
}

using namespace Compiler;

//-----------------------------------------------------------------------------

void StmtNode::addBreakLine(CodeStream &code)
{
    code.addBreakLine(dbgLineNumber, code.tell());
}

//------------------------------------------------------------

StmtNode::StmtNode()
{
    next = NULL;
    dbgFileName = CodeBlock::smCurrentParser->getCurrentFile();
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

static U32 conversionOp(TypeReq src, TypeReq dst)
{
    if(src == TypeReqString)
    {
        switch(dst)
        {
            case TypeReqUInt:
                return OP_STR_TO_UINT;
            case TypeReqFloat:
                return OP_STR_TO_FLT;
            case TypeReqNone:
                return OP_STR_TO_NONE;
            case TypeReqVar:
                return OP_SAVEVAR_STR;
            default:
                break;
        }
    }
    else if(src == TypeReqFloat)
    {
        switch(dst)
        {
            case TypeReqUInt:
                return OP_FLT_TO_UINT;
            case TypeReqString:
                return OP_FLT_TO_STR;
            case TypeReqNone:
                return OP_FLT_TO_NONE;
            case TypeReqVar:
                return OP_SAVEVAR_FLT;
            default:
                break;
        }
    }
    else if(src == TypeReqUInt)
    {
        switch(dst)
        {
            case TypeReqFloat:
                return OP_UINT_TO_FLT;
            case TypeReqString:
                return OP_UINT_TO_STR;
            case TypeReqNone:
                return OP_UINT_TO_NONE;
            case TypeReqVar:
                return OP_SAVEVAR_UINT;
            default:
                break;
        }
    }
    else if(src == TypeReqVar)
    {
        switch(dst)
        {
            case TypeReqUInt:
                return OP_LOADVAR_UINT;
            case TypeReqFloat:
                return OP_LOADVAR_FLT;
            case TypeReqString:
                return OP_LOADVAR_STR;
            case TypeReqNone:
                return OP_COPYVAR_TO_NONE;
            default:
                break;
        }
    }
    return OP_INVALID;
}

//------------------------------------------------------------

U32 BreakStmtNode::compileStmt(CodeStream &codeStream, U32 ip)
{
    if(codeStream.inLoop())
    {
		 codeStream.emitText("break");
    }
    else
    {
        Con::warnf(ConsoleLogEntry::General, "%s (%d): break outside of loop... ignoring.", dbgFileName, dbgLineNumber);
    }
    return codeStream.tell();
}

//------------------------------------------------------------

U32 ContinueStmtNode::compileStmt(CodeStream &codeStream, U32 ip)
{
    if(codeStream.inLoop())
    {
		 Con::warnf("Continue not implemented");
		 codeStream.emitText("continue");
    }
    else
    {
        Con::warnf(ConsoleLogEntry::General, "%s (%d): continue outside of loop... ignoring.", dbgFileName, dbgLineNumber);
    }
    return codeStream.tell();
}

//------------------------------------------------------------

U32 ExprNode::compileStmt(CodeStream &codeStream, U32 ip)
{
    addBreakLine(codeStream);
    return compile(codeStream, ip, TypeReqNone);
}

//------------------------------------------------------------

U32 ReturnStmtNode::compileStmt(CodeStream &codeStream, U32 ip)
{
	addBreakLine(codeStream);
	
	codeStream.emitText("return ");
	TypeReq walkType = expr->getPreferredType();
	/*if (walkType == TypeReqNone)*/ walkType = TypeReqString;
	ip = expr->compile(codeStream, ip, walkType);
	
	return codeStream.tell();
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

U32 IfStmtNode::compileStmt(CodeStream &codeStream, U32 ip)
{
    U32 start = ip;
    U32 endifIp, elseIp;
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
	
	 codeStream.emitText("if (");
	 ip = testExpr->compile(codeStream, ip, TypeReqConditional);
	 codeStream.emitText(") then\n");
	 endifOffset = compileBlock(ifBlock, codeStream, ip);
	
	 if (elseBlock)
	 {
		 codeStream.emitText("else\n");
		 endifOffset = compileBlock(elseBlock, codeStream, ip);
	 }
	
	 codeStream.emitText("end");
    return codeStream.tell();
}

//------------------------------------------------------------

U32 LoopStmtNode::compileStmt(CodeStream &codeStream, U32 ip)
{
    if(testExpr->getPreferredType() == TypeReqUInt)
    {
        integer = true;
    }
    else
    {
        integer = false;
    }
    
    // if it's a for loop or a while loop it goes:
    // initExpr
    // testExpr
    // OP_JMPIFNOT to break point
    // loopStartPoint:
    // loopBlock
    // continuePoint:
    // endLoopExpr
    // testExpr
    // OP_JMPIF loopStartPoint
    // breakPoint:
    
    // otherwise if it's a do ... while() it goes:
    // initExpr
    // loopStartPoint:
    // loopBlock
    // continuePoint:
    // endLoopExpr
    // testExpr
    // OP_JMPIF loopStartPoint
    // breakPoint:
    
    // loopBlockStart == start of loop block
    // continue == skip to end
    // break == exit loop
    
    
    addBreakLine(codeStream);
    codeStream.pushFixScope(true);
	
    U32 start = ip;
    if(initExpr)
        ip = initExpr->compile(codeStream, ip, TypeReqNone);
	
	 codeStream.emitText("while true do\n");
	
    if(!isDoLoop)
    {
		  codeStream.emitText("if (");
        ip = testExpr->compile(codeStream, ip, TypeReqFalseConditional);
		  codeStream.emitText(") then break end\n");
    }
    
    // Compile internals of loop.
	loopBlockStartOffset = codeStream.tell();
	gNLAfterText=true;
    continueOffset = compileBlock(loopBlock, codeStream, ip);
	
    if(endLoopExpr)
        ip = endLoopExpr->compile(codeStream, ip, TypeReqNone);
	
    codeStream.popFixScope();
	
	 codeStream.emitText("\nend");
	
    return codeStream.tell();
}

//------------------------------------------------------------

U32 IterStmtNode::compileStmt( CodeStream &codeStream, U32 ip )
{
    // Instruction sequence:
    //
    //   containerExpr
    //   OP_ITER_BEGIN varName .fail
    // .continue:
    //   OP_ITER .break
    //   body
    //   OP_JMP .continue
    // .break:
    //   OP_ITER_END
    // .fail:
    
    addBreakLine(codeStream);
    
    codeStream.pushFixScope(true);
    
    const U32 startIp = ip;
    containerExpr->compile( codeStream, startIp, TypeReqString );
    
    codeStream.emit(isStringIter ? OP_ITER_BEGIN_STR : OP_ITER_BEGIN);
    codeStream.emitSTE( varName );
    const U32 finalFix = codeStream.emit(0);
    const U32 continueIp = codeStream.emit(OP_ITER);
    codeStream.emitFix(CodeStream::FIXTYPE_BREAK);
    const U32 bodyIp = codeStream.tell();
	
	gNLAfterText=true;
    const U32 jmpIp = compileBlock( body, codeStream, bodyIp);
    const U32 breakIp = jmpIp + 2;
    const U32 finalIp = breakIp + 1;
    
    codeStream.emit(OP_JMP);
    codeStream.emitFix(CodeStream::FIXTYPE_CONTINUE);
    codeStream.emit(OP_ITER_END);
    
    codeStream.patch(finalFix, finalIp);
    codeStream.fixLoop(bodyIp, breakIp, continueIp);
    codeStream.popFixScope();
    
    return codeStream.tell();
}

//------------------------------------------------------------

U32 ConditionalExprNode::compile(CodeStream &codeStream, U32 ip, TypeReq type)
{
    // code is testExpr
    // JMPIFNOT falseStart
    // trueExpr
    // JMP end
    // falseExpr
    if(testExpr->getPreferredType() == TypeReqUInt)
    {
        integer = true;
    }
    else
    {
        integer = false;
    }
    
    ip = testExpr->compile(codeStream, ip, integer ? TypeReqUInt : TypeReqFloat);
    codeStream.emit(integer ? OP_JMPIFNOT : OP_JMPIFFNOT);
    
    U32 jumpElseIp = codeStream.emit(0);
    ip = trueExpr->compile(codeStream, ip, type);
    codeStream.emit(OP_JMP);
    U32 jumpEndIp = codeStream.emit(0);
    codeStream.patch(jumpElseIp, codeStream.tell());
    ip = falseExpr->compile(codeStream, ip, type);
    codeStream.patch(jumpEndIp, codeStream.tell());
    
    return codeStream.tell();
}

TypeReq ConditionalExprNode::getPreferredType()
{
    return trueExpr->getPreferredType();
}

//------------------------------------------------------------

U32 FloatBinaryExprNode::compile(CodeStream &codeStream, U32 ip, TypeReq type)
{
	 char buf[64];
	 dSprintf(buf, 64, ") %c (", op);
	
	 codeStream.emitText("(");
	 ip = right->compile(codeStream, ip, TypeReqFloat);
	 codeStream.emitText(buf);
	 ip = left->compile(codeStream, ip, TypeReqFloat);
 	 codeStream.emitText(")");
	
    return codeStream.tell();
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
	 cmpValue = 0;
	
	 opName[0] = op;
	 opName[1] = '\0';
	
    switch(op)
    {
        case '^':
			 operand = 255;//Compiler2::OP_XOR;
			 dSprintf(opName, 6, "__BXO");
			 cmpValue = 1;
            break;
        case '%':
			 operand = Compiler2::OP_MOD;
			 cmpValue = 1;
            break;
        case '&':
			 operand = 255;//Compiler2::OP_BITAND;
			 dSprintf(opName, 6, "__BAD");
			 cmpValue = 1;
            break;
        case '|':
			 operand = 255;//Compiler2::OP_BITOR;
			 dSprintf(opName, 6, "__BOR");
			 cmpValue = 1;
            break;
        case '<':
            conditionalValue = Compiler2::OP_LT;
            subType = TypeReqFloat;
            break;
        case '>':
            conditionalValue = Compiler2::OP_LE;
            cmpValue = 1;
            break;
		  case opGE:
			   conditionalValue = Compiler2::OP_LT;
			   dSprintf(opName, 6, ">=");
            cmpValue = 1;
            break;
		 case opLE:
			  conditionalValue = Compiler2::OP_LE;
			  dSprintf(opName, 6, "<=");
            subType = TypeReqFloat;
            break;
		 case opEQ:
			 conditionalValue = Compiler2::OP_EQ;
			 subType = TypeReqFloat;
			 dSprintf(opName, 6, "==");
            break;
		 case opNE:
			 conditionalValue = Compiler2::OP_EQ;
			 subType = TypeReqFloat;
			 cmpValue = 1;
			 dSprintf(opName, 6, "!=");
            break;
        case opOR:
			 operand = OP_OR;
			 dSprintf(opName, 6, "or");
            break;
        case opAND:
			 operand = OP_AND;
			 dSprintf(opName, 6, "and");
            break;
        case opSHR:
			 operand = 255;//Compiler2::OP_SHR;
			 dSprintf(opName, 6, "__SHR");
			 cmpValue = 1;
            break;
		 case opSHL:
			 operand = 255;//Compiler2::OP_SHL;
			 dSprintf(opName, 6, "__SHL");
			 cmpValue = 1;
            break;
    }
}

U32 IntBinaryExprNode::compile(CodeStream &codeStream, U32 ip, TypeReq type)
{
    getSubTypeOperand();
	
	if (cmpValue != 1 && type == TypeReqFalseConditional)
	{
		codeStream.emitText("not (");
	}
	
	 if (operand == 255)
	 {
		 codeStream.emitText(opName);
		 codeStream.emitText("((");
		 ip = left->compile(codeStream, ip, subType);
		 codeStream.emitText("),(");
		 ip = right->compile(codeStream, ip, subType);
		 codeStream.emitText("))");
	 }
	else
	{
		codeStream.emitText("(");
		ip = left->compile(codeStream, ip, subType);
		codeStream.emitText(opName);
		ip = right->compile(codeStream, ip, subType);
		codeStream.emitText(")");
	}
	
	if (cmpValue == 1)
	{
		if (type == TypeReqConditional)
		{
			codeStream.emitText("!= 0");
		}
		else if (type == TypeReqFalseConditional)
		{
			codeStream.emitText("== 0");
		}
	}
	else if (type == TypeReqFalseConditional)
	{
		codeStream.emitText(")");
	}
	
    return codeStream.tell();
}

TypeReq IntBinaryExprNode::getPreferredType()
{
    return TypeReqUInt;
}

//------------------------------------------------------------

U32 StreqExprNode::compile(CodeStream &codeStream, U32 ip, TypeReq type)
{
	 codeStream.emitText("(__STREQ(");
    ip = left->compile(codeStream, ip, TypeReqString);
	 codeStream.emitText(", ");
    ip = right->compile(codeStream, ip, TypeReqString);
	 codeStream.emitText(")");
	
    if(!eq)
	 {
        codeStream.emitText(" == 0");
	 }
	else
	{
		codeStream.emitText(" != 0");
	}
    if(type != TypeReqUInt)
        codeStream.emit(conversionOp(TypeReqUInt, type));
    return codeStream.tell();
}

TypeReq StreqExprNode::getPreferredType()
{
    return TypeReqUInt;
}

//------------------------------------------------------------

U32 StrcatExprNode::compile(CodeStream &codeStream, U32 ip, TypeReq type)
{
	codeStream.emitText("(");
	ip = left->compile(codeStream, ip, TypeReqString);
	codeStream.emitText(" .. ");
	if (appendChar)
	{
		char buffer[64];
		dSprintf(buffer, 64, "'%c' .. ", appendChar);
		codeStream.emitText(buffer);
	}
	ip = right->compile(codeStream, ip, TypeReqString);
	codeStream.emitText(")");
	
	return codeStream.tell();
}

TypeReq StrcatExprNode::getPreferredType()
{
    return TypeReqString;
}

//------------------------------------------------------------

U32 CommaCatExprNode::compile(CodeStream &codeStream, U32 ip, TypeReq type)
{
	codeStream.emitText("(");
    ip = left->compile(codeStream, ip, TypeReqString);
	 codeStream.emitText(" .. '_' .. ");
	ip = right->compile(codeStream, ip, TypeReqString);
	codeStream.emitText(")");
    return codeStream.tell();
}

TypeReq CommaCatExprNode::getPreferredType()
{
    return TypeReqString;
}

//------------------------------------------------------------

U32 IntUnaryExprNode::compile(CodeStream &codeStream, U32 ip, TypeReq type)
{
    integer = true;
    TypeReq prefType = expr->getPreferredType();
    if(op == '!' && (prefType == TypeReqFloat || prefType == TypeReqString))
        integer = false;
    
    ip = expr->compile(codeStream, ip, integer ? TypeReqUInt : TypeReqFloat);
    if(op == '!')
        codeStream.emit(integer ? OP_NOT : OP_NOTF);
    else if(op == '~')
        codeStream.emit(OP_ONESCOMPLEMENT);
    if(type != TypeReqUInt)
        codeStream.emit(conversionOp(TypeReqUInt, type));
    return codeStream.tell();
}

TypeReq IntUnaryExprNode::getPreferredType()
{
    return TypeReqUInt;
}

//------------------------------------------------------------

U32 FloatUnaryExprNode::compile(CodeStream &codeStream, U32 ip, TypeReq type)
{
    ip = expr->compile(codeStream, ip, TypeReqFloat);
    codeStream.emit(OP_NEG);
    if(type != TypeReqFloat)
        codeStream.emit(conversionOp(TypeReqFloat, type));
    return codeStream.tell();
}

TypeReq FloatUnaryExprNode::getPreferredType()
{
    return TypeReqFloat;
}

//------------------------------------------------------------

U32 VarNode::compile(CodeStream &codeStream, U32 ip, TypeReq type)
{
    if(type == TypeReqNone)
        return codeStream.tell();
    
    S32 targetRegister = codeStream.getLocalVariable(varName);
    if (targetRegister == -1) targetRegister = codeStream.addLocalVariable(varName);
	
	// Fetch global variable ref if required
	if (varName[0] == '$')
	{
		// $ assign
		//U32 varNameIdx = codeStream.getConstantsTable()->addString(varName+1);
		codeStream.emitText("_G['");
		codeStream.emitText(varName);
		codeStream.emitText("']");
	}
	else if (varName[0] == '%')
	{
		// local assign
		char buffer[64];
		dSprintf(buffer, 64, "_%s", varName+1);
		codeStream.emitText(buffer);
	}
	
	if (arrayIndex)
	{
		codeStream.markLocalArrayVariable(varName);
		codeStream.emitText("[");
		ip = arrayIndex->compile(codeStream, ip, TypeReqString);
		codeStream.emitText("]");
	}
	
	return codeStream.tell();
}

TypeReq VarNode::getPreferredType()
{
    return TypeReqNone; // no preferred type
}

//------------------------------------------------------------

U32 IntNode::compile(CodeStream &codeStream, U32 ip, TypeReq type)
{
	index = codeStream.getConstantsTable()->addInt(value);
	
	U32 cmpValue = 0;
	if (type == TypeReqFalseConditional)
	{
		type = TypeReqConditional;
		cmpValue = 1;
	}
	
	 if (type == TypeReqConditional)
	 {
		 // Compare to true
		 char buffer[64];
		 dSprintf(buffer, 64, "%i != 0", value);
		 codeStream.emitText(buffer);
	 }
	 else
	 {
		 char buffer[64];
		 dSprintf(buffer, 64, "%i", value);
		 codeStream.emitText(buffer);
	 }
	
    return codeStream.tell();
}

TypeReq IntNode::getPreferredType()
{
    return TypeReqUInt;
}

//------------------------------------------------------------

U32 FloatNode::compile(CodeStream &codeStream, U32 ip, TypeReq type)
{
	index = codeStream.getConstantsTable()->addFloat(value);
	
	U32 cmpValue = 0;
	if (type == TypeReqFalseConditional)
	{
		type = TypeReqConditional;
		cmpValue = 1;
	}
	
	if (type == TypeReqConditional)
	{
		char buffer[64];
		dSprintf(buffer, 64, "%.9g != 0", value);
		codeStream.emitText(buffer);
	}
	else
	{
		char buffer[64];
		dSprintf(buffer, 64, "%.9g", value);
		codeStream.emitText(buffer);
	}
	
	return codeStream.tell();
}

TypeReq FloatNode::getPreferredType()
{
    return TypeReqFloat;
}

//------------------------------------------------------------

U32 StrConstNode::compile(CodeStream &codeStream, U32 ip, TypeReq type)
{
	index = codeStream.getConstantsTable()->addString(str);
	
	// Docblock? just make it a comment for now
	if (doc)
	{
		codeStream.emitText("-- ");
		codeStream.emitText(str);
		return ip;
	}
	
	U32 cmpValue = 0;
	if (type == TypeReqFalseConditional)
	{
		type = TypeReqConditional;
		cmpValue = 1;
	}
	
	if (type == TypeReqConditional)
	{
		// Compare to true
		codeStream.emitText("tonumber('");
		codeStream.emitText(str);
		codeStream.emitText(")");
		Con::printf("StrConstNode conditional check [TODO: should be optimized out]");
	}
	else
	{
		codeStream.emitText("'");
		codeStream.emitText(str);
		codeStream.emitText("'");
	}
	
	return codeStream.tell();
}

TypeReq StrConstNode::getPreferredType()
{
    return TypeReqString;
}

//------------------------------------------------------------

U32 ConstantNode::compile(CodeStream &codeStream, U32 ip, TypeReq type)
{
    if(type == TypeReqString)
    {
        precompileIdent(value);
		  index = codeStream.getConstantsTable()->addString(value);
		  codeStream.emitText("'");
		  codeStream.emitText(value);
		  codeStream.emitText("'");
    }
    else if (type != TypeReqNone)
    {
        fVal = consoleStringToNumber(value, dbgFileName, dbgLineNumber);
		  char buffer[64];
		  dSprintf(buffer, 64, "%.9g", fVal);
		  codeStream.emitText(buffer);
        index = codeStream.getConstantsTable()->addFloat(fVal);
    }
	
    return codeStream.tell();
}

TypeReq ConstantNode::getPreferredType()
{
    return TypeReqString;
}

//------------------------------------------------------------

U32 AssignExprNode::compile(CodeStream &codeStream, U32 ip, TypeReq type)
{
    S32 targetRegister = 0;
    subType = expr->getPreferredType();
    if(subType == TypeReqNone)
        subType = type;
    if(subType == TypeReqNone)
	 {
		 subType = TypeReqString;
    }
	
    precompileIdent(varName);
    
    if (arrayIndex)
    {
        targetRegister = codeStream.getLocalVariable(varName);
        if (targetRegister == -1)
        {
            targetRegister = codeStream.addLocalVariable(varName);
        }
		  codeStream.markLocalArrayVariable(varName);
    }
    else
    {
        targetRegister = codeStream.getLocalVariable(varName);
        if (targetRegister == -1)
        {
            targetRegister = codeStream.addLocalVariable(varName);
        }
    }
	
	//
	if (varName[0] == '$')
	{
		// $ assign
		codeStream.emitText("_G['");
		codeStream.emitText(varName);
		codeStream.emitText("']");
		if (arrayIndex)
		{
			codeStream.emitText("[");
			ip = arrayIndex->compile(codeStream, ip, TypeReqString);
			codeStream.emitText("] = ");
		}
		else
		{
			codeStream.emitText(" = ");
		}
	}
	else if (varName[0] == '%')
	{
		// local assign
		char buffer[64];
		dSprintf(buffer, 64, "_%s", varName+1);
		codeStream.emitText(buffer);
		
		
		if (arrayIndex)
		{
			codeStream.emitText("[");
			ip = arrayIndex->compile(codeStream, ip, TypeReqString);
			codeStream.emitText("] = ");
		}
		else
		{
			codeStream.emitText(" = ");
		}
	}
	
    
    ip = expr->compile(codeStream, ip, TypeReqTargetRegister); // everything goes into target
	 codeStream.emitText("\n");
	
	 return ip;
}

TypeReq AssignExprNode::getPreferredType()
{
    return expr->getPreferredType();
}

//------------------------------------------------------------

static void getAssignOpTypeOp(S32 op, TypeReq &type, U32 &operand, char* opName)
{
	opName[0] = op;
	opName[1] = '\0';
	
    switch(op)
    {
        case '+':
            type = TypeReqFloat;
            operand = Compiler2::OP_ADD;
            break;
        case '-':
            type = TypeReqFloat;
            operand = Compiler2::OP_SUB;
            break;
        case '*':
            type = TypeReqFloat;
            operand = Compiler2::OP_MUL;
            break;
        case '/':
            type = TypeReqFloat;
            operand = Compiler2::OP_DIV;
            break;
        case '%':
            type = TypeReqUInt;
            operand = Compiler2::OP_MOD;
            break;
        case '&':
            type = TypeReqUInt;
			 operand = 255;//Compiler2::OP_BITAND;
			 dSprintf(opName, 6, "_BAN");
            break;
        case '^':
            type = TypeReqUInt;
			 operand = 255;//Compiler2::OP_XOR;
			 dSprintf(opName, 6, "_BXO");
            break;
        case '|':
            type = TypeReqUInt;
			 operand = 255;//Compiler2::OP_BITOR;
			 dSprintf(opName, 6, "_BOR");
            break;
        case opSHL:
            type = TypeReqUInt;
			 operand = 255;//Compiler2::OP_SHL;
			 dSprintf(opName, 6, "_SHL");
            break;
        case opSHR:
            type = TypeReqUInt;
			 operand = 255;//Compiler2::OP_SHR;
			 dSprintf(opName, 6, "_SHR");
            break;
    }
}

U32 AssignOpExprNode::compile(CodeStream &codeStream, U32 ip, TypeReq type)
{
	// conversion OP if necessary.
	char opName[6];
    getAssignOpTypeOp(op, subType, operand, &opName[0]);
    precompileIdent(varName);
	
	 S32 targetRegister = codeStream.getLocalVariable(varName);
	 if (targetRegister == -1) targetRegister = codeStream.addLocalVariable(varName);
	
    // Fetch global variable ref if required
	if (varName[0] == '$')
	{
		// $ assign
		//U32 varNameIdx = codeStream.getConstantsTable()->addString(varName+1);
		char buffer[128];
		
		if (!arrayIndex)
		{
			if (operand == 255)
			{
				dSprintf(buffer, 64, "_G['%s'] = _%s(_G['%s'], ", varName, opName, varName);
			}
			else
			{
				dSprintf(buffer, 64, "_G['%s'] = (_G['%s'] %s ", varName, opName, varName);
			}
			codeStream.emitText(buffer);
		}
		else
		{
			dSprintf(buffer, 64, "_G['%s'][", varName);
			codeStream.emitText(buffer);
			ip = arrayIndex->compile(codeStream, ip, TypeReqString);
			codeStream.emitText("] = (");
			codeStream.emitText(buffer);
			ip = arrayIndex->compile(codeStream, ip, TypeReqString);
			dSprintf(buffer, 64, "] %s ", opName);
			codeStream.emitText(buffer);
		}
	}
	else if (varName[0] == '%')
	{
		// local assign
		char buffer[128];
		if (!arrayIndex)
		{
			if (operand == 255)
			{
				dSprintf(buffer, 64, "_%s = _%s(_%s, ", varName+1, opName, varName+1);
			}
			else
			{
				dSprintf(buffer, 64, "_%s = (_%s %s ", varName+1, varName+1, opName);
			}
			codeStream.emitText(buffer);
		}
		else
		{
			dSprintf(buffer, 64, "_%s", varName);
			codeStream.emitText(buffer);
			ip = arrayIndex->compile(codeStream, ip, TypeReqString);
			codeStream.emitText("] = (");
			codeStream.emitText(buffer);
			ip = arrayIndex->compile(codeStream, ip, TypeReqString);
			dSprintf(buffer, 64, "] %s ", opName);
			codeStream.emitText(buffer);
		}
	}
	
	ip = expr->compile(codeStream, ip, subType);
	
	codeStream.emitText(")");
	return codeStream.tell();
}

TypeReq AssignOpExprNode::getPreferredType()
{
	char opName[6];
    getAssignOpTypeOp(op, subType, operand, &opName[0]);
    return subType;
}

//------------------------------------------------------------

U32 TTagSetStmtNode::compileStmt(CodeStream&, U32 ip)
{
    return ip;
}

//------------------------------------------------------------

U32 TTagDerefNode::compile(CodeStream&, U32 ip, TypeReq)
{
    return ip;
}

TypeReq TTagDerefNode::getPreferredType()
{
    return TypeReqNone;
}

//------------------------------------------------------------

U32 TTagExprNode::compile(CodeStream&, U32 ip, TypeReq)
{
    return ip;
}

TypeReq TTagExprNode::getPreferredType()
{
    return TypeReqNone;
}

//------------------------------------------------------------

U32 FuncCallExprNode::compile(CodeStream &codeStream, U32 ip, TypeReq type)
{
    // How this works:
	 // - lookup function with OP_GETFUNC
	 // - OP_CALL(funcStart, params) -> sets returnStart to result
	
	
	// [abc] [a := a(a[...b params])[...c return params]]
	
    precompileIdent(funcName);
    precompileIdent(nameSpace);
	
	// First get function
	if(callType == MethodCall)
	{
		
	}
	else if (callType == ParentCall)
	{
		
	}
	else
	{
		char buffer[256];
		dSprintf(buffer, 256, "%s(", funcName);
		codeStream.emitText(buffer);
		
		// Add on extra params
		
		U32 numArgs = 0;
		for(ExprNode *walk = args; walk; walk = (ExprNode *) walk->getNext())
		{
			if (walk != args)
			{
				codeStream.emitText(", (");
				ip = walk->compile(codeStream, ip, TypeReqVar);
				codeStream.emitText(")");
			}
			else if (walk == args)
			{
				codeStream.emitText("(");
				ip = walk->compile(codeStream, ip, TypeReqVar);
				codeStream.emitText(")");
			}
			
			numArgs++;
		}
		
		codeStream.emitText(")");
	}
	
	return codeStream.tell();
}

TypeReq FuncCallExprNode::getPreferredType()
{
    return TypeReqString;
}


//------------------------------------------------------------

U32 AssertCallExprNode::compile( CodeStream &codeStream, U32 ip, TypeReq type )
{
#ifdef TORQUE_ENABLE_SCRIPTASSERTS
    
    messageIndex = getCurrentStringTable()->add( message, true, false );
    
    ip = testExpr->compile( codeStream, ip, TypeReqUInt );
	
	 codeStream.emitText("assert((");
	 ip = testExpr->compile( codeStream, ip, TypeReqConditional );
	codeStream.emitText(", '");
	 codeStream.emitText(message);
	codeStream.emitText("')");
	
#endif
    
    return codeStream.tell();
}

TypeReq AssertCallExprNode::getPreferredType()
{
    return TypeReqNone;
}

//------------------------------------------------------------

U32 SlotAccessNode::compile(CodeStream &codeStream, U32 ip, TypeReq type)
{
    if(type == TypeReqNone)
        return ip;
    
    precompileIdent(slotName);
	
	 ip = objectExpr->compile(codeStream, ip, TypeReqString);
	codeStream.emitText(".");
	codeStream.emitText(slotName);
	
    if(arrayExpr)
	 {
		 codeStream.emitText("[");
		 ip = arrayExpr->compile(codeStream, ip, TypeReqString);
		 codeStream.emitText("]");
    }
	
    return codeStream.tell();
}

TypeReq SlotAccessNode::getPreferredType()
{
    return TypeReqNone;
}

//-----------------------------------------------------------------------------

U32 InternalSlotAccessNode::compile(CodeStream &codeStream, U32 ip, TypeReq type)
{
    if(type == TypeReqNone)
        return ip;
    
    ip = objectExpr->compile(codeStream, ip, TypeReqString);
    codeStream.emit(OP_SETCUROBJECT);
    
    ip = slotExpr->compile(codeStream, ip, TypeReqString);
    codeStream.emit(OP_SETCUROBJECT_INTERNAL);
    codeStream.emit(recurse);
    
    if(type != TypeReqUInt)
        codeStream.emit(conversionOp(TypeReqUInt, type));
    return codeStream.tell();
}

TypeReq InternalSlotAccessNode::getPreferredType()
{
    return TypeReqUInt;
}

//-----------------------------------------------------------------------------

U32 SlotAssignNode::compile(CodeStream &codeStream, U32 ip, TypeReq type)
{
	if(typeID != -1)
	{
		char buffer[1024];
		 if (objectExpr)
		 {
			 ip = objectExpr->compile(codeStream, ip, TypeReqString);
		 }
		 else
		 {
			 codeStream.emitText("__NOBJ");
		 }
		
		dSprintf(buffer, 1024, ":setFieldType(%s, %i)\n", slotName, typeID);
		codeStream.emitText(buffer);
	}
	
	 if (objectExpr)
	 {
		 ip = objectExpr->compile(codeStream, ip, TypeReqString);
	 }
	 else
	 {
		 codeStream.emitText("__NOBJ");
	 }
	
	 codeStream.emitText(".");
	 codeStream.emitText(slotName);
	
	 if (arrayExpr)
	 {
		 codeStream.emitText("[");
		 ip = arrayExpr->compile(codeStream, ip, TypeReqString);
		 codeStream.emitText("]");
	 }
	
	
	codeStream.emitText(" = ");
	ip = valueExpr->compile(codeStream, ip, TypeReqString);
	
	return codeStream.tell();
}

TypeReq SlotAssignNode::getPreferredType()
{
    return TypeReqString;
}

//------------------------------------------------------------

U32 SlotAssignOpNode::compile(CodeStream &codeStream, U32 ip, TypeReq type)
{
    // first eval the expression as its type
    
    // if it's an array:
    // eval array
    // OP_ADVANCE_STR
    // evaluate object expr
    // OP_SETCUROBJECT
    // OP_SETCURFIELD
    // fieldName
    // OP_TERMINATE_REWIND_STR
    // OP_SETCURFIELDARRAY
    
    // else
    // evaluate object expr
    // OP_SETCUROBJECT
    // OP_SETCURFIELD
    // fieldName
    
    // OP_LOADFIELD of appropriate type
    // operand
    // OP_SAVEFIELD of appropriate type
    // convert to return type if necessary.
	
	char opName[6];
    getAssignOpTypeOp(op, subType, operand, &opName[0]);
    precompileIdent(slotName);
    
    ip = valueExpr->compile(codeStream, ip, subType);
    if(arrayExpr)
    {
        ip = arrayExpr->compile(codeStream, ip, TypeReqString);
        codeStream.emit(OP_ADVANCE_STR);
    }
    ip = objectExpr->compile(codeStream, ip, TypeReqString);
    codeStream.emit(OP_SETCUROBJECT);
    codeStream.emit(OP_SETCURFIELD);
    codeStream.emitSTE(slotName);
    
    if(arrayExpr)
    {
        codeStream.emit(OP_TERMINATE_REWIND_STR);
        codeStream.emit(OP_SETCURFIELD_ARRAY);
    }
    codeStream.emit((subType == TypeReqFloat) ? OP_LOADFIELD_FLT : OP_LOADFIELD_UINT);
    codeStream.emit(operand);
    codeStream.emit((subType == TypeReqFloat) ? OP_SAVEFIELD_FLT : OP_SAVEFIELD_UINT);
    if(subType != type)
        codeStream.emit(conversionOp(subType, type));
    return codeStream.tell();
}

TypeReq SlotAssignOpNode::getPreferredType()
{
	 char opName[6];
    getAssignOpTypeOp(op, subType, operand, &opName[0]);
    return subType;
}

//------------------------------------------------------------

U32 ObjectDeclNode::compileSubObject(CodeStream &codeStream, U32 ip, bool root, const char* parentName)
{
	// goes
	
	// OP_PUSHFRAME 1
	// name expr
	// OP_PUSH 1
	// args... PUSH
	// OP_CREATE_OBJECT 1
	// parentObject 1
	// isDatablock 1
	// internalName 1
	// isSingleton 1
	// lineNumber 1
	// fail point 1
	
	// for each field, eval
	// OP_ADD_OBJECT (to UINT[0]) 1
	// root? 1
	
	// add all the sub objects.
	// OP_END_OBJECT 1
	// root? 1
	// To fix the stack issue [7/9/2007 Black]
	// OP_FINISH_OBJECT <-- fail point jumps to this opcode
	
	
	// jamesu - in order to ensure correct creation logic, we need to create the object within a function. Sub-objects will also be created within this function, so there is no nesting overhead.
	
	CodeStream::CodeData* blankBlock  = NULL;
	CodeStream::CodeData* varBlock  = NULL;
	
	char objName[128];
	char buffer[1024];
	
	
	if (root)
	{
		codeStream.pushFunctionState();
		codeStream.emitText("(function(");
		
		// Need to prepare a list of variables used here so we can pass through
		blankBlock = codeStream.allocCodeSilent(16*1024);
		
		codeStream.emitText(")\n");
		
		// Same for the local vars
		varBlock = codeStream.allocCodeSilent(16*1024);
		
		dSprintf(objName, 128, "__OBJ_SUB");
	}
	else
	{
		dSprintf(objName, 128, "%s_SUB", parentName, ip);
	}
	
	dSprintf(buffer, 1024, "__NOBJ = ");
	codeStream.emitText(buffer);
	
	//
	
	if (isDatablock)
	{
		codeStream.emitText("createDatablock(");
	}
	else if (isSingleton)
	{
		codeStream.emitText("createSingletonObject(");
	}
	else
	{
		codeStream.emitText("createObject(");
	}
	
	ip = classNameExpr->compile(codeStream, ip, TypeReqString);
	
	for(ExprNode *exprWalk = argList; exprWalk; exprWalk = (ExprNode *) exprWalk->getNext())
	{
		TypeReq walkType = exprWalk->getPreferredType();
		codeStream.emitText(", ");
		ip = exprWalk->compile(codeStream, ip, walkType);
	}
	
	codeStream.emitText(")\n");
	
	// Copy fields from parent
	if (parentObject && parentObject[0] != '\0')
	{
		dSprintf(buffer, 1024, "__NOBJ.copyFieldsFrom('%s')\n", parentObject);
		codeStream.emitText(buffer);
	}
	
	// Assign slots first
	for(SlotAssignNode *slotWalk = slotDecls; slotWalk; slotWalk = (SlotAssignNode *) slotWalk->getNext())
	{
		ip = slotWalk->compile(codeStream, ip, TypeReqNone);
		codeStream.emitText("\n");
	}
	
	if (root)
	{
		dSprintf(buffer, 1024, "__NOBJ.registerObject(");
	}
	else
	{
		dSprintf(buffer, 1024, "__NOBJ.registerSubObject(");
	}
	codeStream.emitText(buffer);
	ip = objectNameExpr->compile(codeStream, ip, TypeReqString);
	codeStream.emitText(")\n");
	
	// Now compile & assign sub objects
	U32 count = 0;
	
	if (subObjects)
	{
		// Need a variable to store the parent
		S32 targetRegister = codeStream.getLocalVariable(objName);
		if (targetRegister == -1) targetRegister = codeStream.addTempObjectVariable(StringTable->insert(objName));
		
		dSprintf(buffer, 1024, "%s = __NOBJ\n", objName);
		codeStream.emitText(buffer);
	}
	
	for(ObjectDeclNode *objectWalk = subObjects; objectWalk; objectWalk = (ObjectDeclNode *) objectWalk->getNext())
	{
		ip = objectWalk->compileSubObject(codeStream, count, false, objName);
		count++;
	}
	
	if (root)
	{
		// Emit all the required vars in our allocated block AND at the function call
		
		codeStream.emitText("return __OBJ_SUB\nend)(");
	 U8* data = blankBlock->data;
	 char* ptr = (char*)data;
		
		for (U32 i=codeStream.mCurrentFunctionState.startVar; i<codeStream.mVarInfo.size(); i++)
		{
			char buffer[1024];
			if (codeStream.mVarInfo[i].name[0] != '$' && !codeStream.mVarInfo[i].isParam && !codeStream.mVarInfo[i].isObjTemp)
			{
				if (ptr != (char*)data)
				{
					codeStream.emitText(", ");
					dStrcpy(ptr, ", ");
					ptr += dStrlen(ptr);
				}
				
				// Constructor parameter local
				dSprintf(buffer, 1024, "_%s", codeStream.mVarInfo[i].name+1);
				dStrcpy(ptr, buffer);
				ptr += dStrlen(ptr);
				
				// Input param is our current local
				dSprintf(buffer, 1024, "_%s", codeStream.mVarInfo[i].name+1);
				codeStream.emitText(buffer);
			}
		}
		codeStream.emitText(")");
		
		blankBlock->size = dStrlen((char*)data);
		
		// Now emit the local vars in our preallocated block
		data = varBlock->data;
		ptr = (char*)data;
		
		for (U32 i=codeStream.mCurrentFunctionState.startVar; i<codeStream.mVarInfo.size(); i++)
		{
			char buffer[1024];
			if (codeStream.mVarInfo[i].name[0] != '$' && codeStream.mVarInfo[i].isObjTemp)
			{
				if (!codeStream.mVarInfo[i].isArray)
				{
					dSprintf(buffer, 1024, "local _%s\n", codeStream.mVarInfo[i].name+1);
				}
				else
				{
					dSprintf(buffer, 1024, "local _%s = {}\n", codeStream.mVarInfo[i].name+1);
				}
				dStrcpy(ptr, buffer);
				ptr += dStrlen(ptr);
			}
		}
		
		varBlock->size = dStrlen((char*)data);
		
		codeStream.popFunctionState();
	}
	else
	{
		// Need to add to the parent object
		char buffer[1024];
		dSprintf(buffer, 1024, "%s.addObject(%s)\n", parentName, subObjects ? objName : "__NOBJ");
		codeStream.emitText(buffer);
	}
	
    return codeStream.tell();
}

U32 ObjectDeclNode::compile(CodeStream &codeStream, U32 ip, TypeReq type)
{
	 ip = compileSubObject(codeStream, ip, true, "");
    return codeStream.tell();
}

TypeReq ObjectDeclNode::getPreferredType()
{
    return TypeReqUInt;
}

//------------------------------------------------------------

U32 FunctionDeclStmtNode::compileStmt(CodeStream &codeStream, U32 ip)
{
    argc = 0;
    for(VarNode *walk = args; walk; walk = (VarNode *)((StmtNode*)walk)->getNext())
    {
        precompileIdent(walk->varName);
        argc++;
    }
    
    CodeBlock::smInFunction = true;
    
    precompileIdent(fnName);
    precompileIdent(nameSpace);
    precompileIdent(package);
    
    CodeBlock::smInFunction = false;
	
	if (!CodeBlock::smFunctionListHead)
	{
		CodeBlock::smFunctionListHead = this;
	}
	
	if (CodeBlock::smCurrentFunctionList)
	{
		FunctionDeclStmtNode* head = CodeBlock::smCurrentFunctionList;
		head->nextFunction = this;
	}
	
	CodeBlock::smCurrentFunctionList = this;
	nextFunction = NULL;
	
	CodeBlock::smCurrentFunction = new CodeBlockFunction;
	CodeBlock::smCurrentCodeBlock->functions.push_back(CodeBlock::smCurrentFunction);
	
	// First emit the proto which should go "registerMethod(nsName, funcName, packageName, function (params) end)"
	// OR "registerFunction(funcName, packageName, function (params) end)"
	
	char buffer[1024];
	char nsName[128];
	char pkgName[128];
	
	if (nameSpace)
	{
		dSprintf(nsName, sizeof(nsName), "'%s'", nameSpace);
	}
	else
	{
		dStrcpy(nsName, "nil");
	}
	
	if (package)
	{
		dSprintf(pkgName, sizeof(pkgName), "'%s'", package);
	}
	else
	{
		dStrcpy(pkgName, "nil");
	}
	
	if (nameSpace)
	{
		dSprintf(buffer, 1024, "registerMethod('%s', %s, %s, function(", nsName, fnName, pkgName);
	}
	else
	{
		dSprintf(buffer, 1024, "registerFunction('%s', %s, function(", fnName, pkgName);
	}
	
	codeStream.emitText(buffer);
	for(VarNode *walk = args; walk; walk = (VarNode *)((StmtNode*)walk)->getNext())
	{
		// Register as param
		S32 targetRegister = codeStream.getLocalVariable(walk->varName);
		if (targetRegister != -1)
		{
			Con::warnf("Two variables with the same name!");
		}
		
		targetRegister = codeStream.addLocalParameter(walk->varName);
		
		if (walk == args)
		{
			codeStream.emitText(walk->varName+1);
		}
		else
		{
			codeStream.emitText(", ");
			codeStream.emitText(walk->varName+1);
		}
	}
	codeStream.emitText(")\n");
	
	// First emit a blank block
	CodeStream::CodeData* blankBlock = codeStream.allocCodeSilent(16*1024);
	
	codeStream.pushFunctionState();
	gNLAfterText = true;
	U32 lastIp = compileBlock(stmts, codeStream, 0);
	
	addBreakLine(codeStream);
	
	// Now emit the local vars in our preallocated block
	U8* data = blankBlock->data;
	char* ptr = (char*)data;
	
	for (U32 i=codeStream.mCurrentFunctionState.startVar; i<codeStream.mVarInfo.size(); i++)
	{
		char buffer[1024];
		if (codeStream.mVarInfo[i].name[0] != '$' && !codeStream.mVarInfo[i].isParam)
		{
			if (!codeStream.mVarInfo[i].isArray)
			{
				dSprintf(buffer, 1024, "local _%s\n", codeStream.mVarInfo[i].name+1);
			}
			else
			{
				dSprintf(buffer, 1024, "local _%s = {}\n", codeStream.mVarInfo[i].name+1);
			}
			dStrcpy(ptr, buffer);
			ptr += dStrlen(ptr);
		}
	}
	
	if (codeStream.mCurrentFunctionState.needEnv)
	{
		char buffer[1024];
		dSprintf(buffer, 1024, "local _ENV = pushEnv();\n");
		dStrcpy(ptr, buffer);
		ptr += dStrlen(ptr);
		
		// Need to pop env at end
		codeStream.emitText("popEnv(_ENV)");
	}
	blankBlock->size = dStrlen((const char*)data);
	
	codeStream.emitText("end)\n");
	
	setCurrentStringTable(&getGlobalStringTable());
	setCurrentFloatTable(&getGlobalFloatTable());
	
	CodeBlock::smCurrentFunction = CodeBlock::smCurrentCodeblockFunction;
	codeStream.popFunctionState();
	
	return ip;
}
