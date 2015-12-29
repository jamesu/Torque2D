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
#include "console/consoleBaseType.h"
#include "console/consoleDictionary.h"
#include "console/consoleNamespace.h"
#include "console/consoleTypes.h"
#include "sim/simBase.h"

// Init the globals.
ConsoleBaseType *ConsoleBaseType::smListHead = NULL;
S32              ConsoleBaseType::smConsoleTypeCount = 9; // This makes 0 an invalid console type.

// And, we also privately store the types lookup table.
VectorPtr<ConsoleBaseType*> gConsoleTypeTable;

ConsoleTypeConversionMethodConstructor* ConsoleBaseType::smTypeConversionMethodInitializers = NULL;

StringTableEntry ConsoleBaseType::getFieldIndexName()
{
   static StringTableEntry cbtFieldIndex = StringTable->insert("__index__");
   return cbtFieldIndex;
}

ConsoleBaseType *ConsoleBaseType::getListHead()
{
   return smListHead;
}

void ConsoleBaseType::initialize()
{
   // Prep and empty the vector.
   gConsoleTypeTable.setSize(smConsoleTypeCount+1);
   dMemset(gConsoleTypeTable.address(), 0, sizeof(ConsoleBaseType*) * gConsoleTypeTable.size());

   // Walk the list and register each one with the console system.
   ConsoleBaseType *walk = getListHead();
   while(walk)
   {
      // Store a pointer to the type in the appropriate slot.
      const S32 id = walk->getTypeID();
      AssertFatal(gConsoleTypeTable[id]==NULL, "ConsoleBaseType::initialize - encountered a table slot that contained something!");
      gConsoleTypeTable[id] = walk;

      // Advance down the list...
      walk = walk->getListNext();
   }
   
   // Bind conversion method pointers
   for (ConsoleTypeConversionMethodConstructor* convWalk = smTypeConversionMethodInitializers; convWalk; convWalk = convWalk->next)
   {
      convWalk->set();
   }

   // Alright, we're all done here; we can now achieve fast lookups by ID.
}

//-----------------------------------------------------------------------------

ConsoleBaseType  *ConsoleBaseType::getType(const S32 typeID)
{
   return gConsoleTypeTable[typeID];
}

//-----------------------------------------------------------------------------

ConsoleBaseType* ConsoleBaseType::getTypeByName(const char *typeName)
{
    ConsoleBaseType* walk = getListHead();
    while( walk != NULL )
    {
        if( dStrcmp( walk->getTypeName(), typeName ) == 0 )
            return walk;
        
        walk = walk->getListNext();
    }
    
    return NULL;
}

void cbtDefaultOperatorFunc(ConsoleValuePtr *inOutParam, ConsoleValuePtr *inParam, ConsoleValuePtr *inSubParam)
{
    // First find object
    SimObject *obj = NULL;
    if (Sim::findObject(inParam->getStringValue().c_str(), obj))
    {
        Namespace *ns = obj->getNamespace();
        Namespace::Entry *entry = ns->lookup(inSubParam->getSTEStringValue());
        if (entry)
        {
            //inOutParam->setNSEValue(entry);
        }
        else
        {
            Con::errorf("Couldn't find entry %s!", inSubParam->getTempStringValue());
        }
        //= inParam->getStringValue();
    }
    else
    {
        Con::errorf("Couldn't find object %s", inParam->getTempStringValue());
    }
}

void cbtDefaultOperatorProperty(ConsoleValuePtr *inOutParam, ConsoleValuePtr *inParam, ConsoleValuePtr *inSubParam)
{
    // First find object
    SimObject *obj = NULL;
    if (Sim::findObject(*inParam, obj))
    {
    }
    else
    {
        Con::errorf("Couldn't find object %s", inParam->getTempStringValue());
    }
}

void cbtDefaultOperatorPropertySet(ConsoleValuePtr *inOutParam, ConsoleValuePtr *inParam, ConsoleValuePtr *inSubParam)
{
    // First find object
    SimObject *obj = NULL;
    if (Sim::findObject(*inOutParam, obj))
    {
    }
    else
    {
        Con::errorf("Couldn't find object %s", inOutParam->getTempStringValue());
    }
}

void cbtDefaultOperatorSubObject(ConsoleValuePtr *inOutParam, ConsoleValuePtr *inParam, ConsoleValuePtr *inSubParam)
{
    // First find object
    SimGroup *obj = NULL;
    if (Sim::findObject(*inParam, obj))
    {
        //SimObject *obj = obj->findObject(inParam->getStringValue());
    }
    else
    {
        Con::errorf("Couldn't find object %s", inParam->getTempStringValue());
    }
}

void cbtDefaultOperatorIndex(ConsoleValuePtr *inOutParam, ConsoleValuePtr *inParam, ConsoleValuePtr *inSubParam)
{
    // First find object
    SimGroup *obj = NULL;
    if (Sim::findObject(*inParam, obj))
    {
    }
    else
    {
        Con::errorf("Couldn't find object %s", inParam->getTempStringValue());
    }
}

