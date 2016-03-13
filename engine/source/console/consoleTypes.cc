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

#include "console/console.h"
#include "console/consoleTypes.h"
#include "console/consoleDictionary.h"
#include "console/consoleNamespace.h"
#include "string/stringTable.h"
#include "sim/simBase.h"

#ifndef _STRINGUNIT_H_
#include "string/stringUnit.h"
#endif

#ifndef _VECTOR2_H_
#include "2d/core/Vector2.h"
#endif

//////////////////////////////////////////////////////////////////////////
// TypeString
//////////////////////////////////////////////////////////////////////////
ConsoleType( string, TypeString, sizeof(const char*), "" )
ConsoleUseDefaultReferenceType( TypeString, const char* )

ConsoleTypeToString( TypeString )
{
   return *((const char **)(dataPtr));
}

ConsoleTypeFromConsoleValue( TypeString )
{
   if (ConsoleValue::isRefType(value.type))
   {
      if (value.value.refValue->isEnumerable())
      {
         Con::warnf( "(TypeString) Cannot set multiple args to a single string." );
         return;
      }
   }
   
   *((const char **) dataPtr) = value.getSTEStringValue();
}

/////////////////////////////////////////////////////////////////////////
// TypeStringEntryVector
//////////////////////////////////////////////////////////////////////////
ConsoleType( string, TypeStringTableEntryVector, sizeof(Vector<StringTableEntry>), "" )
ConsoleUseDefaultReferenceType( TypeStringTableEntryVector, Vector<StringTableEntry> )

ConsoleTypeToString( TypeStringTableEntryVector )
{
   Vector<StringTableEntry> *vec = (Vector<StringTableEntry>*)dataPtr;
   char* returnBuffer = Con::getReturnBuffer(1024);
   S32 maxReturn = 1024;
   returnBuffer[0] = '\0';
   S32 returnLeng = 0;
   for (Vector<StringTableEntry>::iterator itr = vec->begin(); itr < vec->end(); itr++)
   {
      // concatenate the next value onto the return string
       if ( itr == vec->begin() )
       {
           dSprintf(returnBuffer + returnLeng, maxReturn - returnLeng, "%s", *itr);
       }
       else
       {
           dSprintf(returnBuffer + returnLeng, maxReturn - returnLeng, ",%s", *itr);
       }
      returnLeng = dStrlen(returnBuffer);
   }
   return (returnBuffer);
}

ConsoleTypeFromConsoleValue( TypeStringTableEntryVector )
{
   Vector<StringTableEntry> *vec = (Vector<StringTableEntry>*)dataPtr;
   vec->clear();
   
   if (ConsoleValue::isRefType(value.type))
   {
      if (value.value.refValue->isEnumerable())
      {
         S32 length = value.value.refValue->getIteratorLength();
         ConsoleValuePtr iterator;
         iterator.type = ConsoleValue::TypeInternalInt;
         iterator.value.ival = 1;
         
         ConsoleValuePtr outValue;
         for (S32 i=0; i<length; i++)
         {
            if (!value.value.refValue->advanceIterator(iterator, outValue))
               break;
            
            vec->push_back(outValue.getSTEStringValue());
         }
         return;
      }
   }
   
   // Fallback: use string
   const char* arg = value.getTempStringValue();
   const U32 unitCount = StringUnit::getUnitCount(arg, ",");
   for( U32 unitIndex = 0; unitIndex < unitCount; ++unitIndex )
   {
      vec->push_back( StringTable->insert( StringUnit::getUnit(arg, unitIndex, ",") ) );
   }
}


//////////////////////////////////////////////////////////////////////////
// TypeCaseString
//////////////////////////////////////////////////////////////////////////
ConsoleType( caseString, TypeCaseString, sizeof(const char*), "" )
ConsoleUseDefaultReferenceType( TypeCaseString, const char* )

