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


#ifndef _COMPILER_H_
#define _COMPILER_H_

//#define DEBUG_CODESTREAM
//#define DEBUG_COMPILER

#ifdef DEBUG_CODESTREAM
#include <stdio.h>
#endif

class Stream;
class DataChunker;
struct CompilerConstantsTable;

#include "platform/platform.h"
#include "console/ast.h"
#include "console/codeBlock.h"
#include "collection/vector.h"
#include "console/consoleDictionary.h"

namespace Compiler
{
   // Decode opcodes

   // BBBBBBBB BCCCCCCC CCAAAAAA AAOOOOOO   ABC format B = 9, C = 9, A = 8
   // BBBBBBBB BBBBBBBB BBAAAAAA AAOOOOOO   ABx format B = 18,       A = 8
   // sBBBBBBB BBBBBBBB BBAAAAAA AAOOOOOO   AsBx format B = 17, A = 8, s = 1

   // iABC (8 bits A, 9 bits B & C)
   // iABx (unsigned A, 18 bits for b)
   // iAsBx (unsigned A, 18 signed bits for b)
   // iAx(26 bits)

   typedef U32 Instruction;
   const U32 OP_MASK = 0x3F; // 6 bits
   const U32 OP_A_SHIFT = 6;
   const U32 OP_B_SHIFT = OP_A_SHIFT + 8;
   const U32 OP_C_SHIFT = OP_B_SHIFT + 9;
    
   const U32 OP_A_MASK = 0xFF;//  >> OP_A_SHIFT; // 8 bits
   const U32 OP_B_MASK = 0x1FF;// >> OP_B_SHIFT; // 9 bits
   const U32 OP_C_MASK = 0x1FF;// >> OP_B_SHIFT; // 9 bits
    
   const U32 OP_Bx_MASK = 0x3FFFF;//   >> OP_B_SHIFT;
   const U32 OP_Ax_MASK = 0x3FFFFFF;// >> OP_A_SHIFT;

   const U32 OP_K_MASK  = 0x100;
   const U32 OP_KV_MASK  = 0xFF;
   const U32 OP_MAX_INT = 2147483647;
   const U32 OP_MAX_Bx = ((1<<18)-1);
   const U32 OP_MAX_sBx = OP_MAX_Bx>>1;

   const U32 CONSTANTS_PER_PAGE = 255;

   #define TS2_OP_DEC(op)    ((op) & Compiler::OP_MASK)
   #define TS2_OP_DEC_A(op)  (((op) >> Compiler::OP_A_SHIFT) & Compiler::OP_A_MASK)
   #define TS2_OP_DEC_B(op)  (((op) >> Compiler::OP_B_SHIFT) & Compiler::OP_B_MASK)
   #define TS2_OP_DEC_C(op)  (((op) >> Compiler::OP_C_SHIFT) & Compiler::OP_C_MASK)
   #define TS2_OP_DEC_Bx(op)  (((op) >> Compiler::OP_B_SHIFT) & Compiler::OP_Bx_MASK)
   #define TS2_OP_DEC_sBx(op) ((S32)TS2_OP_DEC_Bx(op) - Compiler::OP_MAX_sBx)
   #define TS2_OP_MAKE_sBX(i) ((U32)(i+Compiler::OP_MAX_sBx))
   #define TS2_OP_DEC_Ax(op)  (((op) >> Compiler::OP_A_SHIFT) & Compiler::OP_Ax_MASK)

   #define TS2_OP_ENC_A_B_C(op,a,b,c) ((Compiler::Instruction) ( (c << Compiler::OP_C_SHIFT) | (b << Compiler::OP_B_SHIFT) | (a << Compiler::OP_A_SHIFT) | (op) ))
   #define TS2_OP_ENC_A_Bx(op,a,bx) ((Compiler::Instruction) ( (bx << Compiler::OP_B_SHIFT) | (a << Compiler::OP_A_SHIFT) | (op) ))
   #define TS2_OP_ENC_Ax(op,ax) ((Compiler::Instruction) ( (ax << Compiler::OP_A_SHIFT) | (op) ))

