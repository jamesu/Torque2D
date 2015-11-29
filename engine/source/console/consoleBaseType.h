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

#ifndef _CONSOLEVALUE_H_
#include "console/consoleValue.h"
#endif

#include "collection/hashTable.h"

#include "io/stream.h"
#include "collection/bitSet.h"

class ConsoleReferenceCountedType;
class ConsoleValue;
class ConsoleValuePtr;
class EnumTable;
class ConsoleSerializationState;

template< typename T >
struct _ConsoleConstType
{
   typedef const T ConstType;
};

/// Converts a value to string
typedef const char *(*ConsoleTypeToString)(void *dataPtr, EnumTable *tbl);

/// Converts a value to a ConsoleValue (may use a reference or whatever)
typedef ConsoleValuePtr (*ConsoleTypeToConsoleValue)(void *dataPtr, EnumTable *tbl);

/// Sets a value based on a ConsoleValue (may use a reference or plain string)
typedef void (*ConsoleTypeFromConsoleValue)(void *dataPtr, ConsoleValuePtr &value, EnumTable *tbl);

/// Save value to stream
typedef void (*ConsoleTypeSerializeToStream)(void *dataPtr, Stream& s, ConsoleSerializationState &state, EnumTable *tbl);


class ConsoleTypeConversionMethodConstructor;

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
    
   static ConsoleBaseType *getTypeByName(const char *typeName);

   /// @}

   /// The constructor is responsible for linking an element into the
   /// master list, registering the type ID, etc.
   ConsoleBaseType(const S32 size, S32 *idPtr, const char *aTypeName);

   const S32 getTypeID() const { return mTypeID; }
   const dsize_t getTypeSize() const { return mTypeSize; }
   const char *getTypeName() const { return mTypeName; }

   void setInspectorFieldType(const char *type) { mInspectorFieldType = type; }
   const char *getInspectorFieldType() { return mInspectorFieldType; }

   void setData(void *dataPtr, const char *value, EnumTable *tbl);
   ConsoleStringValuePtr getData(void *dataPtr, EnumTable *tbl);
   
   void setDataFromValue(void *dataPtr, const ConsoleValuePtr &value, EnumTable *tbl);
   ConsoleValuePtr getDataValue(void *dataPtr, EnumTable *tbl);
   
   virtual const char *getTypeClassName()=0;
   virtual const bool isDatablock() { return false; };
   virtual StringTableEntry getTypePrefix( void ) const { return StringTable->EmptyString; }
   
   static StringTableEntry getFieldIndexName();
   
   //
   
   ConsoleTypeToString smTypeToStringFunc;
   ConsoleTypeToConsoleValue smTypeToConsoleValueFunc;
   ConsoleTypeFromConsoleValue smTypeFromConsoleValueFunc;
   ConsoleTypeSerializeToStream smTypeToStreamFunc;
   ConsoleTypeSerializeToStream smTypeFromStreamFunc;
   
   static ConsoleTypeConversionMethodConstructor *smTypeConversionMethodInitializers;
};


class ConsoleTypeConversionMethodConstructor
{
public:
   virtual void set();
   
   ConsoleBaseType *typeInstance;
   ConsoleTypeConversionMethodConstructor *next;
};

class ConsoleTypeToStringConstructor : public ConsoleTypeConversionMethodConstructor
{
public:
   
   ConsoleTypeToString funcPtr;
   
   ConsoleTypeToStringConstructor(ConsoleBaseType *instance, ConsoleTypeToString func)
   {
      typeInstance = instance;
      funcPtr = func;
      next = ConsoleBaseType::smTypeConversionMethodInitializers;
      ConsoleBaseType::smTypeConversionMethodInitializers = this;
   }
   
   void set()
   {
      typeInstance->smTypeToStringFunc = funcPtr;
   }
};

class ConsoleTypeToConsoleValueConstructor : public ConsoleTypeConversionMethodConstructor
{
public:
   
   ConsoleTypeToConsoleValue funcPtr;
   
   ConsoleTypeToConsoleValueConstructor(ConsoleBaseType *instance, ConsoleTypeToConsoleValue func)
   {
      typeInstance = instance;
      funcPtr = func;
      next = ConsoleBaseType::smTypeConversionMethodInitializers;
      ConsoleBaseType::smTypeConversionMethodInitializers = this;
   }
   
   void set()
   {
      typeInstance->smTypeToConsoleValueFunc = funcPtr;
   }
};