ConsoleTypeFromConsoleValue( TypeCaseString )
{
   if (ConsoleValue::isRefType(value.type))
   {
      if (value.value.refValue->isEnumerable())
      {
         Con::warnf( "(TypeCaseString) Cannot set multiple args to a single string." );
         return;
      }
   }
   
   *((const char **) dataPtr) = StringTable->insert(value.getTempStringValue(), true);
}

ConsoleTypeToString( TypeCaseString )
{
   return *((const char **)(dataPtr));
}

//////////////////////////////////////////////////////////////////////////
// TypeFileName
//////////////////////////////////////////////////////////////////////////
ConsolePrepType( filename, TypeFilename, sizeof( const char* ), "" )
ConsoleUseDefaultReferenceType( TypeFilename, const char* )

ConsoleTypeFromConsoleValue( TypeFilename )
{
   if (ConsoleValue::isRefType(value.type))
   {
      if (value.value.refValue->isEnumerable())
      {
         Con::warnf( "(TypeFilename) Cannot set multiple args to a single string." );
         return;
      }
   }
   
   char buffer[1024];
   const char* filename = value.getTempStringValue();
   if (Con::expandPath(buffer, 1024, filename))
      *((const char **) dataPtr) = StringTable->insert(buffer);
   else
      Con::warnf("(TypeFilename) illegal filename detected: %s", filename);
}

ConsoleTypeToString( TypeFilename )
{
   return *((const char **)(dataPtr));
}

/* TOFIX
ConsolePrepData( TypeFilename )
{
   if( Con::expandPath( buffer, bufferSize, data ) )
      return buffer;
   else
   {
      Con::warnf("(TypeFilename) illegal filename detected: %s", data);
      return data;
   }
}*/

//////////////////////////////////////////////////////////////////////////
// TypeS8
//////////////////////////////////////////////////////////////////////////
ConsoleType( char, TypeS8, sizeof(U8), "" )
ConsoleUseDefaultReferenceType( TypeS8, S8 )

ConsoleTypeToString( TypeS8 )
{
   char* returnBuffer = Con::getReturnBuffer(256);
   dSprintf(returnBuffer, 256, "%d", *((U8 *) dataPtr) );
   return returnBuffer;
}

ConsoleTypeFromConsoleValue( TypeS8 )
{
   if (ConsoleValue::isRefType(value.type))
   {
      if (value.value.refValue->isEnumerable())
      {
         Con::warnf( "(TypeS8) Cannot set multiple args to a single S8." );
         return;
      }
   }
   
   *((S8 *) dataPtr) = (S8)value.getSignedIntValue();
}

//////////////////////////////////////////////////////////////////////////
// TypeS32
//////////////////////////////////////////////////////////////////////////
ConsoleType( int, TypeS32, sizeof(S32), "" )
ConsoleUseDefaultReferenceType( TypeS32, S32 )

ConsoleTypeToString( TypeS32 )
{
   char* returnBuffer = Con::getReturnBuffer(256);
   dSprintf(returnBuffer, 256, "%d", *((S32 *) dataPtr) );
   return returnBuffer;
}

ConsoleTypeFromConsoleValue( TypeS32 )
{
   if (ConsoleValue::isRefType(value.type))
   {
      if (value.value.refValue->isEnumerable())
      {
         Con::warnf( "(TypeS32) Cannot set multiple args to a single S32." );
         return;
      }
   }
   
   *((S32 *) dataPtr) = (S32)value.getSignedIntValue();
}

//////////////////////////////////////////////////////////////////////////
// TypeS32Vector
//////////////////////////////////////////////////////////////////////////
ConsoleType( intList, TypeS32Vector, sizeof(Vector<S32>), "" )
ConsoleUseDefaultReferenceType( TypeS32Vector, Vector<S32> )

