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

#ifndef _CONSOLE_BASE_TYPE_H_
#define _CONSOLE_BASE_TYPE_H_

#ifndef _SIMBASE_H_
#include "sim/simBase.h"
#endif

class ConsoleReferenceCountedType;
class ConsoleValue;
class ConsoleValuePtr;

// All operator functions set outParam. See comments for usage.
typedef void (*ConsoleTypeOperatorFunc)(ConsoleValuePtr *inOutParam, ConsoleValuePtr *inParam, ConsoleValuePtr *inSubParam);

class ConsoleBaseType
{
protected:
   /// This is used to generate unique IDs for each type.
   static S32 smConsoleTypeCount;

   /// We maintain a linked list of all console types; this is its head.
   static ConsoleBaseType *smListHead;

   /// Next item in the list of all console types.
   ConsoleBaseType *mListNext;

   /// Destructor is private to avoid people mucking up the list.
   ~ConsoleBaseType();

   S32      mTypeID;
   dsize_t  mTypeSize;
   const char *mTypeName;
   const char *mInspectorFieldType;

public:

   /// @name cbt_list List Interface
   ///
   /// Interface for accessing/traversing the list of types.

   /// Get the head of the list.
   static ConsoleBaseType *getListHead();

   /// Get the item that follows this item in the list.
   ConsoleBaseType *getListNext() const
   {
      return mListNext;
   }
    
   virtual ConsoleReferenceCountedType *createReferenceCountedValue()
    {
        return NULL;
    }

   /// Called once to initialize the console type system.
   static void initialize();

   /// Call me to get a pointer to a type's info.
   static ConsoleBaseType *getType(const S32 typeID);
    
   static ConsoleBaseType* getTypeByName(const char *typeName);

   /// @}

   /// The constructor is responsible for linking an element into the
   /// master list, registering the type ID, etc.
   ConsoleBaseType(const S32 size, S32 *idPtr, const char *aTypeName);

   const S32 getTypeID() const { return mTypeID; }
   const dsize_t getTypeSize() const { return mTypeSize; }
   const char *getTypeName() const { return mTypeName; }

   void setInspectorFieldType(const char *type) { mInspectorFieldType = type; }
   const char *getInspectorFieldType() { return mInspectorFieldType; }

   virtual void setData(void *dptr, S32 argc, const char **argv, EnumTable *tbl, BitSet32 flag)=0;
   virtual const char *getData(void *dptr, EnumTable *tbl, BitSet32 flag )=0;
   virtual const char *getTypeClassName()=0;
   virtual const bool isDatablock() { return false; };
   virtual const char *prepData(const char *data, char *buffer, U32 bufferLen) { return data; };
   virtual StringTableEntry getTypePrefix( void ) const { return StringTable->EmptyString; }
    
    // Metamethods for type. To keep things simple we forego numeric operator overloading.
    ConsoleTypeOperatorFunc mMetaFunc;         // __func__ i.e. %foo.func() [=out, %foo, func]
    ConsoleTypeOperatorFunc mMetaProperty;     // __prop__ i.e. %foo.prop [=out, %foo, prop]
    ConsoleTypeOperatorFunc mMetaPropertySet;  // __prop_set__ i.e. %foo.prop [%foo, prop, value]
    ConsoleTypeOperatorFunc mMetaSubObject;    // __sub__ i.e. %foo-->sub [=out, %foo, prop]
    ConsoleTypeOperatorFunc mMetaIndex;        // __idx__ i.e. %foo[...] [=out, %foo, idx]
    ConsoleTypeOperatorFunc mMetaIndexSet;     // __idx__set__ i.e. %foo[...] = 123 [%foo, idx, value]
};

#define DefineConsoleType( type ) extern S32 type;