   #define TS2_BASE_OR_KONST(value) (value & Compiler::OP_K_MASK ? konst[value & Compiler::OP_KV_MASK] : base[value])

   #define TS2_ENC_KONST(value) (value | Compiler::OP_K_MASK)
   #define TS2_ENC_REGIDX(value) (value)
   
   #define TS2_ENC_KONST_OR_REG(target) (target.regNum >= 0 ? TS2_ENC_REGIDX(target.regNum) : TS2_ENC_KONST(target.constRef.idx))
   
   enum CompiledInstructions
   {
      OP_MOVE,         // [abc][a := b]
      OP_LOADK,        // [ab] [a := const(b)]
      OP_PAGEK,        // [a]  [Switch constants table to set a]

      OP_LOADVAR,      //  [ab(c)]   [a := $[b]]        ; loads a global variable
      OP_GETFIELD,     //  a b c [a := b[c]]      ; gets an object property or table field
      OP_GETFIELDA,    //  a b c [a := b[c]]      ; gets an object property or table field
      OP_GETFUNC,      //  a b c [a := b.func ns] ; gets bound ns function for object
      OP_GETFUNC_NS,   //  a b c [a := b(ns).func] ; gets bound ns function

      OP_GETINTERNAL,  // a = b->c
      OP_GETINTERNAL_N, // a = b-->c

      OP_BINDNSFUNC,      //  a b c [a @(pkg, ns).func(b) = funcIdx(c)] ; sets bound ns function for object

      OP_SETVAR,       // [(a)bc]    [name(a) := b]     ; assigns a global variable
      OP_SETFIELD,     // [abc] [a[b] := c]        ; sets an object property or table field
      OP_SETFIELDA,     // [abc] [a.b[c+1] := c]        ; sets an object property or table field
      OP_SETLIST,      // [ab...c]                  ; sets a list of items (similar to lua)
      OP_COPYFIELDS,   // [abc] [a.fields := b.fields] ; copy object data or hash fields to object

      OP_ITR_GET,      // a = itr(b, c)
      OP_ITR_SGET,     // a = itr(b, c)  (getWord used)

      OP_CREATE_VALUE, // [a := new valuetype(size = b,c)] ; creates a new value with type b

      OP_CREATE_OBJECT,         // [abc] create object [a := createObject(b...c)] ; type, name, args
      OP_FINISH_OBJECT,         // [abx] add object ([create a])
      OP_DELETE_OBJECT,         // [abx] delete object a [delete a]

      OP_ADD,            // [abc] [a := b + c]
      OP_SUB,            // [abc] [a := b - c]
      OP_MUL,            // [abc] [a := b * c]
      OP_DIV,            // [abc] [a := b / c]
      OP_MOD,            // [abc] [a := b % c]
      OP_POW,            // [abc] [a := b ** c]
      OP_UMN,            // [ab]  [a := -c]
      OP_NOT,            // [ab]  [a := !b]
      OP_XOR,            // [abc] [a := b ^ c]
      OP_SHL,            // [abc] [a := b << c]
      OP_SHR,            // [abc] [a := b >> c]
      OP_BITAND,         // [abc] [a := b & c]
      OP_BITOR,          // [abc] [a := b | c]
      OP_ONESCOMPLEMENT, // [abc] [a := b ~ c]
      OP_CONCAT,         // [abc] [a := b...c]

      // note: operands in conditions are b & c
      OP_LT,     // [abc] <  (inverse == >=)
      OP_LE,     // [abc] <= (inverse == >)
      OP_EQ,     // [abc] == (inverse == !=)
      OP_EQ_STR, // [abc] $= (specific string comparison)

      OP_JMP,    // [iAsBx pc] jump to pc
      OP_CALL,   // [abc] [a := a(a[...b params])[...c return params]]

      OP_RETURN,  // [abc] return from function, returning a

      OP_FOREACH,     //[abc] Iterate a list [obj, itr, value]
      OP_FOREACH_STR, // [abc] Iterate a string [obj, itr, value]