ConsoleTypeToString( TypeS32Vector )
{
   Vector<S32> *vec = (Vector<S32> *)dataPtr;
   S32 buffSize = ( vec->size() * 15 ) + 16 ;
   char* returnBuffer = Con::getReturnBuffer( buffSize );
   S32 maxReturn = buffSize;
   returnBuffer[0] = '\0';
   S32 returnLeng = 0;
   for (Vector<S32>::iterator itr = vec->begin(); itr != vec->end(); itr++)
   {
      // concatenate the next value onto the return string
      dSprintf(returnBuffer + returnLeng, maxReturn - returnLeng, "%d ", *itr);
      // update the length of the return string (so far)
      returnLeng = dStrlen(returnBuffer);
   }
   // trim off that last extra space
   if (returnLeng > 0 && returnBuffer[returnLeng - 1] == ' ')
      returnBuffer[returnLeng - 1] = '\0';
   return returnBuffer;
}

ConsoleTypeFromConsoleValue( TypeS32Vector )
{
   Vector<S32> *vec = (Vector<S32>*)dataPtr;
   vec->clear();
   
   if (ConsoleValue::isRefType(value.type))
   {
      if (value.value.refValue->isEnumerable())
      {
         S32 length = value.value.refValue->getIteratorLength();
         ConsoleValuePtr iterator;
         iterator.type = ConsoleValue::TypeInternalInt;
         iterator.value.ival = 1;
         ConsoleValuePtr outValue;
         for (S32 i=0; i<length; i++)
         {
            if (!value.value.refValue->advanceIterator(iterator, outValue))
               break;
            
            vec->push_back((S32)outValue.getSignedIntValue());
         }
         return;
      }
   }
   
   // Fallback: use string
   const char *values = value.getTempStringValue();
   const char *endValues = values + dStrlen(values);
   S32 intValue;
   // advance through the string, pulling off S32's and advancing the pointer
   while (values < endValues && dSscanf(values, "%d", &value) != 0)
   {
      vec->push_back(intValue);
      const char *nextValues = dStrchr(values, ' ');
      if (nextValues != 0 && nextValues < endValues)
         values = nextValues + 1;
      else
         break;
   }
}

//////////////////////////////////////////////////////////////////////////
// TypeF32
//////////////////////////////////////////////////////////////////////////
ConsoleType( float, TypeF32, sizeof(F32), "" )
ConsoleUseDefaultReferenceType( TypeF32, Vector<F32> )

ConsoleTypeToString( TypeF32 )
{
   char* returnBuffer = Con::getReturnBuffer(256);
   dSprintf(returnBuffer, 256, "%.9g", *((F32 *) dataPtr) );
   return returnBuffer;
}
ConsoleTypeFromConsoleValue( TypeF32 )
{
   if (ConsoleValue::isRefType(value.type))
   {
      if (value.value.refValue->isEnumerable())
      {
         Con::warnf( "(TypeF32) Cannot set multiple args to a single F32." );
         return;
      }
   }
   
   *((F32 *) dataPtr) = (F32)value.getFloatValue();
}

//////////////////////////////////////////////////////////////////////////
// TypeF32Vector
//////////////////////////////////////////////////////////////////////////
ConsoleType( floatList, TypeF32Vector, sizeof(Vector<F32>), "" )
ConsoleUseDefaultReferenceType( TypeF32Vector, Vector<F32> )

ConsoleTypeToString( TypeF32Vector )
{
   Vector<F32> *vec = (Vector<F32> *)dataPtr;
   S32 buffSize = ( vec->size() * 15 ) + 16 ;
   char* returnBuffer = Con::getReturnBuffer( buffSize );
   S32 maxReturn = buffSize;
   returnBuffer[0] = '\0';
   S32 returnLeng = 0;
   for (Vector<F32>::iterator itr = vec->begin(); itr != vec->end(); itr++)
   {
      // concatenate the next value onto the return string
      dSprintf(returnBuffer + returnLeng, maxReturn - returnLeng, "%g ", *itr);
      // update the length of the return string (so far)
      returnLeng = dStrlen(returnBuffer);
   }
   // trim off that last extra space
   if (returnLeng > 0 && returnBuffer[returnLeng - 1] == ' ')
      returnBuffer[returnLeng - 1] = '\0';
   return returnBuffer;
}