class ConsoleTypeFromConsoleValueConstructor : public ConsoleTypeConversionMethodConstructor
{
public:
   
   ConsoleTypeFromConsoleValue funcPtr;
   
   ConsoleTypeFromConsoleValueConstructor(ConsoleBaseType *instance, ConsoleTypeFromConsoleValue func)
   {
      typeInstance = instance;
      funcPtr = func;
      next = ConsoleBaseType::smTypeConversionMethodInitializers;
      ConsoleBaseType::smTypeConversionMethodInitializers = this;
   }
   
   void set()
   {
      typeInstance->smTypeFromConsoleValueFunc = funcPtr;
   }
};

class ConsoleTypeToStreamConstructor : public ConsoleTypeConversionMethodConstructor
{
public:
   
   ConsoleTypeSerializeToStream funcPtr;
   
   ConsoleTypeToStreamConstructor(ConsoleBaseType *instance, ConsoleTypeSerializeToStream func)
   {
      typeInstance = instance;
      funcPtr = func;
      next = ConsoleBaseType::smTypeConversionMethodInitializers;
      ConsoleBaseType::smTypeConversionMethodInitializers = this;
   }
   
   void set()
   {
      typeInstance->smTypeToStreamFunc = funcPtr;
   }
};

class ConsoleTypeFromStreamConstructor : public ConsoleTypeConversionMethodConstructor
{
public:
   
   ConsoleTypeSerializeToStream funcPtr;
   
   ConsoleTypeFromStreamConstructor(ConsoleBaseType *instance, ConsoleTypeSerializeToStream func)
   {
      typeInstance = instance;
      funcPtr = func;
      next = ConsoleBaseType::smTypeConversionMethodInitializers;
      ConsoleBaseType::smTypeConversionMethodInitializers = this;
   }
   
   void set()
   {
      typeInstance->smTypeFromStreamFunc = funcPtr;
   }
};


#define DefineConsoleType( type ) extern S32 type;

#define DefineNativeConsoleType( type, nativeType ) extern S32 type;\
extern const char *castConsoleTypeToString( _ConsoleConstType< nativeType >::ConstType &arg ); \
extern bool castConsoleTypeFromString( nativeType &arg, const char *str ); \
extern ConsoleValuePtr castConsoleTypeToConsoleValuePtr( _ConsoleConstType< nativeType >::ConstType &arg ); \
extern bool castConsoleTypeFromConsoleValue( nativeType &arg, const ConsoleValuePtr& value );