      OP_ASSERT,      // fire assert
      OP_BREAK,       // breakpoint (used internally by debugger)

      OP_NUM_INST
   };
   
   enum ConditionalFlags
   {
      COND_NONE,
      COND_AND,
      COND_OR,
   };
   
   enum CreateObjectFlags
   {
      CREATEOBJECT_ISSINGLETON=1<<0,
      CREATEOBJECT_ISDATABLOCK=1<<1,
      CREATEOBJECT_ISINTERNAL=1<<2
   };
};

namespace Compiler
{
   //------------------------------------------------------------

   F64 consoleStringToNumber(const char *str, StringTableEntry file = 0, U32 line = 0);
   U32 compileBlock(StmtNode *block, CodeStream &codeStream, U32 ip);

   //------------------------------------------------------------

   struct CompilerConstantRef
   {
      S16 idx;
      S16 page;
      
      CompilerConstantRef() : idx(-1), page(-1) {;}
      CompilerConstantRef(S32 inIdx, S32 inPage) : idx(inIdx), page(inPage) {;}
   };
   
   struct CompilerConstantsTable
   {
       struct Entry
       {
           ConsoleValuePtr value;
           Entry *next;
       };
       Entry** list;
      
       U32 count;
       U32 totalLen;
       U32 numPages;
       U32 currentPage;
       U32 currentPageCount;
      
       char buf[256];
      
       void emitNewPage();
       void debugPrintPage(U32 pageId);
      
       CompilerConstantRef addString(const char *str, bool caseSens = true, bool tag = false);
       CompilerConstantRef addIntString(U32 value);
       CompilerConstantRef addFloatString(F64 value);
       CompilerConstantRef addNull();
       CompilerConstantRef addNamespace(const char* nsName);
       
       CompilerConstantRef addInt(S64 value);
       CompilerConstantRef addFloat(F64 value);
      
       // Checks if current page has required number of constants,
       // otherwise dummies the rest of the slots out and creates a
       // new page
       void checkFlushPage(U32 requiredConstants);
      
       // Duplicate a constant into the new page
       CompilerConstantRef duplicateConstant(CompilerConstantRef idx);
      
       // Grabs an entry for a desired constant
       Entry** getConstantEntry(U32 idx, U32 pageIdx);
       
       void reset();
       void build(Vector<ConsoleValuePtr> &outConstants);
   };

   //------------------------------------------------------------

   CodeBlock *getBreakCodeBlock();
   void setBreakCodeBlock(CodeBlock *cb);

   /// Helper function to reset the float, string, and ident tables to a base
   /// starting state.
   void resetTables();

   void *consoleAlloc(U32 size);
   void consoleAllocReset();

   extern bool gSyntaxError;
};

/// Utility class to emit and patch bytecode
class CodeStream
{
public:
   
   enum FixType
   {
      // For loops
      FIXTYPE_LOOPBLOCKSTART,
      FIXTYPE_BREAK,
      FIXTYPE_CONTINUE
   };
   
   enum Constants
   {
      BlockSize = 16384,
   };
   
protected:
   
   typedef struct PatchEntry
   {
      U32 addr;  ///< Address to patch
      U32 value; ///< Value to place at addr
      
      PatchEntry() {;}
      PatchEntry(U32 a, U32 v)  : addr(a), value(v) {;}
   } PatchEntry;
   
public:
   typedef struct CodeData
   {
      U8 *data;       ///< Allocated data (size is BlockSize)
      U32 size;       ///< Bytes used in data
      CodeData *next; ///< Next block
   } CodeData;
   
protected:
   
   /// @name Emitted code
   /// {
   CodeData *mCode;
   CodeData *mCodeHead;
   U32 mCodePos;
public:
   U32 mLastKonstPage;
   U32 mKonstPageVersion;
   
   void setKonstPage(U32 page) { mLastKonstPage = page; mKonstPageVersion++; }
   U32 getKonstPage() { return mLastKonstPage; }
   U32 getKonstPageVersion() { return mKonstPageVersion; }
protected:
   /// }
   
