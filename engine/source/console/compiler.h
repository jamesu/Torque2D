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

#define DEBUG_CODESTREAM

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

namespace Compiler2
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
    
    #define TS2_OP_DEC(op)    ((op) & Compiler2::OP_MASK)
    #define TS2_OP_DEC_A(op)  (((op) >> Compiler2::OP_A_SHIFT) & Compiler2::OP_A_MASK)
    #define TS2_OP_DEC_B(op)  (((op) >> Compiler2::OP_B_SHIFT) & Compiler2::OP_B_MASK)
    #define TS2_OP_DEC_C(op)  (((op) >> Compiler2::OP_C_SHIFT) & Compiler2::OP_C_MASK)
    #define TS2_OP_DEC_Bx(op)  (((op) >> Compiler2::OP_B_SHIFT) & Compiler2::OP_Bx_MASK)
    #define TS2_OP_DEC_sBx(op) ((S32)TS2_OP_DEC_Bx(op) - Compiler2::OP_MAX_sBx)
    #define TS2_OP_MAKE_sBX(i) ((U32)(i+Compiler2::OP_MAX_sBx))
    #define TS2_OP_DEC_Ax(op)  (((op) >> Compiler2::OP_A_SHIFT) & Compiler2::OP_Ax_MASK)

    #define TS2_OP_ENC_A_B_C(op,a,b,c) ((Compiler2::Instruction) ( (c << Compiler2::OP_C_SHIFT) | (b << Compiler2::OP_B_SHIFT) | (a << Compiler2::OP_A_SHIFT) | (op) ))
    #define TS2_OP_ENC_A_Bx(op,a,bx) ((Compiler2::Instruction) ( (bx << Compiler2::OP_B_SHIFT) | (a << Compiler2::OP_A_SHIFT) | (op) ))
    #define TS2_OP_ENC_Ax(op,ax) ((Compiler2::Instruction) ( (ax << Compiler2::OP_A_SHIFT) | (op) ))
    
    #define TS2_BASE_OR_KONST(value) (value & Compiler2::OP_K_MASK ? konst[value & Compiler2::OP_KV_MASK] : base[value])
	
   #define TS2_ENC_KONST(value) (value | Compiler2::OP_K_MASK)
	#define TS2_ENC_REGIDX(value) (value)
	
#define TS2_ENC_KONST_OR_REG(target) (target.regNum >= 0 ? TS2_ENC_REGIDX(target.regNum) : TS2_ENC_KONST(target.constNum))
	
    enum CompiledInstructions
    {
        OP_MOVE,         // [abc][a := b]
        OP_LOADK,        // [ab] [a := const(b)]
        OP_PAGEK,        // [a]  [Switch constants table to set a]
        
        OP_LOADVAR,      //  [ab(c)]   [a := $[b]]        ; loads a global variable
        OP_GETFIELD,     //  a b c [a := b[c]]      ; gets an object property or table field
        OP_GETFUNC,      //  a b c [a := b.func ns] ; gets bound ns function for object
		  OP_SETFUNC,      //  a b c [a.func name = func at idx] ; gets bound ns function for object
		 
        OP_SETVAR,       // [(a)bc]    [name(a) := b]     ; assigns a global variable
        OP_SETFIELD,     // [abc] [a[b] := c]        ; sets an object property or table field
        OP_SETLIST,      // [ab...c]                  ; sets a list of items (similar to lua)
        OP_COPYFIELDS,   // [abc] [a.fields := b.fields] ; copy object data or hash fields to object
        
        OP_CREATE_TABLE, // [a := new table(size = b,c)] ; creates a new table
        
        OP_SET_EXCEPTION_HANDLER, // [abx] Sets relative jump location for exception handler
        OP_GET_EXCEPTION,         // [ab] ; Stores exception object in a
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
};

namespace Compiler
{
   /// The opcodes for the TorqueScript VM.
    enum CompiledInstructions
    {
        OP_FUNC_DECL,
        OP_CREATE_OBJECT,
        OP_ADD_OBJECT,
        OP_END_OBJECT,
        // Added to fix the stack issue [7/9/2007 Black]
        OP_FINISH_OBJECT,
        
        OP_JMPIFFNOT,
        OP_JMPIFNOT,
        OP_JMPIFF,
        OP_JMPIF,
        OP_JMPIFNOT_NP,
        OP_JMPIF_NP,    // 10
        OP_JMP,
        OP_RETURN,
        // fixes a bug when not explicitly returning a value
        OP_RETURN_VOID,
        OP_RETURN_FLT,
        OP_RETURN_UINT,
        
