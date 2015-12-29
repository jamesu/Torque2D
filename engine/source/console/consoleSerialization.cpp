#include "platform/platform.h"
#include "console/console.h"
#include "console/consoleSerialization.h"

#include "console/consoleDictionary.h"
#include "console/consoleNamespace.h"

#include "console/ast.h"
#include "collection/findIterator.h"
#include "io/resource/resourceManager.h"

#include "string/findMatch.h"
#include "io/fileStream.h"
#include "console/compiler.h"
#include "string/stringStack.h"

#include "sim/simBase.h"

S32 ConsoleSerializationState::getSavedObjectIdx(ConsoleReferenceCountedType* object)
{
   HashTable<void*, U32>::iterator itr = writtenValues.find(object);
   if (itr != writtenValues.end())
   {
      return itr->value;
   }
   
   return -1;
}

void ConsoleSerializationState::addSavedObject(ConsoleReferenceCountedType* object)
{
   writtenValues.insertUnique(object, savedObjectCount);
   savedObjectCount++;
}


SimObject* ConsoleSerializationState::getSavedConsoleObject(S32 objectId)
{
   HashTable<U32, SimObject*>::iterator itr = simObjectsById.find(objectId);
   if (itr != simObjectsById.end())
      return itr->value;
   return NULL;
}

S32 ConsoleSerializationState::getSavedConsoleObjectIDX(SimObject* object)
{
   HashTable<SimObject*, U32>::iterator itr = simObjectsByObject.find(object);
   if (itr != simObjectsByObject.end())
      return itr->value;
   return -1;
}

void ConsoleSerializationState::addSavedConsoleObject(SimObject* object)
{
   simObjectsByObject.insertUnique(object, savedSimObjectCount);
   simObjectsById.insertUnique(savedSimObjectCount, object);
   savedSimObjectCount++;
}

void ConsoleSerializationState::addSimObjects(SimGroup* group)
{
   simObjectsByObject.insertUnique(group, savedSimObjectCount);
   simObjectsById.insertUnique(savedSimObjectCount, group);
   savedSimObjectCount++;
   
   for (SimGroup::iterator itr = group->begin(); itr != group->end(); itr++)
   {
      simObjectsByObject.insertUnique(*itr, savedSimObjectCount);
      simObjectsById.insertUnique(savedSimObjectCount, *itr);
      savedSimObjectCount++;
      
      SimGroup* group = dynamic_cast<SimGroup*>(*itr);
      if (group)
      {
         addSimObjects(group);
      }
   }
}


CodeBlock* ConsoleSerializationState::getSavedCodeblock(S32 objectId)
{
   if (objectId >= 0 && objectId < loadedCodeblocks.size())
   {
      return loadedCodeblocks[objectId];
   }

   return NULL;
}

S32 ConsoleSerializationState::getSavedCodeblockIdx(CodeBlock* block)
{
   for (S32 i=0, sz=loadedCodeblocks.size(); i<sz; i++)
   {
      if (loadedCodeblocks[i] == block)
         return i;
   }
   return -1;
}

S32 ConsoleSerializationState::addReferencedCodeblock(CodeBlock* block)
{
   S32 existingId = getSavedCodeblockIdx(block);
   if (existingId == -1 && block)
   {
      loadedCodeblocks.push_back(block);
      return loadedCodeblocks.size()-1;
   }
   return existingId;
}