   /// @name Code fixing stacks
   /// {
   Vector<U32> mFixList;
   Vector<U32> mFixStack;
   Vector<bool> mFixLoopStack;
   Vector<PatchEntry> mPatchList;
   /// }
   
   /// @name Function constants
   /// {
   Vector<Compiler::CompilerConstantsTable*> mConstantsTableStack;
   Compiler::CompilerConstantsTable* mCurrentConstants;
   /// }
   
public:
   
   Compiler::CompilerConstantsTable* getConstantsTable() { return mCurrentConstants; }
   void setConstantsTable(Compiler::CompilerConstantsTable* table) { mCurrentConstants = table; }
   
   /// @name Register state
   /// {
   typedef struct VarInfo
   {
      StringTableEntry name;
      S32 index;
      
      VarInfo() : name(NULL), index(0) {;}
   } VarInfo;
   
   S32 allocStackPos()
   {
      S32 pos = mVarInfo.size()-mCurrentFunctionState.startVar;
      mVarInfo.push_back(VarInfo());
      mMaxStackSize = pos > mMaxStackSize ? pos : mMaxStackSize;
      return pos;
   }
   
   typedef struct RegisterTarget
   {
      S16 regNum;
      Compiler::CompilerConstantRef constRef;
      bool isRef; // Marks if we are merely a ref of the var (e.g. if we need to clone this for two branches)
      
      RegisterTarget() : regNum(-1), constRef(-1,-1) {;}
      RegisterTarget(S16 r) : regNum(r), constRef(-1,-1) {;}
      RegisterTarget(Compiler::CompilerConstantRef c) : regNum(-1), constRef(c), isRef(false) {;}
      
      bool isAssigned() { return regNum >= 0 || constRef.idx >= 0; }
      const char *toString() { if (!isAssigned()) return "[NULL]"; char *ret = Con::getArgBuffer(64); dSprintf(ret, 64, "%s%i PAGE %i", regNum >= 0 ? "R" : "C", regNum >= 0 ? regNum : constRef.idx, regNum >= 0 ? -1 : constRef.page ); return ret; }
   } RegisterTarget;
   
   Vector<RegisterTarget> mTargetList;
   Vector<VarInfo> mVarInfo;
   
   typedef struct FunctionState
   {
      U32 startTarget;
      U32 startVar;
      U32 stackSize;
      
      FunctionState() : startTarget(0), startVar(0), stackSize(0) {;}
   } FunctionState;
   Vector<FunctionState> mFunctionStates;
   FunctionState mCurrentFunctionState;
   
   S32 mTempStackSize;
   U32 mMaxStackSize;
   
   U32 getMaxStack()
   {
      return mMaxStackSize+1;
   }
   
   S32 getLocalVariable(StringTableEntry name)
   {
      for (U32 i=mCurrentFunctionState.startVar; i<mVarInfo.size(); i++)
      {
         if (mVarInfo[i].name == name)
            return mVarInfo[i].index;
      }
      
      return -1;
   }
   
   S32 addLocalVariable(StringTableEntry name)
   {
      S32 pos = mVarInfo.size() - mCurrentFunctionState.startVar;
      VarInfo var;
      var.name = name;
      var.index = pos;
      mVarInfo.push_back(var);
      mMaxStackSize = pos > mMaxStackSize ? pos : mMaxStackSize;
      return pos;
   }
   
   /// Pushes a target to the register stack, which can be used in one of two ways:
   /// A) A value source (constant, variable, temp register)
   /// B) Target for variable assignment (such as a variable or temp register)
   RegisterTarget pushTarget(RegisterTarget n)
   {
      if (n.isAssigned())
      {
         mTargetList.push_back(n);
         return n;
      }
      
      RegisterTarget r(allocStackPos());
      mTargetList.push_back(r);
      return r;
   }
   
   // Similar to pushTarget except marks target as a reference if unallocated
   RegisterTarget pushTargetReference(RegisterTarget n)
   {
      if (n.isAssigned())
      {
         n.isRef = true;
         mTargetList.push_back(n);
         return n;
      }
      
      RegisterTarget r(allocStackPos());
      mTargetList.push_back(r);
      return r;
   }
   