        OP_CMPEQ,
        OP_CMPGR,
        OP_CMPGE,
        OP_CMPLT,
        OP_CMPLE,
        OP_CMPNE,
        OP_XOR,         // 20
        OP_MOD,
        OP_BITAND,
        OP_BITOR,
        OP_NOT,
        OP_NOTF,
        OP_ONESCOMPLEMENT,
        
        OP_SHR,
        OP_SHL,
        OP_AND,
        OP_OR,          // 30
        
        OP_ADD,
        OP_SUB,
        OP_MUL,
        OP_DIV,
        OP_NEG,
        
        OP_SETCURVAR,
        OP_SETCURVAR_CREATE,
        OP_SETCURVAR_ARRAY,
        OP_SETCURVAR_ARRAY_CREATE,
        
        OP_LOADVAR_UINT,// 40
        OP_LOADVAR_FLT,
        OP_LOADVAR_STR,
        OP_LOADVAR_VAR,
        
        OP_SAVEVAR_UINT,
        OP_SAVEVAR_FLT,
        OP_SAVEVAR_STR,
        OP_SAVEVAR_VAR,
        
        OP_SETCUROBJECT,
        OP_SETCUROBJECT_NEW,
        OP_SETCUROBJECT_INTERNAL,
        
        OP_SETCURFIELD,
        OP_SETCURFIELD_ARRAY, // 50
        OP_SETCURFIELD_TYPE,
        
        OP_LOADFIELD_UINT,
        OP_LOADFIELD_FLT,
        OP_LOADFIELD_STR,
        
        OP_SAVEFIELD_UINT,
        OP_SAVEFIELD_FLT,
        OP_SAVEFIELD_STR,
        
        OP_STR_TO_UINT,
        OP_STR_TO_FLT,
        OP_STR_TO_NONE,  // 60
        OP_FLT_TO_UINT,
        OP_FLT_TO_STR,
        OP_FLT_TO_NONE,
        OP_UINT_TO_FLT,
        OP_UINT_TO_STR,
        OP_UINT_TO_NONE,
        OP_COPYVAR_TO_NONE,
        
        OP_LOADIMMED_UINT,
        OP_LOADIMMED_FLT,
        OP_TAG_TO_STR,
        OP_LOADIMMED_STR, // 70
        OP_DOCBLOCK_STR,
        OP_LOADIMMED_IDENT,
        
        OP_CALLFUNC_RESOLVE,
        OP_CALLFUNC,
        
        OP_ADVANCE_STR,
        OP_ADVANCE_STR_APPENDCHAR,
        OP_ADVANCE_STR_COMMA,
        OP_ADVANCE_STR_NUL,
        OP_REWIND_STR,
        OP_TERMINATE_REWIND_STR,  // 80
        OP_COMPARE_STR,
        
        OP_PUSH,          // String
        OP_PUSH_UINT,     // Integer
        OP_PUSH_FLT,      // Float
        OP_PUSH_VAR,      // Variable
        OP_PUSH_FRAME,    // Frame
        
        OP_ASSERT,
        OP_BREAK,
        
        OP_ITER_BEGIN,       ///< Prepare foreach iterator.
        OP_ITER_BEGIN_STR,   ///< Prepare foreach$ iterator.
        OP_ITER,             ///< Enter foreach loop.
        OP_ITER_END,         ///< End foreach loop.
        
        OP_INVALID   // 90
    };

   //------------------------------------------------------------

   F64 consoleStringToNumber(const char *str, StringTableEntry file = 0, U32 line = 0);
   U32 compileBlock(StmtNode *block, CodeStream &codeStream, U32 ip);

   //------------------------------------------------------------

   struct CompilerIdentTable
   {
      struct Entry
      {
         U32 offset;
         U32 ip;
         Entry *next;
         Entry *nextIdent;
      };
      Entry *list;
      void add(StringTableEntry ste, U32 ip);
      void reset();
      void write(Stream &st);
   };

   //------------------------------------------------------------

   struct CompilerConstantsTable
   {
       struct Entry
       {
           ConsoleValuePtr value;
           Entry *next;
       };
       Entry *list;
       U32 count;
       U32 totalLen;
       
       char buf[256];
       