void cbtDefaultOperatorIndexSet(ConsoleValuePtr *inOutParam, ConsoleValuePtr *inParam, ConsoleValuePtr *inSubParam)
{
    // First find object
    SimGroup *obj = NULL;
    if (Sim::findObject(*inOutParam, obj))
    {
    }
    else
    {
        Con::errorf("Couldn't find object %s", inOutParam->getTempStringValue());
    }
}

typedef void (*ConsoleTypeOperatorFunc)(ConsoleValuePtr *inOutParam, ConsoleValuePtr *inParam, ConsoleValuePtr *inSubParam);


//-------------------------------------------------------------------------

ConsoleBaseType::ConsoleBaseType(const S32 size, S32 *idPtr, const char *aTypeName) :
smTypeToStringFunc(0),
smTypeToConsoleValueFunc(0),
smTypeFromConsoleValueFunc(0),
smTypeToStreamFunc(0),
smTypeFromStreamFunc(0)
{
   // General initialization.
   mInspectorFieldType = NULL;

   // Store general info.
   mTypeSize = size;
   mTypeName = aTypeName;

   // Get our type ID and store it.
   mTypeID = smConsoleTypeCount++;
   *idPtr = mTypeID;

   // Link ourselves into the list.
   mListNext = smListHead;
   smListHead = this;

   // Alright, all done for now. Console initialization time code
   // takes us from having a list of general info and classes to
   // a fully initialized type table.
}

ConsoleBaseType::~ConsoleBaseType()
{
   // Nothing to do for now; we could unlink ourselves from the list, but why?
}

S32 ConsoleReferenceCountedType::getInternalType() { ConsoleBaseType* type = getType(); return type ? ConsoleValue::TypeCustomFieldStart + type->getTypeID() : ConsoleValue::TypeCustomFieldStart; }

bool ConsoleReferenceCountedType::getDataField(StringTableEntry slotName, const ConsoleValuePtr& array, ConsoleValuePtr &outValue)
{
   SimObject* obj = Sim::findObject<SimObject>(getString().c_str());
   if (obj)
   {
      outValue.setValue(obj->getDataField(slotName, array.getTempStringValue()));
   }
   else
   {
      Con::warnf("getDataField: object '%s' not found.", getString().c_str());
      ((ConsoleValuePtr&)outValue).setNull();
   }
   
   return true;
}

void ConsoleReferenceCountedType::setDataField(StringTableEntry slotName, const ConsoleValuePtr &array, const ConsoleValuePtr &newValue)
{
   SimObject* obj = Sim::findObject<SimObject>(getString().c_str());
   if (obj)
   {
      // TODO: directly pass through values
      obj->setDataField(slotName, array.getTempStringValue(), newValue);
   }
   else
   {
      Con::warnf("setDataField: object '%s' not found.", getString().c_str());
   }
}

Namespace* ConsoleReferenceCountedType::getNamespace()
{
   SimObject* obj = Sim::findObject<SimObject>(getString().c_str());
   if (obj)
   {
      // TODO: directly pass through values
      return obj->getNamespace();
   }
   else
   {
      Con::warnf("getNamespace: NS '%s' not found.", getString().c_str());
      return NULL;
   }
   //return Namespace::find(StringTable->insert(getString().c_str()));
}

ConsoleStringValue* ConsoleStringValue::fromInt(S64 num)
{
    char buffer[255];
    dSprintf(buffer, sizeof(buffer), "%lu", num);
    ConsoleStringValue* value = new ConsoleStringValue();
    value->setString(buffer);
    return value;
}

ConsoleStringValue* ConsoleStringValue::fromFloat(F32 num)
{
    char buffer[255];
    dSprintf(buffer, sizeof(buffer), "%.5g", num);
    ConsoleStringValue* value = new ConsoleStringValue();
    value->setString(buffer);
    return value;
}

ConsoleStringValue* ConsoleStringValue::fromString(const char *string)
{
    ConsoleStringValue* value = new ConsoleStringValue();
    if (string) value->setString(string);
    return value;
}

ConsoleStringValue* ConsoleStringValue::fromSimpleString(SimpleString* string)
{
   ConsoleStringValue* value = new ConsoleStringValue();
   value->setString(string->getString());
   return value;
}

void ConsoleBaseType::setData(void *dataPtr, const char* value, EnumTable *tbl)
{
   if (smTypeFromConsoleValueFunc)
   {
      ConsoleValuePtr vvalue;
      vvalue.setValue(ConsoleStringValue::fromString(value)); // TOFIX optimize
      smTypeFromConsoleValueFunc(dataPtr, vvalue, tbl);
   }
}

ConsoleStringValuePtr ConsoleBaseType::getData(void *dataPtr, EnumTable *tbl)
{
   if (smTypeToStringFunc)
   {
      return smTypeToStringFunc(dataPtr, tbl);
   }
   else
   {
      return "";
   }
}