#define ConsoleType( typeName, type, size, typePrefix ) \
   class ConsoleType##type : public ConsoleBaseType \
   { \
   public: \
      ConsoleType##type (const S32 aSize, S32 *idPtr, const char *aTypeName) : ConsoleBaseType(aSize, idPtr, aTypeName) { } \
      virtual void setData(void *dptr, S32 argc, const char **argv, EnumTable *tbl, BitSet32 flag); \
      virtual const char *getData(void *dptr, EnumTable *tbl, BitSet32 flag ); \
      virtual const char *getTypeClassName() { return #typeName ; } \
      virtual StringTableEntry getTypePrefix( void ) const { return StringTable->insert( typePrefix ); }\
      ConsoleReferenceCountedType *createReferenceCountedValue();\
      static ConsoleBaseType *getInstance();\
   }; \
   S32 type = -1; \
   ConsoleType##type gConsoleType##type##Instance(size,&type,#type); \
   ConsoleBaseType *ConsoleType##type::getInstance() { return &gConsoleType##type##Instance; }

#define ConsolePrepType( typeName, type, size, typePrefix ) \
   class ConsoleType##type : public ConsoleBaseType \
   { \
   public: \
      ConsoleType##type (const S32 aSize, S32 *idPtr, const char *aTypeName) : ConsoleBaseType(aSize, idPtr, aTypeName) { } \
      virtual void setData(void *dptr, S32 argc, const char **argv, EnumTable *tbl, BitSet32 flag); \
      virtual const char *getData(void *dptr, EnumTable *tbl, BitSet32 flag ); \
      virtual const char *getTypeClassName() { return #typeName; }; \
      virtual const char *prepData(const char *data, char *buffer, U32 bufferLen); \
      virtual StringTableEntry getTypePrefix( void ) const { return StringTable->insert( typePrefix ); }\
      ConsoleReferenceCountedType *createReferenceCountedValue();\
      static ConsoleBaseType *getInstance();\
   }; \
   S32 type = -1; \
   ConsoleType##type gConsoleType##type##Instance(size,&type,#type); \
   ConsoleBaseType *ConsoleType##type::getInstance() { return &gConsoleType##type##Instance; }

#define ConsoleSetType( type ) \
   void ConsoleType##type::setData(void *dptr, S32 argc, const char **argv, EnumTable *tbl, BitSet32 flag)

#define ConsoleGetType( type ) \
   const char *ConsoleType##type::getData(void *dptr, EnumTable *tbl, BitSet32 flag)

#define ConsolePrepData( type ) \
   const char *ConsoleType##type::prepData(const char *data, char *buffer, U32 bufferSize)

#define ConsoleTypeFieldPrefix( type, typePrefix ) \
   StringTableEntry ConsoleType##type::getTypePrefix( void ) const { return StringTable->insert( typePrefix ); }

#define ConsoleSetNoReferenceType( type, refType ) \
   ConsoleReferenceCountedType *ConsoleType##type::createReferenceCountedValue() { return  NULL }

#define ConsoleSetReferenceType( type, refType ) \
   ConsoleReferenceCountedType *ConsoleType##type::createReferenceCountedValue() { return new refType(); }

#define ConsoleUseDefaultReferenceType( consoleType, type ) \
   ConsoleReferenceCountedType *ConsoleType##consoleType::createReferenceCountedValue() { return new DefaultConsoleReferenceCountedType< ConsoleType##consoleType, type >(); }




#define DatablockConsoleType( typeName, type, size, className ) \
   class ConsoleType##type : public ConsoleBaseType \
   { \
   public: \
      ConsoleType##type (const S32 aSize, S32 *idPtr, const char *aTypeName) : ConsoleBaseType(aSize, idPtr, aTypeName) { } \
      virtual void setData(void *dptr, S32 argc, const char **argv, EnumTable *tbl, BitSet32 flag); \
      virtual const char *getData(void *dptr, EnumTable *tbl, BitSet32 flag ); \
      virtual const char *getTypeClassName() { return #className; }; \
      virtual const bool isDatablock() { return true; }; \
   }; \
   S32 type = -1; \
   ConsoleType##type gConsoleType##type##Instance(size,&type,#type); \



// Base value for a reference counted object
class ConsoleReferenceCountedType
{
public:
    S32 refCount;
    
    ConsoleReferenceCountedType() : refCount(0) {;}
    virtual ~ConsoleReferenceCountedType() {;}
    
    virtual S32 getInternalType();
    virtual ConsoleBaseType *getType() { return NULL; }
    
    virtual void read(Stream &s)
    {
    }
    
    virtual void write(Stream &s)
    {
    }
    
    inline void addRef()
    {
        refCount++;
    }
    
    inline void decRef()
    {
        if (--refCount <= 0)
        {
            delete this;
        }
    }
    
    virtual const char* getString()
    {
        return "";
	 }
	
	 virtual bool stringCompare(const char* other)
	 {
	    return false;
	 }
	
	 virtual bool refCompare(ConsoleReferenceCountedType* other)
	 {
	    return other == this;
	 }
};

/// Default implementation for a referenceable console type, serializing value to and from a string.
template<typename ConsoleType, typename T> class DefaultConsoleReferenceCountedType : public ConsoleReferenceCountedType
{
public:
    T data;
    
    DefaultConsoleReferenceCountedType() {;}
    ~DefaultConsoleReferenceCountedType() {;}
    
    virtual ConsoleBaseType *getType() { return ConsoleType::getInstance(); }
    
    virtual void read(Stream &s)
    {
        char buffer[4096];
        const char *ptr = buffer;
        buffer[0] = '\0';
        s.readLongString(4096, buffer);
        ConsoleType::getInstance()->setData(&data, 1, &ptr, NULL, 0);
    }
    
    virtual void write(Stream &s)
    {
        const char *str = ConsoleType::getInstance()->getData(&data, NULL, 0);
        s.writeLongString(4096, str);
    }
    
    virtual const char* getString()
    {
        return ConsoleType::getInstance()->getData(&data, NULL, 0);
	 }
	
	virtual bool stringCompare(const char* other)
	{
		char buffer[4096];
		const char *str = ConsoleType::getInstance()->getData(&data, NULL, 0);
		dStrncpy(buffer, str, 4096);
		buffer[4095] = '\0';
		
		return dStricmp(buffer, other) == 0;
	}
	
	virtual bool refCompare(ConsoleReferenceCountedType* other)
	{
		return other == this || stringCompare(other->getString());
	}
};


// Simple buffer string type
class SimpleString
{
public:
    U32 bufferLen;
    char* buffer;
    
    SimpleString() : bufferLen(0), buffer(0)
    {
        
    }
    
    virtual ~SimpleString()
    {
        cleanup();
    }
    
    inline void cleanup()
    {
        if (buffer) dFree(buffer);
        buffer = NULL;
    }
    
    inline void setString(const char* str)
    {
        U32 stringLen = dStrlen(str);
        
        // may as well pad to the next cache line
        U32 newLen = ((stringLen + 1) + 15) & ~15;
        
        if(buffer == NULL)
            buffer = (char*) dMalloc(newLen);
        else if(newLen > bufferLen)
        {
            buffer = (char*) dRealloc(buffer, newLen);
            bufferLen = newLen;
        }
        
        dStrcpy(buffer, str);
    }
	
	 const char* getString()
    {
        return buffer ? buffer : "";
    }
};

// Simple buffer string type
class ConsoleStringValue : public ConsoleReferenceCountedType
{
public:
    SimpleString value;
    
    ConsoleStringValue()
    {
        
    }
    
    virtual ~ConsoleStringValue()
    {
    }
    
    void setString(const char* str)
    {
        value.setString(str);
    }
    
    virtual ConsoleBaseType *getType();
    
    virtual void read(Stream &s)
    {
        U32 len = 0;
        s.read(&len);
        
        value.buffer = (char*)dMalloc(len);
        value.bufferLen = len;
        s.read(len, value.buffer);
    }
    
    virtual void write(Stream &s)
    {
        s.write(value.bufferLen);
        s.write(value.bufferLen, value.buffer);
    }
    
    const char* getString()
    {
        return value.getString();
    }
	
	 virtual bool stringCompare(const char* other)
	 {
		 return dStricmp(value.buffer, other) == 0;
	 }
	
	 virtual bool refCompare(ConsoleReferenceCountedType* other)
	 {
	    return other == this || stringCompare(other->getString());
	 }
	
    static ConsoleStringValue* fromInt(U32 num);
    static ConsoleStringValue* fromFloat(F32 num);
    static ConsoleStringValue* fromString(const char *string);
};


#endif // _CONSOLE_BASE_TYPE_H_
