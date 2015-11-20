//
//  consoleSerialization.h
//  Torque2D
//
//  Created by James Urquhart on 05/11/2015.
//

#ifndef _CONSOLESERIALIZATION_H_
#define _CONSOLESERIALIZATION_H_

class SimGroup;

struct ConsoleSerializationState
{
   /// Written reference values
   HashTable<void*, U32> writtenValues;
   
   /// Lookup of loaded values
   Vector<ConsoleValue*> loadedValues;
   
   HashTable<U32, SimObject*> simObjectsById;
   HashTable<SimObject*, U32> simObjectsByObject;
   
   U32 savedObjectCount;
   U32 savedSimObjectCount;
   
   S32 getSavedObjectIdx(ConsoleReferenceCountedType* object);
   void addSavedObject(ConsoleReferenceCountedType* object);
   
   SimObject* getSavedConsoleObject(S32 objectId);
   S32 getSavedConsoleObjectIDX(SimObject* object);
   void addSavedConsoleObject(SimObject* object);
   void addSimObjects(SimGroup* group);
   
   ConsoleSerializationState() : savedObjectCount(0)
   {
      
   }
   
   void clear()
   {
      savedObjectCount = 0;
      savedSimObjectCount = 0;
      writtenValues.clear();
      loadedValues.clear();
      simObjectsById.clear();
      simObjectsByObject.clear();
   }
};

#endif
