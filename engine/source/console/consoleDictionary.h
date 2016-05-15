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

#ifndef _CONSOLE_DICTIONARY_H_
#define _CONSOLE_DICTIONARY_H_

#ifndef _STRINGTABLE_H_
#include "string/stringTable.h"
#endif
#ifndef _VECTOR_H_
#include "collection/vector.h"
#endif
#ifndef _CONSOLETYPES_H_
#include "console/consoleTypes.h"
#endif

//-----------------------------------------------------------------------------

class ExprEvalState;
class CodeBlock;

extern char *typeValueEmpty;

class ConsoleReferenceCountedType;
class ConsoleStringValue;

typedef union ConsoleValueBase
{
    // Number
    struct
    {
        F64 fval;
    };
    
    // Int
    struct
    {
        U32 ival;
    };
    
    // StringStackPtr
    struct
    {
        U32 stringStackPtr;
    };
    
    // STE
    struct
    {
        StringTableEntry string;
    };
    
    // String
    struct
    {
        ConsoleStringValue *stringValue;
    };
    
    // Generic refcounted object
    struct
    {
        ConsoleReferenceCountedType *refValue;
    };
    
    // Generic pointer
    struct
    {
        void *ptrValue;
    };
    
    // Custom type
    /*struct
    {
        /// The real data pointer.
        void *dataPtr;
        
        /// The enum lookup table for enumerated types.
        const EnumTable *enumTable;
    };*/
} ConsoleValueBase;

/// Base struct for all console values
struct ConsoleValue
{
    /// Internal type list
    enum
    {
        /// @name Directly stored types
        /// {
        TypeInternalNull = 0,
        TypeInternalFloat,
        TypeInternalInt,
        TypeInternalStringStackPtr,   // STR.mBuffer + value
        TypeInternalStringTableEntry, // (StringTableEntry)value
        TypeInternalNamespaceEntry,   // Resolved namespace entry (NOTE: these are never deleted)
        TypeInternalSimObjectId,     // Identifier for SimObject
        /// }
		 
		 /// Pointer to other variable (internal to saving code)
		 TypeSavedReference,
        
        /// @name Reference counted types
		 /// {
		  TypeReferenceCounted,
        TypeTable,
		 
		 
        /// Types beyond this are custom types
        /// (i.e. we go through the type system to manipulate them)
        TypeCustomFieldStart,
        /// }
    };
    
    /// Type and flags for value
    U32 type;
    
    /// Associated value. This will either be directly stored or
    /// we point to a refcounted value
    ConsoleValueBase value;
    
    ConsoleValue() : type(TypeInternalNull) {;}
    
    static inline bool isRefType(S32 type)
    {
        return type > ConsoleValue::TypeInternalSimObjectId;
    };
    
    
    StringTableEntry getSTEStringValue();
    const char *getTempStringValue();
    void getInternalStringValue(char* outBuffer, U32 bufferSize);
    
    inline U32 getIntValue()
    {
        switch (type)
        {
            case TypeInternalNull:
                return 0;
            case TypeInternalInt:
                return value.ival;
            case TypeInternalFloat:
                return (U32)value.fval;
            default:
                return dAtoi(getTempStringValue());
        }
    }
    
    inline S32 getSignedIntValue()
    {
        switch (type)
        {
            case TypeInternalNull:
                return 0;
            case TypeInternalInt:
                return value.ival;
            case TypeInternalFloat:
                return (S32)value.fval;
            default:
                return dAtoi(getTempStringValue());
        }
    }
    
    inline F64 getFloatValue()
    {
        switch (type)
        {
            case TypeInternalNull:
                return 0;
            case TypeInternalInt:
                return value.ival;
            case TypeInternalFloat:
                return value.fval;
            default:
                return dAtof(getTempStringValue());
        }
    }
    
    inline bool getBoolValue()
    {
        switch (type)
        {
            case TypeInternalNull:
                return false;
            case TypeInternalInt:
                return value.ival;
            case TypeInternalFloat:
                return value.fval > 0;
            default:
                return dAtob(getTempStringValue());
        }
    }
    