void ConsoleBaseType::setDataFromValue(void *dataPtr, const ConsoleValuePtr &value, EnumTable *tbl)
{
   if (smTypeToConsoleValueFunc)
   {
      smTypeToConsoleValueFunc(dataPtr, tbl);
   }
   else
   {
      setData(dataPtr, value.getTempStringValue(), tbl);
   }
}

ConsoleValuePtr ConsoleBaseType::getDataValue(void *dataPtr, EnumTable *tbl)
{
   if (smTypeToConsoleValueFunc)
   {
      return smTypeToConsoleValueFunc(dataPtr, tbl);
   }
   else
   {
      ConsoleValuePtr outValue;
      ConsoleStringValuePtr outString = getData(dataPtr, tbl);
      outValue.setValue(outString.value);
      return outValue;
   }
}


void ConsoleArrayValue::clear()
{
   for (U32 i=0; i<mValues.size(); i++)
   {
      mValues[i].setNull();
   }
}

S32 ConsoleArrayValue::getInternalType()
{
   ConsoleBaseType* type = getType();
   return type ? ConsoleValue::TypeCustomFieldStart + type->getTypeID() : ConsoleValue::TypeCustomFieldStart;
}

void ConsoleArrayValue::read(Stream &s, ConsoleSerializationState &state)
{
   ConsoleValuePtr::readStack(s, state, mValues);
}

void ConsoleArrayValue::write(Stream &s, ConsoleSerializationState &state)
{
   ConsoleValuePtr::writeStack(s, state, mValues);
}

bool ConsoleArrayValue::getDataField(StringTableEntry slotName, const ConsoleValuePtr &array, ConsoleValuePtr &outValue)
{
   if (array.type != ConsoleValue::TypeInternalNull && slotName == NULL)
   {
      // Grab index
      S32 index = (S32)array.getSignedIntValue();
      
      if (index >= 0 && index < mValues.size())
      {
         ((ConsoleValuePtr*)&outValue)->setValue(mValues[index]);
      }
      else
      {
         Con::warnf("Invalid array index %u", index);
         ((ConsoleValuePtr*)&outValue)->setNull();
         return false;
      }
   }
   else
   {
      Con::warnf("Invalid array index");
      ((ConsoleValuePtr&)outValue).setNull();
      return false;
   }
   
   return true;
}

void ConsoleArrayValue::setDataField(StringTableEntry slotName, const ConsoleValuePtr &array, const ConsoleValuePtr &newValue)
{
   if (array.type != ConsoleValue::TypeInternalNull && slotName == NULL)
   {
      // Grab index
      S32 index = (S32)array.getSignedIntValue();
      
      if (index >= 0)
      {
         // Make sure we can accomodate this index
         if (index >= mValues.size())
         {
            U32 start = mValues.size();
            mValues.setSize(index+1);
            dMemset(mValues.address() + start, 0, ((index+1)-start) * sizeof(ConsoleValuePtr)); // this will set them to NULL
         }
         mValues[index].setValue(newValue);
      }
      else
      {
         Con::warnf("Invalid array index %u", index);
      }
   }
   else
   {
      Con::warnf("Invalid array index");
   }
}

Namespace* ConsoleArrayValue::getNamespace()
{
   return NULL;
}

S32 ConsoleArrayValue::getIteratorLength()
{
   return mValues.size();
}

bool ConsoleArrayValue::advanceIterator(ConsoleValuePtr &iterator, ConsoleValuePtr &iteratorValue)
{
   S32 size = mValues.size();
   S32 iterValue = (S32)iterator.getIntValue()-1;
   if (iterValue < 0 || iterValue >= size)
   {
      ((ConsoleValuePtr&)iterator).setNull();
   }
   else
   {
      ((ConsoleValuePtr&)iteratorValue).setValue(mValues[iterValue]);
      ((ConsoleValuePtr&)iterator).setValue(iterValue+1);
   }
}

ConsoleStringValuePtr ConsoleArrayValue::getString()
{
   SimpleString str;
   
   for (U32 i=0; i<mValues.size(); i++)
   {
      if (i > 0)
         str.append(" ");
      
      str.append(mValues[i].getTempStringValue());
   }
   
   return ConsoleStringValuePtr(&str);
}

bool ConsoleArrayValue::stringCompare(const char* other)
{
   ConsoleStringValuePtr str = getString();
   return dStricmp(str.c_str(), other) == 0;
}

bool ConsoleArrayValue::refCompare(ConsoleReferenceCountedType* other)
{
   return this == other;
}

ConsoleArrayValue* ConsoleArrayValue::fromValues(int argc, ConsoleValue* argv)
{
   ConsoleArrayValue* values = new ConsoleArrayValue();
   values->mValues.setSize(argc);
   if (argc > 0)
   {
      dMemset(values->mValues.address(), '\0', sizeof(ConsoleValuePtr) * argc);
   }
   
   for (U32 i=0; i<argc; i++)
   {
      values->mValues[i].setValue(argv[i]);
   }
   
   return values;
}

void ConsoleTypeConversionMethodConstructor::set()
{
   
}