ConsoleTypeFromConsoleValue( TypeF32Vector )
{
   Vector<F32> *vec = (Vector<F32>*)dataPtr;
   vec->clear();
   
   if (ConsoleValue::isRefType(value.type))
   {
      if (value.value.refValue->isEnumerable())
      {
         S32 length = value.value.refValue->getIteratorLength();
         ConsoleValuePtr iterator;
         iterator.type = ConsoleValue::TypeInternalInt;
         iterator.value.ival = 1;
         ConsoleValuePtr outValue;
         for (S32 i=0; i<length; i++)
         {
            if (!value.value.refValue->advanceIterator(iterator, outValue))
               break;
            
            vec->push_back((F32)outValue.getFloatValue());
         }
         return;
      }
   }
   
   // Fallback: use string
   const char *values = value.getTempStringValue();
   const char *endValues = values + dStrlen(values);
   F32 floatValue;
   // advance through the string, pulling off S32's and advancing the pointer
   while (values < endValues && dSscanf(values, "%.9g", &floatValue) != 0)
   {
      vec->push_back(floatValue);
      const char *nextValues = dStrchr(values, ' ');
      if (nextValues != 0 && nextValues < endValues)
         values = nextValues + 1;
      else
         break;
   }
}

//////////////////////////////////////////////////////////////////////////
// TypeBool
//////////////////////////////////////////////////////////////////////////
ConsoleType( bool, TypeBool, sizeof(bool), "" )
ConsoleUseDefaultReferenceType( TypeBool, bool )

ConsoleTypeToString( TypeBool )
{
   return *((bool *) dataPtr) ? "1" : "0";
}

ConsoleTypeFromConsoleValue( TypeBool )
{
   if (ConsoleValue::isRefType(value.type))
   {
      if (value.value.refValue->isEnumerable())
      {
         Con::warnf( "(TypeBool) Cannot set multiple args to a single bool." );
         return;
      }
   }
   
   *((bool *) dataPtr) = value.getBoolValue();
}


//////////////////////////////////////////////////////////////////////////
// TypeBoolVector
//////////////////////////////////////////////////////////////////////////
ConsoleType( boolList, TypeBoolVector, sizeof(Vector<bool>), "" )
ConsoleUseDefaultReferenceType( TypeBoolVector, Vector<bool> )

ConsoleTypeToString( TypeBoolVector )
{
   Vector<bool> *vec = (Vector<bool>*)dataPtr;
   char* returnBuffer = Con::getReturnBuffer(1024);
   S32 maxReturn = 1024;
   returnBuffer[0] = '\0';
   S32 returnLeng = 0;
   for (Vector<bool>::iterator itr = vec->begin(); itr < vec->end(); itr++)
   {
      // concatenate the next value onto the return string
      dSprintf(returnBuffer + returnLeng, maxReturn - returnLeng, "%d ", (*itr == true ? 1 : 0));
      returnLeng = dStrlen(returnBuffer);
   }
   // trim off that last extra space
   if (returnLeng > 0 && returnBuffer[returnLeng - 1] == ' ')
      returnBuffer[returnLeng - 1] = '\0';
   return(returnBuffer);
}

ConsoleTypeFromConsoleValue( TypeBoolVector )
{
   Vector<bool> *vec = (Vector<bool>*)dataPtr;
   vec->clear();
   
   if (ConsoleValue::isRefType(value.type))
   {
      if (value.value.refValue->isEnumerable())
      {
         S32 length = value.value.refValue->getIteratorLength();
         ConsoleValuePtr iterator;
         iterator.type = ConsoleValue::TypeInternalInt;
         iterator.value.ival = 1;
         ConsoleValuePtr outValue;
         for (S32 i=0; i<length; i++)
         {
            if (!value.value.refValue->advanceIterator(iterator, outValue))
               break;
            
            vec->push_back(outValue.getBoolValue());
         }
         return;
      }
   }
   
   // Fallback: use string
   const char *values = value.getTempStringValue();
   const char *endValues = values + dStrlen(values);
   bool boolValue;
   // advance through the string, pulling off S32's and advancing the pointer
   while (values < endValues && dSscanf(values, "%d", &boolValue) != 0)
   {
      vec->push_back(boolValue);
      const char *nextValues = dStrchr(values, ' ');
      if (nextValues != 0 && nextValues < endValues)
         values = nextValues + 1;
      else
         break;
   }
}

