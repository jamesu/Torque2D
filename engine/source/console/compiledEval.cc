//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
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
#include "console/consoleBaseType.h"

#include "console/compiler/consoleAST.h"
#include "collection/findIterator.h"
#include "io/resource/resourceManager.h"

#include "string/findMatch.h"
#include "string/stringUnit.h"
#include "console/consoleInternal.h"
#include "io/fileStream.h"
#include "console/compiler/consoleCompiler.h"

#include "sim/simBase.h"
#include "network/netStringTable.h"
#include "component/dynamicConsoleMethodComponent.h"
#include "string/stringStack.h"
#include "messaging/message.h"
#include "memory/frameAllocator.h"

#include "debug/telnetDebugger.h"

#ifndef _REMOTE_DEBUGGER_BASE_H_
#include "debug/remote/RemoteDebuggerBase.h"
#endif

using namespace Compiler;

namespace Con
{
// Current script file name and root, these are registered as
// console variables.
extern StringTableEntry gCurrentFile;
extern StringTableEntry gCurrentRoot;
}

StringStack STR;

static const char *getNamespaceList(Namespace *ns)
{
   U32 size = 1;
   Namespace *walk;
   for(walk = ns; walk; walk = walk->mParent)
      size += dStrlen(walk->mName) + 4;
   char *ret = Con::getReturnBuffer(size);
   ret[0] = 0;
   for(walk = ns; walk; walk = walk->mParent)
   {
      dStrcat(ret, walk->mName);
      if(walk->mParent)
         dStrcat(ret, " -> ");
   }
   return ret;
}

//------------------------------------------------------------

namespace Con
{
    char *getReturnBuffer(U32 bufferSize)
    {
        return STR.getReturnBuffer(bufferSize);
    }

    char *getReturnBuffer( const char *stringToCopy )
    {
        char *ret = STR.getReturnBuffer( dStrlen( stringToCopy ) + 1 );
        dStrcpy( ret, stringToCopy );
        ret[dStrlen( stringToCopy )] = '\0';
        return ret;
    }

    char *getArgBuffer(U32 bufferSize)
    {
        return STR.getArgBuffer(bufferSize);
    }

    char *getFloatArg(F64 arg)
    {
        char *ret = STR.getArgBuffer(32);
        dSprintf(ret, 32, "%g", arg);
        return ret;
    }

    char *getIntArg(S32 arg)
    {
        char *ret = STR.getArgBuffer(32);
        dSprintf(ret, 32, "%d", arg);
        return ret;
    }

    char *getBoolArg(bool arg)
    {
        char *ret = STR.getArgBuffer(32);
        dSprintf(ret, 32, "%d", arg);
        return ret;
    }
}

//------------------------------------------------------------

void CodeBlock::getFunctionArgs(char buffer[1024], U32 ip)
{
   CodeBlockFunction *func = mFunctions[ip];
   U32 fnArgc = func->numArgs;
   buffer[0] = 0;
   for(U32 i = 0; i < fnArgc; i++)
   {
      StringTableEntry var = func->args[i].varName;
      
      // Add a comma so it looks nice!
      if(i != 0)
         dStrcat(buffer, ", ");

      dStrcat(buffer, "var ");

      // Try to capture junked parameters
      if(var[0])
        dStrcat(buffer, var+1);
      else
        dStrcat(buffer, "JUNK");
   }
}

//-----------------------------------------------------------------------------

#define vmdispatch(o)   switch(o)
#define vmcase(l)   case l:
#define vmbreak   break

inline U32 ts2_equal(ConsoleValue *a, ConsoleValue *b)
{
   // TODO: handle same reference types
   return a->getFloatValue() == b->getFloatValue() ? 1 : 0;
}

inline U32 ts2_lt(ConsoleValue *a, ConsoleValue *b)
{
   return a->getFloatValue() < b->getFloatValue() ? 1 : 0;
}

inline U32 ts2_le(ConsoleValue *a, ConsoleValue *b)
{
   return a->getFloatValue() <= b->getFloatValue() ? 1 : 0;
}

// jamesu - string equals will compare values as if they were case insensitive strings
inline U32 ts2_eq_str(ConsoleValue *a, ConsoleValue *b)
{
   if (a->type == b->type)
   {
      if (a->type == ConsoleValue::TypeInternalStringTableEntry)
      {
         return a->value.string == b->value.string;
      }
      else if (a->type == ConsoleValue::TypeInternalNamespaceName)
      {
         return a->value.string == b->value.string;
      }
      else if (a->type > ConsoleValue::TypeReferenceCounted)
      {
         return a->value.refValue->stringCompare(b->value.refValue->getString().c_str());
      }
      else
      {
         return a->getFloatValue() == b->getFloatValue();
      }
   }
   else if (a->type > ConsoleValue::TypeReferenceCounted && b->type >= ConsoleValue::TypeReferenceCounted)
   {
      // Both values are custom, easy!
      return a->value.refValue->refCompare(b->value.refValue);
   }
   else if (a->type > ConsoleValue::TypeReferenceCounted)
   {
      return a->value.refValue->stringCompare(b->getTempStringValue());
   }
   else if (b->type > ConsoleValue::TypeReferenceCounted)
   {
      return b->value.refValue->stringCompare(a->getTempStringValue());
   }
   else
   {
      // Must be some sort of numeric comparison
      return a->getFloatValue() == b->getFloatValue();
   }
}

static char printBuf[128];
static char printBuf2[128];
static char *printBufPtr = NULL;
inline const char *ts2PrintKonstOrRef(U32 value, ConsoleValue *konst)
{
   if (printBufPtr == NULL || printBufPtr == printBuf2)
      printBufPtr = printBuf;
   else
      printBufPtr = printBuf2;
   
   if (value & Compiler::OP_K_MASK)
   {
      //dSprintf(printBufPtr, 128, "C%i(%s)", value & Compiler::OP_KV_MASK, konst[value & Compiler::OP_KV_MASK].getTempStringValue());
      dSprintf(printBufPtr, 128, "C%i", value & Compiler::OP_KV_MASK);
   }
   else
   {
      dSprintf(printBufPtr, 128, "R%i", value);
   }
   
   return printBufPtr;
}

