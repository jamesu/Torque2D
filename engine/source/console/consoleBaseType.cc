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

#include "console/consoleBaseType.h"
#include "console/consoleDictionary.h"
#include "console/consoleNamespace.h"
#include "console/consoleTypes.h"

// Init the globals.
ConsoleBaseType *ConsoleBaseType::smListHead = NULL;
S32              ConsoleBaseType::smConsoleTypeCount = 8; // This makes 0 an invalid console type.

// And, we also privately store the types lookup table.
VectorPtr<ConsoleBaseType*> gConsoleTypeTable;

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

/*
 
 IDEA
 
 struct CallableNSMethod
 {
    Namespace *ns;
    StringTableEntry key;
 };
 
 (internal type)
 
 */

void cbtDefaultOperatorFunc(ConsoleValuePtr *inOutParam, ConsoleValuePtr *inParam, ConsoleValuePtr *inSubParam)
{
    // First find object
    SimObject *obj = NULL;
    if (Sim::findObject(inParam->getStringValue(), obj))
    {
        Namespace *ns = obj->getNamespace();
        Namespace::Entry *entry = ns->lookup(StringTable->insert(inSubParam->getStringValue()));
        if (entry)
        {
            //inOutParam->setNSEValue(entry);
        }
        else
        {
            Con::errorf("Couldn't find entry %s!", inSubParam->getStringValue());
        }
        //= inParam->getStringValue();
    }
    else
    {
        Con::errorf("Couldn't find object %s", inParam->getStringValue());
    }
}

void cbtDefaultOperatorProperty(ConsoleValuePtr *inOutParam, ConsoleValuePtr *inParam, ConsoleValuePtr *inSubParam)
{
    // First find object
    SimObject *obj = NULL;
    if (Sim::findObject(inParam->getStringValue(), obj))
    {
    }
    else
    {
        Con::errorf("Couldn't find object %s", inParam->getStringValue());
    }
}

void cbtDefaultOperatorPropertySet(ConsoleValuePtr *inOutParam, ConsoleValuePtr *inParam, ConsoleValuePtr *inSubParam)
{
    // First find object
    SimObject *obj = NULL;
    if (Sim::findObject(inOutParam->getStringValue(), obj))
    {
    }
    else
    {
        Con::errorf("Couldn't find object %s", inOutParam->getStringValue());
    }
}

void cbtDefaultOperatorSubObject(ConsoleValuePtr *inOutParam, ConsoleValuePtr *inParam, ConsoleValuePtr *inSubParam)
{
    // First find object
    SimGroup *obj = NULL;
    if (Sim::findObject(inParam->getStringValue(), obj))
    {
        //SimObject *obj = obj->findObject(inParam->getStringValue());
    }
    else
    {
        Con::errorf("Couldn't find object %s", inParam->getStringValue());
    }
}

void cbtDefaultOperatorIndex(ConsoleValuePtr *inOutParam, ConsoleValuePtr *inParam, ConsoleValuePtr *inSubParam)
{
    // First find object
    SimGroup *obj = NULL;
    if (Sim::findObject(inParam->getStringValue(), obj))
    {
    }
    else
    {
        Con::errorf("Couldn't find object %s", inParam->getStringValue());
    }
}

void cbtDefaultOperatorIndexSet(ConsoleValuePtr *inOutParam, ConsoleValuePtr *inParam, ConsoleValuePtr *inSubParam)
{
    // First find object
    SimGroup *obj = NULL;
    if (Sim::findObject(inOutParam->getStringValue(), obj))
    {
    }
    else
    {
        Con::errorf("Couldn't find object %s", inOutParam->getStringValue());
    }
}

typedef void (*ConsoleTypeOperatorFunc)(ConsoleValuePtr *inOutParam, ConsoleValuePtr *inParam, ConsoleValuePtr *inSubParam);


//-------------------------------------------------------------------------

ConsoleBaseType::ConsoleBaseType(const S32 size, S32 *idPtr, const char *aTypeName) :
mMetaFunc(0),
mMetaProperty(0),
mMetaPropertySet(0),
mMetaSubObject(0),
mMetaIndex(0),
mMetaIndexSet(0)
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


ConsoleStringValue* ConsoleStringValue::fromInt(U32 num)
{
    char buffer[255];
    dSprintf(buffer, sizeof(buffer), "%u", num);
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
    value->setString(string);
    return value;
}