   // Pop a target from the stack.
   RegisterTarget popTarget()
   {
      if (mTargetList.size() - mCurrentFunctionState.startTarget == 0)
      {
         AssertFatal(false, "NO TARGET AVAILABLE, CHECK STACK BALANCE!");
         return RegisterTarget();
      }
      
      RegisterTarget target = mTargetList[mTargetList.size()-1];
      
      if (target.regNum >= 0 && !target.isRef)
      {
         AssertFatal(target.regNum < mVarInfo.size(), "Errm");
         VarInfo &var = mVarInfo[mCurrentFunctionState.startVar + target.regNum];
         if (var.name == NULL)
         {
            mVarInfo.pop_back();
            //mTempStackSize++;
         }
      }
      
      mTargetList.pop_back();
      return target;
   }
   
   RegisterTarget topTarget()
   {
      return mTargetList[mTargetList.size()-1];
   }
   
   // Pushes a function scope. This will create a new scope for variables
   void pushFunctionState()
   {
      mCurrentFunctionState.stackSize = mCurrentFunctionState.stackSize > mMaxStackSize ? mMaxStackSize : mCurrentFunctionState.stackSize;
      mFunctionStates.push_back(mCurrentFunctionState);
      mCurrentFunctionState.startTarget = mTargetList.size();
      mCurrentFunctionState.startVar = mVarInfo.size();
      mCurrentFunctionState.stackSize = 0;
      mConstantsTableStack.push_back(mCurrentConstants);
      mMaxStackSize = 0;
   }
   
   void popFunctionState()
   {
      AssertFatal(mFunctionStates.size() > 0, "Push/pop function state mismatch");
      mCurrentFunctionState = mFunctionStates[mFunctionStates.size()-1];
      mFunctionStates.pop_back();
      mCurrentConstants = mConstantsTableStack[mConstantsTableStack.size()-1];
      mConstantsTableStack.pop_back();
      mMaxStackSize = mCurrentFunctionState.stackSize;
   }
   
   /// }
   
   Vector<U32> mBreakLines; ///< Line numbers
   
public:
   
   CodeStream() : mCode(0), mCodeHead(NULL), mCodePos(0), mCurrentConstants(0), mLastKonstPage(0), mMaxStackSize(0)
   {
   }
   
   ~CodeStream()
   {
      reset();
      
      if (mCode)
      {
         dFree(mCode->data);
         delete mCode;
      }
   }
   
   U8 *allocCode(U32 sz);
   CodeData* allocCodeData();
   
   inline U32 emitOpcodeABC(U32 op, U32 a, U32 b, U32 c)
   {
      U32 *ptr = (U32*)allocCode(4);
      U32 code = TS2_OP_ENC_A_B_C(op, a, b, c);
      
      *ptr = code;
#ifdef DEBUG_CODESTREAM
      printf("code[%u] = %u\n", mCodePos, code);
#endif
      return mCodePos++;
   }
   
   inline U32 emitOpcodeABCRef(U32 op, U32 a, RegisterTarget b, RegisterTarget c)
   {
      // First ensure all constants are value
      if (b.constRef.idx != -1 && c.constRef.idx != -1)
      {
         if (b.constRef.page != c.constRef.page)
         {
            U32 curPage = getConstantsTable()->currentPage;
            
            if (b.constRef.page == curPage || c.constRef.page == curPage)
            {
               getConstantsTable()->checkFlushPage(1);
            }
            else
            {
               getConstantsTable()->checkFlushPage(2);
            }
            
            Compiler::CompilerConstantRef newB = getConstantsTable()->duplicateConstant(b.constRef);
            Compiler::CompilerConstantRef newC = getConstantsTable()->duplicateConstant(c.constRef);
            
            b = RegisterTarget(newB);
            c = RegisterTarget(newC);
            
            S16 newPage = newC.page > newB.page ? newC.page : newB.page;
            emitOpcodeABx(Compiler::OP_PAGEK, 0, newPage);
            mLastKonstPage = newPage;
         }
      }
      
      if (b.constRef.idx >= 0 && b.constRef.page != mLastKonstPage)
      {
         emitOpcodeABx(Compiler::OP_PAGEK, 0, b.constRef.page);
         mLastKonstPage = b.constRef.page;
      }
      else if (c.constRef.idx >= 0 && c.constRef.page != mLastKonstPage)
      {
         emitOpcodeABx(Compiler::OP_PAGEK, 0, c.constRef.page);
         mLastKonstPage = c.constRef.page;
      }
      
      U32 code = TS2_OP_ENC_A_B_C(op, a, TS2_ENC_KONST_OR_REG(b), TS2_ENC_KONST_OR_REG(c));
      
      return emit(code);
   }
   
