
#ifndef _CONSOLEVALUE_H_
#define _CONSOLEVALUE_H_

#ifndef _TORQUE_TYPES_H_
#include "platform/types.h"
#endif
#ifndef _PLATFORM_STRING_H_
#include "platform/platformString.h"
#endif
#ifndef _PLATFORM_ASSERT_H_
#include "platform/platformAssert.h"
#endif

extern char *typeValueEmpty;

class ConsoleReferenceCountedType;
class ConsoleValue;
class ConsoleValuePtr;
class ConsoleStringValue;
class ConsoleBaseType;
class ConsoleSerializationState;
class SimObject;
class Namespace;
class Stream;

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
      U64 ival;
   };
   
   // StringStackPtr
   struct
   {
      const char *stringStackPtr;
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

class ConsoleStringValue;
class SimpleString;


// Pointer to a console string type
class ConsoleStringValuePtr
{
public:
   ConsoleStringValue* value;
   
   ConsoleStringValuePtr() : value(NULL) {;}
   ConsoleStringValuePtr(const char *str);
   ConsoleStringValuePtr(SimpleString* str);
   ConsoleStringValuePtr(ConsoleStringValuePtr* other);
   ConsoleStringValuePtr(ConsoleStringValue* other);
   
   inline void AddRef();
   inline void DecRef();
   
   inline bool isNull();
   inline void setNull();
   
   inline ConsoleStringValuePtr& operator=(const ConsoleStringValuePtr& other);
   
   inline const char* c_str();
   inline ConsoleStringValue* getPtr();
};

// Base value for a reference counted object
class ConsoleReferenceCountedType
{
public:
   S32 refCount;
   
   ConsoleReferenceCountedType() : refCount(0) {;}
   virtual ~ConsoleReferenceCountedType() {;}
   
   virtual S32 getInternalType();
   virtual ConsoleBaseType* getType() { return NULL; }
   
   virtual void read(Stream &s, ConsoleSerializationState &state) = 0;
   virtual void write(Stream &s, ConsoleSerializationState &state) = 0;
   
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
   
   virtual bool getDataField(const StringTableEntry slotName, const ConsoleValuePtr &array, ConsoleValuePtr &outValue);
   virtual void setDataField(const StringTableEntry slotName, const ConsoleValuePtr &array, const ConsoleValuePtr &newValue);
   virtual Namespace* getNamespace();
   
   virtual bool isEnumerable() { return false; }
   virtual S32 getIteratorLength() { return 0; }
   virtual bool advanceIterator(ConsoleValuePtr &iterator, ConsoleValuePtr &iteratorValue) { return false; }
   
   virtual ConsoleStringValuePtr getString()
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
      TypeInternalStringTableEntry, // (StringTableEntry)value
      TypeInternalNamespaceName,    // To differentiate namespace calls from object references
      TypeInternalNamespaceEntry,   // Resolved namespace entry (NOTE: these are never deleted)
      TypeInternalSimObjectId,      // Identifier for SimObject
      /// }
      
      /// Pointer to other variable (internal to saving code)
      TypeSavedReference,
      
      /// @name Reference counted types
      /// {
      TypeReferenceCounted,
      
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
   
   StringTableEntry getSTEStringValue() const;
   const char *getTempStringValue() const;
   void getInternalStringValue(char* outBuffer, U32 bufferSize) const;
   
   inline U64 getIntValue() const
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
   
   inline S64 getSignedIntValue() const
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
   
   inline F64 getFloatValue() const
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
   
   inline bool getBoolValue() const
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
   
   SimObject* getSimObject();
};

extern S32 TypeBufferString;

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
      if (isRefType(type))
      {
         value.refValue->addRef();
      }
   }
   
   inline void DecRef()
   {
      if (isRefType(type))
      {
         value.refValue->decRef();
      }
   }
   
   ConsoleValuePtr(const ConsoleValuePtr &other)
   {
      value = other.value;
      type = other.type;
      AddRef();
   }
   
   ConsoleValuePtr(ConsoleValuePtr &other)
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
   
   ConsoleValuePtr(const char* other)
   {
      type = ConsoleValue::TypeInternalNull;
      setString(other);
   }
   
   ConsoleValuePtr(const ConsoleStringValuePtr &other)
   {
      type = ConsoleValue::TypeInternalNull;
      setValue(other);
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
   
   void setValue(U64 inValue)
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
   
   void setValue(F64 inValue)
   {
      DecRef();
      type = ConsoleValue::TypeInternalFloat;
      value.fval = inValue;
   }
   
   void setValue(S32 inValue)
   {
      DecRef();
      type = ConsoleValue::TypeInternalInt;
      value.ival = inValue;
   }
   
   void setValue(const ConsoleValue& other)
   {
      if (isRefType(other.type))
      {
         other.value.refValue->addRef();
      }
      DecRef();
      
      type = other.type;
      value = other.value;
   }
   
   void setValue(ConsoleReferenceCountedType* other)
   {
      other->addRef();
      DecRef();
      
      type = other->getInternalType();
      value.refValue = other;
   }
   
   void setValue(const ConsoleStringValuePtr& other);
   
   void setString(const char* other);
   
   void setSTE(StringTableEntry other)
   {
      DecRef();
      
      type = ConsoleValue::TypeInternalStringTableEntry;
      value.string = other;
   }
   
   inline ConsoleValuePtr& operator=(ConsoleReferenceCountedType* other)
   {
      other->addRef();
      DecRef();
      
      type = other->getInternalType();
      value.refValue = other;
      return *this;
   }
   
   inline ConsoleValuePtr& operator=(const ConsoleValuePtr& other)
   {
      AssertFatal(false, "operator=  called");
      return *this;
   }
   
   /// Get string value
   ConsoleStringValuePtr getStringValue() const;
   
   inline U64 getIntValue() const
   {
      switch (type)
      {
         case TypeInternalNull:
            return 0;
         case TypeInternalInt:
            return (U32)value.ival;
         case TypeInternalFloat:
            return (U32)value.fval;
         default:
            return dAtoi(getStringValue().c_str());
      }
   }
   
   inline S32 getSignedIntValue() const
   {
      switch (type)
      {
         case TypeInternalNull:
            return 0;
         case TypeInternalInt:
            return (S32)value.ival;
         case TypeInternalFloat:
            return (S32)value.fval;
         default:
            return dAtoi(getStringValue().c_str());
      }
   }
   
   inline F64 getFloatValue() const
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
            return dAtof(getStringValue().c_str());
      }
   }
   
   inline bool getBoolValue() const
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
            return dAtob(getStringValue().c_str());
      }
   }
   