    inline SimObject* getSimObject()
    {
        switch (type)
        {
            case TypeInternalNull:
                return NULL;
            case TypeInternalInt:
                return Sim::findObject(value.ival);
            case TypeInternalFloat:
                return Sim::findObject((S32)value.fval);
            default:
                return Sim::findObject(getTempStringValue());
        }
        
    }
};

#include "collection/hashTable.h"

struct ConsoleStackSerializationState
{
	/// Written reference values
	HashTable<void*, U32> writtenValues;
	
	/// Lookup of loaded values
	Vector<ConsoleValue*> loadedValues;
	
	U32 savedObjectCount;
	
	S32 getSavedObjectIdx(ConsoleReferenceCountedType* object);
	void addSavedObject(ConsoleReferenceCountedType* object);
	
	ConsoleStackSerializationState() : savedObjectCount(0)
	{
		
	}
	
	void clear()
	{
		savedObjectCount = 0;
		writtenValues.clear();
		loadedValues.clear();
	}
};

/// Helper class for console values on stack. This performs extra book-keeping.
class ConsoleValuePtr : public ConsoleValue
{
public:
    ConsoleValuePtr()
    {
    }
    
    ~ConsoleValuePtr()
    {
        if (isRefType(type))
        {
            value.refValue->decRef();
        }
    }
    
    inline void AddRef()
    {
        /*if (isRefType(type))
        {
            value.refValue->addRef();
        }*/
    }
    
    inline void DecRef()
    {/*
        if (isRefType(type))
        {
            value.refValue->decRef();
        }*/
    }
    
    ConsoleValuePtr(const ConsoleValuePtr &other)
    {
        value = other.value;
        type = other.type;
        AddRef();
    }
    
    ConsoleValuePtr(ConsoleValue &other)
    {
        value = other.value;
        type = other.type;
        AddRef();
    }
    
    ConsoleValuePtr(U32 other)
    {
        DecRef();
        type = ConsoleValue::TypeInternalInt;
        value.ival = other;
    }
    
    ConsoleValuePtr(S32 other)
    {
        DecRef();
        type = ConsoleValue::TypeInternalFloat;
        value.fval = other;
    }
    
    ConsoleValuePtr(F32 other)
    {
        DecRef();
        type = ConsoleValue::TypeInternalFloat;
        value.fval = other;
    }
    
    ConsoleValuePtr(F64 other)
    {
        DecRef();
        type = ConsoleValue::TypeInternalFloat;
        value.fval = other;
    }
    
    ConsoleValuePtr(bool other)
    {
        DecRef();
        type = ConsoleValue::TypeInternalInt;
        value.ival = other? 1 : 0;
    }
    
    ConsoleValuePtr(ConsoleReferenceCountedType* other)
    {
        other->addRef();
        DecRef();
        
        type = other->getInternalType();
        value.refValue = other;
    }
    
    void setNull()
    {
        DecRef();
        type = ConsoleValue::TypeInternalNull;
        value.ival = 0;
    }
    
    void setValue(U32 inValue)
    {
        DecRef();
        type = ConsoleValue::TypeInternalInt;
        value.ival = inValue;
    }
    
    void setValue(F32 inValue)
    {
        DecRef();
        type = ConsoleValue::TypeInternalFloat;
        value.fval = inValue;
    }
    
    void setValue(S32 inValue)
    {
        DecRef();
        type = ConsoleValue::TypeInternalInt;
        value.fval = inValue;
    }
    
    inline ConsoleValuePtr& operator=(ConsoleReferenceCountedType* other)
    {
        other->addRef();
        DecRef();
        
        type = other->getInternalType();
        value.refValue = other;
        return *this;
    }
    