#define ConsoleType( typeName, type, size, typePrefix ) \
   class ConsoleType##type : public ConsoleBaseType \
   { \
   public: \
      ConsoleType##type (const S32 aSize, S32 *idPtr, const char *aTypeName) : ConsoleBaseType(aSize, idPtr, aTypeName) { } \
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
      virtual const char *getTypeClassName() { return #typeName; }; \
      virtual StringTableEntry getTypePrefix( void ) const { return StringTable->insert( typePrefix ); }\
      ConsoleReferenceCountedType *createReferenceCountedValue();\
      static ConsoleBaseType *getInstance();\
   }; \
   S32 type = -1; \
   ConsoleType##type gConsoleType##type##Instance(size,&type,#type); \
   ConsoleBaseType *ConsoleType##type::getInstance() { return &gConsoleType##type##Instance; }

#define ConsoleTypeToString( type ) \
const char *ConsoleType##type##_toString(void *dataPtr, EnumTable *tbl);\
ConsoleTypeToStringConstructor gConsoleType##type##_toStringConstructor(&gConsoleType##type##Instance, &ConsoleType##type##_toString);\
const char *ConsoleType##type##_toString(void *dataPtr, EnumTable *tbl)

#define ConsoleTypeToConsoleValue( type ) \
ConsoleValuePtr ConsoleType##type##_toConsoleValue(void *dataPtr, EnumTable *tbl);\
ConsoleTypeToConsoleValueConstructor gConsoleType##type##_toConsoleValueConstructor(&gConsoleType##type##Instance, &ConsoleType##type##_toConsoleValue);\
ConsoleValuePtr ConsoleType##type##_toConsoleValue(void *dataPtr, EnumTable *tbl)

#define ConsoleTypeFromConsoleValue( type ) \
void ConsoleType##type##_fromConsoleValue(void *dataPtr, ConsoleValuePtr &value, EnumTable *tbl);\
ConsoleTypeFromConsoleValueConstructor gConsoleType##type##_fromConsoleValueConstructor(&gConsoleType##type##Instance, &ConsoleType##type##_fromConsoleValue);\
void ConsoleType##type##_fromConsoleValue(void *dataPtr, ConsoleValuePtr &value, EnumTable *tbl)

#define ConsoleTypeToStream( type ) \
void ConsoleType##type##_toStream(void *dataPtr, Stream& s, ConsoleSerializationState &state, EnumTable *tbl);\
ConsoleTypeFromStreamConstructor gConsoleType##type##_toStreamConstructor(&gConsoleType##type##Instance, &ConsoleType##type##_toStream);\
void ConsoleType##type##_toStream(void *dataPtr, Stream& s, ConsoleSerializationState &state, EnumTable *tbl)

#define ConsoleTypeFromStream( type ) \
void ConsoleType##type##_fromStream(void *dataPtr, Stream& s, ConsoleSerializationState &state, EnumTable *tbl);\
ConsoleTypeToStreamConstructor gConsoleType##type##_fromStreamConstructor(&gConsoleType##type##Instance, &ConsoleType##type##_fromStream);\
void ConsoleType##type##_fromStream(void *dataPtr, Stream& s, ConsoleSerializationState &state, EnumTable *tbl)

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
      virtual const char *getTypeClassName() { return #className; }; \
      virtual const bool isDatablock() { return true; }; \
   }; \
   S32 type = -1; \
   ConsoleType##type gConsoleType##type##Instance(size,&type,#type); \


/// Default implementation for a referenceable console type, serializing value to and from a string.
// TOFIX: pass this through the serializer for read/write
template<typename ConsoleType, typename T> class DefaultConsoleReferenceCountedType : public ConsoleReferenceCountedType
{
public:
    T data;
    
    DefaultConsoleReferenceCountedType() {;}
    ~DefaultConsoleReferenceCountedType() {;}
    
    virtual ConsoleBaseType *getType() { return ConsoleType::getInstance(); }
    
    virtual void read(Stream &s, ConsoleSerializationState &state)
    {
        char buffer[4096];
        const char *ptr = buffer;
        buffer[0] = '\0';
        s.readLongString(4096, buffer);
        ConsoleType::getInstance()->setData(&data, ptr, NULL);
    }
    
    virtual void write(Stream &s, ConsoleSerializationState &state)
    {
        ConsoleStringValuePtr str = ConsoleType::getInstance()->getData(&data, NULL);
        s.writeLongString(4096, str.c_str());
    }
    
    virtual ConsoleStringValuePtr getString()
    {
        return ConsoleType::getInstance()->getData(&data, NULL);
    }
   
   virtual bool stringCompare(const char *other)
   {
      ConsoleStringValuePtr str = ConsoleType::getInstance()->getData(&data, NULL);
     return dStricmp(str.c_str(), other) == 0;
   }
   
   virtual bool refCompare(ConsoleReferenceCountedType *other)
   {
      return other == this || stringCompare(other->getString().c_str());
   }
};

class ConsoleSimObjectPtr;

// Simple buffer string type
class ConsoleNamespacePtr : public ConsoleReferenceCountedType
{
public:
   Namespace *value;
   
   ConsoleNamespacePtr()
   {
      
   }
   
   virtual ~ConsoleNamespacePtr()
   {
   }
   
   void setString(const char *str);
   
   virtual ConsoleStringValuePtr getString();
   virtual ConsoleBaseType *getType();
   
   virtual bool getDataField(StringTableEntry slotName, const ConsoleValuePtr &array, ConsoleValuePtr &outValue);
   virtual void setDataField(StringTableEntry slotName, const ConsoleValuePtr &array, const ConsoleValuePtr &newValue);
   virtual Namespace *getNamespace() { return value; }
   
   virtual void read(Stream &s, ConsoleSerializationState &state);
   virtual void write(Stream &s, ConsoleSerializationState &state);
   
   virtual bool stringCompare(const char *other)
   {
      return dStricmp(getString().c_str(), other) == 0;
   }
   
   virtual bool refCompare(ConsoleReferenceCountedType *other)
   {
      ConsoleNamespacePtr *oso = dynamic_cast<ConsoleNamespacePtr*>(other);
      return other == this || (oso && oso->value == value);
   }
   
   static ConsoleSimObjectPtr *fromObject(const SimObject *obj);
};


#endif // _CONSOLE_BASE_TYPE_H_