#ifndef EXCLUDE_CONSOLEVALUE_CASTERS
   inline operator const char*() { return getStringValue().c_str(); } const
   inline operator U32() { return (U32)getIntValue(); } const
   inline operator S32() { return getSignedIntValue(); } const
   inline operator F64() { return getFloatValue(); } const
   inline operator bool() { return getBoolValue(); } const
   
   inline bool isString() { return (type >= ConsoleValue::TypeInternalStringTableEntry && type <= ConsoleValue::TypeInternalNamespaceName) || type == TypeBufferString; }
   inline bool isInt() { return type == TypeInternalInt; }
   inline bool isFloat() { return type == TypeInternalFloat; }
#endif
   
   /// Read a serialized stack
   static void readStack(Stream &s, ConsoleSerializationState &serializationState, Vector<ConsoleValuePtr> &stack);
   
   /// Write a serialized stack
   static void writeStack(Stream &s, ConsoleSerializationState &serializationState, Vector<ConsoleValuePtr> &stack);
   
   static ConsoleValuePtr NullValue;
};

// Overrides to allow ConsoleValuePtrs to be directly converted to S32&F32

inline S32 dAtoi(const ConsoleValuePtr &ref)
{
   return ref.getSignedIntValue();
}

inline F32 dAtof(const ConsoleValuePtr &ref)
{
   return ref.getFloatValue();
}

inline bool dAtob(const ConsoleValuePtr &ref)
{
   return ref.getBoolValue();
}

#include "collection/vector.h"

class ConsoleArrayValue : public ConsoleReferenceCountedType
{
public:
   Vector<ConsoleValuePtr> mValues;
   
   ConsoleArrayValue() {;}
   virtual ~ConsoleArrayValue() { clear(); }
   
   void clear();
   
   virtual ConsoleBaseType* getType();
   virtual S32 getInternalType();
   