//////////////////////////////////////////////////////////////////////////
// TypeEnum
//////////////////////////////////////////////////////////////////////////
ConsoleType( enumval, TypeEnum, sizeof(S32), "" )
ConsoleUseDefaultReferenceType( TypeEnum, S32 )

ConsoleTypeToString( TypeEnum )
{
   AssertFatal(tbl, "Null enum table passed to getDataTypeEnum()");
   S32 dataPtrVal = *(S32*)dataPtr;
   for (S32 i = 0; i < tbl->size; i++)
   {
      if (dataPtrVal == tbl->table[i].index)
      {
         return tbl->table[i].label;
      }
   }

   //not found
   return "";
}

ConsoleTypeFromConsoleValue( TypeEnum )
{
   AssertFatal(tbl, "Null enum table passed to setDataTypeEnum()");
   
   if (ConsoleValue::isRefType(value.type))
   {
      if (value.value.refValue->isEnumerable())
      {
         Con::warnf( "(TypeEnum) Cannot set multiple args to a single enum." );
         return;
      }
   }
   
   const char* enumValue = value.getTempStringValue();
   
   S32 val = 0;
   for (S32 i = 0; i < tbl->size; i++)
   {
      if (! dStricmp(enumValue, tbl->table[i].label))
      {
         val = tbl->table[i].index;
         break;
      }
   }
   *((S32 *) dataPtr) = val;
}

//////////////////////////////////////////////////////////////////////////
// TypeNamespacePtr
//////////////////////////////////////////////////////////////////////////
ConsoleType( Namespace*, TypeNamespacePtr, sizeof(Namespace*), "" )
ConsoleUseDefaultReferenceType( TypeNamespacePtr, Namespace* )

ConsoleTypeFromConsoleValue( TypeNamespacePtr )
{
   if (ConsoleValue::isRefType(value.type))
   {
      if (value.value.refValue->isEnumerable())
      {
         Con::warnf( "(TypeNamespacePtr) Cannot set multiple args to a single Namespace." );
         return;
      }
   }
   
   Namespace **obj = (Namespace **)dataPtr;
   *obj = Namespace::find(value.getTempStringValue());
}

ConsoleTypeToString( TypeNamespacePtr )
{
   Namespace **obj = (Namespace**)dataPtr;
   char* returnBuffer = Con::getReturnBuffer(256);
   const char* Id =  *obj ? (*obj)->mName : StringTable->EmptyString;
   dSprintf(returnBuffer, 256, "%s", Id);
   return returnBuffer;
}


//////////////////////////////////////////////////////////////////////////
// TypeSimObjectPtr
//////////////////////////////////////////////////////////////////////////
ConsoleType( SimObjectPtr, TypeSimObjectPtr, sizeof(SimObject*), "" )
ConsoleUseDefaultReferenceType( TypeSimObjectPtr, SimObject* )

ConsoleTypeFromConsoleValue( TypeSimObjectPtr )
{
   if (ConsoleValue::isRefType(value.type))
   {
      if (value.value.refValue->isEnumerable())
      {
         Con::warnf( "(TypeSimObjectPtr) Cannot set multiple args to a single SimObject." );
         return;
      }
   }
   
   SimObject **obj = (SimObject **)dataPtr;
   *obj = Sim::findObject(value);
}

ConsoleTypeToString( TypeSimObjectPtr )
{
   SimObject **obj = (SimObject**)dataPtr;
   char* returnBuffer = Con::getReturnBuffer(256);
   const char* Id =  *obj ? (*obj)->getName() ? (*obj)->getName() : (*obj)->getIdString() : StringTable->EmptyString;
   dSprintf(returnBuffer, 256, "%s", Id);
   return returnBuffer;
}

