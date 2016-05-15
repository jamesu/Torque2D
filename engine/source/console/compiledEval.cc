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

#include "console/ast.h"
#include "collection/findIterator.h"
#include "io/resource/resourceManager.h"

#include "string/findMatch.h"
#include "string/stringUnit.h"
#include "console/consoleInternal.h"
#include "io/fileStream.h"
#include "console/compiler.h"

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

enum EvalConstants {
   MaxStackSize = 1024,
   MethodOnComponent = -2
};

namespace Con
{
// Current script file name and root, these are registered as
// console variables.
extern StringTableEntry gCurrentFile;
extern StringTableEntry gCurrentRoot;
}

F64 floatStack[MaxStackSize];
S64 intStack[MaxStackSize];

StringStack STR;

U32 FLT = 0;
U32 UINT = 0;

static const char *getNamespaceList(Namespace *ns)
{
   U32 size = 1;
   Namespace * walk;
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

F64 consoleStringToNumber(const char *str, StringTableEntry file, U32 line)
{
   F64 val = dAtof(str);
   if(val != 0)
      return val;
   else if(!dStricmp(str, "true"))
      return 1;
   else if(!dStricmp(str, "false"))
      return 0;
   else if(file)
   {
      Con::warnf(ConsoleLogEntry::General, "%s (%d): string always evaluates to 0.", file, line);
      return 0;
   }
   return 0;
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

    char* getBoolArg(bool arg)
    {
        char *ret = STR.getArgBuffer(32);
        dSprintf(ret, 32, "%d", arg);
        return ret;
    }
}

//------------------------------------------------------------

inline void ExprEvalState::setCurVarName(StringTableEntry name)
{
   if(name[0] == '$')
      currentVariable = globalVars.lookup(name);
   else if(stack.size())
      currentVariable = stack.last()->lookup(name);
   if(!currentVariable && gWarnUndefinedScriptVariables)
       Con::warnf(ConsoleLogEntry::Script, "Variable referenced before assignment: %s", name);
}

inline void ExprEvalState::setCurVarNameCreate(StringTableEntry name)
{
   if(name[0] == '$')
      currentVariable = globalVars.add(name);
   else if(stack.size())
      currentVariable = stack.last()->add(name);
   else
   {
      currentVariable = NULL;
      Con::warnf(ConsoleLogEntry::Script, "Accessing local variable in global scope... failed: %s", name);
   }
}

//------------------------------------------------------------

inline S32 ExprEvalState::getIntVariable()
{
   return currentVariable ? currentVariable->getIntValue() : 0;
}

inline F64 ExprEvalState::getFloatVariable()
{
   return currentVariable ? currentVariable->getFloatValue() : 0;
}

inline const char *ExprEvalState::getStringVariable()
{
   return currentVariable ? currentVariable->getStringValue() : "";
}

//------------------------------------------------------------

inline void ExprEvalState::setIntVariable(S32 val)
{
   AssertFatal(currentVariable != NULL, "Invalid evaluator state - trying to set null variable!");
   currentVariable->setIntValue(val);
}

inline void ExprEvalState::setFloatVariable(F64 val)
{
   AssertFatal(currentVariable != NULL, "Invalid evaluator state - trying to set null variable!");
   currentVariable->setFloatValue((F32)val);
}

inline void ExprEvalState::setStringVariable(const char *val)
{
   AssertFatal(currentVariable != NULL, "Invalid evaluator state - trying to set null variable!");
   currentVariable->setStringValue(val);
}

//------------------------------------------------------------

void CodeBlock::getFunctionArgs(char buffer[1024], U32 ip)
{
   U32 fnArgc = code[ip + 5];
   buffer[0] = 0;
   for(U32 i = 0; i < fnArgc; i++)
   {
      StringTableEntry var = CodeToSTE(code, ip + (i*2) + 6);
      
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

// Returns, in 'val', the specified component of a string.
static void getUnit(const char *string, U32 index, const char *set, char val[], S32 len)
{
   U32 sz;
   while(index--)
   {
      if(!*string)
         return;
      sz = dStrcspn(string, set);
      if (string[sz] == 0)
         return;
      string += (sz + 1);
   }
   sz = dStrcspn(string, set);
   if (sz == 0)
      return;

   if( ( sz + 1 ) > (U32)len )
      return;

   dStrncpy(val, string, sz);
   val[sz] = '\0';
}

// Copies a string, replacing the (space separated) specified component. The
// return value is stored in 'val'.
static void setUnit(const char *string, U32 index, const char *replace, const char *set, char val[], S32 len)
{
   U32 sz;
   const char *start = string;
   if( ( dStrlen(string) + dStrlen(replace) + 1 ) > (U32)len )
      return;

   U32 padCount = 0;

   while(index--)
   {
      sz = dStrcspn(string, set);
      if(string[sz] == 0)
      {
         string += sz;
         padCount = index + 1;
         break;
      }
      else
         string += (sz + 1);
   }
   // copy first chunk
   sz = (U32)(string-start);
   dStrncpy(val, start, sz);
   for(U32 i = 0; i < padCount; i++)
      val[sz++] = set[0];

   // replace this unit
   val[sz] = '\0';
   dStrcat(val, replace);

   // copy remaining chunks
   sz = dStrcspn(string, set);         // skip chunk we're replacing
   if(!sz && !string[sz])
      return;

   string += sz;
   dStrcat(val, string);
   return;
}

//-----------------------------------------------------------------------------

static bool isDigitsOnly( const char* pString )
{
    // Sanity.
    AssertFatal( pString != NULL, "isDigits() - Cannot check a NULL string." );

    const char* pDigitCursor = pString;
    if ( *pDigitCursor == 0 )
        return false;

    // Check for digits only.
    do
    {
        if ( dIsdigit( *pDigitCursor++ ) )
            continue;

        return false;
    }
    while( *pDigitCursor != 0 );

    return true;
}

//-----------------------------------------------------------------------------

static const StringTableEntry _xyzw[] = 
{
    StringTable->insert( "x" ),
    StringTable->insert( "y" ),
    StringTable->insert( "z" ),
    StringTable->insert( "w" )
};

static const StringTableEntry _rgba[] = 
{
    StringTable->insert( "r" ),
    StringTable->insert( "g" ),
    StringTable->insert( "b" ),
    StringTable->insert( "a" )
};

static const StringTableEntry _size[] = 
{
    StringTable->insert( "width" ),
    StringTable->insert( "height" )
};

static const StringTableEntry _count = StringTable->insert( "count" );

//-----------------------------------------------------------------------------

// Gets a component of an object's field value or a variable and returns it in val.
static void getFieldComponent( SimObject* object, StringTableEntry field, const char* array, StringTableEntry subField, char* val, const U32 bufferSize )
{
    const char* prevVal = NULL;
   
    // Grab value from object.
    if( object && field )
        prevVal = object->getDataField( field, array );
   
    // Otherwise, grab from the string stack. The value coming in will always
    // be a string because that is how multi-component variables are handled.
    else
        prevVal = STR.getStringValue();

    // Make sure we got a value.
    if ( prevVal && *prevVal )
    {
        if ( subField == _count )
            dSprintf( val, bufferSize, "%d", StringUnit::getUnitCount( prevVal, " \t\n" ) );

        else if ( subField == _xyzw[0] || subField == _rgba[0] || subField == _size[0] )
            dStrncpy( val, StringUnit::getUnit( prevVal, 0, " \t\n"), bufferSize );

        else if ( subField == _xyzw[1] || subField == _rgba[1] || subField == _size[1] )
            dStrncpy( val, StringUnit::getUnit( prevVal, 1, " \t\n"), bufferSize );

        else if ( subField == _xyzw[2] || subField == _rgba[2] )
            dStrncpy( val, StringUnit::getUnit( prevVal, 2, " \t\n"), bufferSize );

        else if ( subField == _xyzw[3] || subField == _rgba[3] )
            dStrncpy( val, StringUnit::getUnit( prevVal, 3, " \t\n"), bufferSize );

        else if ( *subField == '_' && isDigitsOnly(subField+1) )
            dStrncpy( val, StringUnit::getUnit( prevVal, dAtoi(subField+1), " \t\n"), bufferSize );

        else
            val[0] = 0;
    }
    else
        val[0] = 0;
}

//-----------------------------------------------------------------------------

// Sets a component of an object's field value based on the sub field. 'x' will
// set the first field, 'y' the second, and 'z' the third.
static void setFieldComponent( SimObject* object, StringTableEntry field, const char* array, StringTableEntry subField )
{
    // Copy the current string value
    char strValue[1024];
    dStrncpy( strValue, STR.getStringValue(), sizeof(strValue) );

    char val[1024] = "";
    const U32 bufferSize = sizeof(val);
    const char* prevVal = NULL;

    // Set the value on an object field.
    if( object && field )
        prevVal = object->getDataField( field, array );

    // Set the value on a variable.
    else if( gEvalState.currentVariable )
        prevVal = gEvalState.getStringVariable();

    // Ensure that the variable has a value
    if (!prevVal)
        return;

    if ( subField == _xyzw[0] || subField == _rgba[0] || subField == _size[0] )
        dStrncpy( val, StringUnit::setUnit( prevVal, 0, strValue, " \t\n"), bufferSize );

    else if ( subField == _xyzw[1] || subField == _rgba[1] || subField == _size[1] )
        dStrncpy( val, StringUnit::setUnit( prevVal, 1, strValue, " \t\n"), bufferSize );

    else if ( subField == _xyzw[2] || subField == _rgba[2] )
        dStrncpy( val, StringUnit::setUnit( prevVal, 2, strValue, " \t\n"), bufferSize );

    else if ( subField == _xyzw[3] || subField == _rgba[3] )
        dStrncpy( val, StringUnit::setUnit( prevVal, 3, strValue, " \t\n"), bufferSize );

    else if ( *subField == '_' && isDigitsOnly(subField+1) )
        dStrncpy( val, StringUnit::setUnit( prevVal, dAtoi(subField+1), strValue, " \t\n"), bufferSize );

    if ( val[0] != 0 )
    {
        // Update the field or variable.
        if( object && field )
            object->setDataField( field, 0, val );
        else if( gEvalState.currentVariable )
            gEvalState.setStringVariable( val );
    }
}

#define vmdispatch(o)	switch(o)
#define vmcase(l)	case l:
#define vmbreak		break

inline U32 ts2_equal(ConsoleValue* a, ConsoleValue* b)
{
	// TODO: handle same reference types
	return a->getFloatValue() == b->getFloatValue() ? 1 : 0;
}

inline U32 ts2_lt(ConsoleValue* a, ConsoleValue* b)
{
	return a->getFloatValue() < b->getFloatValue() ? 1 : 0;
}

inline U32 ts2_le(ConsoleValue* a, ConsoleValue* b)
{
	return a->getFloatValue() <= b->getFloatValue() ? 1 : 0;
}

// jamesu - string equals will compare values as if they were case insensitive strings
inline U32 ts2_eq_str(ConsoleValue* a, ConsoleValue* b)
{
	if (a->type == b->type)
	{
		if (a->type == ConsoleValue::TypeInternalStringTableEntry)
		{
			return a->value.string == b->value.string;
		}
		else if (a->type == ConsoleValue::TypeInternalStringStackPtr)
		{
			return a->value.stringStackPtr == b->value.stringStackPtr; // TOFIX
		}
		else if (a->type == ConsoleValue::TypeReferenceCounted)
		{
			return a->value.refValue->stringCompare(b->value.refValue->getString());
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

void CodeBlock::execBlock(CodeBlockEvalState *state)
{
    // Executes code block as normal
    U32 ip = state->savedIP;
    ConsoleValue *konst = state->constants;
    ConsoleValue *konstBase = konst;
    ConsoleValuePtr* const base = state->stack.address() + state->stackTop;
    const char *sval;
    
    char temp1[256];
    char temp2[256];
    StringTableEntry fnName;
	U32 targetRegTmp = 0;
    
    // TODO: remove these
    U32 callArgc;
    const char **callArgv;
    
    ConsoleValue acc;
    acc.type = ConsoleValue::TypeInternalFloat;
    U32 end;
	
	// Debug print instructions
	for (int j=0; j<codeSize; j++)
	{
		
		Compiler2::Instruction i = (code[j]);
		switch (TS2_OP_DEC(i))
		{
				vmcase(Compiler2::OP_MOVE) {
					
					Con::printf("[%i] OP_MOV %i %i", j, TS2_OP_DEC_A(i),TS2_OP_DEC_B(i));
					vmbreak;
				}
				vmcase(Compiler2::OP_LOADK) {
					
					Con::printf("[%i] OP_LOADK %i %i", j, TS2_OP_DEC_A(i),TS2_OP_DEC_Bx(i));
					vmbreak;
				}
				vmcase(Compiler2::OP_PAGEK) {
					Con::printf("[%i] OP_PAGEK %i %i", j, TS2_OP_DEC_A(i));
					konst = konstBase + (256 * TS2_OP_DEC_A(i));
					vmbreak;
				}
				vmcase(Compiler2::OP_LOADVAR) {
					// A := B
					Con::printf("[%i] OP_LOADVAR %i %i", j, TS2_OP_DEC_A(i),TS2_OP_DEC_B(i));
					vmbreak;
				}
				vmcase(Compiler2::OP_SETVAR) {
					// B := C
					Con::printf("[%i] OP_SETVAR %i %i %i", j, TS2_OP_DEC_A(i),TS2_OP_DEC_B(i),TS2_OP_DEC_C(i));
					vmbreak;
				}
				
				vmcase(Compiler2::OP_GETFUNC) {
					Con::printf("[%i] OP_GETFUNC -> %i", j, TS2_OP_DEC_A(i));
					vmbreak;
				}
				
#define _TS2_INT_OP(_op, _operand) \
				vmcase(_op) { \
					targetRegTmp = TS2_OP_DEC_A(i);\
					Con::printf(#_op "[%i] -> %i", j,  targetRegTmp);\
					vmbreak; \
				}
				
#define _TS2_FLOAT_OP(_op, _operand) \
				vmcase(_op) { \
					targetRegTmp = TS2_OP_DEC_A(i); \
					Con::printf(#_op "[%i] -> (%i & %i) (%f & %f) -> %i", j, TS2_OP_DEC_B(i), TS2_OP_DEC_C(i), TS2_BASE_OR_KONST(TS2_OP_DEC_B(i)).getFloatValue() , TS2_BASE_OR_KONST(TS2_OP_DEC_C(i)).getFloatValue(), targetRegTmp);\
					vmbreak; \
				}
				
				//TS2_FLOAT_OP(Compiler2::OP_ADD, +)
				vmcase(Compiler2::OP_ADD) {
					targetRegTmp = TS2_OP_DEC_A(i);
					Con::printf("Compiler2::OP_ADD" "[%i] -> (%i & %i) (%f & %f) -> %i", j, TS2_OP_DEC_B(i), TS2_OP_DEC_C(i), TS2_BASE_OR_KONST(TS2_OP_DEC_B(i)).getFloatValue() , TS2_BASE_OR_KONST(TS2_OP_DEC_C(i)).getFloatValue(), targetRegTmp);
					vmbreak;
				}
				
				//TS2_FLOAT_OP(Compiler2::OP_SUB, -)
				vmcase(Compiler2::OP_SUB) {
					targetRegTmp = TS2_OP_DEC_A(i);
					Con::printf("Compiler2::OP_SUB" "[%i] -> (%i & %i) (%f & %f) -> %i", j, TS2_OP_DEC_B(i), TS2_OP_DEC_C(i), TS2_BASE_OR_KONST(TS2_OP_DEC_B(i)).getFloatValue() , TS2_BASE_OR_KONST(TS2_OP_DEC_C(i)).getFloatValue(), targetRegTmp);
					vmbreak;
				}
				
				
				_TS2_FLOAT_OP(Compiler2::OP_MUL, *)
				_TS2_FLOAT_OP(Compiler2::OP_DIV, /)
				_TS2_INT_OP(Compiler2::OP_MOD, %)
				//_TS2_FLOAT_OP(Compiler2::OP_POW, +)
				//_TS2_FLOAT_OP(Compiler2::OP_UMN, -)
				//_TS2_MATH_OP(Compiler2::OP_NOT, !)
				_TS2_INT_OP(Compiler2::OP_XOR, ^)
				_TS2_INT_OP(Compiler2::OP_SHL, <<)
				_TS2_INT_OP(Compiler2::OP_SHR, >>)
				_TS2_INT_OP(Compiler2::OP_BITAND, &)
				_TS2_INT_OP(Compiler2::OP_BITOR, |)
				//_TS2_INT_OP(Compiler2::OP_ONESCOMPLEMENT, ~)
				
				
				vmcase(Compiler2::OP_CONCAT) {
					Con::printf("[%i] OP_CONCAT %i..%i", j, TS2_OP_DEC_B(i), TS2_OP_DEC_C(i));
					vmbreak;
				}
				
				vmcase(Compiler2::OP_JMP) {
					Con::printf("[%i] OP_JMP %i", j, TS2_OP_DEC_sBx(i));
					vmbreak;
				}
				
				vmcase(Compiler2::OP_LT) {
					Con::printf("[%i] OP_LT %i %i %i", j, TS2_OP_DEC_A(i), TS2_OP_DEC_B(i), TS2_OP_DEC_C(i));
					vmbreak;
				}
				
				vmcase(Compiler2::OP_LE) {
					Con::printf("[%i] OP_LE %i %i %i", j, TS2_OP_DEC_A(i), TS2_OP_DEC_B(i), TS2_OP_DEC_C(i));
					vmbreak;
				}
				
				vmcase(Compiler2::OP_EQ) {
					Con::printf("[%i] OP_EQ %i %i %i", j, TS2_OP_DEC_A(i), TS2_OP_DEC_B(i), TS2_OP_DEC_C(i));
					vmbreak;
				}
				
				vmcase(Compiler2::OP_EQ_STR) {
					Con::printf("[%i] OP_EQ_STR %i %i %i", j, TS2_OP_DEC_A(i), TS2_OP_DEC_B(i), TS2_OP_DEC_C(i));
					vmbreak;
				}
				
				vmcase(Compiler2::OP_CALL) {
					// [abc] [a := a(a[...b params])[...c return params]]
					U32 returnStart = TS2_OP_DEC_A(i);
					U32 numParams = TS2_OP_DEC_B(i);
					U32 numReturn = TS2_OP_DEC_C(i);
					
					Con::printf("[%i] OP_CALL %i %i %i", j, returnStart, numParams, numReturn);
					vmbreak;
				}
				
				vmcase(Compiler2::OP_RETURN) {
					Con::printf("[%i] OP_RETURN %i", j, TS2_OP_DEC_A(i));
					vmbreak;
				}
		}
	}

    for (;;)
    {
        const Compiler2::Instruction i = (code[ip++]);
		 
		  ConsoleValue* const ra = base+TS2_OP_DEC_A(i);
        vmdispatch (TS2_OP_DEC(i)) {
            vmcase(Compiler2::OP_MOVE) {
					
#ifdef TS2_VM_DEBUG
					Con::printf("[%i] OP_MOV %i %i", ip-1, ra,TS2_OP_DEC_B(i));
#endif
					 //ConsoleValue* v1 = base+ra;
					 //ConsoleValue* v2 = base+TS2_OP_DEC_B(i);
					//*v1 = *v2;
					
					//*ra = *(base+TS2_OP_DEC_B(i));
					
					*ra = *(base+(((i) >> 14) & 0x1FF));
					
                //base[ra] = base[TS2_OP_DEC_B(i)];
                vmbreak;
            }
            vmcase(Compiler2::OP_LOADK) {
					
#ifdef TS2_VM_DEBUG
					Con::printf("[%i] OP_LOADK %i %i", ip-1, ra,TS2_OP_DEC_Bx(i));
#endif
                *ra = konst[TS2_OP_DEC_Bx(i)];
                vmbreak;
            }
            vmcase(Compiler2::OP_PAGEK) {
                konst = konstBase + (256 * TS2_OP_DEC_A(i));
                vmbreak;
            }
            vmcase(Compiler2::OP_LOADVAR) {
					// A := B
#ifdef TS2_VM_DEBUG
					Con::printf("[%i] OP_LOADVAR %i %i", ip-1, ra,TS2_OP_DEC_B(i));
#endif
                *((ConsoleValuePtr*)(ra)) = ConsoleStringValue::fromString(Con::getVariable(TS2_BASE_OR_KONST(TS2_OP_DEC_B(i)).getTempStringValue()));
                vmbreak;
            }
            vmcase(Compiler2::OP_SETVAR) {
                // B := C
					
#ifdef TS2_VM_DEBUG
					Con::printf("[%i] OP_SETVAR %i %i %i", ip-1, ra,TS2_OP_DEC_B(i),TS2_OP_DEC_C(i));
#endif
					U32 bValue = TS2_OP_DEC_B(i);
                TS2_BASE_OR_KONST(TS2_OP_DEC_B(i)).getInternalStringValue(temp1, 256);
                Con::setVariable(temp1, TS2_BASE_OR_KONST(TS2_OP_DEC_C(i)).getTempStringValue());
                vmbreak;
            }
			  
			  vmcase(Compiler2::OP_GETFUNC) {
				  U32 setReg = TS2_OP_DEC_A(i);
				  ConsoleValuePtr objId = TS2_BASE_OR_KONST(TS2_OP_DEC_B(i));
				  ConsoleValuePtr nameId = TS2_BASE_OR_KONST(TS2_OP_DEC_C(i));
				  
				  
#ifdef TS2_VM_DEBUG
				  Con::printf("[%i] OP_GETFUNC -> %i", ip-1, ra);
#endif
				  
				  if (objId.type == ConsoleValue::TypeInternalNull)
				  {
					  Namespace* ns = Namespace::global();
					  
					  Namespace::Entry* entry = ns->lookup(nameId.getSTEStringValue());
					  if (!entry)
					  {
						  base[setReg].setNull();
					  }
					  else
					  {
						  base[setReg].setNull();
						  base[setReg].type = ConsoleValue::TypeInternalNamespaceEntry;
						  base[setReg].value.ptrValue = entry;
					  }
				  }
				  
				  vmbreak;
			  }
			  
            /*
				 
				 
				 
            OP_GETFIELD,     //  a b c [a := b[c]]      ; gets an object property or table field
            OP_GETFUNC,      //  a b c [a := b.func ns] ; gets bound ns function for object
				 
            OP_SETVAR,       // [ab]    [name(a) := b]     ; assigns a global variable
            OP_SETFIELD,     // [abc] [a[b] := c]        ; sets an object property or table field
            OP_SETLIST,      // [ab...c]                  ; sets a list of items (similar to lua)
            OP_COPYFIELDS,   // [abc] [a.fields := b.fields] ; copy object data or hash fields to object
				 
            */
			  
			  
			  //Con::printf(#_op "[%i] -> %i", ip-1,  targetRegTmp);
#define TS2_INT_OP(_op, _operand) \
			  vmcase(_op) { \
				  acc.value.ival = (TS2_BASE_OR_KONST(TS2_OP_DEC_B(i)).getIntValue() _operand TS2_BASE_OR_KONST(TS2_OP_DEC_C(i)).getIntValue()); \
				  *ra = acc; \
				  vmbreak; \
			  }
			  
					//Con::printf(#_op "[%i] -> (%i & %i) (%f & %f) -> %i", ip-1, TS2_OP_DEC_B(i), TS2_OP_DEC_C(i), TS2_BASE_OR_KONST(TS2_OP_DEC_B(i)).getFloatValue() , TS2_BASE_OR_KONST(TS2_OP_DEC_C(i)).getFloatValue(), targetRegTmp);
#define TS2_FLOAT_OP(_op, _operand) \
            vmcase(_op) { \
                acc.value.fval = (TS2_BASE_OR_KONST(TS2_OP_DEC_B(i)).getFloatValue() _operand TS2_BASE_OR_KONST(TS2_OP_DEC_C(i)).getFloatValue()); \
                *ra = acc; \
                vmbreak; \
            }
            
			  //TS2_FLOAT_OP(Compiler2::OP_ADD, +)
			  vmcase(Compiler2::OP_ADD) {
				  //Con::printf("Compiler2::OP_ADD" "[%i] -> (%i & %i) (%f & %f) -> %i", ip-1, TS2_OP_DEC_B(i), TS2_OP_DEC_C(i), TS2_BASE_OR_KONST(TS2_OP_DEC_B(i)).getFloatValue() , TS2_BASE_OR_KONST(TS2_OP_DEC_C(i)).getFloatValue(), targetRegTmp);
				  acc.value.fval = (TS2_BASE_OR_KONST(TS2_OP_DEC_B(i)).getFloatValue() + TS2_BASE_OR_KONST(TS2_OP_DEC_C(i)).getFloatValue());
				  *ra = acc;
				  vmbreak;
			  }
			  
			  //TS2_FLOAT_OP(Compiler2::OP_SUB, -)
			  vmcase(Compiler2::OP_SUB) {
				  //Con::printf("Compiler2::OP_SUB" "[%i] -> (%i & %i) (%f & %f) -> %i", ip-1, TS2_OP_DEC_B(i), TS2_OP_DEC_C(i), TS2_BASE_OR_KONST(TS2_OP_DEC_B(i)).getFloatValue() , TS2_BASE_OR_KONST(TS2_OP_DEC_C(i)).getFloatValue(), targetRegTmp);
				  acc.value.fval = (TS2_BASE_OR_KONST(TS2_OP_DEC_B(i)).getFloatValue() - TS2_BASE_OR_KONST(TS2_OP_DEC_C(i)).getFloatValue());
				  *ra = acc;
				  vmbreak;
			  }
			  
			  
            TS2_FLOAT_OP(Compiler2::OP_MUL, *)
            TS2_FLOAT_OP(Compiler2::OP_DIV, /)
            TS2_INT_OP(Compiler2::OP_MOD, %)
            //TS2_FLOAT_OP(Compiler2::OP_POW, +)
            //TS2_FLOAT_OP(Compiler2::OP_UMN, -)
            //TS2_MATH_OP(Compiler2::OP_NOT, !)
            TS2_INT_OP(Compiler2::OP_XOR, ^)
            TS2_INT_OP(Compiler2::OP_SHL, <<)
            TS2_INT_OP(Compiler2::OP_SHR, >>)
            TS2_INT_OP(Compiler2::OP_BITAND, &)
            TS2_INT_OP(Compiler2::OP_BITOR, |)
            //TS2_INT_OP(Compiler2::OP_ONESCOMPLEMENT, ~)
            
            
            vmcase(Compiler2::OP_CONCAT) {
					
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
            
            vmcase(Compiler2::OP_JMP) {
					//U32 rawIp = TS2_OP_DEC_Bx(i);
					//S32 ipAddr = TS2_OP_DEC_sBx(i);
#ifdef TS2_VM_DEBUG
					Con::printf("[%i] OP_JMP %i", ip-1, ipAddr);
#endif
                ip += TS2_OP_DEC_sBx(i);
                vmbreak;
				}
			  
			  vmcase(Compiler2::OP_LT) {
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
			  
			  vmcase(Compiler2::OP_LE) {
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
			  
			  vmcase(Compiler2::OP_EQ) {
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
			  
			  vmcase(Compiler2::OP_EQ_STR) {
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
			  
            vmcase(Compiler2::OP_CALL) {
                // [abc] [a := a(a[...b params])[...c return params]]
                U32 returnStart = TS2_OP_DEC_A(i);
                U32 numParams = TS2_OP_DEC_B(i);
                U32 numReturn = TS2_OP_DEC_C(i);
					
					
#ifdef TS2_VM_DEBUG
					Con::printf("[%i] OP_CALL %i %i %i", ip-1, returnStart, numParams, numReturn);
#endif
					
                if (base[returnStart].type != ConsoleValue::TypeInternalNamespaceEntry)
                {
                    Con::errorf("Invalid namespace entry!");
                    continue;
                }
                
                Namespace::Entry* nsEntry = static_cast<Namespace::Entry*>(base[returnStart].value.ptrValue);
                
                STR.pushFrame();
                for (U32 j=1; j<numParams+1; j++)
                {
						 STR.setStringValue(base[returnStart+j].getTempStringValue());
						 STR.push();
                }
                STR.getArgcArgv(nsEntry->mFunctionName, &callArgc, &callArgv);
                
                // Now we have to call it!
                S32 nsType = -1;
                S32 nsMinArgs = 0;
                S32 nsMaxArgs = 0;
                Namespace::Entry::CallbackUnion * nsCb = NULL;
                const char * nsUsage = NULL;
                if (nsEntry)
                {
                    nsType = nsEntry->mType;
                    nsMinArgs = nsEntry->mMinArgs;
                    nsMaxArgs = nsEntry->mMaxArgs;
                    nsCb = &nsEntry->cb;
                    nsUsage = nsEntry->mUsage;
                }
                if(nsEntry->mType == Namespace::Entry::ScriptFunctionType)
                {
                    const char *ret = "";
                    if(nsEntry->mFunctionOffset)
                        ret = nsEntry->mCode->exec(nsEntry->mFunctionOffset, nsEntry->mFunctionName, nsEntry->mNamespace, callArgc, callArgv, false, nsEntry->mPackage);
                    
                    base[returnStart] = ConsoleStringValue::fromString(ret);
                }
                else
                {
                    const char* nsName = "";
                    if((nsEntry->mMinArgs && S32(callArgc) < nsEntry->mMinArgs) || (nsEntry->mMaxArgs && S32(callArgc) > nsEntry->mMaxArgs))
                    {
                        Con::warnf(ConsoleLogEntry::Script, "%s: %s::%s - wrong number of arguments.", getFileLine(ip), nsName, fnName);
                        Con::warnf(ConsoleLogEntry::Script, "%s: usage: %s", getFileLine(ip), nsEntry->mUsage);
                    }
                    else
                    {
                        SimObject* thisObject = numParams > 0 ? base[returnStart+1].getSimObject() : NULL;
                        
                        switch(nsEntry->mType)
                        {
                            case Namespace::Entry::StringCallbackType:
                            {
                                const char *ret = nsEntry->cb.mStringCallbackFunc(thisObject, callArgc, callArgv);
                                
                                base[returnStart] = ConsoleStringValue::fromString(ret);
                                break;
                            }
                            case Namespace::Entry::IntCallbackType:
                            {
                                S32 result = nsEntry->cb.mIntCallbackFunc(thisObject, callArgc, callArgv);
                                base[returnStart].setValue(result);
                                break;
                            }
                            case Namespace::Entry::FloatCallbackType:
                            {
                                F64 result = nsEntry->cb.mFloatCallbackFunc(thisObject, callArgc, callArgv);
                                base[returnStart].setValue((F32)result);
                                break;
                            }
                            case Namespace::Entry::VoidCallbackType:
                            {
                                nsEntry->cb.mVoidCallbackFunc(thisObject, callArgc, callArgv);
                                
                                base[returnStart].setNull();
                                break;
                            }
                            case Namespace::Entry::BoolCallbackType:
                            {
                                bool result = nsEntry->cb.mBoolCallbackFunc(thisObject, callArgc, callArgv);
                                
                                base[returnStart].setValue(result ? 1 : 0);
                                break;
                            }
                        }
                    }
                }
                
                STR.popFrame();
                vmbreak;
				}
			  
			  vmcase(Compiler2::OP_RETURN) {
				  
#ifdef TS2_VM_DEBUG
				  Con::printf("OP_RETURN %i", ra);
#endif
				  state->yieldValue = *ra;
				  return;
				  vmbreak;
			  }
			  
            /*
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
             
             OP_LT,     // [abc] <  (inverse == >=)
             OP_LE,     // [abc] <= (inverse == >)
             OP_EQ,     // [abc] == (inverse == !=)
             OP_EQ_STR, // [abc] $= (specific string comparison)
             
             OP_JMP,    // [iAsBx pc] jump to pc
             OP_CALL,   // [abc] [a(b...c)]
             
             OP_RETURN,  // [abc] return from function, returning a
             */
        }
    }
    
    state->savedIP = ip;
}

void CodeBlock::execWithEnv(CodeBlockEvalState *state, CodeBlockFunction *srcEnv, CodeBlockFunction *destEnv)
{
    // Copies references to variables in srcEnv's scope into their corresponding position in destEnv
}

void CodeBlock::execFunction(CodeBlockEvalState *state, CodeBlockFunction *env, U32 argc, const char **argv, StringTableEntry packageName)
{
    // Prepares an executes a function, copying argv into the stack according to env
    
}

const char *CodeBlock::exec(U32 ip, const char *functionName, Namespace *thisNamespace, U32 argc, const char **argv, bool noCalls, StringTableEntry packageName, S32 setFrame)
{
#ifdef TORQUE_DEBUG
    U32 stackStart = STR.mStartStackSize;
#endif
    
#if 0
    
    static char traceBuffer[1024];
    U32 i;
    
    incRefCount();
    STR.clearFunctionOffset();
    
    /* 
     
     We will now execute code. However before we do this we need to ensure the following:
     
     - If we are calling exec, we need to assign references to values in the parent stack, which is achieved by enumerating the local value indexes and pushing refs to them. i.e.
            [function stack (a,b,c)] [exec stack](aref, bref, cref).
       Since in TS2 all variables are inserted into the start of the function, references will be copied HOWEVER if for instance we have two exec calls which both use a private value they will not 
           see it. i.e. the value needs to be present in the parent scope for it to work.
     - If we are entering a function, we push our function then parameters. Then we just move the base stack pointer and we're done.
     
     */
    StringTableEntry thisFunctionName = NULL;
    bool popFrame = false;
    
    // NOTE
    if(argv)
    {
        
        
        
        if(gEvalState.traceOn)
        {
            traceBuffer[0] = 0;
            dStrcat(traceBuffer, "Entering ");
            if(packageName)
            {
                dStrcat(traceBuffer, "[");
                dStrcat(traceBuffer, packageName);
                dStrcat(traceBuffer, "]");
            }
            if(thisNamespace && thisNamespace->mName)
            {
                dSprintf(traceBuffer + dStrlen(traceBuffer), sizeof(traceBuffer) - dStrlen(traceBuffer),
                         "%s::%s(", thisNamespace->mName, thisFunctionName);
            }
            else
            {
                dSprintf(traceBuffer + dStrlen(traceBuffer), sizeof(traceBuffer) - dStrlen(traceBuffer),
                         "%s(", thisFunctionName);
            }
            for(i = 0; i < argc; i++)
            {
                dStrcat(traceBuffer, argv[i+1]);
                if(i != argc - 1)
                    dStrcat(traceBuffer, ", ");
            }
            dStrcat(traceBuffer, ")");
            Con::printf("%s", traceBuffer);
        }
        gEvalState.pushFrame(thisFunctionName, thisNamespace);
        popFrame = true;
        for(i = 0; i < argc; i++)
        {
            StringTableEntry var = CodeToSTE(code, ip + (2 + 6 + 1) + (i * 2));
            gEvalState.setCurVarNameCreate(var);
            gEvalState.setStringVariable(argv[i+1]);
        }
        ip = ip + (fnArgc * 2) + (2 + 6 + 1);
        curFloatTable = functionFloats;
        curStringTable = functionStrings;
    }
    else
    {
        curFloatTable = globalFloats;
        curStringTable = globalStrings;
        
        // Do we want this code to execute using a new stack frame?
        if (setFrame < 0)
        {
            gEvalState.pushFrame(NULL, NULL);
            popFrame = true;
        }
        else if (!gEvalState.stack.empty())
        {
            // We want to copy a reference to an existing stack frame
            // on to the top of the stack.  Any change that occurs to
            // the locals during this new frame will also occur in the
            // original frame.
            S32 stackIndex = gEvalState.stack.size() - setFrame - 1;
            gEvalState.pushFrameRef( stackIndex );
            popFrame = true;
        }
    }
    
#endif
}

#if 0

const char *CodeBlock::exec(U32 ip, const char *functionName, Namespace *thisNamespace, U32 argc, const char **argv, bool noCalls, StringTableEntry packageName, S32 setFrame)
{
#ifdef TORQUE_DEBUG
   U32 stackStart = STR.mStartStackSize;
#endif

   static char traceBuffer[1024];
   U32 i;

   incRefCount();
   F64 *curFloatTable;
   char *curStringTable;
   STR.clearFunctionOffset();
   StringTableEntry thisFunctionName = NULL;
   bool popFrame = false;
   if(argv)
   {
      // assume this points into a function decl:
      U32 fnArgc = code[ip + 2 + 6];
      thisFunctionName = CodeToSTE(code, ip);
      argc = getMin(argc-1, fnArgc); // argv[0] is func name
      if(gEvalState.traceOn)
      {
         traceBuffer[0] = 0;
         dStrcat(traceBuffer, "Entering ");
         if(packageName)
         {
            dStrcat(traceBuffer, "[");
            dStrcat(traceBuffer, packageName);
            dStrcat(traceBuffer, "]");
         }
         if(thisNamespace && thisNamespace->mName)
         {
            dSprintf(traceBuffer + dStrlen(traceBuffer), sizeof(traceBuffer) - dStrlen(traceBuffer),
               "%s::%s(", thisNamespace->mName, thisFunctionName);
         }
         else
         {
            dSprintf(traceBuffer + dStrlen(traceBuffer), sizeof(traceBuffer) - dStrlen(traceBuffer),
               "%s(", thisFunctionName);
         }
         for(i = 0; i < argc; i++)
         {
            dStrcat(traceBuffer, argv[i+1]);
            if(i != argc - 1)
               dStrcat(traceBuffer, ", ");
         }
         dStrcat(traceBuffer, ")");
         Con::printf("%s", traceBuffer);
      }
      gEvalState.pushFrame(thisFunctionName, thisNamespace);
      popFrame = true;
      for(i = 0; i < argc; i++)
      {
         StringTableEntry var = CodeToSTE(code, ip + (2 + 6 + 1) + (i * 2));
         gEvalState.setCurVarNameCreate(var);
         gEvalState.setStringVariable(argv[i+1]);
      }
      ip = ip + (fnArgc * 2) + (2 + 6 + 1);
      curFloatTable = functionFloats;
      curStringTable = functionStrings;
   }
   else
   {
      curFloatTable = globalFloats;
      curStringTable = globalStrings;

      // Do we want this code to execute using a new stack frame?
      if (setFrame < 0)
      {
         gEvalState.pushFrame(NULL, NULL);
         popFrame = true;
      }
      else if (!gEvalState.stack.empty())
      {
         // We want to copy a reference to an existing stack frame
         // on to the top of the stack.  Any change that occurs to 
         // the locals during this new frame will also occur in the 
         // original frame.
         S32 stackIndex = gEvalState.stack.size() - setFrame - 1;
         gEvalState.pushFrameRef( stackIndex );
         popFrame = true;
      }
   }

   // Grab the state of the telenet debugger here once
   // so that the push and pop frames are always balanced.
   const bool telDebuggerOn = TelDebugger && TelDebugger->isConnected();
   if ( telDebuggerOn && setFrame < 0 )
      TelDebugger->pushStackFrame();

   // Notify the remote debugger.
   RemoteDebuggerBase* pRemoteDebugger = RemoteDebuggerBase::getRemoteDebugger();
   if ( pRemoteDebugger != NULL && setFrame < 0 )
       pRemoteDebugger->pushStackFrame();

   StringTableEntry var, objParent;
   U32 failJump;
   StringTableEntry fnName;
    StringTableEntry fnNamespace, fnPackage;
    // Add local object creation stack [7/9/2007 Black]
    static const U32 objectCreationStackSize = 32;
    U32 objectCreationStackIndex = 0;
    struct {
        SimObject *newObject;
        U32 failJump;
    } objectCreationStack[ objectCreationStackSize ];
   SimObject *currentNewObject = 0;
   StringTableEntry prevField = NULL;
   StringTableEntry curField = NULL;
   SimObject *prevObject = NULL;
   SimObject *curObject = NULL;
   SimObject *saveObject=NULL;
   Namespace::Entry *nsEntry;
   Namespace *ns;
   const char* curFNDocBlock = NULL;
   const char* curNSDocBlock = NULL;
   const S32 nsDocLength = 128;
   char nsDocBlockClass[nsDocLength];

   U32 callArgc;
   const char **callArgv;

   static char curFieldArray[256];
   static char prevFieldArray[256];

   CodeBlock *saveCodeBlock = smCurrentCodeBlock;
   smCurrentCodeBlock = this;
   if(this->name)
   {
      Con::gCurrentFile = this->name;
      Con::gCurrentRoot = mRoot;
   }
   const char * val;

   // The frame temp is used by the variable accessor ops (OP_SAVEFIELD_* and
   // OP_LOADFIELD_*) to store temporary values for the fields.
   static S32 VAL_BUFFER_SIZE = 1024;
   FrameTemp<char> valBuffer( VAL_BUFFER_SIZE );
   
   for(;;)
   {
      U32 instruction = code[ip++];
breakContinue:
      switch(instruction)
      {
         case OP_FUNC_DECL:
            if(!noCalls)
            {
               fnName       = CodeToSTE(code, ip);
               fnNamespace  = CodeToSTE(code, ip+2);
               fnPackage    = CodeToSTE(code, ip+4);
               bool hasBody = bool(code[ip+6]);
               
               Namespace::unlinkPackages();
               ns = Namespace::find(fnNamespace, fnPackage);
               ns->addFunction(fnName, this, hasBody ? ip : 0, curFNDocBlock ? dStrdup( curFNDocBlock ) : NULL );// if no body, set the IP to 0
               if( curNSDocBlock )
               {
                  if( fnNamespace == StringTable->lookup( nsDocBlockClass ) )
                  {
                     char *usageStr = dStrdup( curNSDocBlock );
                     usageStr[dStrlen(usageStr)] = '\0';
                     ns->mUsage = usageStr;
                     ns->mCleanUpUsage = true;
                     curNSDocBlock = NULL;
                  }
               }
               Namespace::relinkPackages();

               // If we had a docblock, it's definitely not valid anymore, so clear it out.
               curFNDocBlock = NULL;

               //Con::printf("Adding function %s::%s (%d)", fnNamespace, fnName, ip);
            }
            ip = code[ip + 7];
            break;

         case OP_CREATE_OBJECT:
         {
            // Read some useful info.
            objParent        = CodeToSTE(code, ip);
            bool isDataBlock =          code[ip + 2];
            bool isInternal  =          code[ip + 3];
            bool isSingleton =          code[ip + 4];
            U32  lineNumber  =          code[ip + 5];
            failJump         =          code[ip + 6];
            
            // If we don't allow calls, we certainly don't allow creating objects!
            // Moved this to after failJump is set. Engine was crashing when
            // noCalls = true and an object was being created at the beginning of
            // a file. ADL.
            if(noCalls)
            {
               ip = failJump;
               break;
            }

            // Get the constructor information off the stack.
            STR.getArgcArgv(NULL, &callArgc, &callArgv, true);
            const char *objectName = callArgv[ 2 ];

            // Con::printf("Creating object...");

            // objectName = argv[1]...
            currentNewObject = NULL;

            // Are we creating a datablock? If so, deal with case where we override
            // an old one.
            if(isDataBlock)
            {
               // Con::printf("  - is a datablock");

               // Find the old one if any.
               SimObject *db = Sim::getDataBlockGroup()->findObject(callArgv[2]);
               
               // Make sure we're not changing types on ourselves...
               if(db && dStricmp(db->getClassName(), callArgv[1]))
               {
                  Con::errorf(ConsoleLogEntry::General, "Cannot re-declare data block %s with a different class.", callArgv[2]);
                  ip = failJump;
                  break;
               }

               // If there was one, set the currentNewObject and move on.
               if(db)
                  currentNewObject = db;
            }

            if(!currentNewObject)
            {
               // Well, looks like we have to create a new object.
               ConsoleObject *object = ConsoleObject::create(callArgv[1]);

               // Deal with failure!
               if(!object)
               {
                  Con::errorf(ConsoleLogEntry::General, "%s: Unable to instantiate non-conobject class %s.", getFileLine(ip-1), callArgv[1]);
                  ip = failJump;
                  break;
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
                     Con::errorf(ConsoleLogEntry::General, "%s: Unable to instantiate non-datablock class %s.", getFileLine(ip-1), callArgv[1]);

                     // Clean up...
                     delete object;
                     ip = failJump;
                     break;
                  }
               }

               // Finally, set currentNewObject to point to the new one.
               currentNewObject = dynamic_cast<SimObject *>(object);

               // Deal with the case of a non-SimObject.
               if(!currentNewObject)
               {
                  Con::errorf(ConsoleLogEntry::General, "%s: Unable to instantiate non-SimObject class %s.", getFileLine(ip-1), callArgv[1]);
                  delete object;
                  ip = failJump;
                  break;
               }

               // Does it have a parent object? (ie, the copy constructor : syntax, not inheriance)
               // [tom, 9/8/2006] it is inheritance if it's a message ... muwahahah!
               if(*objParent)
               {
                  // Find it!
                  SimObject *parent;
                  if(Sim::findObject(objParent, parent))
                  {
                     // Con::printf(" - Parent object found: %s", parent->getClassName());

                     // and suck the juices from it!
                     currentNewObject->assignFieldsFrom(parent);
                  }
                  else
                     Con::errorf(ConsoleLogEntry::General, "%s: Unable to find parent object %s for %s.", getFileLine(ip-1), objParent, callArgv[1]);

                  // Mm! Juices!
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
               if(!currentNewObject->processArguments(callArgc-3, callArgv+3))
               {
                  delete currentNewObject;
                  currentNewObject = NULL;
                  ip = failJump;
                  break;
               }

               // If it's not a datablock, allow people to modify bits of it.
               if(!isDataBlock)
               {
                  currentNewObject->setModStaticFields(true);
                  currentNewObject->setModDynamicFields(true);
               }
            }

            // Advance the IP past the create info...
            ip += 7;
            break;
         }

         case OP_ADD_OBJECT:
         {
            // See OP_SETCURVAR for why we do this.
            curFNDocBlock = NULL;
            curNSDocBlock = NULL;
            
            // Do we place this object at the root?
            bool placeAtRoot = code[ip++];

            // Con::printf("Adding object %s", currentNewObject->getName());

            // Make sure it wasn't already added, then add it.
            if (currentNewObject == NULL)
            {
               break;
            }

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
                     Con::errorf("%s: No more object IDs available for messages", getFileLine(ip-2));
               }
               else
                  ret = currentNewObject->registerObject();
               
               if(! ret)
               {
                  // This error is usually caused by failing to call Parent::initPersistFields in the class' initPersistFields().
                  Con::warnf(ConsoleLogEntry::General, "%s: Register object failed for object %s of class %s.", getFileLine(ip-2), currentNewObject->getName(), currentNewObject->getClassName());
                  delete currentNewObject;
                  ip = failJump;
                  break;
               }
            }

            // Are we dealing with a datablock?
            SimDataBlock *dataBlock = dynamic_cast<SimDataBlock *>(currentNewObject);
            static char errorBuffer[256];

            // If so, preload it.
            if(dataBlock && !dataBlock->preload(true, errorBuffer))
            {
               Con::errorf(ConsoleLogEntry::General, "%s: preload failed for %s: %s.", getFileLine(ip-2),
                           currentNewObject->getName(), errorBuffer);
               dataBlock->deleteObject();
               ip = failJump;
               break;
            }

            // What group will we be added to, if any?
            U32 groupAddId = (U32)intStack[UINT];
            SimGroup *grp = NULL;
            SimSet   *set = NULL;
            SimComponent *comp = NULL;
            bool isMessage = dynamic_cast<Message *>(currentNewObject) != NULL;

            if(!placeAtRoot || !currentNewObject->getGroup())
            {
               if(! isMessage)
               {
                  if(! placeAtRoot)
                  {
                     // Otherwise just add to the requested group or set.
                     if(!Sim::findObject(groupAddId, grp))
                        if(!Sim::findObject(groupAddId, comp))
                           Sim::findObject(groupAddId, set);
                  }
                  
                  if(placeAtRoot || comp != NULL)
                  {
                     // Deal with the instantGroup if we're being put at the root or we're adding to a component.
                     const char *addGroupName = Con::getVariable("instantGroup");
                     if(!Sim::findObject(addGroupName, grp))
                        Sim::findObject(RootGroupId, grp);
                  }

                  if(comp)
                  {
                     SimComponent *newComp = dynamic_cast<SimComponent *>(currentNewObject);
                     if(newComp)
                     {
                        if(! comp->addComponent(newComp))
                           Con::errorf("%s: Unable to add component %s, template not loaded?", getFileLine(ip-2), currentNewObject->getName() ? currentNewObject->getName() : currentNewObject->getIdString());
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

            // store the new object's ID on the stack (overwriting the group/set
            // id, if one was given, otherwise getting pushed)
            if(placeAtRoot) 
               intStack[UINT] = currentNewObject->getId();
            else
               intStack[++UINT] = currentNewObject->getId();

            break;
         }

         case OP_END_OBJECT:
         {
            // If we're not to be placed at the root, make sure we clean up
            // our group reference.
            bool placeAtRoot = code[ip++];
            if(!placeAtRoot)
               UINT--;
            break;
         }
              
          case OP_FINISH_OBJECT:
          {
              //Assert( objectCreationStackIndex >= 0 );
              // Restore the object info from the stack [7/9/2007 Black]
              currentNewObject = objectCreationStack[ --objectCreationStackIndex ].newObject;
              failJump = objectCreationStack[ objectCreationStackIndex ].failJump;
              break;
          }

         case OP_JMPIFFNOT:
            if(floatStack[FLT--])
            {
               ip++;
               break;
            }
            ip = code[ip];
            break;
         case OP_JMPIFNOT:
            if(intStack[UINT--])
            {
               ip++;
               break;
            }
            ip = code[ip];
            break;
         case OP_JMPIFF:
            if(!floatStack[FLT--])
            {
               ip++;
               break;
            }
            ip = code[ip];
            break;
         case OP_JMPIF:
            if(!intStack[UINT--])
            {
               ip ++;
               break;
            }
            ip = code[ip];
            break;
         case OP_JMPIFNOT_NP:
            if(intStack[UINT])
            {
               UINT--;
               ip++;
               break;
            }
            ip = code[ip];
            break;
         case OP_JMPIF_NP:
            if(!intStack[UINT])
            {
               UINT--;
               ip++;
               break;
            }
            ip = code[ip];
            break;
         case OP_JMP:
            ip = code[ip];
            break;
         // This fixes a bug when not explicitly returning a value.
         case OP_RETURN_VOID:
            STR.setStringValue("");
            // We're falling thru here on purpose.
         case OP_RETURN:
            goto execFinished;
         case OP_CMPEQ:
            intStack[UINT+1] = bool(floatStack[FLT] == floatStack[FLT-1]);
            UINT++;
            FLT -= 2;
            break;

         case OP_CMPGR:
            intStack[UINT+1] = bool(floatStack[FLT] > floatStack[FLT-1]);
            UINT++;
            FLT -= 2;
            break;

         case OP_CMPGE:
            intStack[UINT+1] = bool(floatStack[FLT] >= floatStack[FLT-1]);
            UINT++;
            FLT -= 2;
            break;

         case OP_CMPLT:
            intStack[UINT+1] = bool(floatStack[FLT] < floatStack[FLT-1]);
            UINT++;
            FLT -= 2;
            break;

         case OP_CMPLE:
            intStack[UINT+1] = bool(floatStack[FLT] <= floatStack[FLT-1]);
            UINT++;
            FLT -= 2;
            break;

         case OP_CMPNE:
            intStack[UINT+1] = bool(floatStack[FLT] != floatStack[FLT-1]);
            UINT++;
            FLT -= 2;
            break;

         case OP_XOR:
            intStack[UINT-1] = intStack[UINT] ^ intStack[UINT-1];
            UINT--;
            break;

         case OP_MOD:
            if(  intStack[UINT-1] != 0 )
               intStack[UINT-1] = intStack[UINT] % intStack[UINT-1];
            else
               intStack[UINT-1] = 0;
            UINT--;
            break;

         case OP_BITAND:
            intStack[UINT-1] = intStack[UINT] & intStack[UINT-1];
            UINT--;
            break;

         case OP_BITOR:
            intStack[UINT-1] = intStack[UINT] | intStack[UINT-1];
            UINT--;
            break;

         case OP_NOT:
            intStack[UINT] = !intStack[UINT];
            break;

         case OP_NOTF:
            intStack[UINT+1] = !floatStack[FLT];
            FLT--;
            UINT++;
            break;

         case OP_ONESCOMPLEMENT:
            intStack[UINT] = ~intStack[UINT];
            break;

         case OP_SHR:
            intStack[UINT-1] = intStack[UINT] >> intStack[UINT-1];
            UINT--;
            break;

         case OP_SHL:
            intStack[UINT-1] = intStack[UINT] << intStack[UINT-1];
            UINT--;
            break;

         case OP_AND:
            intStack[UINT-1] = intStack[UINT] && intStack[UINT-1];
            UINT--;
            break;

         case OP_OR:
            intStack[UINT-1] = intStack[UINT] || intStack[UINT-1];
            UINT--;
            break;

         case OP_ADD:
            floatStack[FLT-1] = floatStack[FLT] + floatStack[FLT-1];
            FLT--;
            break;

         case OP_SUB:
            floatStack[FLT-1] = floatStack[FLT] - floatStack[FLT-1];
            FLT--;
            break;

         case OP_MUL:
            floatStack[FLT-1] = floatStack[FLT] * floatStack[FLT-1];
            FLT--;
            break;
         case OP_DIV:
            floatStack[FLT-1] = floatStack[FLT] / floatStack[FLT-1];
            FLT--;
            break;
         case OP_NEG:
            floatStack[FLT] = -floatStack[FLT];
            break;

         case OP_SETCURVAR:
            var = CodeToSTE(code, ip);
            ip += 2;

            // If a variable is set, then these must be NULL. It is necessary
            // to set this here so that the vector parser can appropriately
            // identify whether it's dealing with a vector.
            prevField = NULL;
            prevObject = NULL;
            curObject = NULL;

            gEvalState.setCurVarName(var);

            // In order to let docblocks work properly with variables, we have
            // clear the current docblock when we do an assign. This way it 
            // won't inappropriately carry forward to following function decls.
            curFNDocBlock = NULL;
            curNSDocBlock = NULL;
            break;

         case OP_SETCURVAR_CREATE:
            var = CodeToSTE(code, ip);
            ip += 2;

            // See OP_SETCURVAR
            prevField = NULL;
            prevObject = NULL;
            curObject = NULL;

            gEvalState.setCurVarNameCreate(var);

            // See OP_SETCURVAR for why we do this.
            curFNDocBlock = NULL;
            curNSDocBlock = NULL;
            break;

         case OP_SETCURVAR_ARRAY:
            var = STR.getSTValue();

            // See OP_SETCURVAR
            prevField = NULL;
            prevObject = NULL;
            curObject = NULL;

            gEvalState.setCurVarName(var);

            // See OP_SETCURVAR for why we do this.
            curFNDocBlock = NULL;
            curNSDocBlock = NULL;
            break;

         case OP_SETCURVAR_ARRAY_CREATE:
            var = STR.getSTValue();

            // See OP_SETCURVAR
            prevField = NULL;
            prevObject = NULL;
            curObject = NULL;

            gEvalState.setCurVarNameCreate(var);

            // See OP_SETCURVAR for why we do this.
            curFNDocBlock = NULL;
            curNSDocBlock = NULL;
            break;

         case OP_LOADVAR_UINT:
            intStack[UINT+1] = gEvalState.getIntVariable();
            UINT++;
            break;

         case OP_LOADVAR_FLT:
            floatStack[FLT+1] = gEvalState.getFloatVariable();
            FLT++;
            break;

         case OP_LOADVAR_STR:
            val = gEvalState.getStringVariable();
            STR.setStringValue(val);
            break;

         case OP_SAVEVAR_UINT:
            gEvalState.setIntVariable((S32)intStack[UINT]);
            break;

         case OP_SAVEVAR_FLT:
            gEvalState.setFloatVariable(floatStack[FLT]);
            break;

         case OP_SAVEVAR_STR:
            gEvalState.setStringVariable(STR.getStringValue());
            break;

         case OP_SETCUROBJECT:
            // Save the previous object for parsing vector fields.
            prevObject = curObject;
            val = STR.getStringValue();

            // Sim::findObject will sometimes find valid objects from
            // multi-component strings. This makes sure that doesn't
            // happen.
            for( const char* check = val; *check; check++ )
            {
               if( *check == ' ' )
               {
                  val = "";
                  break;
               }
            }
            curObject = Sim::findObject(val);
            break;

         case OP_SETCUROBJECT_INTERNAL:
            ++ip; // To skip the recurse flag if the object wasnt found
            if(curObject)
            {
               SimGroup *group = dynamic_cast<SimGroup *>(curObject);
               if(group)
               {
                  StringTableEntry intName = StringTable->insert(STR.getStringValue());
                  bool recurse = code[ip-1];
                  SimObject *obj = group->findObjectByInternalName(intName, recurse);
                  intStack[UINT+1] = obj ? obj->getId() : 0;
                  UINT++;
               }
               else
               {
                  Con::errorf(ConsoleLogEntry::Script, "%s: Attempt to use -> on non-group %s of class %s.", getFileLine(ip-2), curObject->getName(), curObject->getClassName());
                  intStack[UINT] = 0;
               }
            }
            break;

         case OP_SETCUROBJECT_NEW:
            curObject = currentNewObject;
            break;

         case OP_SETCURFIELD:
            // Save the previous field for parsing vector fields.
            prevField = curField;
            dStrcpy( prevFieldArray, curFieldArray );
            curField = CodeToSTE(code, ip);
            curFieldArray[0] = 0;
            ip += 2;
            break;

         case OP_SETCURFIELD_ARRAY:
            dStrcpy(curFieldArray, STR.getStringValue());
            break;

         case OP_LOADFIELD_UINT:
            if(curObject)
               intStack[UINT+1] = U32(dAtoi(curObject->getDataField(curField, curFieldArray)));
            else
            {
               // The field is not being retrieved from an object. Maybe it's
               // a special accessor?

               getFieldComponent( prevObject, prevField, prevFieldArray, curField, valBuffer, VAL_BUFFER_SIZE );
               intStack[UINT+1] = dAtoi( valBuffer );
            }
            UINT++;
            break;

         case OP_LOADFIELD_FLT:
            if(curObject)
               floatStack[FLT+1] = dAtof(curObject->getDataField(curField, curFieldArray));
            else
            {
               // The field is not being retrieved from an object. Maybe it's
               // a special accessor?
               getFieldComponent( prevObject, prevField, prevFieldArray, curField, valBuffer, VAL_BUFFER_SIZE );
               floatStack[FLT+1] = dAtof( valBuffer );
            }
            FLT++;
            break;

         case OP_LOADFIELD_STR:
            if(curObject)
            {
               val = curObject->getDataField(curField, curFieldArray);
               STR.setStringValue( val );
            }
            else
            {
               // The field is not being retrieved from an object. Maybe it's
               // a special accessor?
               getFieldComponent( prevObject, prevField, prevFieldArray, curField, valBuffer, VAL_BUFFER_SIZE );
               STR.setStringValue( valBuffer );
            }

            break;

         case OP_SAVEFIELD_UINT:
            STR.setIntValue((U32)intStack[UINT]);
            if(curObject)
               curObject->setDataField(curField, curFieldArray, STR.getStringValue());
            else
            {
               // The field is not being set on an object. Maybe it's
               // a special accessor?
               setFieldComponent( prevObject, prevField, prevFieldArray, curField );
               prevObject = NULL;
            }
            break;

         case OP_SAVEFIELD_FLT:
            STR.setFloatValue(floatStack[FLT]);
            if(curObject)
               curObject->setDataField(curField, curFieldArray, STR.getStringValue());
            else
            {
               // The field is not being set on an object. Maybe it's
               // a special accessor?
               setFieldComponent( prevObject, prevField, prevFieldArray, curField );
               prevObject = NULL;
            }
            break;

         case OP_SAVEFIELD_STR:
            if(curObject)
               curObject->setDataField(curField, curFieldArray, STR.getStringValue());
            else
            {
               // The field is not being set on an object. Maybe it's
               // a special accessor?
               setFieldComponent( prevObject, prevField, prevFieldArray, curField );
               prevObject = NULL;
            }
            break;

         case OP_STR_TO_UINT:
            intStack[UINT+1] = STR.getIntValue();
            UINT++;
            break;

         case OP_STR_TO_FLT:
            floatStack[FLT+1] = STR.getFloatValue();
            FLT++;
            break;

         case OP_STR_TO_NONE:
            // This exists simply to deal with certain typecast situations.
            break;

         case OP_FLT_TO_UINT:
            intStack[UINT+1] = (S64)floatStack[FLT];
            FLT--;
            UINT++;
            break;

         case OP_FLT_TO_STR:
            STR.setFloatValue(floatStack[FLT]);
            FLT--;
            break;

         case OP_FLT_TO_NONE:
            FLT--;
            break;

         case OP_UINT_TO_FLT:
            floatStack[FLT+1] = (F64)intStack[UINT];
            UINT--;
            FLT++;
            break;

         case OP_UINT_TO_STR:
            STR.setIntValue((U32)intStack[UINT]);
            UINT--;
            break;

         case OP_UINT_TO_NONE:
            UINT--;
            break;

         case OP_LOADIMMED_UINT:
            intStack[UINT+1] = code[ip++];
            UINT++;
            break;

         case OP_LOADIMMED_FLT:
            floatStack[FLT+1] = curFloatTable[code[ip]];
            ip++;
            FLT++;
            break;
         case OP_TAG_TO_STR:
            code[ip-1] = OP_LOADIMMED_STR;
            // it's possible the string has already been converted
            if(U8(curStringTable[code[ip]]) != StringTagPrefixByte)
            {
               U32 id = GameAddTaggedString(curStringTable + code[ip]);
               dSprintf(curStringTable + code[ip] + 1, 7, "%d", id);
               *(curStringTable + code[ip]) = StringTagPrefixByte;
            }
         case OP_LOADIMMED_STR:
            STR.setStringValue(curStringTable + code[ip++]);
            break;

         case OP_DOCBLOCK_STR:
            {
               // If the first word of the doc is '\class' or '@class', then this
               // is a namespace doc block, otherwise it is a function doc block.
               const char* docblock = curStringTable + code[ip++];

               const char* sansClass = dStrstr( docblock, "@class" );
               if( !sansClass )
                  sansClass = dStrstr( docblock, "\\class" );

               if( sansClass )
               {
                  // Don't save the class declaration. Scan past the 'class'
                  // keyword and up to the first whitespace.
                  sansClass += 7;
                  S32 index = 0;
                  while( ( *sansClass != ' ' ) && ( *sansClass != '\n' ) && *sansClass && ( index < ( nsDocLength - 1 ) ) )
                  {
                     nsDocBlockClass[index++] = *sansClass;
                     sansClass++;
                  }
                  nsDocBlockClass[index] = '\0';

                  curNSDocBlock = sansClass + 1;
               }
               else
                  curFNDocBlock = docblock;
            }

            break;

         case OP_LOADIMMED_IDENT:
            STR.setStringValue(CodeToSTE(code, ip));
            ip += 2;
            break;

         case OP_CALLFUNC_RESOLVE:
            // This deals with a function that is potentially living in a namespace.
            fnNamespace = CodeToSTE(code, ip+2);
            fnName      = CodeToSTE(code, ip);

            // Try to look it up.
            ns = Namespace::find(fnNamespace);
            nsEntry = ns->lookup(fnName);
            if(!nsEntry)
            {
               ip+= 5;
               Con::warnf(ConsoleLogEntry::General,
                  "%s: Unable to find function %s%s%s",
                  getFileLine(ip-4), fnNamespace ? fnNamespace : "",
                  fnNamespace ? "::" : "", fnName);
               STR.popFrame();
               break;
            }
            // Now, rewrite our code a bit (ie, avoid future lookups) and fall
            // through to OP_CALLFUNC
#ifdef TORQUE_64
            *((U64*)(code+ip+2)) = ((U64)nsEntry);
#else
            code[ip+2] = ((U32)nsEntry);
#endif
            code[ip-1] = OP_CALLFUNC;

         case OP_CALLFUNC:
         {
            // This routingId is set when we query the object as to whether
            // it handles this method.  It is set to an enum from the table
            // above indicating whether it handles it on a component it owns
            // or just on the object.
            S32 routingId = 0;

            fnName = CodeToSTE(code, ip);

            //if this is called from inside a function, append the ip and codeptr
            if (!gEvalState.stack.empty())
            {
               gEvalState.stack.last()->code = this;
               gEvalState.stack.last()->ip = ip - 1;
            }

            U32 callType = code[ip+4];

            ip += 5;
            STR.getArgcArgv(fnName, &callArgc, &callArgv);

            if(callType == FuncCallExprNode::FunctionCall) 
            {
#ifdef TORQUE_64
               nsEntry = ((Namespace::Entry *) *((U64*)(code+ip-3)));
#else
               nsEntry = ((Namespace::Entry *) *(code+ip-3));
#endif
               ns = NULL;
            }
            else if(callType == FuncCallExprNode::MethodCall)
            {
               saveObject = gEvalState.thisObject;
               gEvalState.thisObject = Sim::findObject(callArgv[1]);
               if(!gEvalState.thisObject)
               {
                  gEvalState.thisObject = 0;
                  Con::warnf(ConsoleLogEntry::General,"%s: Unable to find object: '%s' attempting to call function '%s'", getFileLine(ip-6), callArgv[1], fnName);
                  
                  STR.popFrame(); // [neo, 5/7/2007 - #2974]

                  break;
               }
               
               bool handlesMethod = gEvalState.thisObject->handlesConsoleMethod(fnName,&routingId);
               if( handlesMethod && routingId == MethodOnComponent )
               {
                  DynamicConsoleMethodComponent *pComponent = dynamic_cast<DynamicConsoleMethodComponent*>( gEvalState.thisObject );
                  if( pComponent )
                     pComponent->callMethodArgList( callArgc, callArgv, false );
               }
               
               ns = gEvalState.thisObject->getNamespace();
               if(ns)
                  nsEntry = ns->lookup(fnName);
               else
                  nsEntry = NULL;
            }
            else // it's a ParentCall
            {
               if(thisNamespace)
               {
                  ns = thisNamespace->mParent;
                  if(ns)
                     nsEntry = ns->lookup(fnName);
                  else
                     nsEntry = NULL;
               }
               else
               {
                  ns = NULL;
                  nsEntry = NULL;
               }
            }

            S32 nsType = -1;
            S32 nsMinArgs = 0;
            S32 nsMaxArgs = 0;
            Namespace::Entry::CallbackUnion * nsCb = NULL;
            //Namespace::Entry::CallbackUnion cbu;
            const char * nsUsage = NULL;
            if (nsEntry)
            {
               nsType = nsEntry->mType;
               nsMinArgs = nsEntry->mMinArgs;
               nsMaxArgs = nsEntry->mMaxArgs;
               nsCb = &nsEntry->cb;
               nsUsage = nsEntry->mUsage;
               routingId = 0;
            }
            if(!nsEntry || noCalls)
            {
               if(!noCalls && !( routingId == MethodOnComponent ) )
               {
                  Con::warnf(ConsoleLogEntry::General,"%s: Unknown command %s.", getFileLine(ip-6), fnName);
                  if(callType == FuncCallExprNode::MethodCall)
                  {
                     Con::warnf(ConsoleLogEntry::General, "  Object %s(%d) %s",
                           gEvalState.thisObject->getName() ? gEvalState.thisObject->getName() : "",
                           gEvalState.thisObject->getId(), getNamespaceList(ns) );
                  }
               }
               STR.popFrame();
               STR.setStringValue("");
               break;
            }
            if(nsEntry->mType == Namespace::Entry::ScriptFunctionType)
            {
               const char *ret = "";
               if(nsEntry->mFunctionOffset)
                  ret = nsEntry->mCode->exec(nsEntry->mFunctionOffset, fnName, nsEntry->mNamespace, callArgc, callArgv, false, nsEntry->mPackage);
               
               STR.popFrame();
               STR.setStringValue(ret);
            }
            else
            {
               const char* nsName = ns? ns->mName: "";
               if((nsEntry->mMinArgs && S32(callArgc) < nsEntry->mMinArgs) || (nsEntry->mMaxArgs && S32(callArgc) > nsEntry->mMaxArgs))
               {
                  Con::warnf(ConsoleLogEntry::Script, "%s: %s::%s - wrong number of arguments.", getFileLine(ip-6), nsName, fnName);
                  Con::warnf(ConsoleLogEntry::Script, "%s: usage: %s", getFileLine(ip-4), nsEntry->mUsage);
                  STR.popFrame();
               }
               else
               {
                  switch(nsEntry->mType)
                  {
                     case Namespace::Entry::StringCallbackType:
                     {
                        const char *ret = nsEntry->cb.mStringCallbackFunc(gEvalState.thisObject, callArgc, callArgv);
                        STR.popFrame();
                        if(ret != STR.getStringValue())
                           STR.setStringValue(ret);
                        else
                           STR.setLen(dStrlen(ret));
                        break;
                     }
                     case Namespace::Entry::IntCallbackType:
                     {
                        S32 result = nsEntry->cb.mIntCallbackFunc(gEvalState.thisObject, callArgc, callArgv);
                        STR.popFrame();
                        if(code[ip] == OP_STR_TO_UINT)
                        {
                           ip++;
                           intStack[++UINT] = result;
                           break;
                        }
                        else if(code[ip] == OP_STR_TO_FLT)
                        {
                           ip++;
                           floatStack[++FLT] = result;
                           break;
                        }
                        else if(code[ip] == OP_STR_TO_NONE)
                           ip++;
                        else
                           STR.setIntValue(result);
                        break;
                     }
                     case Namespace::Entry::FloatCallbackType:
                     {
                        F64 result = nsEntry->cb.mFloatCallbackFunc(gEvalState.thisObject, callArgc, callArgv);
                        STR.popFrame();
                        if(code[ip] == OP_STR_TO_UINT)
                        {
                           ip++;
                           intStack[++UINT] = (S64)result;
                           break;
                        }
                        else if(code[ip] == OP_STR_TO_FLT)
                        {
                           ip++;
                           floatStack[++FLT] = result;
                           break;
                        }
                        else if(code[ip] == OP_STR_TO_NONE)
                           ip++;
                        else
                           STR.setFloatValue(result);
                        break;
                     }
                     case Namespace::Entry::VoidCallbackType:
                        nsEntry->cb.mVoidCallbackFunc(gEvalState.thisObject, callArgc, callArgv);
                        if(code[ip] != OP_STR_TO_NONE)
                           Con::warnf(ConsoleLogEntry::General, "%s: Call to %s in %s uses result of void function call.", getFileLine(ip-6), fnName, functionName);
                        
                        STR.popFrame();
                        STR.setStringValue("");
                        break;
                     case Namespace::Entry::BoolCallbackType:
                     {
                        bool result = nsEntry->cb.mBoolCallbackFunc(gEvalState.thisObject, callArgc, callArgv);
                        STR.popFrame();
                        if(code[ip] == OP_STR_TO_UINT)
                        {
                           ip++;
                           intStack[++UINT] = result;
                           break;
                        }
                        else if(code[ip] == OP_STR_TO_FLT)
                        {
                           ip++;
                           floatStack[++FLT] = result;
                           break;
                        }
                        else if(code[ip] == OP_STR_TO_NONE)
                           ip++;
                        else
                           STR.setIntValue(result);
                        break;
                     }
                  }
               }
            }

            if(callType == FuncCallExprNode::MethodCall)
               gEvalState.thisObject = saveObject;
            break;
         }
         case OP_ADVANCE_STR:
            STR.advance();
            break;
         case OP_ADVANCE_STR_APPENDCHAR:
            STR.advanceChar(code[ip++]);
            break;

         case OP_ADVANCE_STR_COMMA:
            STR.advanceChar('_');
            break;

         case OP_ADVANCE_STR_NUL:
            STR.advanceChar(0);
            break;

         case OP_REWIND_STR:
            STR.rewind();
            break;

         case OP_TERMINATE_REWIND_STR:
            STR.rewindTerminate();
            break;

         case OP_COMPARE_STR:
            intStack[++UINT] = STR.compare();
            break;
         case OP_PUSH:
            STR.push();
            break;

         case OP_PUSH_FRAME:
            STR.pushFrame();
            break;
         case OP_BREAK:
         {
            //append the ip and codeptr before managing the breakpoint!
            AssertFatal( !gEvalState.stack.empty(), "Empty eval stack on break!");
            gEvalState.stack.last()->code = this;
            gEvalState.stack.last()->ip = ip - 1;

            U32 breakLine;
            findBreakLine(ip-1, breakLine, instruction);
            if(!breakLine)
               goto breakContinue;
            TelDebugger->executionStopped(this, breakLine);

            // Notify the remote debugger.
            if ( pRemoteDebugger != NULL )
                pRemoteDebugger->executionStopped(this, breakLine);

            goto breakContinue;
         }
         case OP_INVALID:

         default:
            // error!
            goto execFinished;
      }
   }
execFinished:

   if ( telDebuggerOn && setFrame < 0 )
      TelDebugger->popStackFrame();

   // Notify the remote debugger.
   if ( pRemoteDebugger != NULL && setFrame < 0 )
       pRemoteDebugger->popStackFrame();

   if ( popFrame )
      gEvalState.popFrame();

   if(argv)
   {
      if(gEvalState.traceOn)
      {
         traceBuffer[0] = 0;
         dStrcat(traceBuffer, "Leaving ");

         if(packageName)
         {
            dStrcat(traceBuffer, "[");
            dStrcat(traceBuffer, packageName);
            dStrcat(traceBuffer, "]");
         }
         if(thisNamespace && thisNamespace->mName)
         {
            dSprintf(traceBuffer + dStrlen(traceBuffer), sizeof(traceBuffer) - dStrlen(traceBuffer),
               "%s::%s() - return %s", thisNamespace->mName, thisFunctionName, STR.getStringValue());
         }
         else
         {
            dSprintf(traceBuffer + dStrlen(traceBuffer), sizeof(traceBuffer) - dStrlen(traceBuffer),
               "%s() - return %s", thisFunctionName, STR.getStringValue());
         }
         Con::printf("%s", traceBuffer);
      }
   }
   else
   {
      delete[] const_cast<char*>(globalStrings);
      delete[] globalFloats;
      globalStrings = NULL;
      globalFloats = NULL;
   }
   smCurrentCodeBlock = saveCodeBlock;
   if(saveCodeBlock && saveCodeBlock->name)
   {
      Con::gCurrentFile = saveCodeBlock->name;
      Con::gCurrentRoot = saveCodeBlock->mRoot;
   }

   decRefCount();

#ifdef TORQUE_DEBUG
   AssertFatal(!(STR.mStartStackSize > stackStart), "String stack not popped enough in script exec");
   AssertFatal(!(STR.mStartStackSize < stackStart), "String stack popped too much in script exec");
#endif
   return STR.getStringValue();
}

#endif

//------------------------------------------------------------