       U32 addString(const char *str, bool caseSens = true, bool tag = false);
       U32 addIntString(U32 value);
       U32 addFloatString(F64 value);
		 U32 addNull();
		 U32 addNamespace(const char* nsName);
       
       U32 addInt(U32 value);
       U32 addFloat(F64 value);
       
       void reset();
       void build(Vector<ConsoleValuePtr> &outConstants);
   };
    
   struct CompilerStringTable
   {
      U32 totalLen;
      struct Entry
      {
         char *string;
         U32 start;
         U32 len;
         bool tag;
         Entry *next;
      };
      Entry *list;

      char buf[256];

      U32 add(const char *str, bool caseSens = true, bool tag = false);
      U32 addIntString(U32 value);
      U32 addFloatString(F64 value);
      void reset();
      char *build();
      void write(Stream &st);
   };

   //------------------------------------------------------------

   struct CompilerFloatTable
   {
      struct Entry
      {
         F64 val;
         Entry *next;
      };
      U32 count;
      Entry *list;

      U32 add(F64 value);
      void reset();
      F64 *build();
      void write(Stream &st);
   };

   //------------------------------------------------------------
   
   inline StringTableEntry CodeToSTE(U32 *code, U32 ip)
   {
#ifdef TORQUE_64
      return (StringTableEntry)(*((U64*)(code+ip)));
#else
      return (StringTableEntry)(*(code+ip));
#endif
   }
   
   extern void (*STEtoCode)(StringTableEntry ste, U32 ip, U32 *codeStream);

   void evalSTEtoCode(StringTableEntry ste, U32 ip, U32 *codeStream);
   void compileSTEtoCode(StringTableEntry ste, U32 ip, U32 *codeStream);

   CompilerStringTable *getCurrentStringTable();
   CompilerStringTable &getGlobalStringTable();
   CompilerStringTable &getFunctionStringTable();

   void setCurrentStringTable (CompilerStringTable* cst);

   CompilerFloatTable *getCurrentFloatTable();
   CompilerFloatTable &getGlobalFloatTable();
   CompilerFloatTable &getFunctionFloatTable();

   void setCurrentFloatTable (CompilerFloatTable* cst);

   CompilerIdentTable &getIdentTable();

   void precompileIdent(StringTableEntry ident);

   CodeBlock *getBreakCodeBlock();
   void setBreakCodeBlock(CodeBlock *cb);

   /// Helper function to reset the float, string, and ident tables to a base
   /// starting state.
   void resetTables();

   void *consoleAlloc(U32 size);
   void consoleAllocReset();

   extern bool gSyntaxError;
};

struct CodeOpcode
{
    U32 op;
    
    CodeOpcode(U32 inOp) : op(inOp) {;}
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
	 U32 mCodeLine;
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
		  bool isParam;
		  bool isArray;
		 bool isObjTemp;
        