//////////////////////////////////////////////////////////////////////////
// TypeSimObjectSafePtr
//////////////////////////////////////////////////////////////////////////
ConsoleType( TypeSimObjectSafePtr, TypeSimObjectSafePtr, sizeof(SimObjectPtr<SimObject>), "" )
//ConsoleUseDefaultReferenceType( TypeSimObjectPtr, SimObject* )
ConsoleSetReferenceType( TypeSimObjectSafePtr, ConsoleSimObjectPtr )

ConsoleTypeFromConsoleValue( TypeSimObjectSafePtr )
{
   if (ConsoleValue::isRefType(value.type))
   {
      if (value.value.refValue->isEnumerable())
      {
         Con::warnf( "(TypeSimObjectSafePtr) Cannot set multiple args to a single SimObject." );
         return;
      }
   }
   
   SimObjectPtr<SimObject> *obj = (SimObjectPtr<SimObject> *)dataPtr;
   *obj = Sim::findObject(value);
}

ConsoleTypeToString( TypeSimObjectSafePtr )
{
   SimObjectPtr<SimObject> *obj = (SimObjectPtr<SimObject> *)dataPtr;
   char* returnBuffer = Con::getReturnBuffer(256);
   const char* Id =  *obj ? (*obj)->getName() ? (*obj)->getName() : (*obj)->getIdString() : StringTable->EmptyString;
   dSprintf(returnBuffer, 256, "%s", Id);
   return returnBuffer;
}

void ConsoleSimObjectPtr::setString(const char *str)
{
   value = Sim::findObject<SimObject>(str);
}

ConsoleBaseType* ConsoleSimObjectPtr::getType()
{
   return ConsoleTypeTypeSimObjectSafePtr::getInstance();
}


bool ConsoleSimObjectPtr::getDataField(StringTableEntry slotName, const ConsoleValuePtr &array, ConsoleValuePtr &outValue)
{
   // Don't allow array accessor
   if (slotName == ConsoleBaseType::getFieldIndexName())
   {
      ((ConsoleValuePtr&)outValue).setNull();
      return false;
   }
   
   if (value)
   {
      outValue.setValue(value->getDataField(slotName, array));
   }
   else
   {
      outValue.setNull();
   }
   
   return true;
}

void ConsoleSimObjectPtr::setDataField(StringTableEntry slotName, const ConsoleValuePtr &array, const ConsoleValuePtr &newValue)
{
   if (value)
   {
      // TODO: directly pass through values
      value->setDataField(slotName, array, newValue);
   }
}

Namespace* ConsoleSimObjectPtr::getNamespace()
{
   return value ? value->getNamespace() : NULL;
}

S32 ConsoleSimObjectPtr::getIteratorLength()
{
   SimObject* obj = value;
   if (!obj)
      return 0;
   
   SimSet* setObj = dynamic_cast<SimSet*>(obj);
   return setObj ? setObj->size() : 0;
}

bool ConsoleSimObjectPtr::advanceIterator(ConsoleValuePtr &iterator, ConsoleValuePtr& iteratorValue)
{
   SimObject* obj = value;
   if (!obj)
   {
      ((ConsoleValuePtr&)iterator).setNull();
      return false;
   }
   
   SimSet* setObj = dynamic_cast<SimSet*>(obj);
   if (!setObj)
   {
      ((ConsoleValuePtr&)iterator).setNull();
      return false;
   }
   
   S32 size = setObj->size();
   S32 iterValue = (S32)iterator.getIntValue()-1;
   if (iterValue < 0 || iterValue >= size)
   {
      ((ConsoleValuePtr&)iterator).setNull();
		return false;
   }
   else
   {
      ((ConsoleValuePtr&)iteratorValue).setValue(ConsoleSimObjectPtr::fromObject(setObj->first() + iterValue));
      ((ConsoleValuePtr&)iterator).setValue(iterValue+1);
   }
	
	return true;
}