   virtual void read(Stream &s, ConsoleSerializationState &state);
   virtual void write(Stream &s, ConsoleSerializationState &state);
   
   virtual bool getDataField(const StringTableEntry slotName, const ConsoleValuePtr &array, ConsoleValuePtr &outValue);
   virtual void setDataField(const StringTableEntry slotName, const ConsoleValuePtr &array, const ConsoleValuePtr &newValue);
   virtual Namespace* getNamespace();
   
   virtual S32 getIteratorLength();
   virtual bool advanceIterator(ConsoleValuePtr &iterator, ConsoleValuePtr &iteratorValue);
   
   virtual ConsoleStringValuePtr getString();
   
   virtual bool stringCompare(const char* other);
   
   virtual bool refCompare(ConsoleReferenceCountedType* other);
   
   static ConsoleArrayValue* fromValues(int argc, ConsoleValue* argv);
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
   
   inline void setString(const char *str)
   {
      U32 stringLen = str ? dStrlen(str) : 0;
      
      // may as well pad to the next cache line
      U32 newLen = ((stringLen + 1) + 15) & ~15;
      
      if(buffer == NULL)
      {
         buffer = (char*) dMalloc(newLen);
         bufferLen = newLen;
      }
      else if(newLen > bufferLen)
      {
         buffer = (char*) dRealloc(buffer, newLen);
         bufferLen = newLen;
      }
      
      if (str)
         dStrcpy(buffer, str);
      else
         buffer[0] = '\0';
   }
   
   inline void append(const char *str, S32 pageSize=1024)
   {
      U32 endStr = 0;
      
      if(buffer == NULL)
      {
         U32 stringLen = dStrlen(str);
         U32 newLen = ((stringLen + 1) + (pageSize-1)) & ~(pageSize-1);
         
         buffer = (char*) dMalloc(newLen);
      }
      else
      {
         endStr = dStrlen(buffer);
         U32 strLen = dStrlen(str);
         if (strLen == 0)
            return;
         
         U32 stringLen = endStr + strLen;
         // may as well pad to the next cache line
         U32 newLen = ((stringLen + 1) + (pageSize-1)) & ~(pageSize-1);
         
         if (bufferLen < newLen)
         {
            buffer = (char*) dRealloc(buffer, newLen);
            bufferLen = newLen;
         }
      }
      
      dStrcpy(buffer+endStr, str);
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
   
   void setString(const char *str)
   {
      value.setString(str);
   }
   
   virtual ConsoleBaseType *getType();
   
   virtual void read(Stream &s, ConsoleSerializationState &state);
   
   virtual void write(Stream &s, ConsoleSerializationState &state);
   
   ConsoleStringValuePtr getString()
   {
      return ConsoleStringValuePtr(this);
   }
   
   inline SimpleString& getInternalString()
   {
      return value;
   }
   
   virtual bool stringCompare(const char* other)
   {
      return dStricmp(value.buffer, other) == 0;
   }
   
   virtual bool refCompare(ConsoleReferenceCountedType* other)
   {
      return other == this || stringCompare(other->getString().c_str());
   }
   
   static ConsoleStringValue* fromInt(S64 num);
   static ConsoleStringValue* fromFloat(F32 num);
   static ConsoleStringValue* fromString(const char *string);
   static ConsoleStringValue* fromSimpleString(SimpleString *string);
};

inline void ConsoleStringValuePtr::AddRef()
{
   if (value)
   {
      value->addRef();
   }
}

inline void ConsoleStringValuePtr::DecRef()
{
   if (value)
   {
      value->decRef();
   }
}

inline bool ConsoleStringValuePtr::isNull() { return value == NULL; }
inline void ConsoleStringValuePtr::setNull() { DecRef(); value = NULL; }

inline ConsoleStringValuePtr& ConsoleStringValuePtr::operator=(const ConsoleStringValuePtr& other)
{
   if (other.value == value)
      return *this;
   DecRef();
   value = other.value;
   AddRef();
   return *this;
}

inline const char* ConsoleStringValuePtr::c_str() { return value ? value->getInternalString().getString() : ""; }
inline ConsoleStringValue* ConsoleStringValuePtr::getPtr() { return value; }

#endif