        VarInfo() : name(NULL), index(0), isParam(false), isArray(false), isObjTemp(false) {;}
    } VarInfo;
    
    S32 allocStackPos()
    {
        S32 pos = mVarInfo.size();
        mVarInfo.push_back(VarInfo());
        return pos;
    }
    
    typedef struct RegisterTarget
    {
        S16 regNum;
        S16 constNum;
        
        RegisterTarget() : regNum(-1), constNum(-1) {;}
        RegisterTarget(S16 r, S16 c) : regNum(r), constNum(c) {;}
        
        bool isAssigned() { return regNum >= 0 || constNum >= 0; }
        const char *toString() { if (!isAssigned()) return "[NULL]"; char *ret = Con::getArgBuffer(64); dSprintf(ret, 64, "%s%i", regNum >= 0 ? "R" : "C", regNum >= 0 ? regNum : constNum); return ret; }
    } RegisterTarget;
    
    Vector<RegisterTarget> mTargetList;
    Vector<VarInfo> mVarInfo;
	
	 typedef struct FunctionState
 	 {
		 U32 startTarget;
		 U32 startVar;
		 
		 bool needEnv;
		 
		 FunctionState() : startTarget(0), startVar(0), needEnv(0) {;}
	 } FunctionState;
	 Vector<FunctionState> mFunctionStates;
	 FunctionState mCurrentFunctionState;
    
    S32 mTempStackSize;
    
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
        S32 pos = mVarInfo.size();
        VarInfo var;
        var.name = name;
        var.index = pos;
        var.isParam = false;
        mVarInfo.push_back(var);
        return pos;
	 }
	
	S32 addTempObjectVariable(StringTableEntry name)
	{
		S32 pos = mVarInfo.size();
		VarInfo var;
		var.name = name;
		var.index = pos;
		var.isObjTemp = true;
		mVarInfo.push_back(var);
		return pos;
	}
	
	void markLocalArrayVariable(StringTableEntry name)
	{
		
		for (U32 i=mCurrentFunctionState.startVar; i<mVarInfo.size(); i++)
		{
			if (mVarInfo[i].name == name)
			{
				mVarInfo[i].isArray = true;
				break;
			}
		}
	}
	
	S32 addLocalParameter(StringTableEntry name)
	{
		S32 pos = mVarInfo.size();
		VarInfo var;
		var.name = name;
		var.index = pos;
		var.isParam = true;
		mVarInfo.push_back(var);
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
        
        RegisterTarget r(allocStackPos(), -1);
        mTargetList.push_back(r);
        return r;
    }
    
    // Pop a target from the stack.
    RegisterTarget popTarget()
    {
        if (mTargetList.size() - mCurrentFunctionState.startTarget == 0)
        {
            Con::warnf("NO TARGET AVAILABLE!");
            return RegisterTarget(-1,-1);
        }
        
        RegisterTarget target = mTargetList[mTargetList.size()-1];
        
        if (target.regNum >= 0)
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
		mFunctionStates.push_back(mCurrentFunctionState);
		mCurrentFunctionState.startTarget = mTargetList.size();
		mCurrentFunctionState.startVar = mVarInfo.size();
		mConstantsTableStack.push_back(mCurrentConstants);
	}
	
	void popFunctionState()
	{
		U32 oldVarStart = mCurrentFunctionState.startVar;
		U32 oldTargetStart = mCurrentFunctionState.startTarget;
		
		AssertFatal(mFunctionStates.size() > 0, "Push/pop function state mismatch");
		mCurrentFunctionState = mFunctionStates[mFunctionStates.size()-1];
		mFunctionStates.pop_back();
		mCurrentConstants = mConstantsTableStack[mConstantsTableStack.size()-1];
		mConstantsTableStack.pop_back();
		
		mTargetList.setSize(oldTargetStart);
		mVarInfo.setSize(oldVarStart);
	}
    
    /// }
    
    Vector<U32> mBreakLines; ///< Line numbers
    
public:
    
    CodeStream() : mCode(0), mCodeHead(NULL), mCodePos(0), mCurrentConstants(0)
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
	 CodeData *allocCodeSilent(U32 sz);
	
	 inline U32 emitOpcodeABC(U32 op, U32 a, U32 b, U32 c)
	 {
		 U32 *ptr = (U32*)allocCode(4);
		 U32 code = TS2_OP_ENC_A_B_C(op, a, b, c);
		 
		 U32 codeA = a << Compiler2::OP_A_SHIFT;
		 U32 codeB = b << Compiler2::OP_B_SHIFT;
		 U32 codeC = c << Compiler2::OP_C_SHIFT;
		 
		 
		 *ptr = code;
#ifdef DEBUG_CODESTREAM
		 printf("code[%u] = %u\n", mCodePos, code);
#endif
		 return mCodePos++;
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
	
	inline U32 emitOpcodeAx(U32 op, U32 ax)
	{
		U32 *ptr = (U32*)allocCode(4);
		U32 code = TS2_OP_ENC_Ax(op, ax);
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
    
    inline U32 emitSTE(const char *code)
    {
        U64 *ptr = (U64*)allocCode(8);
        *ptr = 0;
        Compiler::STEtoCode(code, mCodePos, (U32*)ptr);
#ifdef DEBUG_CODESTREAM
        printf("code[%u] = %s\n", mCodePos, code);
#endif
        mCodePos += 2;
        return mCodePos-2;
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
		 emitOpcodeABx(Compiler2::OP_JMP, 0, type);
        
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
            mFixList.pop_back();
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
	
	 inline U32 emitText(const char* text)
	 {
		 U32 len = dStrlen(text);
		 U8* ptr = allocCode(len);
		 dMemcpy(ptr, text, len);
		 for (U32 i=0; i<len; i++)
		 {
			 if (text[len] == '\n')
			 {
				 mCodeLine++;
			 }
		 }
		 
		 Con::printf("EMIT: %s", text);
		 return mCodePos;
 	 }
	
    void emitCodeStream(U32 *size, U32 **stream, U32 **lineBreaks);
    
    void reset();
};

#endif