	inline  __attribute__((always_inline)) ConsoleValuePtr& operator=(ConsoleValuePtr& other)
    {
        /*if (isRefType(other.type))
        {
            other.value.refValue->addRef();
        }
        DecRef();*/
        
        type = other.type;
        value = other.value;
        return *this;
    }
    
    inline ConsoleValuePtr& operator=(ConsoleValue& other)
    {/*
        if (isRefType(other.type))
        {
            other.value.refValue->addRef();
        }
        DecRef();*/
        
        type = other.type;
        value = other.value;
        return *this;
    }
    
    /// Get string value
    /// NOTE: this
    const char *getStringValue();
    
    inline U32 getIntValue()
    {
        switch (type)
        {
            case TypeInternalNull:
                return 0;
            case TypeInternalInt:
                return value.ival;
            case TypeInternalFloat:
                return (U32)value.fval;
            default:
                return dAtoi(getStringValue());
        }
    }
    
    inline S32 getSignedIntValue()
    {
        switch (type)
        {
            case TypeInternalNull:
                return 0;
            case TypeInternalInt:
                return value.ival;
            case TypeInternalFloat:
                return (S32)value.fval;
            default:
                return dAtoi(getStringValue());
        }
    }
    
    inline F64 getFloatValue()
    {
        switch (type)
        {
            case TypeInternalNull:
                return 0;
            case TypeInternalInt:
                return value.ival;
            case TypeInternalFloat:
                return value.fval;
            default:
                return dAtof(getStringValue());
        }
    }
    
    inline bool getBoolValue()
    {
        switch (type)
        {
            case TypeInternalNull:
                return false;
            case TypeInternalInt:
                return value.ival;
            case TypeInternalFloat:
                return value.fval > 0;
            default:
                return dAtob(getStringValue());
        }
    }
    
    /// Read a serialized stack
    static void readStack(Stream &s, ConsoleStackSerializationState& serializationState, Vector<ConsoleValuePtr> &stack);
    
    /// Write a serialized stack
    static void writeStack(Stream &s, ConsoleStackSerializationState& serializationState, Vector<ConsoleValuePtr> &stack);
};

/// Extension to ConsoleValuePtr which is used in function callbacks
class ConsoleValueRef : public ConsoleValuePtr
{
public:
    
    inline operator const char*() { return getStringValue(); }
    inline operator U32() { return getIntValue(); }
    inline operator S32() { return getSignedIntValue(); }
    inline operator F64() { return getFloatValue(); }
    inline operator bool() { return getBoolValue(); }
    
    inline bool isStringStackPtr() { return type == TypeInternalStringStackPtr; }
    inline bool isString() { return (type >= TypeInternalStringStackPtr && type <= TypeInternalStringTableEntry) || type == TypeBufferString; }
    inline bool isInt() { return type == TypeInternalInt; }
    inline bool isFloat() { return type == TypeInternalFloat; }
};

// Overrides to allow ConsoleValueRefs to be directly converted to S32&F32

inline S32 dAtoi(ConsoleValueRef &ref)
{
    return ref.getSignedIntValue();
}

inline F32 dAtof(ConsoleValueRef &ref)
{
    return ref.getFloatValue();
}

inline bool dAtob(ConsoleValueRef &ref)
{
    return ref.getBoolValue();
}

//-----------------------------------------------------------------------------

class Dictionary
{
public:
    struct Entry
    {
        enum
        {
            TypeInternalInt = -3,
            TypeInternalFloat = -2,
            TypeInternalString = -1,
        };

        StringTableEntry name;
        Entry *nextEntry;
        S32 type;
        char *sval;
        U32 ival;  // doubles as strlen when type = -1
        F32 fval;
        U32 bufferLen;
        void *dataPtr;

        Entry(StringTableEntry name);
        ~Entry();