   inline U32 emitOpcodeABx(U32 op, U32 a, U32 bx)
   {
      U32 *ptr = (U32*)allocCode(4);
      U32 code = TS2_OP_ENC_A_Bx(op, a, bx);
      *ptr = code;
#ifdef DEBUG_CODESTREAM
      printf("code[%u] = %u\n", mCodePos, code);
#endif
      
      return mCodePos++;
   }
   
   inline U32 emit(U32 code)
   {
      U32 *ptr = (U32*)allocCode(4);
      *ptr = code;
#ifdef DEBUG_CODESTREAM
      printf("code[%u] = %u\n", mCodePos, code);
#endif
      return mCodePos++;
   }
   
   inline void patch(U32 addr, U32 code)
   {
#ifdef DEBUG_CODESTREAM
      printf("patch[%u] = %u\n", addr, code);
#endif
      mPatchList.push_back(PatchEntry(addr, code));
   }
   
   inline U32 emitKonstRef(Compiler::CompilerConstantRef konstRef)
   {
      if (konstRef.page != mLastKonstPage)
      {
         emitOpcodeABx(Compiler::OP_PAGEK, 0, konstRef.page);
         mLastKonstPage = konstRef.page;
      }
      
      return konstRef.idx;
   }
   
   inline U32 emitTargetRef(RegisterTarget target)
   {
      if (target.constRef.idx >= 0)
         return TS2_ENC_KONST(emitKonstRef(target.constRef));
      else
         return target.regNum;
   }
   
   inline U32 tell()
   {
      return mCodePos;
   }
   
   inline bool inLoop()
   {
      for (U32 i=0; i<mFixLoopStack.size(); i++)
      {
         if (mFixLoopStack[i])
            return true;
      }
      return false;
   }
   
   inline U32 emitFix(FixType type)
   {
      emitOpcodeABx(Compiler::OP_JMP, 0, type);
      
#ifdef DEBUG_CODESTREAM
      printf("code[%u] = [FIX:%u]\n", mCodePos-1, (U32)type);
#endif
      
      mFixList.push_back(mCodePos-1);
      mFixList.push_back((U32)type);
      return mCodePos;
   }
   
   inline void pushFixScope(bool isLoop)
   {
      mFixStack.push_back(mFixList.size());
      mFixLoopStack.push_back(isLoop);
   }
   
   inline void popFixScope()
   {
      AssertFatal(mFixStack.size() > 0, "Fix stack mismatch");
      
      U32 newSize = mFixStack[mFixStack.size()-1];
      while (mFixList.size() > newSize)
      {
         mFixList.pop_back();
      }
      mFixStack.pop_back();
      mFixLoopStack.pop_back();
   }
   
   void fixLoop(U32 loopBlockStart, U32 breakPoint, U32 continuePoint);
   
   inline void addBreakLine(U32 lineNumber, U32 ip)
   {
      mBreakLines.push_back(lineNumber);
      mBreakLines.push_back(ip);
   }
   
   inline U32 getNumLineBreaks()
   {
      return mBreakLines.size() / 2;
   }
   
   void emitCodeStream(U32 *size, U32 **stream, U32 **lineBreaks);
   
   void reset();
};

#endif