void CodeBlock::dumpOpcodes(CodeBlockEvalState *state)
{
   ConsoleValuePtr *konst = state->currentFrame.constants;
   ConsoleValuePtr *konstBase = konst;
   U32 targetRegTmp = 0;
   
   // Debug print instructions
   for (int j=0; j<state->currentFrame.code->codeSize; j++)
   {
      Compiler::Instruction i = (state->currentFrame.code->code[j]);
      
      switch (TS2_OP_DEC(i))
      {
            vmcase(Compiler::OP_MOVE) {
               
               Con::printf("[%i] OP_MOV %i %i", j, TS2_OP_DEC_A(i),TS2_OP_DEC_B(i));
               vmbreak;
            }
            vmcase(Compiler::OP_LOADK) {
               
               Con::printf("[%i] OP_LOADK %i %i (%s)", j, TS2_OP_DEC_A(i),TS2_OP_DEC_Bx(i), konst[TS2_OP_DEC_Bx(i)].getTempStringValue());
               vmbreak;
            }
            vmcase(Compiler::OP_PAGEK) {
               Con::printf("[%i] OP_PAGEK %i %i", j, TS2_OP_DEC_A(i), TS2_OP_DEC_Bx(i));
               konst = konstBase + (Compiler::CONSTANTS_PER_PAGE * TS2_OP_DEC_Bx(i));
               vmbreak;
            }
            vmcase(Compiler::OP_LOADVAR) {
               // A := B
               Con::printf("[%i] OP_LOADVAR %i %s", j, TS2_OP_DEC_A(i),ts2PrintKonstOrRef(TS2_OP_DEC_B(i), konst));
               vmbreak;
            }
            vmcase(Compiler::OP_GETFIELD) {
               // a := object(b).slot(c)
               Con::printf("[%i] OP_GETFIELD R%i %s %s", j, TS2_OP_DEC_A(i),ts2PrintKonstOrRef(TS2_OP_DEC_B(i), konst), ts2PrintKonstOrRef(TS2_OP_DEC_C(i), konst));
               vmbreak;
            }
            vmcase(Compiler::OP_GETFIELDA) {
               // a := object(b).slot(c).array(b+1)
               Con::printf("[%i] OP_GETFIELDA R%i %s %s [%s]", j, TS2_OP_DEC_A(i),ts2PrintKonstOrRef(TS2_OP_DEC_B(i), konst), ts2PrintKonstOrRef(TS2_OP_DEC_C(i), konst), ts2PrintKonstOrRef(TS2_OP_DEC_B(i)+1, konst));
               vmbreak;
            }
            
            vmcase(Compiler::OP_SETFIELD) {
               // a := object(b).slot(c)
               Con::printf("[%i] OP_SETFIELD R%i %s %s", j, TS2_OP_DEC_A(i),ts2PrintKonstOrRef(TS2_OP_DEC_B(i), konst), ts2PrintKonstOrRef(TS2_OP_DEC_C(i), konst));
               vmbreak;
            }
            vmcase(Compiler::OP_SETFIELDA) {
               // object(a).slot(b).array(b+1) := c
               Con::printf("[%i] OP_SETFIELDA R%i %s [%s] %s", j, TS2_OP_DEC_A(i),ts2PrintKonstOrRef(TS2_OP_DEC_B(i), konst), ts2PrintKonstOrRef(TS2_OP_DEC_C(i)+1, konst), ts2PrintKonstOrRef(TS2_OP_DEC_C(i), konst));
               vmbreak;
            }
            
            vmcase(Compiler::OP_SETVAR) {
               // B := C
               Con::printf("[%i] OP_SETVAR %i %s %s", j, TS2_OP_DEC_A(i),ts2PrintKonstOrRef(TS2_OP_DEC_B(i), konst),ts2PrintKonstOrRef(TS2_OP_DEC_C(i), konst));
               vmbreak;
            }
            
            vmcase(Compiler::OP_GETFUNC) {
               Con::printf("[%i] OP_GETFUNC %s %s -> R%i", j, ts2PrintKonstOrRef(TS2_OP_DEC_B(i), konst), ts2PrintKonstOrRef(TS2_OP_DEC_C(i), konst), TS2_OP_DEC_A(i));
               vmbreak;
            }
            
            vmcase(Compiler::OP_GETFUNC_NS) {
               Con::printf("[%i] OP_GETFUNC_NS %s %s -> R%i", j, ts2PrintKonstOrRef(TS2_OP_DEC_B(i), konst), ts2PrintKonstOrRef(TS2_OP_DEC_C(i), konst), TS2_OP_DEC_A(i));
               vmbreak;
            }
            
            vmcase(Compiler::OP_GETINTERNAL) {
               Con::printf("[%i] OP_GETINTERNAL %s->%s -> R%i", j, ts2PrintKonstOrRef(TS2_OP_DEC_B(i), konst), ts2PrintKonstOrRef(TS2_OP_DEC_C(i), konst), TS2_OP_DEC_A(i));
               vmbreak;
            }
            
            vmcase(Compiler::OP_GETINTERNAL_N) {
               Con::printf("[%i] OP_GETINTERNAL_N %s-->%s -> R%i", j, ts2PrintKonstOrRef(TS2_OP_DEC_B(i), konst), ts2PrintKonstOrRef(TS2_OP_DEC_C(i), konst), TS2_OP_DEC_A(i));
               vmbreak;
            }
            
            vmcase(Compiler::OP_CREATE_VALUE) {
               Con::printf("[%i] OP_CREATE_OBJECT R%i %s args:%i", j, TS2_OP_DEC_A(i), ts2PrintKonstOrRef(TS2_OP_DEC_B(i), konst), TS2_OP_DEC_C(i));
               vmbreak;
            }
            
            vmcase(Compiler::OP_CREATE_OBJECT) {
               Con::printf("[%i] OP_CREATE_OBJECT R%i %s args:%i", j, TS2_OP_DEC_A(i), ts2PrintKonstOrRef(TS2_OP_DEC_B(i), konst), TS2_OP_DEC_C(i));
               vmbreak;
            }
            
            vmcase(Compiler::OP_FINISH_OBJECT) {
               // registerObject(A), B.addObject(A), [flags(c)]
               Con::printf("[%i] OP_FINISH_OBJECT R%i %s %i", j, TS2_OP_DEC_A(i), ts2PrintKonstOrRef(TS2_OP_DEC_B(i), konst), TS2_OP_DEC_C(i));
               vmbreak;
            }
            
            vmcase(Compiler::OP_DELETE_OBJECT) {
               // delete A
               Con::printf("[%i] OP_DELETE_OBJECT R%i", j, TS2_OP_DEC_A(i));
               vmbreak;
            }
            
            
            
#define _TS2_INT_OP(_op, _operand) \
            vmcase(_op) { \
               targetRegTmp = TS2_OP_DEC_A(i);\
               Con::printf(#_op "[%i] -> (%s & %s) -> %i", j, ts2PrintKonstOrRef(TS2_OP_DEC_B(i), konst) , ts2PrintKonstOrRef(TS2_OP_DEC_C(i), konst), targetRegTmp);\
               vmbreak; \
            }
            
#define _TS2_FLOAT_OP(_op, _operand) \
            vmcase(_op) { \
               targetRegTmp = TS2_OP_DEC_A(i); \
               Con::printf(#_op "[%i] -> (%s & %s) -> %i", j, ts2PrintKonstOrRef(TS2_OP_DEC_B(i), konst) , ts2PrintKonstOrRef(TS2_OP_DEC_C(i), konst), targetRegTmp);\
               vmbreak; \
            }
            
            //TS2_FLOAT_OP(Compiler::OP_ADD, +)
            vmcase(Compiler::OP_ADD) {
               targetRegTmp = TS2_OP_DEC_A(i);
               Con::printf("[%i] Compiler::OP_ADD -> (%s & %s) -> R%i", j, ts2PrintKonstOrRef(TS2_OP_DEC_B(i), konst) , ts2PrintKonstOrRef(TS2_OP_DEC_C(i), konst), targetRegTmp);
               vmbreak;
            }
            
            //TS2_FLOAT_OP(Compiler::OP_SUB, -)
            vmcase(Compiler::OP_SUB) {
               targetRegTmp = TS2_OP_DEC_A(i);
               Con::printf("[%i] Compiler::OP_SUB -> (%s & %s) -> R%i", j, ts2PrintKonstOrRef(TS2_OP_DEC_B(i), konst) , ts2PrintKonstOrRef(TS2_OP_DEC_C(i), konst), targetRegTmp);
               vmbreak;
            }
            
            _TS2_FLOAT_OP(Compiler::OP_MUL, *)
            _TS2_FLOAT_OP(Compiler::OP_DIV, /)
            _TS2_INT_OP(Compiler::OP_MOD, %)
            
            vmcase(Compiler::OP_POW) {
               Con::printf("[%i] Compiler::POW (%s ** %s) -> R%i", j, ts2PrintKonstOrRef(TS2_OP_DEC_B(i), konst), ts2PrintKonstOrRef(TS2_OP_DEC_C(i), konst), TS2_OP_DEC_A(i));
               vmbreak;
            }
            
            vmcase(Compiler::OP_UMN) {
               Con::printf("[%i] Compiler::UMN (-%s) -> R%i", j, ts2PrintKonstOrRef(TS2_OP_DEC_B(i), konst), TS2_OP_DEC_A(i));
               vmbreak;
            }
            
            vmcase(Compiler::OP_NOT) {
               Con::printf("[%i] Compiler::NOT (!%s) -> R%i", j, ts2PrintKonstOrRef(TS2_OP_DEC_B(i), konst), TS2_OP_DEC_A(i));
               vmbreak;
            }
            
            _TS2_INT_OP(Compiler::OP_XOR, ^)
            _TS2_INT_OP(Compiler::OP_SHL, <<)
            _TS2_INT_OP(Compiler::OP_SHR, >>)
            _TS2_INT_OP(Compiler::OP_BITAND, &)
            _TS2_INT_OP(Compiler::OP_BITOR, |)
            
            vmcase(Compiler::OP_ONESCOMPLEMENT) {
               targetRegTmp = TS2_OP_DEC_A(i);
               Con::printf("[%i] Compiler::OP_ONESCOMPLEMENT -> (%s & %s) -> R%i", j, ts2PrintKonstOrRef(TS2_OP_DEC_B(i), konst) , ts2PrintKonstOrRef(TS2_OP_DEC_C(i), konst), targetRegTmp);
               vmbreak;
            }
            
            
            vmcase(Compiler::OP_CONCAT) {
               Con::printf("[%i] OP_CONCAT %i..%i", j, TS2_OP_DEC_B(i), TS2_OP_DEC_C(i));
               vmbreak;
            }
            
            vmcase(Compiler::OP_JMP) {
               Con::printf("[%i] OP_JMP %i [to %i]", j, TS2_OP_DEC_sBx(i), j+TS2_OP_DEC_sBx(i)+1);
               vmbreak;
            }
            
            vmcase(Compiler::OP_LT) {
               Con::printf("[%i] OP_LT %i %s %s", j, TS2_OP_DEC_A(i), ts2PrintKonstOrRef(TS2_OP_DEC_B(i), konst), ts2PrintKonstOrRef(TS2_OP_DEC_C(i), konst));
               vmbreak;
            }
            
            vmcase(Compiler::OP_LE) {
               Con::printf("[%i] OP_LE %i %s %s", j, TS2_OP_DEC_A(i), ts2PrintKonstOrRef(TS2_OP_DEC_B(i), konst), ts2PrintKonstOrRef(TS2_OP_DEC_C(i), konst));
               vmbreak;
            }
            
            vmcase(Compiler::OP_EQ) {
               Con::printf("[%i] OP_EQ %i %s %s", j, TS2_OP_DEC_A(i), ts2PrintKonstOrRef(TS2_OP_DEC_B(i), konst), ts2PrintKonstOrRef(TS2_OP_DEC_C(i), konst));
               vmbreak;
            }
            
            vmcase(Compiler::OP_EQ_STR) {
               Con::printf("[%i] OP_EQ_STR %i %s %s", j, TS2_OP_DEC_A(i), ts2PrintKonstOrRef(TS2_OP_DEC_B(i), konst), ts2PrintKonstOrRef(TS2_OP_DEC_C(i), konst));
               vmbreak;
            }
            
            vmcase(Compiler::OP_CALL) {
               // [abc] [a := a(a[...b params])[...c return params]]
               U32 returnStart = TS2_OP_DEC_A(i);
               U32 numParams = TS2_OP_DEC_B(i);
               U32 numReturn = TS2_OP_DEC_C(i);
               
               Con::printf("[%i] OP_CALL R%i %i %i", j, returnStart, numParams, numReturn);
               vmbreak;
            }
            
            vmcase(Compiler::OP_BINDNSFUNC) {
               U32 defStart = TS2_OP_DEC_A(i);
               U32 funcIdx = TS2_OP_DEC_B(i);
               U32 dummy = TS2_OP_DEC_C(i);
               
               Con::printf("[%i] OP_BINDNSFUNC R%i %i %i", j, defStart, funcIdx, dummy);
               vmbreak;
            }
            
            vmcase(Compiler::OP_DOCNSFUNC) {
               U32 flags = TS2_OP_DEC_A(i);
               
               Con::printf("[%i] OP_DOCNSFUNC %s %s %i", j, ts2PrintKonstOrRef(TS2_OP_DEC_B(i), konst), ts2PrintKonstOrRef(TS2_OP_DEC_C(i), konst), flags );
               vmbreak;
            }
            
            vmcase(Compiler::OP_COPYFIELDS) {
               Con::printf("[%i] OP_COPYFIELDS R%i <- ", j, TS2_OP_DEC_A(i), TS2_OP_DEC_B(i));
               vmbreak;
            }
            
            vmcase(Compiler::OP_ITR_GET) {
               U32 defStart = TS2_OP_DEC_A(i);
               
               Con::printf("[%i] OP_ITR_GET R%i %s %s", j, defStart, ts2PrintKonstOrRef(TS2_OP_DEC_B(i), konst), ts2PrintKonstOrRef(TS2_OP_DEC_C(i), konst));
               vmbreak;
            }
            
            
            vmcase(Compiler::OP_ITR_SGET) {
               U32 outValue = TS2_OP_DEC_A(i);
               
               Con::printf("[%i] OP_ITR_SGET R%i %s %s", j, outValue, ts2PrintKonstOrRef(TS2_OP_DEC_B(i), konst), ts2PrintKonstOrRef(TS2_OP_DEC_C(i), konst));
               vmbreak;
            }
            
            vmcase(Compiler::OP_RETURN) {
               U32 outValue = TS2_OP_DEC_A(i);
               
               Con::printf("[%i] OP_RETURN R%i %s %s", j, outValue, ts2PrintKonstOrRef(TS2_OP_DEC_B(i), konst), ts2PrintKonstOrRef(TS2_OP_DEC_C(i), konst));
               vmbreak;
            }
            
            vmcase(Compiler::OP_BREAK) {
               Con::printf("[%i] OP_BREAK", j);
            }
            
            vmcase(Compiler::OP_ASSERT) {
               
               Con::printf("[%i] OP_ASSERT %i %s", j, TS2_OP_DEC_A(i), ts2PrintKonstOrRef(TS2_OP_DEC_B(i), konst));
               vmbreak;
            }
      }
   }
}

void CodeBlock::execBlock(CodeBlockEvalState *state)
{
    // Executes code block as normal
    U32 ip = state->currentFrame.savedIP;
    ConsoleValuePtr *konst = state->currentFrame.constants;
    ConsoleValuePtr *konstBase = konst;
    ConsoleValuePtr *base = state->stack.address() + state->currentFrame.stackTop;
    
    ConsoleValuePtr acc;
   acc.type = ConsoleValue::TypeInternalFloat;
   ConsoleValuePtr iacc;
   iacc.type = ConsoleValue::TypeInternalInt;
   U32 end;
   U32 startFrameSize = state->frames.size();
   CodeBlockEvalState *startState = state;
   
   
#ifdef DEBUG_COMPILER
   state->currentFrame.code->dumpOpcodes(state);
#endif
   
   U32 *code = state->currentFrame.code->code;

    for (;;)
    {
        const Compiler::Instruction i = (code[ip++]);
       
        ConsoleValuePtr *ra = base+TS2_OP_DEC_A(i);
        vmdispatch (TS2_OP_DEC(i)) {
           
           
            vmcase(Compiler::OP_MOVE) {
               
#ifdef TS2_VM_DEBUG
               Con::printf("[%i] OP_MOV %i %i", ip-1, ra,TS2_OP_DEC_B(i));
#endif
                //ConsoleValue *v1 = base+ra;
                //ConsoleValue *v2 = base+TS2_OP_DEC_B(i);
               //*v1 = *v2;
               
               //*ra = *(base+TS2_OP_DEC_B(i));
               
               //*ra = *(base+(((i) >> 14) & 0x1FF));
               
               U32 xOffs = TS2_OP_DEC_A(i);
               U32 yOffs = TS2_OP_DEC_B(i);
                base[xOffs].setValue(base[yOffs]);
                vmbreak;
            }
            vmcase(Compiler::OP_LOADK) {
               
//#ifdef TS2_VM_DEBUG
               //Con::printf("[%i] OP_LOADK %i %i", ip-1, TS2_OP_DEC_A(i),TS2_OP_DEC_Bx(i));
//#endif
                ra->setValue(konst[TS2_OP_DEC_Bx(i)]);
                vmbreak;
            }
            vmcase(Compiler::OP_PAGEK) {
               konst = konstBase + (Compiler::CONSTANTS_PER_PAGE * TS2_OP_DEC_Bx(i));
                vmbreak;
            }
           
           vmcase(Compiler::OP_GETFIELD) {
              // a := object(b).slot(c)
              //Con::printf("[%i] OP_GETFIELD R%i %s %s", ip-1, TS2_OP_DEC_A(i),ts2PrintKonstOrRef(TS2_OP_DEC_B(i), konst), ts2PrintKonstOrRef(TS2_OP_DEC_C(i), konst));
              
              
              ConsoleValuePtr &targetObject = base[TS2_OP_DEC_B(i)];
              ConsoleValuePtr &targetSlot = TS2_BASE_OR_KONST(TS2_OP_DEC_C(i));
              ConsoleValuePtr arrayValue;
              
              if (targetObject.type > ConsoleValue::TypeReferenceCounted)
              {
                 targetObject.value.refValue->getDataField(targetSlot.getSTEStringValue(), arrayValue, *ra);
              }
              else
              {
                 SimObject *obj = Sim::findObject<SimObject>(targetObject);
                 if (obj)
                 {
                    ((ConsoleValuePtr*)(ra))->setValue(obj->getDataField(targetSlot.getSTEStringValue(), arrayValue));
                 }
                 else
                 {
                    ((ConsoleValuePtr*)(ra))->setNull();
                 }
              }
              
              vmbreak;
           }
           vmcase(Compiler::OP_GETFIELDA) {
              // a := object(b).slot(c).array(b+1)
              //Con::printf("[%i] OP_GETFIELD R%i %s %s [%s]", ip-1, TS2_OP_DEC_A(i),ts2PrintKonstOrRef(TS2_OP_DEC_B(i), konst), ts2PrintKonstOrRef(TS2_OP_DEC_C(i), konst), ts2PrintKonstOrRef(TS2_OP_DEC_B(i)+1, konst));
              
              ConsoleValuePtr& targetObject = base[TS2_OP_DEC_B(i)];
              ConsoleValuePtr& targetSlot = TS2_BASE_OR_KONST(TS2_OP_DEC_C(i));
              ConsoleValuePtr& targetArray = TS2_BASE_OR_KONST(TS2_OP_DEC_B(i)+1);
              
              if (targetObject.type > ConsoleValue::TypeReferenceCounted)
              {
                 targetObject.value.refValue->getDataField(targetSlot.getSTEStringValue(), targetArray, *ra);
              }
              else
              {
                 SimObject *obj = Sim::findObject<SimObject>(targetObject);
                 if (obj)
                 {
                     ((ConsoleValuePtr*)(ra))->setValue(obj->getDataField(targetSlot.getSTEStringValue(), targetArray));
                 }
                 else
                 {
                    ((ConsoleValuePtr*)(ra))->setNull();
                 }
              }
              
              vmbreak;
           }
           
            vmcase(Compiler::OP_LOADVAR) {
               // A := B
#ifdef TS2_VM_DEBUG
               Con::printf("[%i] OP_LOADVAR %i %i", ip-1, ra,TS2_OP_DEC_B(i));
#endif
                ((ConsoleValuePtr*)(ra))->setValue(Con::getConsoleValueVariable(TS2_BASE_OR_KONST(TS2_OP_DEC_B(i)).getTempStringValue()));
                vmbreak;
            }
           
           
            vmcase(Compiler::OP_SETFIELD) {
              // a := object(b).slot(c)
               ConsoleValuePtr& targetObject = base[TS2_OP_DEC_A(i)];
               ConsoleValuePtr& targetSlot = TS2_BASE_OR_KONST(TS2_OP_DEC_B(i));
               ConsoleValuePtr& newValue = TS2_BASE_OR_KONST(TS2_OP_DEC_C(i));
               ConsoleValuePtr arrayValue;
               
               // We use two methods here:
               // 1) If we have a basic type, assume we have a SimObject
               // 2) If we have a custom type, use that types field accessors (this will default to SimObject)
               if (targetObject.type > ConsoleValue::TypeReferenceCounted)
               {
                  ((ConsoleValuePtr*)&targetObject)->value.refValue->setDataField(targetSlot.getSTEStringValue(), arrayValue, newValue);
                  //*((ConsoleValuePtr*)(ra)) = newValue;
               }
               else
               {
                  SimObject *obj = Sim::findObject<SimObject>(targetObject);
                  if (obj)
                  {
                     obj->setDataField(targetSlot.getSTEStringValue(), arrayValue, newValue);
                  }
               }
               
              vmbreak;
            }
            vmcase(Compiler::OP_SETFIELDA) {
              // a.b[c+1] := c
					
               ConsoleValuePtr& targetObject = base[TS2_OP_DEC_A(i)];
               ConsoleValuePtr& targetSlot = TS2_BASE_OR_KONST(TS2_OP_DEC_B(i));
               ConsoleValuePtr& targetArray = base[TS2_OP_DEC_C(i)+1];
               ConsoleValuePtr& newValue = base[TS2_OP_DEC_C(i)];
               
               if (targetObject.type > ConsoleValue::TypeReferenceCounted)
               {
                  ((ConsoleValuePtr)targetObject).value.refValue->setDataField(targetSlot.getSTEStringValue(), targetArray, newValue);
               }
               else
               {
                  SimObject *obj = Sim::findObject<SimObject>(targetObject);
                  if (obj)
                  {
                     obj->setDataField(targetSlot.getSTEStringValue(), targetArray, newValue);
                  }
                  else
                  {
                     Con::warnf("OP_SETFIELDA object '%s' not found", targetObject.getTempStringValue());
                  }
               }
               
               vmbreak;
            }
           
            vmcase(Compiler::OP_SETVAR) {
                // B := C
               
#ifdef TS2_VM_DEBUG
               Con::printf("[%i] OP_SETVAR %i %i %i", ip-1, ra,TS2_OP_DEC_B(i),TS2_OP_DEC_C(i));
#endif
                ConsoleValuePtr &namePtr = TS2_BASE_OR_KONST(TS2_OP_DEC_B(i));
                ConsoleValuePtr &valuePtr = TS2_BASE_OR_KONST(TS2_OP_DEC_C(i));
               
                Con::setValueVariable(namePtr.getSTEStringValue(), valuePtr);
                vmbreak;
            }
           
           vmcase(Compiler::OP_GETFUNC) {
              U32 setReg = TS2_OP_DEC_A(i);
              ConsoleValuePtr &targetObject = TS2_BASE_OR_KONST(TS2_OP_DEC_B(i));
              ConsoleValuePtr &nameId = TS2_BASE_OR_KONST(TS2_OP_DEC_C(i));
              
              
#ifdef TS2_VM_DEBUG
              Con::printf("[%i] OP_GETFUNC %s %s -> %i", ip-1, ra);
#endif
              
              if (targetObject.type > ConsoleValue::TypeReferenceCounted)
              {
                 Namespace *ns = targetObject.value.refValue->getNamespace();
                 if (!ns)
                 {
                    ns = targetObject.value.refValue->getNamespace();
                    Con::errorf("Couldn't find namespace for %s.", targetObject.getTempStringValue());
                    vmbreak;
                 }
                 Namespace::Entry *entry = ns->lookup(nameId.getSTEStringValue());
                 
                 if (!entry)
                 {
                    Con::warnf("Couldn't find function %s in object %s (%s)", nameId.getSTEStringValue(), targetObject.getTempStringValue(), state->currentFrame.code->getFileLine(ip-1));
                    base[setReg].setNull();
                 }
                 else
                 {
                    base[setReg].setNull();
                    base[setReg].type = ConsoleValue::TypeInternalNamespaceEntry;
                    base[setReg].value.ptrValue = entry;
                 }
              }
              else if (targetObject.type == ConsoleValue::TypeInternalNamespaceName)
              {
                 StringTableEntry steValue = targetObject.getSTEStringValue();
                 Namespace *ns = steValue == StringTable->EmptyString ? Namespace::global() : Namespace::find(targetObject.getSTEStringValue());
                 Namespace::Entry *entry = ns->lookup(nameId.getSTEStringValue());
                 
                 if (!entry)
                 {
                    if (steValue == StringTable->EmptyString)
                    {
                       Con::errorf("Couldn't find global function %s (%s)", nameId.getSTEStringValue(), state->currentFrame.code->getFileLine(ip-1));
                    }
                    else
                    {
                       Con::errorf("Couldn't find function %s::%s (%s)", steValue, nameId.getSTEStringValue(), state->currentFrame.code->getFileLine(ip-1));
                    }
                    base[setReg].setNull();
                 }
                 else
                 {
                    base[setReg].setNull();
                    base[setReg].type = ConsoleValue::TypeInternalNamespaceEntry;
                    base[setReg].value.ptrValue = entry;
                 }
              }
              else
              {
                 SimObject *object = Sim::findObject<SimObject>(targetObject);
                 if (object)
                 {
                    Namespace *ns = object->getNamespace();
                    Namespace::Entry *entry = ns->lookup(nameId.getSTEStringValue());
                    
                    if (!entry)
                    {
                       Con::errorf("Object %s has no function %s", targetObject.getTempStringValue(), nameId.getSTEStringValue());
                       base[setReg].setNull();
                    }
                    else
                    {
                       base[setReg].setNull();
                       base[setReg].type = ConsoleValue::TypeInternalNamespaceEntry;
                       base[setReg].value.ptrValue = entry;
                    }
                 }
                 else
                 {
                    Con::errorf("Object %s does not exist, cannot call %s.", targetObject.getTempStringValue(), nameId.getSTEStringValue());
                    base[setReg].setNull();
                 }
              }
              
              vmbreak;
           }
           
           vmcase(Compiler::OP_GETFUNC_NS) {
              U32 setReg = TS2_OP_DEC_A(i);
              ConsoleValuePtr &targetObject = TS2_BASE_OR_KONST(TS2_OP_DEC_B(i));
              ConsoleValuePtr &nameId = TS2_BASE_OR_KONST(TS2_OP_DEC_C(i));
              
              
#ifdef TS2_VM_DEBUG
              Con::printf("[%i] OP_GETFUNC_NS %s %s -> %i", ip-1, ra);
#endif
              
                 Namespace *ns = Namespace::find(targetObject.getSTEStringValue());
                 if (!ns)
                 {
                    ns = targetObject.value.refValue->getNamespace();
                    Con::errorf("Couldn't find namespace for %s.", targetObject.getTempStringValue());
                    vmbreak;
                 }
                 StringTableEntry steValue = nameId.getSTEStringValue();
                 Namespace::Entry *entry = ns->lookup(nameId.getSTEStringValue());
                 
                 if (!entry)
                 {
                    if (steValue == StringTable->EmptyString)
                    {
                       Con::errorf("Couldn't find global function %s (%s)", nameId.getSTEStringValue(), state->currentFrame.code->getFileLine(ip-1));
                    }
                    else
                    {
                       Con::errorf("Couldn't find function %s::%s (%s)", steValue, nameId.getSTEStringValue(), state->currentFrame.code->getFileLine(ip-1));
                    }
                    base[setReg].setNull();
                 }
                 else
                 {
                    base[setReg].setNull();
                    base[setReg].type = ConsoleValue::TypeInternalNamespaceEntry;
                    base[setReg].value.ptrValue = entry;
                 }
              
              vmbreak;
           }
           
           
           vmcase(Compiler::OP_GETINTERNAL) {
              U32 setReg = TS2_OP_DEC_A(i);
              ConsoleValuePtr &targetObject = TS2_BASE_OR_KONST(TS2_OP_DEC_B(i));
              ConsoleValuePtr &nameId = TS2_BASE_OR_KONST(TS2_OP_DEC_C(i));
              
              SimSet *object = Sim::findObject<SimSet>(targetObject);
              if (object)
              {
                 SimObject *findObject = object->findObjectByInternalName(nameId.getSTEStringValue(), false);
                 if (findObject)
                 {
                    base[setReg].setValue(ConsoleSimObjectPtr::fromObject(findObject));
                    vmbreak;
                 }
              }
              
              base[setReg].setNull();
              vmbreak;
           }
           
           vmcase(Compiler::OP_GETINTERNAL_N) {
              U32 setReg = TS2_OP_DEC_A(i);
              ConsoleValuePtr &targetObject = TS2_BASE_OR_KONST(TS2_OP_DEC_B(i));
              ConsoleValuePtr &nameId = TS2_BASE_OR_KONST(TS2_OP_DEC_C(i));
              
              SimSet *object = Sim::findObject<SimSet>(targetObject);
              if (object)
              {
                 SimObject *findObject = object->findObjectByInternalName(nameId.getSTEStringValue(), true);
                 if (findObject)
                 {
                    base[setReg].setValue(ConsoleSimObjectPtr::fromObject(findObject));
                    vmbreak;
                 }
              }
              
              base[setReg].setNull();
              vmbreak;
           }
           
           
           vmcase(Compiler::OP_CREATE_VALUE) {
              ConsoleValuePtr &typeName = TS2_BASE_OR_KONST(TS2_OP_DEC_B(i));
              StringTableEntry typeSTE = typeName.getSTEStringValue();
              
              ConsoleBaseType *typeInst = ConsoleBaseType::getTypeByName(typeSTE);
              if (!typeInst)
              {
                 Con::errorf("Could not create value of type %s", typeSTE);
                 ((ConsoleValuePtr*)ra)->setNull();
                 vmbreak;
              }
              
              ConsoleReferenceCountedType *typeRef = typeInst->createReferenceCountedValue();
              
              if (!typeRef)
              {
                 Con::errorf("Could not create value of type %s", typeSTE);
                 ((ConsoleValuePtr*)ra)->setNull();
                 vmbreak;
              }
              
              ((ConsoleValuePtr*)ra)->setValue(typeRef);
              vmbreak;
           }
           
           vmcase(Compiler::OP_CREATE_OBJECT) {
              U32 flagArgs = TS2_OP_DEC_C(i);
              U32 flags = (flagArgs >> 8);
              U32 numParams = flagArgs & 0xFF;
              bool isDataBlock = flags & Compiler::CREATEOBJECT_ISDATABLOCK;
              bool isInternal  = flags & Compiler::CREATEOBJECT_ISINTERNAL;
              bool isSingleton = flags & Compiler::CREATEOBJECT_ISSINGLETON;
              
              S32 failJump = TS2_OP_DEC_sBx(code[ip++]);
              failJump += ip;
				  
              U32 classNameTargetReg = TS2_OP_DEC_B(i);
              
              StringTableEntry klassName = base[classNameTargetReg].getSTEStringValue();//callArgv[ 2 ];
              StringTableEntry objectName = base[classNameTargetReg+1].getSTEStringValue();//callArgv[ 2 ];
              
#ifdef DEBUG_COMPILER
              Con::printf("Creating object... %s of klass %s", objectName, klassName);
#endif
              // objectName = argv[1]...
              SimObject *currentNewObject = NULL;
              
              // Are we creating a datablock? If so, deal with case where we override
              // an old one.
              if(isDataBlock)
              {
                 // Con::printf("  - is a datablock");
                 
                 // Find the old one if any.
                 SimObject *db = Sim::getDataBlockGroup()->findObject(objectName);
                 
                 // Make sure we're not changing types on ourselves...
                 if(db && dStricmp(db->getClassName(), klassName))
                 {
                    Con::errorf(ConsoleLogEntry::General, "Cannot re-declare data block %s with a different class.", objectName);
                    ip = failJump;
                    vmbreak;
                 }
                 
                 // If there was one, set the currentNewObject and move on.
                 if(db)
                    currentNewObject = db;
              }
              else if (isSingleton)
              {
                 // For singletons the object may already be present
                 currentNewObject = Sim::findObject(objectName);
              }
              
              if(!currentNewObject)
              {
                 // Well, looks like we have to create a new object.
                 ConsoleObject *object = ConsoleObject::create(klassName);
                 
                 // Deal with failure!
                 if(!object)
                 {
                    Con::errorf(ConsoleLogEntry::General, "%s: Unable to instantiate non-conobject class %s.", state->currentFrame.code->getFileLine(ip-1), klassName);
                    ip = failJump;
                    vmbreak;
                 }
                 
                 // Do special datablock init if appropros
                 if(isDataBlock)
                 {
                    SimDataBlock *dataBlock = dynamic_cast<SimDataBlock *>(object);
                    if(dataBlock)
                    {
                       dataBlock->assignId();
                    }
                    else
                    {
                       // They tried to make a non-datablock with a datablock keyword!
                       Con::errorf(ConsoleLogEntry::General, "%s: Unable to instantiate non-datablock class %s.", state->currentFrame.code->getFileLine(ip-1), klassName);
                       
                       // Clean up...
                       delete object;
                       ip = failJump;
                       vmbreak;
                    }
                 }
                 
                 // Finally, set currentNewObject to point to the new one.
                 currentNewObject = dynamic_cast<SimObject *>(object);
                 
                 // Deal with the case of a non-SimObject.
                 if(!currentNewObject)
                 {
                    Con::errorf(ConsoleLogEntry::General, "%s: Unable to instantiate non-SimObject class %s.", state->currentFrame.code->getFileLine(ip-1), klassName);
                    delete object;
                    ip = failJump;
                    vmbreak;
                 }
                 
                 // If a name was passed, assign it.
                 if( objectName[ 0 ] )
                 {
                    if( !isInternal )
                       currentNewObject->assignName( objectName );
                    else
                       currentNewObject->setInternalName( objectName );
                    
                    // Set the original name
                    //currentNewObject->setOriginalName( objectName );
                 }
                 
                 // Do the constructor parameters.
                 if(!currentNewObject->processArguments(numParams, base+classNameTargetReg+2))
                 {
                    delete currentNewObject;
                    currentNewObject = NULL;
                    ip = failJump;
                    vmbreak;
                 }
                 
                 // If it's not a datablock, allow people to modify bits of it.
                 if(!isDataBlock)
                 {
                    currentNewObject->setModStaticFields(true);
                    currentNewObject->setModDynamicFields(true);
                 }
                 
                 ((ConsoleValuePtr*)ra)->setValue(ConsoleSimObjectPtr::fromObject(currentNewObject));
              }
              
              vmbreak;
           }
           
           vmcase(Compiler::OP_FINISH_OBJECT) {
              // registerObject(A), B.addObject(A), [flags(c)]
              ConsoleValuePtr &targetObject = base[TS2_OP_DEC_A(i)];
              ConsoleValuePtr &parentObject = TS2_BASE_OR_KONST(TS2_OP_DEC_B(i));
              
              SimObject *currentNewObject;
              currentNewObject = Sim::findObject<SimObject>(targetObject);
                 
              if(currentNewObject->isProperlyAdded() == false)
              {
                 bool ret = false;
                 
                 Message *msg = dynamic_cast<Message *>(currentNewObject);
                 if(msg)
                 {
                    SimObjectId id = Message::getNextMessageID();
                    if(id != 0xffffffff)
                       ret = currentNewObject->registerObject(id);
                    else
                       Con::errorf("%s: No more object IDs available for messages", state->currentFrame.code->getFileLine(ip-2));
                 }
                 else
                    ret = currentNewObject->registerObject();
                 
                 if(! ret)
                 {
                    // This error is usually caused by failing to call Parent::initPersistFields in the class' initPersistFields().
                    Con::warnf(ConsoleLogEntry::General, "%s: Register object failed for object %s of class %s.", state->currentFrame.code->getFileLine(ip-2), currentNewObject->getName(), currentNewObject->getClassName());
                    delete currentNewObject;
                    //ip = failJump;
                    vmbreak;
                 }
              }
              
              // Are we dealing with a datablock?
              SimDataBlock *dataBlock = dynamic_cast<SimDataBlock *>(currentNewObject);
              static char errorBuffer[256];
              
              // If so, preload it.
              if(dataBlock && !dataBlock->preload(true, errorBuffer))
              {
                 Con::errorf(ConsoleLogEntry::General, "%s: preload failed for %s: %s.", state->currentFrame.code->getFileLine(ip-2),
                             currentNewObject->getName(), errorBuffer);
                 dataBlock->deleteObject();
                 //ip = failJump;
                 vmbreak;
              }
              
              // What group will we be added to, if any?
              SimGroup *grp = NULL;
              SimSet   *set = NULL;
              SimComponent *comp = NULL;
              bool isMessage = dynamic_cast<Message *>(currentNewObject) != NULL;
              bool placeAtRoot = parentObject.type == ConsoleValue::TypeInternalNull;
              
              if(!placeAtRoot || !currentNewObject->getGroup())
              {
                 if(! isMessage)
                 {
                    if(! placeAtRoot)
                    {
                       // Otherwise just add to the requested group or set.
                       if(!Sim::findObject(parentObject, grp))
                          if(!Sim::findObject(parentObject, comp))
                             Sim::findObject(parentObject, set);
                    }
                    
                    if(placeAtRoot || comp != NULL)
                    {
                       // Deal with the instantGroup if we're being put at the root or we're adding to a component.
                       ConsoleStringValuePtr addGroupName = Con::getVariable("instantGroup");
                       if(!Sim::findObject(addGroupName.c_str(), grp))
                          Sim::findObject(RootGroupId, grp);
                    }
                    
                    if(comp)
                    {
                       SimComponent *newComp = dynamic_cast<SimComponent *>(currentNewObject);
                       if(newComp)
                       {
                          if(! comp->addComponent(newComp))
                             Con::errorf("%s: Unable to add component %s, template not loaded?", state->currentFrame.code->getFileLine(ip-2), currentNewObject->getName() ? currentNewObject->getName() : currentNewObject->getIdString());
                       }
                    }
                 }
                 
                 // If we didn't get a group, then make sure we have a pointer to
                 // the rootgroup.
                 if(!grp)
                    Sim::findObject(RootGroupId, grp);
                 
                 // add to the parent group
                 grp->addObject(currentNewObject);
                 
                 // add to any set we might be in
                 if(set)
                    set->addObject(currentNewObject);
              }
              vmbreak;
           }
           
           vmcase(Compiler::OP_DELETE_OBJECT) {
              // delete A
              Con::printf("[%i] OP_DELETE_OBJECT R%i", ip-1, TS2_OP_DEC_A(i));
              vmbreak;
           }
           
           //Con::printf(#_op "[%i] -> %i", ip-1,  targetRegTmp);
#define TS2_INT_OP(_op, _operand) \
           vmcase(_op) { \
              iacc.value.ival = (TS2_BASE_OR_KONST(TS2_OP_DEC_B(i)).getIntValue() _operand TS2_BASE_OR_KONST(TS2_OP_DEC_C(i)).getIntValue()); \
              ra->setValue(iacc); \
              vmbreak; \
           }
           
               //Con::printf(#_op "[%i] -> (%i & %i) (%f & %f) -> %i", ip-1, TS2_OP_DEC_B(i), TS2_OP_DEC_C(i), TS2_BASE_OR_KONST(TS2_OP_DEC_B(i)).getFloatValue() , TS2_BASE_OR_KONST(TS2_OP_DEC_C(i)).getFloatValue(), targetRegTmp);
#define TS2_FLOAT_OP(_op, _operand) \
            vmcase(_op) { \
                acc.value.fval = (TS2_BASE_OR_KONST(TS2_OP_DEC_B(i)).getFloatValue() _operand TS2_BASE_OR_KONST(TS2_OP_DEC_C(i)).getFloatValue()); \
                ra->setValue(acc); \
                vmbreak; \
            }
            
           //TS2_FLOAT_OP(Compiler::OP_ADD, +)
           vmcase(Compiler::OP_ADD) {
              //Con::printf("Compiler::OP_ADD" "[%i] -> (%i & %i) (%f & %f) -> %i", ip-1, TS2_OP_DEC_B(i), TS2_OP_DEC_C(i), TS2_BASE_OR_KONST(TS2_OP_DEC_B(i)).getFloatValue() , TS2_BASE_OR_KONST(TS2_OP_DEC_C(i)).getFloatValue(), targetRegTmp);
              acc.value.fval = (TS2_BASE_OR_KONST(TS2_OP_DEC_B(i)).getFloatValue() + TS2_BASE_OR_KONST(TS2_OP_DEC_C(i)).getFloatValue());
              ((ConsoleValuePtr*)ra)->setValue(acc);
              vmbreak;
           }
           
           //TS2_FLOAT_OP(Compiler::OP_SUB, -)
           vmcase(Compiler::OP_SUB) {
              //Con::printf("Compiler::OP_SUB" "[%i] -> (%i & %i) (%f & %f) -> %i", ip-1, TS2_OP_DEC_B(i), TS2_OP_DEC_C(i), TS2_BASE_OR_KONST(TS2_OP_DEC_B(i)).getFloatValue() , TS2_BASE_OR_KONST(TS2_OP_DEC_C(i)).getFloatValue(), targetRegTmp);
              acc.value.fval = (TS2_BASE_OR_KONST(TS2_OP_DEC_B(i)).getFloatValue() - TS2_BASE_OR_KONST(TS2_OP_DEC_C(i)).getFloatValue());
              ra->setValue(acc);
              vmbreak;
           }
           
           
            TS2_FLOAT_OP(Compiler::OP_MUL, *)
            TS2_FLOAT_OP(Compiler::OP_DIV, /)
            TS2_INT_OP(Compiler::OP_MOD, %)
           
           vmcase(Compiler::OP_POW) {
              //Con::printf("Compiler::OP_SUB" "[%i] -> (%i & %i) (%f & %f) -> %i", ip-1, TS2_OP_DEC_B(i), TS2_OP_DEC_C(i), TS2_BASE_OR_KONST(TS2_OP_DEC_B(i)).getFloatValue() , TS2_BASE_OR_KONST(TS2_OP_DEC_C(i)).getFloatValue(), targetRegTmp);
              acc.value.fval = mPow(TS2_BASE_OR_KONST(TS2_OP_DEC_B(i)).getFloatValue(), TS2_BASE_OR_KONST(TS2_OP_DEC_C(i)).getFloatValue());
              ra->setValue(acc);
              vmbreak;
           }
           
           vmcase(Compiler::OP_UMN) {
              //Con::printf("Compiler::OP_SUB" "[%i] -> (%i & %i) (%f & %f) -> %i", ip-1, TS2_OP_DEC_B(i), TS2_OP_DEC_C(i), TS2_BASE_OR_KONST(TS2_OP_DEC_B(i)).getFloatValue() , TS2_BASE_OR_KONST(TS2_OP_DEC_C(i)).getFloatValue(), targetRegTmp);
              acc.value.fval = -(TS2_BASE_OR_KONST(TS2_OP_DEC_B(i)).getFloatValue());
              ra->setValue(acc);
              vmbreak;
           }
           
           vmcase(Compiler::OP_NOT) {
              //Con::printf("Compiler::OP_SUB" "[%i] -> (%i & %i) (%f & %f) -> %i", ip-1, TS2_OP_DEC_B(i), TS2_OP_DEC_C(i), TS2_BASE_OR_KONST(TS2_OP_DEC_B(i)).getFloatValue() , TS2_BASE_OR_KONST(TS2_OP_DEC_C(i)).getFloatValue(), targetRegTmp);
              acc.value.fval = !(TS2_BASE_OR_KONST(TS2_OP_DEC_B(i)).getFloatValue());
              ra->setValue(acc);
              vmbreak;
           }
           
            TS2_INT_OP(Compiler::OP_XOR, ^)
            TS2_INT_OP(Compiler::OP_SHL, <<)
            TS2_INT_OP(Compiler::OP_SHR, >>)
            TS2_INT_OP(Compiler::OP_BITAND, &)
            TS2_INT_OP(Compiler::OP_BITOR, |)
           
           
           vmcase(Compiler::OP_ONESCOMPLEMENT) {
              //Con::printf("Compiler::OP_SUB" "[%i] -> (%i & %i) (%f & %f) -> %i", ip-1, TS2_OP_DEC_B(i), TS2_OP_DEC_C(i), TS2_BASE_OR_KONST(TS2_OP_DEC_B(i)).getFloatValue() , TS2_BASE_OR_KONST(TS2_OP_DEC_C(i)).getFloatValue(), targetRegTmp);
              acc.value.ival = ~((U64)(TS2_BASE_OR_KONST(TS2_OP_DEC_B(i)).getFloatValue()));
              *ra = iacc;
              vmbreak;
           }
            
            
            vmcase(Compiler::OP_CONCAT) {
               
#ifdef TS2_VM_DEBUG
               Con::printf("[%i] OP_CONCAT %i..%i", ip-1, TS2_OP_DEC_B(i), TS2_OP_DEC_C(i));
#endif
               
                // A := B...C
                end = TS2_OP_DEC_C(i);
                for (U32 j=TS2_OP_DEC_B(i); j<=end; j++)
                {
                   STR.setStringValue(base[j].getTempStringValue());
                   STR.advance();
                }
                STR.rewindTerminate();
                for (U32 j=TS2_OP_DEC_B(i)+1; j<=end; j++)
                {
                   STR.rewind();
                }
                *((ConsoleValuePtr*)ra) = ConsoleStringValue::fromString(STR.getStringValue());
                vmbreak;
            }
            
            vmcase(Compiler::OP_JMP) {
               //U32 rawIp = TS2_OP_DEC_Bx(i);
               //S32 ipAddr = TS2_OP_DEC_sBx(i);
#ifdef TS2_VM_DEBUG
               Con::printf("[%i] OP_JMP %i", ip-1, ipAddr);
#endif
                ip += TS2_OP_DEC_sBx(i);
                vmbreak;
            }
           
           vmcase(Compiler::OP_LT) {
              ConsoleValue v1 = TS2_BASE_OR_KONST(TS2_OP_DEC_B(i));
              ConsoleValue v2 = TS2_BASE_OR_KONST(TS2_OP_DEC_C(i));
              
#ifdef TS2_VM_DEBUG
              Con::printf("[%i] OP_LT %i %i %i", ip-1, ra, TS2_OP_DEC_B(i), TS2_OP_DEC_C(i));
#endif
              if ((v1.getFloatValue() < v2.getFloatValue()) != TS2_OP_DEC_A(i))
              {
                 // skip JMP (i.e. follow true branch)
                 ip++;
              }
              else
              {
                 // Perform next JMP
                 S32 relJmp = TS2_OP_DEC_sBx(code[ip])+1;
                 ip += relJmp;
              }
              vmbreak;
           }
           
           vmcase(Compiler::OP_LE) {
              ConsoleValue v1 = TS2_BASE_OR_KONST(TS2_OP_DEC_B(i));
              ConsoleValue v2 = TS2_BASE_OR_KONST(TS2_OP_DEC_C(i));
              
#ifdef TS2_VM_DEBUG
              Con::printf("[%i] OP_LE %i %i %i", ip-1, ra, TS2_OP_DEC_B(i), TS2_OP_DEC_C(i));
#endif
              
              if ((v1.getFloatValue() <= v2.getFloatValue()) != TS2_OP_DEC_A(i))
              {
                 // skip JMP (i.e. follow true branch)
                 ip++;
              }
              else
              {
                 // Perform next JMP
                 ip += TS2_OP_DEC_sBx(code[ip])+1;
              }
              vmbreak;
           }
           
           vmcase(Compiler::OP_EQ) {
              ConsoleValue v1 = TS2_BASE_OR_KONST(TS2_OP_DEC_B(i));
              ConsoleValue v2 = TS2_BASE_OR_KONST(TS2_OP_DEC_C(i));
              
#ifdef TS2_VM_DEBUG
              Con::printf("[%i] OP_EQ %i %i %i", ip-1, ra, TS2_OP_DEC_B(i), TS2_OP_DEC_C(i));
#endif
              
              if ((v1.getFloatValue() == v2.getFloatValue()) != TS2_OP_DEC_A(i))
              {
                 // skip JMP (i.e. follow true branch)
                 ip++;
              }
              else
              {
                 // Perform next JMP
                 ip += TS2_OP_DEC_sBx(code[ip])+1;
              }
              vmbreak;
           }
           
           vmcase(Compiler::OP_EQ_STR) {
              ConsoleValue v1 = TS2_BASE_OR_KONST(TS2_OP_DEC_B(i));
              ConsoleValue v2 = TS2_BASE_OR_KONST(TS2_OP_DEC_C(i));
              
#ifdef TS2_VM_DEBUG
              Con::printf("[%i] OP_EQ_STR %i %i %i", ip-1, ra, TS2_OP_DEC_B(i), TS2_OP_DEC_C(i));
#endif
              if (ts2_eq_str(&v1, &v2) != TS2_OP_DEC_A(i))
              {
                 // skip JMP (i.e. follow true branch)
                 ip++;
              }
              else
              {
                 // Perform next JMP
                 ip += TS2_OP_DEC_sBx(code[ip])+1;
              }
              vmbreak;
           }
           
            vmcase(Compiler::OP_CALL) {
                // [abc] [a := a(a[...b params])[...c return params]]
                U32 returnStart = TS2_OP_DEC_A(i);
                U32 numParams = TS2_OP_DEC_B(i);
               
               
#ifdef TS2_VM_DEBUG
               Con::printf("[%i] OP_CALL %i %i %i", ip-1, returnStart, numParams, numReturn);
#endif
               
                if (base[returnStart].type != ConsoleValue::TypeInternalNamespaceEntry)
                {
#ifdef COMPILER_DEBUG
                    Con::errorf("Invalid namespace entry!");
#endif
                    base[returnStart].setNull();
                    continue;
                }
                
                Namespace::Entry *nsEntry = static_cast<Namespace::Entry*>(base[returnStart].value.ptrValue);
                
                // Now we have to call it!
                S32 nsType = -1;
                S32 nsMinArgs = 0;
                S32 nsMaxArgs = 0;
                Namespace::Entry::CallbackUnion *nsCb = NULL;
                const char *nsUsage = NULL;
                if (nsEntry)
                {
                    nsType = nsEntry->mType;
                    nsMinArgs = nsEntry->mMinArgs;
                    nsMaxArgs = nsEntry->mMaxArgs;
                    nsCb = &nsEntry->cb;
                    nsUsage = nsEntry->mUsage;
                }
               
               {
                  const char *nsName = "";
                  if((nsEntry->mMinArgs && S32(numParams+1) < nsEntry->mMinArgs) || (nsEntry->mMaxArgs && S32(numParams+1) > nsEntry->mMaxArgs))
                  {
                     Con::warnf(ConsoleLogEntry::Script, "%s: %s::%s - wrong number of arguments.", state->currentFrame.code->getFileLine(ip), nsName, nsEntry->mFunctionName);
                     Con::warnf(ConsoleLogEntry::Script, "%s: usage: %s", state->currentFrame.code->getFileLine(ip), nsEntry->mUsage);
                  }
                  else
                  {
                     // TOFIX
                     SimObject *thisObject = numParams > 0 ? base[returnStart+1].getSimObject() : NULL;
                     
                     state->currentFrame.returnReg = returnStart;
                     state->currentFrame.savedIP = ip;
                     state->currentFrame.constantTop = (U32)(konst - konstBase);
                     state->currentFrame.stackTop = base - state->stack.address();
                     
                     switch(nsEntry->mType)
                     {
                        case Namespace::Entry::ScriptFunctionType:
                        {
                           //Con::printf("Call %s", nsEntry->mFunctionName);
                           CodeBlockFunction *newFunc = nsEntry->mCode->mFunctions[nsEntry->mFunctionOffset];
                           state->pushFunction(newFunc, nsEntry->mCode, nsEntry, numParams);
                           
                           // Set appropriate state based on current frame
                           state = CodeBlockEvalState::getCurrent(); // in case coroutine is set
                           ip = state->currentFrame.savedIP;
                           konstBase = state->currentFrame.constants;
                           konst = konstBase + state->currentFrame.constantTop;
                           code = state->currentFrame.code->code;
                           base = state->stack.address() + state->currentFrame.stackTop;
                           
                           vmbreak;
                        }
                        case Namespace::Entry::StringCallbackType:
                        {
                           ConsoleStringValuePtr ret = nsEntry->cb.mStringCallbackFunc(thisObject, numParams+1, base+returnStart);
                           
                           base = state->stack.address() + state->currentFrame.stackTop;
                           base[state->currentFrame.returnReg].setValue(ret.value);
                           vmbreak;
                        }
                        case Namespace::Entry::IntCallbackType:
                        {
                           S32 result = nsEntry->cb.mIntCallbackFunc(thisObject, numParams+1, base+returnStart);
                           
                           base = state->stack.address() + state->currentFrame.stackTop;
                           base[state->currentFrame.returnReg].setValue(result);
                           vmbreak;
                        }
                        case Namespace::Entry::FloatCallbackType:
                        {
                           F64 result = nsEntry->cb.mFloatCallbackFunc(thisObject, numParams+1, base+returnStart);
                           
                           base = state->stack.address() + state->currentFrame.stackTop;
                           base[state->currentFrame.returnReg].setValue((F32)result);
                           vmbreak;
                        }
                        case Namespace::Entry::VoidCallbackType:
                        {
                           nsEntry->cb.mVoidCallbackFunc(thisObject, numParams+1, base+returnStart);
                           
                           base = state->stack.address() + state->currentFrame.stackTop;
                           base[state->currentFrame.returnReg].setNull();
                           vmbreak;
                        }
                        case Namespace::Entry::ValueCallbackType:
                        {
                           ConsoleValuePtr result = nsEntry->cb.mValueCallbackFunc(thisObject, numParams+1, base+returnStart);
                           
                           // Set appropriate state based on current frame
                           // (in case continuation stuff or script has changed it)
                           state = CodeBlockEvalState::getCurrent(); // in case coroutine is set
                           ip = state->currentFrame.savedIP;
                           konstBase = state->currentFrame.constants;
                           konst = konstBase + state->currentFrame.constantTop;
                           code = state->currentFrame.code->code;
                           base = state->stack.address() + state->currentFrame.stackTop;
                           
                           base[state->currentFrame.returnReg].setValue(result);
                           break;
                        }
                        case Namespace::Entry::BoolCallbackType:
                        {
                           bool result = nsEntry->cb.mBoolCallbackFunc(thisObject, numParams+1, base+returnStart);
                           
                           base = state->stack.address() + state->currentFrame.stackTop;
                           base[state->currentFrame.returnReg].setValue(result ? 1 : 0);
                           vmbreak;
                        }
                     }
                     
                     // Set appropriate state based on current frame
                     // (in case continuation stuff or script has changed it)
                     state = CodeBlockEvalState::getCurrent(); // in case coroutine is set
                     ip = state->currentFrame.savedIP;
                     konstBase = state->currentFrame.constants;
                     konst = konstBase + state->currentFrame.constantTop;
                     code = state->currentFrame.code->code;
                     base = state->stack.address() + state->currentFrame.stackTop;
                  }
               }
               
                vmbreak;
            }
           
           vmcase(Compiler::OP_BINDNSFUNC) {
              U32 defStart = TS2_OP_DEC_A(i);
              U32 funcIdx = TS2_OP_DEC_B(i);
              //CodeBlockFunction *func = state->currentFrame.code->mFunctions[funcIdx];
           
              StringTableEntry fnPackage    = base[defStart].getSTEStringValue();
              StringTableEntry fnNamespace  = base[defStart+1].getSTEStringValue();
              StringTableEntry fnName       = base[defStart+2].getSTEStringValue();
              
              if (fnNamespace == StringTable->EmptyString)
              {
                 fnNamespace = NULL;
              }
              
              Namespace::unlinkPackages();
              Namespace *ns = Namespace::find(fnNamespace, fnPackage);
              ns->addFunction(fnName, state->currentFrame.code, funcIdx, NULL );// TODO: docblock
              Namespace::relinkPackages();
              
              //Con::printf("Adding function %s::%s (%d)", fnNamespace, fnName, func->ip);
              vmbreak;
           }
           
           vmcase(Compiler::OP_COPYFIELDS) {
              ConsoleValuePtr &destObject = base[TS2_OP_DEC_A(i)];
              ConsoleValuePtr &srcObject = TS2_BASE_OR_KONST(TS2_OP_DEC_B(i));
              
              SimObject *dstObj = Sim::findObject<SimObject>(destObject);
              SimObject *srcObj = Sim::findObject<SimObject>(srcObject);
              
              if (dstObj && srcObj)
              {
                 //Con::printf("Copying fields to %s from %s", dstObj->getName(), srcObj->getName());
                 dstObj->assignFieldsFrom(srcObj);
              }
              else
              {
                 if (srcObj)
                    Con::warnf("CopyFields: couldn't find destination object %s", destObject.getTempStringValue());
                 else
                    Con::warnf("CopyFields: couldn't src object %s", srcObject.getTempStringValue());
              }
              
              vmbreak;
           }
           
           vmcase(Compiler::OP_DOCNSFUNC) {
              ConsoleValuePtr docNS = TS2_BASE_OR_KONST(TS2_OP_DEC_B(i));
              ConsoleValuePtr docSTR = TS2_BASE_OR_KONST(TS2_OP_DEC_C(i));
              
              if (docNS.type == ConsoleValue::TypeInternalNamespaceName)
              {
                 // @class
                 Namespace *ns = Namespace::find(docNS.getSTEStringValue());
                 if (ns)
                 {
                    ns->setUsage(docSTR.getTempStringValue());
                 }
              }
              else if (docNS.type == ConsoleValue::TypeInternalNamespaceEntry)
              {
                 // function
                 Namespace::Entry *entry = (Namespace::Entry *)docNS.value.ptrValue;
                 entry->setUsage(docSTR.getTempStringValue());
              }
              else
              {
                 Con::warnf("Invalid docblock");
              }
              
              vmbreak;
           }
           
           vmcase(Compiler::OP_ITR_GET) {
              // a := b.itr(c)
              
              ConsoleValuePtr itrObject = TS2_BASE_OR_KONST(TS2_OP_DEC_B(i));
              ConsoleValuePtr itrValue = base[TS2_OP_DEC_C(i)];
              
              if (itrObject.type > ConsoleValue::TypeReferenceCounted)
              {
                 if (!itrObject.value.refValue->advanceIterator(itrValue, *ra))
                 {
                    ip += TS2_OP_DEC_sBx(code[ip])+1; // iterator finished
                 }
                 else
                 {
                    ip++; // skip JMP
                 }
              }
              else
              {
                 // Fallback to convert to reference
                 SimSet *itrObj = Sim::findObject<SimSet>(itrObject);
                 
                 if (itrObj)
                 {
                    S32 itrIndex = 0;
                    S32 maxItr = itrObj->size();
                    itrIndex = (S32)itrValue.getSignedIntValue();
                    
                    // Start iterator
                    if (itrIndex <= 0)
                    {
                       itrIndex = 1;
                    }
                    
                    if (itrIndex > maxItr)
                    {
                       ip += TS2_OP_DEC_sBx(code[ip])+1; // exit
                    }
                    else
                    {
                       base[TS2_OP_DEC_A(i)].setValue(ConsoleSimObjectPtr::fromObject(itrObj->at(itrIndex-1)));
                       itrIndex++;
                       
                       iacc.value.ival = itrIndex;
                       base[TS2_OP_DEC_C(i)].setValue(iacc);
                       
                       ip++; // Skip JMP
                    }
                 }
                 else
                 {
                    Con::warnf("Couldn't find iterator object.");
                    ip += TS2_OP_DEC_sBx(code[ip])+1; // exit
                 }
              }
              
              vmbreak;
           }
           
           vmcase(Compiler::OP_ITR_SGET) {
              U32 outValue = TS2_OP_DEC_A(i);
              U32 outItr = TS2_OP_DEC_C(i);
              
              ConsoleValue itrObject = TS2_BASE_OR_KONST(TS2_OP_DEC_B(i));
              ConsoleValue itrIteratorValue = base[outItr];
              
              {
                 const char *str = itrObject.getTempStringValue();
                 
                 if (str)
                 {
                    S32 itrIndex = 0;
                    S32 maxItr = dStrlen(str);
                    itrIndex = (S32)itrIteratorValue.getSignedIntValue();
                    
                    // Start iterator
                    if (itrIndex <= 0)
                    {
                       itrIndex = 1;
                    }
                    
                    if (itrIndex > maxItr)
                    {
                       ip += TS2_OP_DEC_sBx(code[ip])+1; // exit
                    }
                    else
                    {
                       S32 startIndex = itrIndex-1;
                       S32 endIndex = startIndex;
                       
                       if( !dIsspace( str[ endIndex ] ) )
                          do ++ endIndex;
                       while( str[ endIndex ] && !dIsspace( str[ endIndex ] ) );
                       
                       //itr->setValue(ConsoleSimObjectPtr::fromObject(itr[itrIndex-1]));
                       
                       if( endIndex != startIndex || (str[endIndex]) )
                       {
                          char tempStr[4096];
                          dStrncpy(tempStr, str+startIndex, endIndex-startIndex);
                          tempStr[endIndex-startIndex] = '\0';
                          
                          base[outValue].setValue(ConsoleStringValue::fromString(tempStr));
                       }
                       else
                       {
                          base[outValue].setNull();
                       }
                       
                       // Skip separator.
                       if( str[ endIndex ] != '\0' )
                       {
                          ++endIndex;
                       }
                       
                       iacc.value.ival = endIndex+1;
                       base[outItr].setValue(iacc);
                       
                       ip++; // Skip JMP
                    }
                 }
                 else
                 {
                    Con::warnf("Couldn't find iterator object.");
                    ip += TS2_OP_DEC_sBx(code[ip])+1; // exit
                 }
              }
              
              vmbreak;
           }
           
           vmcase(Compiler::OP_RETURN) {
              
              ConsoleValuePtr returnValue;
              
#ifdef TS2_VM_DEBUG
              Con::printf("OP_RETURN %i", ra);
#endif
              if (TS2_OP_DEC_A(i) > 0)
              {
                 returnValue.setValue(TS2_BASE_OR_KONST(TS2_OP_DEC_B(i)));
              }
              else
              {
                 returnValue.setNull();
              }
              
              state->yieldValue.setValue(returnValue);
              state->popFunction();
              
              // Set appropriate state based on current frame
              ip = state->currentFrame.savedIP;
              konstBase = state->currentFrame.constants;
              konst = konstBase + state->currentFrame.constantTop;
              base = state->stack.address() + state->currentFrame.stackTop;
              base[state->currentFrame.returnReg].setValue(state->yieldValue);
              code = state->currentFrame.code ? state->currentFrame.code->code : NULL;
              
              // Copy our locals from the dict if we have it
              if (state->currentFrame.localVars)
              {
                 state->copyLocalsToFrame(state->currentFrame.localVars, &state->currentFrame);
              }
              
              // Check if we are in a coroutine, in which case restore it.
              // @note this will also kill the coroutine
              if (state->coroutine)
              {
                 state->saveCoroutine(*state->coroutine);
                 state->coroutine->currentState = CodeBlockCoroutineState::DEAD;
                 state = CodeBlockEvalState::getCurrent();
                 state->yieldValue.setValue(returnValue); // have to set this again since we're in parent
                 
                 // Set appropriate state based on current frame
                 ip = state->currentFrame.savedIP;
                 konstBase = state->currentFrame.constants;
                 konst = konstBase + state->currentFrame.constantTop;
                 base = state->stack.address() + state->currentFrame.stackTop;
                 base[state->currentFrame.returnReg].setValue(state->yieldValue);
                 code = state->currentFrame.code ? state->currentFrame.code->code : NULL;
                 vmbreak;
              }
              
              if (state->currentFrame.isRoot || code == NULL || (state->frames.size() < startFrameSize && startState == state))
              {
#ifdef DEBUG_COMPILER
                 Con::printf("Return from execBlock");
#endif
                 
                 return;
              }
              
              vmbreak;
           }
           
           vmcase(Compiler::OP_BREAK) {
              state->currentFrame.savedIP = ip;
              
              U32 breakLine;
              U32 instruction;
              state->currentFrame.code->findBreakLine(ip-1, breakLine, instruction);
              if(!breakLine)
                 vmbreak;
              TelDebugger->executionStopped(state->currentFrame.code, breakLine);
              
              // Notify the remote debugger.
              RemoteDebuggerBase *pRemoteDebugger = RemoteDebuggerBase::getRemoteDebugger();
              if ( pRemoteDebugger != NULL )
                 pRemoteDebugger->executionStopped(state->currentFrame.code, breakLine);
              
              vmbreak;
           }
           
           vmcase(Compiler::OP_ASSERT) {
              
              ConsoleValue assertMessage = TS2_BASE_OR_KONST(TS2_OP_DEC_B(i));
              AssertISV(ra->getBoolValue(), assertMessage.getTempStringValue());
              
              vmbreak;
           }
        }
    }
    
    state->currentFrame.savedIP = ip;
   
   AssertFatal(state->frames.size() == startFrameSize-1, "Frame size mismatch");
}

void CodeBlock::execWithEnv(CodeBlockEvalState *state, CodeBlockFunction *srcEnv, CodeBlockFunction *destEnv)
{
    // Copies references to variables in srcEnv's scope into their corresponding position in destEnv
}

void CodeBlock::execFunction(CodeBlockEvalState *state, CodeBlockFunction *env, U32 argc, ConsoleValuePtr argv[], StringTableEntry packageName)
{
    // Prepares an executes a function, copying argv into the stack according to env
    
}

ConsoleValuePtr CodeBlock::exec(U32 ip, const char *functionName, Namespace *thisNamespace, U32 argc, ConsoleValuePtr argv[], bool noCalls, StringTableEntry packageName)
{
   // Push exec state
   
   // jamesu - new exec function
   CodeBlockEvalState *state = CodeBlockEvalState::getCurrent();
   CodeBlockFunction *newFunc = mFunctions[ip];
   
   // As an optimization we only increment the stack to returnStart, and
   // blank out any unused vars. This means any register slots after will get
   // trashed (though there shouldn't be any anyway!).
   Namespace::Entry *nsEntry = thisNamespace->lookup(StringTable->insert(functionName));
   
   if (!nsEntry)
   {
      Con::errorf("Missing function %s", functionName);
      return "";
   }
   
   U32 usedVars = argc > nsEntry->mMaxArgs ? nsEntry->mMaxArgs : argc;
   
   if (usedVars < nsEntry->mMinArgs)
   {
      Con::errorf("Insufficient parameters passed to function");
      return "";
   }
   
   U32 oldReturnReg = state->currentFrame.returnReg;
   state->currentFrame.returnReg = state->getFrameEnd();
   state->pushFunction(newFunc, nsEntry->mCode, nsEntry, argc);
   state->currentFrame.noCalls = noCalls;
   
   // Copy argv to stack
   for (U32 i=1; i<argc; i++)
   {
      ConsoleValue *base = &state->stack[state->currentFrame.stackTop];
      (((ConsoleValuePtr*)base)+i-1)->setValue(argv[i]);
   }
   
   execBlock(state);
   
   state->currentFrame.returnReg = oldReturnReg;
   
   // pop
   
   return state->yieldValue;
}

bool CodeBlock::prepCoroutine(U32 ip, const char *functionName, Namespace *thisNamespace, U32 argc, ConsoleValuePtr argv[], bool noCalls, StringTableEntry packageName)
{
   // Push exec state
   
   // jamesu - new exec function
   CodeBlockEvalState *state = CodeBlockEvalState::getCurrent();
   CodeBlockFunction *newFunc = mFunctions[ip];
   
   // As an optimization we only increment the stack to returnStart, and
   // blank out any unused vars. This means any register slots after will get
   // trashed (though there shouldn't be any anyway!).
   Namespace::Entry *nsEntry = thisNamespace->lookup(StringTable->insert(functionName));
   
   if (!nsEntry)
   {
      Con::errorf("Missing function %s", functionName);
      return false;
   }
   
   U32 usedVars = argc > nsEntry->mMaxArgs ? nsEntry->mMaxArgs : argc;
   
   if (usedVars < nsEntry->mMinArgs)
   {
      Con::errorf("Insufficient parameters passed to function");
      return false;
   }
   
   U32 oldReturnReg = state->currentFrame.returnReg;
   state->currentFrame.returnReg = state->getFrameEnd();
   state->pushFunction(newFunc, nsEntry->mCode, nsEntry, argc);
   state->currentFrame.noCalls = noCalls;
   
   // Copy argv to stack
   for (U32 i=1; i<argc; i++)
   {
      ConsoleValue *base = &state->stack[state->currentFrame.stackTop];
      (((ConsoleValuePtr*)base)+i-1)->setValue(argv[i]);
   }
   
   state->currentFrame.returnReg = oldReturnReg;
   return true;
}

StringTableEntry CodeBlock::getDSOPath(const char *scriptPath)
{
   const char *slash = dStrrchr(scriptPath, '/');
   return StringTable->insertn(scriptPath, (U32)(slash - scriptPath), true);
}

//------------------------------------------------------------