bool ConsoleSimObjectPtr::refCompare(ConsoleReferenceCountedType* other)
{
   ConsoleSimObjectPtr *oso = dynamic_cast<ConsoleSimObjectPtr*>(other);
   return other == this || (oso && oso->value == value) || Sim::findObject<SimObject>(other->getString().c_str()) == value;
}

ConsoleSimObjectPtr *ConsoleSimObjectPtr::fromObject(SimObject* obj)
{
   ConsoleSimObjectPtr *value = new ConsoleSimObjectPtr();
   value->value = obj;
   return value;
}

//////////////////////////////////////////////////////////////////////////
// TypeSimObjectName
//////////////////////////////////////////////////////////////////////////
ConsoleType( SimObjectName, TypeSimObjectName, sizeof(SimObject*), "" )
ConsoleUseDefaultReferenceType( TypeSimObjectName, SimObject* )

ConsoleTypeFromConsoleValue( TypeSimObjectName )
{
   if (ConsoleValue::isRefType(value.type))
   {
      if (value.value.refValue->isEnumerable())
      {
         Con::warnf( "(TypeSimObjectName) Cannot set multiple args to a single SimObject." );
         return;
      }
   }
   
   SimObject **obj = (SimObject **)dataPtr;
   *obj = Sim::findObject(value);
}

ConsoleTypeToString( TypeSimObjectName )
{
   SimObject **obj = (SimObject**)dataPtr;
   char* returnBuffer = Con::getReturnBuffer(128);
   dSprintf(returnBuffer, 128, "%s", *obj && (*obj)->getName() ? (*obj)->getName() : "");
   return returnBuffer;
}


//////////////////////////////////////////////////////////////////////////
// TypeSimObjectId
//////////////////////////////////////////////////////////////////////////
ConsoleType( SimObjectId, TypeSimObjectId, sizeof(SimObject*), "" )
ConsoleUseDefaultReferenceType( TypeSimObjectId, SimObject* )

ConsoleTypeFromConsoleValue( TypeSimObjectId )
{
   if (ConsoleValue::isRefType(value.type))
   {
      if (value.value.refValue->isEnumerable())
      {
         Con::warnf( "(TypeSimObjectId) Cannot set multiple args to a single SimObject." );
         return;
      }
   }
   
   SimObject **obj = (SimObject **)dataPtr;
   *obj = Sim::findObject(value);
}

ConsoleTypeToString( TypeSimObjectId )
{
   SimObject **obj = (SimObject**)dataPtr;
   char* returnBuffer = Con::getReturnBuffer(128);
   dSprintf(returnBuffer, 128, "%s", *obj ? (*obj)->getIdString() : StringTable->EmptyString );
   return returnBuffer;
}

ConsoleType( SimpleString, TypeBufferString, sizeof(SimpleString), "" )
ConsoleSetReferenceType( TypeBufferString, ConsoleStringValue )

ConsoleTypeFromConsoleValue( TypeBufferString )
{
   if (ConsoleValue::isRefType(value.type))
   {
      if (value.value.refValue->isEnumerable())
      {
         Con::warnf( "(TypeBufferString) Cannot set multiple args to a single S32." );
         return;
      }
   }
   
   SimpleString *obj = (SimpleString *)dataPtr;
   obj->setString(value.getTempStringValue());
}

ConsoleTypeToString( TypeBufferString )
{
    SimpleString *obj = (SimpleString *)dataPtr;
    return obj->getString();
}

ConsoleBaseType* ConsoleStringValue::getType()
{
    return ConsoleTypeTypeBufferString::getInstance();
}

ConsoleBaseType* ConsoleNamespacePtr::getType()
{
   return ConsoleTypeTypeNamespacePtr::getInstance();
}

void ConsoleNamespacePtr::setString(const char *str)
{
   char buffer[256];
   dStrncpy(buffer, str, sizeof(buffer));
   buffer[255] = '\0';
   
   char* packageName = NULL;
   char* namespaceName = dStrstr( buffer, (char*)"::" );
   
   if (namespaceName != NULL)
   {
      *namespaceName = '\0';
      packageName = namespaceName+2;
   }
   
   value = Namespace::find(StringTable->insert(namespaceName), StringTable->insert(packageName));
}