        U32 getIntValue()
        {
            if(type <= TypeInternalString)
                return ival;
            else
                return dAtoi(Con::getData(type, dataPtr, 0));
        }
        F32 getFloatValue()
        {
            if(type <= TypeInternalString)
                return fval;
            else
                return dAtof(Con::getData(type, dataPtr, 0));
        }
        const char *getStringValue()
        {
            if(type == TypeInternalString)
                return sval;
            if(type == TypeInternalFloat)
                return Con::getData(TypeF32, &fval, 0);
            else if(type == TypeInternalInt)
                return Con::getData(TypeS32, &ival, 0);
            else
                return Con::getData(type, dataPtr, 0);
        }
        void setIntValue(U32 val)
        {
            if(type <= TypeInternalString)
            {
                fval = (F32)val;
                ival = val;
                if(sval != typeValueEmpty)
                {
                    dFree(sval);
                    sval = typeValueEmpty;
                }
                type = TypeInternalInt;
                return;
            }
            else
            {
                const char *dptr = Con::getData(TypeS32, &val, 0);
                Con::setData(type, dataPtr, 0, 1, &dptr);
            }
        }
        void setFloatValue(F32 val)
        {
            if(type <= TypeInternalString)
            {
                fval = val;
                ival = static_cast<U32>(val);
                if(sval != typeValueEmpty)
                {
                    dFree(sval);
                    sval = typeValueEmpty;
                }
                type = TypeInternalFloat;
                return;
            }
            else
            {
                const char *dptr = Con::getData(TypeF32, &val, 0);
                Con::setData(type, dataPtr, 0, 1, &dptr);
            }
        }
        void setStringValue(const char *value);
    };

private:
    struct HashTableData
    {
        Dictionary* owner;
        S32 size;
        S32 count;
        Entry **data;
    };

    HashTableData *hashTable;
    ExprEvalState *exprState;

public:
    StringTableEntry scopeName;
    Namespace *scopeNamespace;
    CodeBlock *code;
    U32 ip;

    Dictionary();
    Dictionary(ExprEvalState *state, Dictionary* ref=NULL);
    ~Dictionary();
    Entry *lookup(StringTableEntry name);
    Entry *add(StringTableEntry name);
    void setState(ExprEvalState *state, Dictionary* ref=NULL);
    void remove(Entry *);
    void reset();

    void exportVariables(const char *varString, const char *fileName, bool append);
    void deleteVariables(const char *varString);

    void setVariable(StringTableEntry name, const char *value);
    const char *getVariable(StringTableEntry name, bool *valid = NULL);

    void addVariable(const char *name, S32 type, void *dataPtr);
    bool removeVariable(StringTableEntry name);

    /// Return the best tab completion for prevText, with the length
    /// of the pre-tab string in baseLen.
    const char *tabComplete(const char *prevText, S32 baseLen, bool);
};


// Function or execution environment for code
class CodeBlockFunction
{
public:
    typedef struct Symbol
    {
        U32 registerIdx;
        StringTableEntry varName;
    } Symbol;
    
    U32 ip;
    StringTableEntry name;
    
    /// List of local variables
    Vector<Symbol> vars;
    
    /// Number of parameters (if a function)
    U32 numArgs;
    
    /// Maximum stack position used
    U32 maxStack;
    
    void read(Stream &s, ConsoleStackSerializationState& serializationState)
    {
        name = s.readSTString();
        s.read(&numArgs);
        s.read(&maxStack);
        s.read(&ip);
        
        U8 numVars;
        s.read(&numVars);
        vars.setSize(numVars);
        for (U32 i=0; i<numVars; i++)
        {
            s.read(&vars[i].registerIdx);
            vars[i].varName = s.readSTString();
        }
    }
    
    void write(Stream &s, ConsoleStackSerializationState& serializationState)
    {
        s.writeString(name);
        s.write(&numArgs);
        s.write(&maxStack);
        s.write(&ip);
        
        // vars
        U8 numVars = vars.size();
        for (U32 i=0; i<numVars; i++)
        {
            s.write(&vars[i].registerIdx);
            s.writeString(vars[i].varName);
        }
    }
};


#endif // _CONSOLE_DICTIONARY_H_