ConsoleStringValuePtr ConsoleNamespacePtr::getString()
{
   if (value)
   {
      if (!value->mPackage || value->mPackage == StringTable->EmptyString)
      {
         return value->mName;
      }
      else
      {
         char buffer[256];
         dSprintf(buffer, sizeof(buffer), "%s::%s", value->mPackage, value->mName);
         return StringTable->insert(buffer);
      }
   }
   
   return StringTable->EmptyString;
}

bool ConsoleNamespacePtr::getDataField(StringTableEntry slotName, const ConsoleValuePtr &, ConsoleValuePtr &outValue)
{
   return false;
}

void ConsoleNamespacePtr::setDataField(StringTableEntry slotName, const ConsoleValuePtr &array, const ConsoleValuePtr &newValue)
{
   
}

void ConsoleNamespacePtr::read(Stream &s, ConsoleSerializationState &state)
{
   // namespace, package
   StringTableEntry pkgName = s.readSTString();
   StringTableEntry nsName = s.readSTString();
   value = Namespace::find(nsName, pkgName);
}

void ConsoleNamespacePtr::write(Stream &s, ConsoleSerializationState &state)
{
   // namespace, package
   if (value)
   {
      s.writeString(value->mPackage);
      s.writeString(value->mName);
   }
   else
   {
      s.writeString("");
      s.writeString("");
   }
}


//////////////////////////////////////////////////////////////////////////
// TypeConsoleArray
//////////////////////////////////////////////////////////////////////////
ConsoleType( TypeConsoleArray, TypeConsoleArray, sizeof(Vector<ConsoleValuePtr>), "" )
//ConsoleUseDefaultReferenceType( TypeSimObjectPtr, SimObject* )
ConsoleSetReferenceType( TypeConsoleArray, ConsoleArrayValue )

ConsoleTypeFromConsoleValue( TypeConsoleArray )
{
   Vector<ConsoleValuePtr> &obj = *((Vector<ConsoleValuePtr> *)dataPtr);
   
   if (ConsoleValue::isRefType(value.type))
   {
      // Clear existing
      for (U32 i=0, sz = obj.size(); i<sz; i++)
      {
         obj[i].setNull();
      }
      
      if (value.type == TypeConsoleArray)
      {
         ConsoleArrayValue* v = (ConsoleArrayValue*)value.value.refValue;
         
         // Set new
         obj.setSize(v->mValues.size());
         dMemset(obj.address(), 0, obj.size()*sizeof(ConsoleValuePtr));
         
         for (U32 i=0, sz = obj.size(); i<sz; i++)
         {
            obj[i].setValue(v->mValues[i]);
         }
         return;
      }
      else if (value.value.refValue->isEnumerable())
      {
         S32 length = value.value.refValue->getIteratorLength();
         ConsoleValuePtr iterator;
         iterator.type = ConsoleValue::TypeInternalInt;
         iterator.value.ival = 1;
         ConsoleValuePtr outValue;
         obj.clear();
         
         for (S32 i=0; i<length; i++)
         {
            if (!value.value.refValue->advanceIterator(iterator, outValue))
               break;
            
            obj.push_back(outValue);
         }
         return;
      }
   }
   
   obj.setSize(0);
   Con::warnf("(TypeConsoleArray) can't set values from a non-enumerable value.");
}

ConsoleTypeToString( TypeConsoleArray )
{
   Vector<ConsoleValuePtr> &obj = *(Vector<ConsoleValuePtr> *)dataPtr;
   SimpleString str;
   
   for (U32 i=0, sz = obj.size(); i<sz; i++)
   {
      if (i > 0)
         str.append("\t");
      str.append(obj[i].getTempStringValue());
   }
   
   return str.getString();
}

ConsoleBaseType* ConsoleArrayValue::getType()
{
   return ConsoleTypeTypeConsoleArray::getInstance();
}




