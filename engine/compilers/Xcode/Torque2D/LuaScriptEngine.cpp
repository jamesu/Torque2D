//
// Copyright (c) 2014 James S Urquhart. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//


#include "ScriptEngine.h"
#include "string/stringTable.h"
#include "sim/simBase.h"
#include "console/consoleNamespace.h"
#include "io/resource/resourceManager.h"

#include "console/consoleInternal.h"
#include "console/consoleDictionary.h"
extern ExprEvalState gEvalState;

// Lua includes
extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}


class LuaScriptStack : public ScriptStack
{
public:
   LuaScriptStack()
   {
   }
   
   virtual ~LuaScriptStack()
   {
      
   }
   
   // Push values
   virtual int pushNull()
   {
      lua_pushnil(mLua);
      return lua_gettop(mLua);
   }
   
   virtual int pushIndex(int value)
   {
      lua_pushvalue(mLua, value);
      return lua_gettop(mLua);
   }
   
   virtual int pushInteger(U32 value)
   {
      lua_pushunsigned(mLua, value);
      return lua_gettop(mLua);
   }
   
   virtual int pushSignedInteger(S32 value)
   {
      lua_pushinteger(mLua, value);
      return lua_gettop(mLua);
   }
   
   virtual int pushNumber(F32 value)
   {
      lua_pushnumber(mLua, value);
      return lua_gettop(mLua);
   }
   
   virtual int pushBool(bool value)
   {
      lua_pushboolean(mLua, value);
      return lua_gettop(mLua);
   }
   
   virtual int pushSimFields(SimFieldDictionary *values)
   {
      // basically: make a table
      lua_newtable(mLua);
      int table = lua_gettop(mLua);
      
      for(SimFieldDictionaryIterator ditr(values); *ditr; ++ditr)
      {
         SimFieldDictionary::Entry * entry = (*ditr);
         lua_pushstring(mLua, entry->slotName);
         lua_pushstring(mLua, entry->value);
         lua_settable(mLua, table);
      }
      
      return lua_gettop(mLua);
   }
   
   virtual int pushString(const char *value)
   {
      lua_pushstring(mLua, value);
      return lua_gettop(mLua);
   }
   
   virtual int pushSimObject(SimObject *value)
   {
      // basically: lookup this SimObject in the instance table
      lua_pushglobaltable(mLua);
      int table = lua_gettop(mLua);
      lua_pushstring(mLua, "InstanceTable");
      lua_gettable(mLua, table);
      
      // Get Object from InstanceTable
      lua_pushinteger(mLua, value->getId());
      lua_gettable(mLua, -2);
      
      // Return object
      lua_replace(mLua, table);
      lua_settop(mLua, table);
      return table;
   }
   
   
   // Get values
   virtual const char* getString(int index)
   {
      return lua_tostring(mLua, index);
   }
   
   virtual U32 getInteger(int index)
   {
      return lua_tounsigned(mLua, index);
   }
   
   virtual S32 getSignedInteger(int index)
   {
      return lua_tointeger(mLua, index);
   }
   
   virtual F32 getNumber(int index)
   {
      return lua_tonumber(mLua, index);
   }
   
   virtual void getSimFields(int index, SimFieldDictionary *outValues)
   {
      lua_pushvalue(mLua, index);
      int table = lua_gettop(mLua);
      lua_pushnil(mLua);  /* first key */
      while (lua_next(mLua, table) != 0) {
         /* uses 'key' (at index -2) and 'value' (at index -1) */
         printf("%s - %s\n",
                lua_typename(mLua, lua_type(mLua, -2)),
                lua_typename(mLua, lua_type(mLua, -1)));
         /* removes 'value'; keeps 'key' for next iteration */
         outValues->setFieldValue(StringTable->insert(lua_tostring(mLua, -2)), lua_tostring(mLua, -1));
         lua_pop(mLua, 1);
      }
      lua_pop(mLua, 1);
   }
   
   virtual bool getBool(int index)
   {
      return lua_toboolean(mLua, index);
   }
   
   virtual ScriptValueBaseType getTypeAtIndex(int index)
   {
      int tid = lua_type(mLua, index);
      
      switch (tid)
      {
         case LUA_TNIL:
            return SCRIPTTYPE_NULL;
         case LUA_TNUMBER:
            return SCRIPTTYPE_NUMBER;
         case LUA_TBOOLEAN:
            return SCRIPTTYPE_BOOL;
         case LUA_TSTRING:
            return SCRIPTTYPE_STRING;
         case LUA_TTABLE:
            return SCRIPTTYPE_TABLE;
         case LUA_TFUNCTION:
            return SCRIPTTYPE_NULL;
         case LUA_TUSERDATA:
            return SCRIPTTYPE_OBJECT;
         default:
            return SCRIPTTYPE_NULL;
      }
   }
   
   
   // Set values
   virtual void setString(int index, const char *value)
   {
      lua_pushstring(mLua, value);
      lua_replace(mLua, index);
   }
   
   virtual void setInteger(int index, U32 value)
   {
      lua_pushunsigned(mLua, value);
      lua_replace(mLua, index);
   }
   
   virtual void setSignedInteger(int index, S32 value)
   {
      lua_pushinteger(mLua, value);
      lua_replace(mLua, index);
   }
   
   virtual void setNumber(int index, F32 value)
   {
      lua_pushnumber(mLua, value);
      lua_replace(mLua, index);
   }
   
   virtual void setSimFields(int index, SimFieldDictionary *values)
   {
      lua_pushvalue(mLua, index);
      int table = lua_gettop(mLua);
      
      for(SimFieldDictionaryIterator ditr(values); *ditr; ++ditr)
      {
         SimFieldDictionary::Entry * entry = (*ditr);
         lua_pushstring(mLua, entry->slotName);
         lua_pushstring(mLua, entry->value);
         lua_settable(mLua, table);
      }
      
      lua_pop(mLua, 1);
   }
   
   virtual void setSimObject(int index, SimObject *value)
   {
      // basically: lookup this SimObject in the instance table
      lua_pushglobaltable(mLua);
      int table = lua_gettop(mLua);
      lua_pushstring(mLua, "InstanceTable");
      lua_gettable(mLua, -1);
      
      // Get Object from InstanceTable
      lua_pushinteger(mLua, value->getId());
      lua_gettable(mLua, table+1);
      
      // Replace old index with new object
      lua_replace(mLua, index);
      lua_settop(mLua, table-1);
   }
   
   void setBool(int index, bool value)
   {
      lua_pushboolean(mLua, value);
      lua_replace(mLua, index);
   }
   
public:
   lua_State *mLua;
};

class ScriptStackValueRef;
class LuaScriptEngine;

extern ScriptEngine *sScriptInstance;


// taken from lua code
static int typeerror (lua_State *L, int narg, const char *tname) {
   const char *msg = lua_pushfstring(L, "%s expected, got %s",
                                     tname, luaL_typename(L, narg));
   return luaL_argerror(L, narg, msg);
}


class LuaScriptEngine : public ScriptEngine
{
public:
   
   lua_State *mLuaState;
   LuaScriptStack mLuaStack;
   
   bool mRootExec; // flag to determine if an exec call is the root
   bool mShouldReset; // flag to determine if next exec call should reset the stack
   
   LuaScriptEngine()
   {
      mLuaState = luaL_newstate();
      mLuaStack.mLua = mLuaState;
      sScriptInstance = this;
      mRootExec = false;
      mShouldReset = false;
      
      luaL_openlibs(mLuaState);
      
      //lua_debug_print_stack(mLuaState);
      
      // Need to setup a few things in the script engine first!
      lua_newtable(mLuaState);
      lua_setglobal(mLuaState, "InstanceTable");
      
      // Globals table
      lua_newtable(mLuaState);
      int globalsTable = lua_gettop(mLuaState);
      
      lua_newtable(mLuaState);
      int globalsMetaTable = globalsTable+1;
      
      lua_pushstring(mLuaState, "__index");
      lua_pushlightuserdata(mLuaState, &(gEvalState.globalVars));
      lua_pushcclosure(mLuaState, &getGlobalVariable, 1);
      lua_settable(mLuaState, globalsMetaTable);
      
      lua_pushstring(mLuaState, "__newindex");
      lua_pushlightuserdata(mLuaState, &(gEvalState.globalVars));
      lua_pushcclosure(mLuaState, &setGlobalVariable, 1);
      lua_settable(mLuaState, globalsMetaTable);
      
      lua_setmetatable(mLuaState, globalsTable);
      
      lua_setglobal(mLuaState, "TorqueVars");
      
      //lua_debug_print_stack(mLuaState);
   }
   
   virtual ~LuaScriptEngine()
   {
      lua_close(mLuaState);
   }
   
   static int getGlobalVariable(lua_State *L)
   {
      Dictionary *globals = (Dictionary*)lua_touserdata(L, lua_upvalueindex(1));
      
      bool valid = false;
      const char *var = globals->getVariable(StringTable->insert(lua_tostring(L, -1)), &valid);
      if (valid)
      {
         lua_pushstring(L, var);
      }
      else
      {
         lua_pushnil(L);
      }
      
      return 1;
   }
   
   static int setGlobalVariable(lua_State *L)
   {
      Dictionary *globals = (Dictionary*)lua_touserdata(L, lua_upvalueindex(1));
      globals->setVariable(StringTable->insert(lua_tostring(L, -2)), lua_tostring(L, -1));
      return 0;
   }
   
   static void setObjectFieldArray(lua_State *L, int table, StringTableEntry fieldName, SimObject *obj)
   {
      lua_pushnil(L);
      while (lua_next(L, table) != 0) {
         /* uses 'key' (at index -2) and 'value' (at index -1) */
         printf("%s - %s\n",
                lua_typename(L, lua_type(L, -2)),
                lua_typename(L, lua_type(L, -1)));
         
         // Use current index
         char idxBuf[16];
         dSprintf(idxBuf, 16, "%u", dAtoi(lua_tostring(L, -2))+1);
         obj->setDataField(fieldName, idxBuf, lua_tostring(L, -1));
         lua_pop(L, 1);
      }
      lua_pop(L, 1);
   }
   
   // Constructs an instance of an object in lua
   static int makeObject(lua_State *L)
   {
      StringTableEntry className = StringTable->insert(lua_tostring(L, lua_upvalueindex(1)));
      ConsoleObject *conObject = ConsoleObject::create(className);
      SimObject *instance = dynamic_cast<SimObject*>(conObject);
      SimDataBlock *dataBlock = dynamic_cast<SimDataBlock *>(instance);
      SimGroup *grp = NULL;
      
      if (!instance)
      {
         if (conObject)
            delete conObject;
         
         Con::errorf("Could not create object of type %s!", className);
         lua_pushnil(L);
         return 1;
      }
      
      // Second parameter: fields to set
      int param = lua_gettop(L);
      int startArgs = 2;
      if (param > 1)
      {
         int table = 1;
         
         // Is first parameter a class name?
         if (lua_isstring(L, 1))
         {
            table++;
            instance->assignName(lua_tostring(L, 1));
            startArgs++;
         }
         
         // Check table parameter for initial field values
         if (param >= table && lua_istable(L, table))
         {
            lua_pushnil(L);  /* first key */
            while (lua_next(L, table) != 0) {
               /* uses 'key' (at index -2) and 'value' (at index -1) */
               printf("%s - %s\n",
                      lua_typename(L, lua_type(L, -2)),
                      lua_typename(L, lua_type(L, -1)));
               
               if (lua_istable(L, -1))
               {
                  // If this field is a table, we need to be a bit more clever...
                  setObjectFieldArray(L, -1, StringTable->insert(lua_tostring(L, -2)), instance);
               }
               else
               {
                  // Just set the field the dumb way
                  instance->setDataField(StringTable->insert(lua_tostring(L, -2)), NULL, lua_tostring(L, -1));
               }
               lua_pop(L, 1);
            }
         }
      }
      
      // Do the constructor parameters.
      const char* fakeArgs[3] = {NULL, NULL, NULL};
      if(!instance->processArguments(0, fakeArgs))
      {
         delete instance;
         Con::errorf("Could not create object of type %s!", className);
         lua_pushnil(L);
         return 1;
      }
      
      // If it's not a datablock, allow people to modify bits of it.
      if(dataBlock == NULL)
      {
         instance->setModStaticFields(true);
         instance->setModDynamicFields(true);
      }
      
      int rest = lua_gettop(L);
      if (instance->registerObject())
      {
         // Handle RootGroup & instantGroup
         if(!instance->getGroup())
         {
            // Deal with the instantGroup if we're being put at the root or we're adding to a component.
            const char *addGroupName = Con::getVariable("instantGroup");
            if(!Sim::findObject(addGroupName, grp))
               Sim::findObject(RootGroupId, grp);
            
            // If we didn't get a group, then make sure we have a pointer to
            // the rootgroup.
            if(!grp)
               Sim::findObject(RootGroupId, grp);
            
            // add to the parent group
            grp->addObject(instance);
         }
         
         // Are we dealing with a datablock?
         static char errorBuffer[256];
         
         // If so, preload it.
         if(dataBlock && !dataBlock->preload(true, errorBuffer))
         {
            Con::errorf(ConsoleLogEntry::General, "%s: preload failed for %s: %s.", "",
                        instance->getName(), errorBuffer);
            dataBlock->deleteObject();
            lua_pushnil(L);
            return 1;
         }
         
         // Grab instance from InstanceTable
         lua_pushglobaltable(L);
         int table = lua_gettop(L);
         lua_pushstring(L, "InstanceTable");
         lua_gettable(L, -2);
         lua_pushinteger(L, instance->getId());
         lua_gettable(L, -2);
         
         return 1;
      }
      else
      {
         // This error is usually caused by failing to call Parent::initPersistFields in the class' initPersistFields().
         Con::warnf(ConsoleLogEntry::General, "Register object failed for object %s of class %s.", instance->getName(), instance->getClassName());
         delete instance;
         
         lua_pushnil(L);
         return 1;
      }
   }
   
   static int gcObject(lua_State *L)
   {
      // NOTE: we don't garbage collect since the lifetime of the SimObject is controlled by deleteObject.
      // TODO: handle case where deleteObject is called but lua still retains a reference to the object.
      return 0;
      /*
       int ud = lua_gettop(L);
       const char *className = lua_tostring(L, lua_upvalueindex(1));
       SimObject** obj = static_cast<SimObject**>(lua_touserdata(L, ud));
       if (obj != NULL)
       {
       if (lua_getmetatable(L, ud))
       {
       luaL_getmetatable(L, className);
       if (!lua_rawequal(L, -1, -2))  // not the same?
       obj = NULL;
       lua_pop(L, 2);
       }
       else
       {
       obj = NULL;
       }
       }
       
       if (obj == NULL)
       {
       typeerror(L, ud, className);
       return 0;
       }
       else
       {
       (*obj)->unregisterObject();
       }*/
      return 0;
   }
   
   static int getObjectField(lua_State *L)
   {
      int ud = lua_gettop(L)-1;
      const char *className = lua_tostring(L, lua_upvalueindex(1));
#ifdef LUA_USE_DIRECT_SIMOBJECT_PTR
      SimObject** objPtr = static_cast<SimObject**>(lua_touserdata(L, ud));
      SimObject *obj = objPtr ? *objPtr : NULL;
#else
      SimObjectId* objPtr = static_cast<SimObjectId*>(lua_touserdata(L, ud));
      SimObject *obj = objPtr ? Sim::findObject(*objPtr) : NULL;
#endif
      if (obj != NULL)
      {
         if (lua_getmetatable(L, ud))
         {
            luaL_getmetatable(L, className);
            if (!lua_rawequal(L, -1, -2))  // not the same?
               obj = NULL;
            lua_pop(L, 2);
         }
         else
         {
            obj = NULL;
         }
      }
      
      if (obj == NULL)
      {
         typeerror(L, ud, className);
         lua_pushnil(L);
         return 1;
      }
      else
      {
         // Grab the field name
         luaL_getmetatable(L, className);
         int meta = lua_gettop(L);
         
         // First, see if klassMetaTable has this...
         lua_pushstring(L, "__mthd");
         lua_rawget(L, meta);       // __mthd
         const char *typeOfTop = lua_typename(L, lua_type(L, -1));
         lua_pushvalue(L, meta-1);
         lua_gettable(L, -2);       // fieldName
         
         //lua_debug_print_stack(L);
         
         typeOfTop = lua_typename(L, lua_type(L, -1));
         
         // Grab the field from the object
         if (lua_isuserdata(L, -1))
         {
            StringTableEntry ste = (StringTableEntry)lua_touserdata(L, -1);
            const char *data = obj->getDataField(ste, NULL);
            if (data)
               lua_pushstring(L, data);
            else
               lua_pushnil(L);
         }
         else if (lua_isnil(L, -1))
         {
            // nil? check user fields
            StringTableEntry ste = StringTable->insert(lua_tostring(L, meta-1));
            const char *data = obj->getDataField(ste, NULL);
            if (data)
               lua_pushstring(L, data);
            else
               lua_pushnil(L);
         }
      }
      
      // Otherwise could be anything, just return it.
      return 1;
   }
   
   static int setObjectField(lua_State *L)
   {
      int ud = lua_gettop(L)-2;
      int value = lua_gettop(L);
      const char *className = lua_tostring(L, lua_upvalueindex(1));
#ifdef LUA_USE_DIRECT_SIMOBJECT_PTR
      SimObject** objPtr = static_cast<SimObject**>(lua_touserdata(L, ud));
      SimObject *obj = objPtr ? *objPtr : NULL;
#else
      SimObjectId* objPtr = static_cast<SimObjectId*>(lua_touserdata(L, ud));
      SimObject *obj = objPtr ? Sim::findObject(*objPtr) : NULL;
#endif
      if (obj != NULL)
      {
         if (lua_getmetatable(L, ud))
         {
            luaL_getmetatable(L, className);
            if (!lua_rawequal(L, -1, -2))  // not the same?
               obj = NULL;
            lua_pop(L, 2);
         }
         else
         {
            obj = NULL;
         }
      }
      
      if (obj == NULL)
      {
         typeerror(L, ud, className);
         return 0;
      }
      else
      {
         // Grab the field name
         luaL_getmetatable(L, className);
         lua_pushvalue(L, -3);
         const char *fieldName = lua_tostring(L, -1);
         lua_rawget(L, -2);
         
         // Grab the field from the object
         if (lua_isuserdata(L, -1))
         {
            StringTableEntry ste = (StringTableEntry)lua_touserdata(L, -1);
            
            // Determine what type the incoming data is
            if (lua_istable(L, value))
            {
               // Table? Gotta set the array!
               setObjectFieldArray(L, value, ste, obj);
            }
            else
            {
               // Something else? just set it for now
               obj->setDataField(ste, NULL, lua_tostring(L, value));
            }
         }
         else if (lua_isnil(L, -1))
         {
            StringTableEntry ste = StringTable->insert(fieldName);
            
            // Determine what type the incoming data is
            if (lua_istable(L, value))
            {
               // Table? Gotta set the array!
               setObjectFieldArray(L, value, ste, obj);
            }
            else
            {
               // Something else? just set it for now
               obj->setDataField(ste, NULL, lua_tostring(L, value));
            }
         }
      }
      
      return 0;
   }
   
#define MAX_THUNKARGS 32
   
   static int thunkDummy(lua_State *L)
   {
      return 0;
   }
   
   static inline void* thunkCheckAndReturnObject(lua_State *L, Namespace *requiredNS)
   {
#ifdef LUA_USE_DIRECT_SIMOBJECT_PTR
      SimObject** objPtr = static_cast<SimObject**>(lua_touserdata(L, 1));
      SimObject *obj = objPtr ? *objPtr : NULL;
#else
      SimObjectId* objPtr = static_cast<SimObjectId*>(lua_touserdata(L, 1));
      SimObject* obj = objPtr ? Sim::findObject(*objPtr) : NULL;
#endif
      
      if (obj != NULL)
      {
         if (lua_getmetatable(L, 1))
         {
            lua_pushstring(L, "__ns");
            lua_rawget(L, -2);
            
            Namespace *objNS = (Namespace*)lua_touserdata(L, -1);
            if (objNS)
            {
               SimObject *oldObj = obj;
               obj = NULL;
               for (Namespace *itr = objNS; itr != NULL; itr = itr->mParent)
               {
                  if (itr == requiredNS)
                  {
                     // Found the NS, the object is ok!
                     obj = oldObj;
                     break;
                  }
               }
            }
            else
            {
               obj = NULL;
            }
            
            lua_pop(L, 1);
            
            if (obj != NULL)
            {
               return obj;
            }
         }
         else
         {
            obj = NULL;
         }
      }
      
      return NULL;
   }
   
   static int thunkString(lua_State *L)
   {
      int args = lua_gettop(L);
      Namespace *ns = (Namespace*)lua_touserdata(L, lua_upvalueindex(5));
      SimObject *obj = (SimObject*)thunkCheckAndReturnObject(L, ns);
      
      if (obj == NULL)
      {
         typeerror(L, 1, ns->mName);
         return 0;
      }
      
      int minArgs = lua_tointeger(L, lua_upvalueindex(1));
      int maxArgs = lua_tointeger(L, lua_upvalueindex(2));
      const char *usage = lua_tostring(L, lua_upvalueindex(3));
      StringTableEntry fnName = (StringTableEntry)lua_touserdata(L, lua_upvalueindex(4));
      
      if (args+1 < minArgs || args+1 > maxArgs)
      {
         Con::warnf(ConsoleLogEntry::Script, "%s::%s - wrong number of arguments.", ns->mName, fnName);
         Con::warnf(ConsoleLogEntry::Script, "usage: %s", usage);
         return 0;
      }
      
      // We have enough args, prepare the list
      const char *thunkValues[MAX_THUNKARGS];
      
      // Set function name and %this
      thunkValues[0] = fnName;
      thunkValues[1] = obj->getIdString();
      
      // Populate incomming args
      for (int i=0; i<args; i++)
      {
         thunkValues[i+2] = lua_tostring(L, i+2);
      }
      
      StringCallback func = (StringCallback)lua_touserdata(L, lua_upvalueindex(6));
      if (func)
      {
         lua_pushstring(L, func(obj, args+1, thunkValues));
      }
      else
      {
         lua_pushstring(L, "");
      }
      return 1;
   }
   
   static int thunkInt(lua_State *L)
   {
      int args = lua_gettop(L);
      Namespace *ns = (Namespace*)lua_touserdata(L, lua_upvalueindex(5));
      SimObject *obj = (SimObject*)thunkCheckAndReturnObject(L, ns);
      
      if (obj == NULL)
      {
         typeerror(L, 1, ns->mName);
         return 0;
      }
      
      int minArgs = lua_tointeger(L, lua_upvalueindex(1));
      int maxArgs = lua_tointeger(L, lua_upvalueindex(2));
      const char *usage = lua_tostring(L, lua_upvalueindex(3));
      StringTableEntry fnName = (StringTableEntry)lua_touserdata(L, lua_upvalueindex(4));
      
      if (args+1 < minArgs || args+1 > maxArgs)
      {
         Con::warnf(ConsoleLogEntry::Script, "%s::%s - wrong number of arguments.", ns->mName, fnName);
         Con::warnf(ConsoleLogEntry::Script, "usage: %s", usage);
         return 0;
      }
      
      // We have enough args, prepare the list
      const char *thunkValues[MAX_THUNKARGS];
      
      // Set function name and %this
      thunkValues[0] = fnName;
      thunkValues[1] = obj->getIdString();
      
      // Populate incomming args
      for (int i=0; i<args-1; i++)
      {
         thunkValues[i+2] = lua_tostring(L, i+2);
      }
      
      IntCallback func = (IntCallback)lua_touserdata(L, lua_upvalueindex(6));
      if (func)
      {
         lua_pushinteger(L, func(obj, args+1, thunkValues));
      }
      else
      {
         lua_pushinteger(L, 0);
      }
      return 1;
   }
   
   static int thunkFloat(lua_State *L)
   {
      int args = lua_gettop(L);
      Namespace *ns = (Namespace*)lua_touserdata(L, lua_upvalueindex(5));
      SimObject *obj = (SimObject*)thunkCheckAndReturnObject(L, ns);
      
      if (obj == NULL)
      {
         typeerror(L, 1, ns->mName);
         return 0;
      }
      
      int minArgs = lua_tointeger(L, lua_upvalueindex(1));
      int maxArgs = lua_tointeger(L, lua_upvalueindex(2));
      const char *usage = lua_tostring(L, lua_upvalueindex(3));
      StringTableEntry fnName = (StringTableEntry)lua_touserdata(L, lua_upvalueindex(4));
      
      if (args+1 < minArgs || args+1 > maxArgs)
      {
         Con::warnf(ConsoleLogEntry::Script, "%s::%s - wrong number of arguments.", ns->mName, fnName);
         Con::warnf(ConsoleLogEntry::Script, "usage: %s", usage);
         return 0;
      }
      
      // We have enough args, prepare the list
      const char *thunkValues[MAX_THUNKARGS];
      
      // Set function name and %this
      thunkValues[0] = fnName;
      thunkValues[1] = obj->getIdString();
      
      // Populate incomming args
      for (int i=0; i<args; i++)
      {
         thunkValues[i+2] = lua_tostring(L, i+2);
      }
      
      FloatCallback func = (FloatCallback)lua_touserdata(L, lua_upvalueindex(6));
      if (func)
      {
         lua_pushnumber(L, func(obj, args+1, thunkValues));
      }
      else
      {
         lua_pushnumber(L, 0);
      }
      return 1;
   }
   
   static int thunkBool(lua_State *L)
   {
      int args = lua_gettop(L);
      Namespace *ns = (Namespace*)lua_touserdata(L, lua_upvalueindex(5));
      SimObject *obj = (SimObject*)thunkCheckAndReturnObject(L, ns);
      
      if (obj == NULL)
      {
         typeerror(L, 1, ns->mName);
         return 0;
      }
      
      int minArgs = lua_tointeger(L, lua_upvalueindex(1));
      int maxArgs = lua_tointeger(L, lua_upvalueindex(2));
      const char *usage = lua_tostring(L, lua_upvalueindex(3));
      StringTableEntry fnName = (StringTableEntry)lua_touserdata(L, lua_upvalueindex(4));
      
      if (args+1 < minArgs || args+1 > maxArgs)
      {
         Con::warnf(ConsoleLogEntry::Script, "%s::%s - wrong number of arguments.", ns->mName, fnName);
         Con::warnf(ConsoleLogEntry::Script, "usage: %s", usage);
         return 0;
      }
      
      // We have enough args, prepare the list
      const char *thunkValues[MAX_THUNKARGS];
      
      // Set function name and %this
      thunkValues[0] = fnName;
      thunkValues[1] = obj->getIdString();
      
      // Populate incomming args
      for (int i=0; i<args; i++)
      {
         thunkValues[i+2] = lua_tostring(L, i+2);
      }
      
      BoolCallback func = (BoolCallback)lua_touserdata(L, lua_upvalueindex(6));
      if (func)
      {
         lua_pushboolean(L, func(obj, args+1, thunkValues));
      }
      else
      {
         lua_pushboolean(L, false);
      }
      return 1;
   }
   
   static int thunkVoid(lua_State *L)
   {
      int args = lua_gettop(L);
      Namespace *ns = (Namespace*)lua_touserdata(L, lua_upvalueindex(5));
      SimObject *obj = (SimObject*)thunkCheckAndReturnObject(L, ns);
      
      if (obj == NULL)
      {
         typeerror(L, 1, ns->mName);
         return 0;
      }
      
      int minArgs = lua_tointeger(L, lua_upvalueindex(1));
      int maxArgs = lua_tointeger(L, lua_upvalueindex(2));
      const char *usage = lua_tostring(L, lua_upvalueindex(3));
      StringTableEntry fnName = (StringTableEntry)lua_touserdata(L, lua_upvalueindex(4));
      
      if (args+1 < minArgs || args+1 > maxArgs)
      {
         Con::warnf(ConsoleLogEntry::Script, "%s::%s - wrong number of arguments.", ns->mName, fnName);
         Con::warnf(ConsoleLogEntry::Script, "usage: %s", usage);
         return 0;
      }
      
      // We have enough args, prepare the list
      const char *thunkValues[MAX_THUNKARGS];
      
      // Set function name and %this
      thunkValues[0] = fnName;
      thunkValues[1] = obj->getIdString();
      
      // Populate incomming args
      for (int i=0; i<args; i++)
      {
         thunkValues[i+2] = lua_tostring(L, i+2);
      }
      
      VoidCallback func = (VoidCallback)lua_touserdata(L, lua_upvalueindex(6));
      if (func)
         func(obj, args+1, thunkValues);
      
      return 0;
   }
   
   // Global thunks
   
   static int thunkGlobalString(lua_State *L)
   {
      int args = lua_gettop(L);
      Namespace *ns = (Namespace*)lua_touserdata(L, lua_upvalueindex(5));
      
      int minArgs = lua_tointeger(L, lua_upvalueindex(1));
      int maxArgs = lua_tointeger(L, lua_upvalueindex(2));
      const char *usage = lua_tostring(L, lua_upvalueindex(3));
      StringTableEntry fnName = (StringTableEntry)lua_touserdata(L, lua_upvalueindex(4));
      
      if (args+1 < minArgs || (maxArgs != 0 && (args+1 > maxArgs)))
      {
         Con::warnf(ConsoleLogEntry::Script, "%s::%s - wrong number of arguments.", ns->mName, fnName);
         Con::warnf(ConsoleLogEntry::Script, "usage: %s", usage);
         return 0;
      }
      
      // We have enough args, prepare the list
      const char *thunkValues[MAX_THUNKARGS];
      
      // Set function name and %this
      thunkValues[0] = fnName;
      
      // Populate incomming args
      for (int i=0; i<args; i++)
      {
         thunkValues[i+1] = lua_tostring(L, i+1);
      }
      
      StringCallback func = (StringCallback)lua_touserdata(L, lua_upvalueindex(6));
      if (func)
      {
         lua_pushstring(L, func(NULL, args+1, thunkValues));
      }
      else
      {
         lua_pushstring(L, "");
      }
      return 1;
   }
   
   static int thunkGlobalInt(lua_State *L)
   {
      int args = lua_gettop(L);
      Namespace *ns = (Namespace*)lua_touserdata(L, lua_upvalueindex(5));
      
      int minArgs = lua_tointeger(L, lua_upvalueindex(1));
      int maxArgs = lua_tointeger(L, lua_upvalueindex(2));
      const char *usage = lua_tostring(L, lua_upvalueindex(3));
      StringTableEntry fnName = (StringTableEntry)lua_touserdata(L, lua_upvalueindex(4));
      
      if (args+1 < minArgs || (maxArgs != 0 && (args+1 > maxArgs)))
      {
         Con::warnf(ConsoleLogEntry::Script, "%s::%s - wrong number of arguments.", ns->mName, fnName);
         Con::warnf(ConsoleLogEntry::Script, "usage: %s", usage);
         return 0;
      }
      
      // We have enough args, prepare the list
      const char *thunkValues[MAX_THUNKARGS];
      
      // Set function name and %this
      thunkValues[0] = fnName;
      
      // Populate incomming args
      for (int i=0; i<args; i++)
      {
         thunkValues[i+1] = lua_tostring(L, i+1);
      }
      
      IntCallback func = (IntCallback)lua_touserdata(L, lua_upvalueindex(6));
      if (func)
      {
         lua_pushinteger(L, func(NULL, args+1, thunkValues));
      }
      else
      {
         lua_pushinteger(L, 0);
      }
      return 1;
   }
   
   static int thunkGlobalFloat(lua_State *L)
   {
      int args = lua_gettop(L);
      Namespace *ns = (Namespace*)lua_touserdata(L, lua_upvalueindex(5));
      
      int minArgs = lua_tointeger(L, lua_upvalueindex(1));
      int maxArgs = lua_tointeger(L, lua_upvalueindex(2));
      const char *usage = lua_tostring(L, lua_upvalueindex(3));
      StringTableEntry fnName = (StringTableEntry)lua_touserdata(L, lua_upvalueindex(4));
      
      if (args+1 < minArgs || (maxArgs != 0 && (args+1 > maxArgs)))
      {
         Con::warnf(ConsoleLogEntry::Script, "%s::%s - wrong number of arguments.", ns->mName, fnName);
         Con::warnf(ConsoleLogEntry::Script, "usage: %s", usage);
         return 0;
      }
      
      // We have enough args, prepare the list
      const char *thunkValues[MAX_THUNKARGS];
      
      // Set function name and %this
      thunkValues[0] = fnName;
      
      // Populate incomming args
      for (int i=0; i<args; i++)
      {
         thunkValues[i+1] = lua_tostring(L, i+1);
      }
      
      FloatCallback func = (FloatCallback)lua_touserdata(L, lua_upvalueindex(6));
      if (func)
      {
         lua_pushnumber(L, func(NULL, args+1, thunkValues));
      }
      else
      {
         lua_pushnumber(L, 0);
      }
      return 1;
   }
   
   static int thunkGlobalBool(lua_State *L)
   {
      int args = lua_gettop(L);
      Namespace *ns = (Namespace*)lua_touserdata(L, lua_upvalueindex(5));
      
      int minArgs = lua_tointeger(L, lua_upvalueindex(1));
      int maxArgs = lua_tointeger(L, lua_upvalueindex(2));
      const char *usage = lua_tostring(L, lua_upvalueindex(3));
      StringTableEntry fnName = (StringTableEntry)lua_touserdata(L, lua_upvalueindex(4));
      
      if (args+1 < minArgs || (maxArgs != 0 && (args+1 > maxArgs)))
      {
         Con::warnf(ConsoleLogEntry::Script, "%s::%s - wrong number of arguments.", ns->mName, fnName);
         Con::warnf(ConsoleLogEntry::Script, "usage: %s", usage);
         return 0;
      }
      
      // We have enough args, prepare the list
      const char *thunkValues[MAX_THUNKARGS];
      
      // Set function name and %this
      thunkValues[0] = fnName;
      
      // Populate incomming args
      for (int i=0; i<args; i++)
      {
         thunkValues[i+1] = lua_tostring(L, i+1);
      }
      
      BoolCallback func = (BoolCallback)lua_touserdata(L, lua_upvalueindex(6));
      if (func)
      {
         lua_pushboolean(L, func(NULL, args+1, thunkValues));
      }
      else
      {
         lua_pushboolean(L, false);
      }
      return 1;
   }
   
   static int thunkGlobalVoid(lua_State *L)
   {
      int args = lua_gettop(L);
      Namespace *ns = (Namespace*)lua_touserdata(L, lua_upvalueindex(5));
      
      int minArgs = lua_tointeger(L, lua_upvalueindex(1));
      int maxArgs = lua_tointeger(L, lua_upvalueindex(2));
      const char *usage = lua_tostring(L, lua_upvalueindex(3));
      StringTableEntry fnName = (StringTableEntry)lua_touserdata(L, lua_upvalueindex(4));
      
      if (args+1 < minArgs || (maxArgs != 0 && (args+1 > maxArgs)))
      {
         Con::warnf(ConsoleLogEntry::Script, "%s::%s - wrong number of arguments.", ns->mName, fnName);
         Con::warnf(ConsoleLogEntry::Script, "usage: %s", usage);
         return 0;
      }
      
      // We have enough args, prepare the list
      const char *thunkValues[MAX_THUNKARGS];
      
      // Set function name and %this
      thunkValues[0] = fnName;
      
      // Populate incomming args
      for (int i=0; i<args; i++)
      {
         thunkValues[i+1] = lua_tostring(L, i+1);
      }
      
      VoidCallback func = (VoidCallback)lua_touserdata(L, lua_upvalueindex(6));
      if (func)
         func(NULL, args+1, thunkValues);
      
      return 0;
   }
   
   void registerClassWithNS(AbstractClassRep *rep, Namespace *ns)
   {
      // Make master class table
      lua_newtable(mLuaState);
      int klassTable = lua_gettop(mLuaState);
      
      lua_pushvalue(mLuaState, klassTable);
      lua_setglobal(mLuaState, ns->mName);
      
      lua_newtable(mLuaState);
      int klassMetaTable = lua_gettop(mLuaState);
      
      // Define constructor function
      lua_pushstring(mLuaState, "__call");
      lua_pushstring(mLuaState, ns->mName);
      lua_pushcclosure(mLuaState, &makeObject, 1);
      lua_settable(mLuaState, klassMetaTable);
      
      int newTop = lua_gettop(mLuaState);
      
      // Register userdata metatable in the lua registry
      luaL_newmetatable(mLuaState, ns->mName);
      int metatable = lua_gettop(mLuaState);
      
      // Assign field StringTableEntry map for property get/set
      int fieldIndex = 0;
      for (Vector<AbstractClassRep::Field>::const_iterator itr = rep->mFieldList.begin(); itr != rep->mFieldList.end(); itr++)
      {
         if (itr->type == AbstractClassRep::StartGroupFieldType || itr->type == AbstractClassRep::EndGroupFieldType) {
            fieldIndex++;
            continue;
         }
         
         lua_pushlightuserdata(mLuaState, (void*)StringTable->insert(itr->pFieldname));
         lua_pushstring(mLuaState, itr->pFieldname);
         lua_settable(mLuaState, metatable);
         fieldIndex++;
      }
      
      // Assign function pointers for functions on native class
      Vector<Namespace::Entry*> entryList;
      Namespace *klassNS = rep->getNameSpace();
      klassNS->getLocalEntryList(&entryList);
      
      for (Vector<Namespace::Entry*>::const_iterator itr = entryList.begin(); itr != entryList.end(); itr++)
      {
         const Namespace::Entry *ent = *itr;
         
         // Skip doc entries
         if (ent->mType < Namespace::Entry::ScriptFunctionType)
            continue;
         
         // function name
         lua_pushstring(mLuaState, ent->mFunctionName);
         int funcName = lua_gettop(mLuaState);
         
         // min, max args, usage, STE name
         lua_pushinteger(mLuaState, ent->mMinArgs);
         lua_pushinteger(mLuaState, ent->mMaxArgs);
         lua_pushstring(mLuaState, ent->mUsage);
         lua_pushlightuserdata(mLuaState, (void*)ent->mFunctionName);
         lua_pushlightuserdata(mLuaState, ns);
         
         // function pointer
         lua_pushlightuserdata(mLuaState, (void*)ent->cb.mStringCallbackFunc);
         
         switch (ent->mType)
         {
            case Namespace::Entry::ScriptFunctionType:
               lua_pushcclosure(mLuaState, &thunkDummy, 6);
               break;
            case Namespace::Entry::StringCallbackType:
               lua_pushcclosure(mLuaState, &thunkString, 6);
               break;
            case Namespace::Entry::IntCallbackType:
               lua_pushcclosure(mLuaState, &thunkInt, 6);
               break;
            case Namespace::Entry::FloatCallbackType:
               lua_pushcclosure(mLuaState, &thunkFloat, 6);
               break;
            case Namespace::Entry::BoolCallbackType:
               lua_pushcclosure(mLuaState, &thunkBool, 6);
               break;
            case Namespace::Entry::VoidCallbackType:
            default:
               lua_pushcclosure(mLuaState, &thunkVoid, 6);
               break;
         }
         
         int topNow = lua_gettop(mLuaState);
         
         // Set entry
         lua_settable(mLuaState, klassTable);
      }
      
      // garbage collection
      lua_pushstring(mLuaState, "__gc");
      lua_pushstring(mLuaState, ns->mName);
      lua_pushcclosure(mLuaState, &gcObject, 1);
      lua_settable(mLuaState, metatable);
      
      // property setter
      lua_pushstring(mLuaState, "__newindex");
      lua_pushstring(mLuaState, ns->mName);
      lua_pushcclosure(mLuaState, &setObjectField, 1);
      lua_settable(mLuaState, metatable);
      
      lua_pushstring(mLuaState, "__ns");
      lua_pushlightuserdata(mLuaState, ns);
      lua_settable(mLuaState, metatable);
      
      // Set class metatable
      lua_pushvalue(mLuaState, klassMetaTable);
      lua_setmetatable(mLuaState, klassTable);
      
      // parent namespace
      Namespace *rootNS = rep->getNameSpace();
      if (rootNS != ns)
      {
         Namespace *itrNS = ns;
         for (itrNS = ns; itrNS != rootNS; itrNS = itrNS->mParent)
         {
            // Basically what we need to do is as follows:
            // - Ensure the table exists for any namespaces
            // - Link the previous namespace to the table
            lua_getglobal(mLuaState, itrNS->mName);
            
            // create table if not present
            if (lua_isnil(mLuaState, -1))
            {
               lua_pop(mLuaState, 1);
               lua_newtable(mLuaState);
               lua_pushvalue(mLuaState, -1);
               lua_pushvalue(mLuaState, -1);
               lua_setglobal(mLuaState, ns->mName);
            }
            
            // create metatable if not present
            lua_getmetatable(mLuaState, -1);
            if (lua_isnil(mLuaState, -1))
            {
               lua_pop(mLuaState, 1);
               lua_newtable(mLuaState);
               lua_pushvalue(mLuaState, -1);
               lua_pushvalue(mLuaState, -1);
               lua_setmetatable(mLuaState, -3);
            }
            
            // create parent table if it doesn't exist (metatable be set on the next iteration)
            lua_getglobal(mLuaState, ns->mParent->mName);
            if (lua_isnil(mLuaState, -1))
            {
               lua_pop(mLuaState, 1);
               lua_newtable(mLuaState);
               lua_pushvalue(mLuaState, -1);
               lua_pushvalue(mLuaState, -1);
               lua_setglobal(mLuaState, ns->mParent->mName);
            }
            
            // now we have: [parent] [namespace metatable]
            // finally set the metatable __index to the parent table
            lua_pushstring(mLuaState, "__index");
            lua_settable(mLuaState, -2);
         }
         
         // We should now be linked up!
      }
      else if (rep->getParentClass()) // parent class (only applicable for inbuilt classes)
      {
         lua_pushstring(mLuaState, "__index");
         lua_getglobal(mLuaState, rep->getParentClass()->getClassName());
         
         if (!lua_isnil(mLuaState, -1))
         {
            // defer to the users parent class table
            lua_settable(mLuaState, klassMetaTable); // klassMetaTable.__index == $G[rep->getParentClass()->getClassName()]
         }
         else
         {
            lua_pop(mLuaState, 1);
         }
      }
      
      
      // set __mthd fallback table
      lua_pushstring(mLuaState, "__mthd");
      lua_pushvalue(mLuaState, klassTable);
      lua_settable(mLuaState, metatable);
      
      // Root class table property getter goes through c function
      lua_pushstring(mLuaState, "__index");
      lua_pushstring(mLuaState, ns->mName);
      lua_pushcclosure(mLuaState, &getObjectField, 1);
      lua_settable(mLuaState, metatable);
      
      lua_settop(mLuaState, klassTable-1);
   }
   
   // register class
   virtual void registerClass(AbstractClassRep *rep)
   {
      registerClassWithNS(rep, rep->getNameSpace());
   }
   
   int linkLuaNamespaces(AbstractClassRep *rep, Namespace *ns)
   {
      Namespace *rootNS = rep->getNameSpace();
      // TODO: since linkNamespaces is called in onAdd, this won't work without refactoring!
      //if (rootNS == NULL) {
      // This will usually happen when Sim is not init'd yet. In this case lets just use the class name
      luaL_getmetatable(mLuaState, rep->getClassName());
      return lua_gettop(mLuaState);
      //}
      /*
       // First check the desired class name. If it's different, we'll likely have to register a new class
       if (ns != rootNS)
       {
       // Firstly, is the name already registered? If so we can skip this
       lua_pushstring(mLuaState, ns->mName);
       lua_getmetatable(mLuaState, -1);
       
       if (!lua_isnil(mLuaState, -1))
       {
       return lua_gettop(mLuaState);
       }
       
       // Register this new class
       registerClassWithNS(rep, rep->getNameSpace());
       }
       
       // Grab the global class metatable
       luaL_getmetatable(mLuaState, ns->mName);
       return lua_gettop(mLuaState);*/
   }
   
   // register object instance
   virtual void registerObject(SimObject *object)
   {
      int top = lua_gettop(mLuaState);
      
      Namespace *ns = object->getNamespace();
      int objNS = linkLuaNamespaces(object->getClassRep(), ns);
      
      int klassMetatable = -1;
      
      //
      
      
      
      // Basically make a userdata object bound to our class name
#ifdef LUA_USE_DIRECT_SIMOBJECT_PTR
      SimObject** ptr = (SimObject**)lua_newuserdata(mLuaState, sizeof(SimObject*));
      *ptr = object;
#else
      SimObjectId* ptr = (SimObjectId*)lua_newuserdata(mLuaState, sizeof(SimObjectId));
      *ptr = object->getId();
#endif
      
      int userdata = lua_gettop(mLuaState);
      
      lua_pushvalue(mLuaState, objNS);
      lua_setmetatable(mLuaState, userdata);
      
      // Now store this in InstanceTable
      lua_pushglobaltable(mLuaState);
      lua_pushstring(mLuaState, "InstanceTable");
      lua_gettable(mLuaState, -2);
      lua_pushinteger(mLuaState, object->getId());
      lua_pushvalue(mLuaState, userdata);
      lua_settable(mLuaState, -3);
      
      // Restore stack
      lua_settop(mLuaState, top);
   }
   
   virtual void registerNamespace(Namespace *ns)
   {
      // Add all functions from namespace
      Vector<Namespace::Entry*> entryList;
      ns->getLocalEntryList(&entryList);
      
      lua_newtable(mLuaState);
      int nsTable = lua_gettop(mLuaState);
      lua_pushglobaltable(mLuaState);
      
      // Globals go in Torque, otherwise we use the namespace name
      if (Namespace::global() == ns)
      {
         lua_pushstring(mLuaState, "Torque");
         lua_pushvalue(mLuaState, 1);
         lua_rawset(mLuaState, -3);
      }
      else
      {
         lua_pushstring(mLuaState, ns->mName);
         lua_pushvalue(mLuaState, 1);
         lua_rawset(mLuaState, -3);
      }
      
      lua_pop(mLuaState, 1);
      
      for (Vector<Namespace::Entry*>::const_iterator itr = entryList.begin(); itr != entryList.end(); itr++)
      {
         const Namespace::Entry *ent = *itr;
         
         // Skip doc entries
         if (ent->mType < Namespace::Entry::ScriptFunctionType)
            continue;
         
         // function name
         lua_pushstring(mLuaState, ent->mFunctionName);
         int funcName = lua_gettop(mLuaState);
         
         // min, max args, usage, STE name
         lua_pushinteger(mLuaState, ent->mMinArgs);
         lua_pushinteger(mLuaState, ent->mMaxArgs);
         lua_pushstring(mLuaState, ent->mUsage);
         lua_pushlightuserdata(mLuaState, (void*)ent->mFunctionName);
         lua_pushlightuserdata(mLuaState, ns);
         
         // function pointer
         lua_pushlightuserdata(mLuaState, (void*)ent->cb.mStringCallbackFunc);
         
         switch (ent->mType)
         {
            case Namespace::Entry::ScriptFunctionType:
               lua_pushcclosure(mLuaState, &thunkDummy, 6);
               break;
            case Namespace::Entry::StringCallbackType:
               lua_pushcclosure(mLuaState, &thunkGlobalString, 6);
               break;
            case Namespace::Entry::IntCallbackType:
               lua_pushcclosure(mLuaState, &thunkGlobalInt, 6);
               break;
            case Namespace::Entry::FloatCallbackType:
               lua_pushcclosure(mLuaState, &thunkGlobalFloat, 6);
               break;
            case Namespace::Entry::BoolCallbackType:
               lua_pushcclosure(mLuaState, &thunkGlobalBool, 6);
               break;
            case Namespace::Entry::VoidCallbackType:
            default:
               lua_pushcclosure(mLuaState, &thunkGlobalVoid, 6);
               break;
         }
         
         // Set entry
         lua_settable(mLuaState, nsTable);
      }
      
      lua_pop(mLuaState, 1);
   }
   
   virtual void removeObject(SimObject *object)
   {
      // Remove object from InstanceTable
      lua_pushglobaltable(mLuaState);
      int table = lua_gettop(mLuaState);
      lua_pushstring(mLuaState, "InstanceTable");
      lua_rawget(mLuaState, -2);
      if (!lua_isnil(mLuaState, -1))
      {
         lua_pushinteger(mLuaState, object->getId());
         lua_pushnil(mLuaState);
         lua_settable(mLuaState, -3);
      }
      lua_settop(mLuaState, table-1);
   }
   
   // gets current stack
   virtual ScriptStack *getStack()
   {
      return &mLuaStack;
   }
   
   virtual ScriptStackValueRef execute(S32 argc, ScriptStackValueRef argv[])
   {
      ScriptStackValueRef ret;
      ret.type = SCRIPTTYPE_NULL;
      ret.index = -1;
      
      int start = lua_gettop(mLuaState) - argc;
      bool wasRootExec = false;
      
      if (!mRootExec)
      {
         wasRootExec = true;
         mRootExec = true;
      }
      
      // Resolve function
      lua_pushglobaltable(mLuaState);
      lua_pushvalue(mLuaState, argv[0].index);
      const char *funcName = lua_tostring(mLuaState, argv[0].index);
      lua_gettable(mLuaState, -2);
      lua_replace(mLuaState, argv[0].index);
      lua_pop(mLuaState, 1);
      
      lua_debug_print_stack(mLuaState);
      
      if (lua_pcall(mLuaState, argc-1, 1, 0) != 0)
      {
         Con::errorf("Error calling %s (%s)", funcName, lua_tostring(mLuaState, -1));
         lua_settop(mLuaState, 0);
         lua_pushnil(mLuaState);
         ret.type = SCRIPTTYPE_NULL;
         ret.index = 1;
         
         if (wasRootExec)
         {
            mShouldReset = true;
         }
         
         return ret;
      }
      
      if (wasRootExec)
      {
         mShouldReset = true;
      }
      
      // Return result
      ret.index = lua_gettop(mLuaState);
      ret.type = mLuaStack.getTypeAtIndex(-1);
      return ret;
   }
   
   // print stack. useful if you get confused.
   static void lua_debug_print_stack(lua_State *L)
   {
      int newtop = lua_gettop(L);
      printf("--------\n");
      for (int i=1; i<newtop+1; i++)
      {
         printf("Stack value[%i]: %s [%s]\n", i, lua_tostring(L, i), lua_typename(L, lua_type(L, i)));
      }
      printf("--------\n");
   }
   
   virtual ScriptStackValueRef executeOnObject(ScriptStackValueRef obj, S32 argc, ScriptStackValueRef argv[])
   {
      ScriptStackValueRef ret;
      ret.type = SCRIPTTYPE_NULL;
      ret.index = -1;
      
      bool wasRootExec = false;
      
      if (!mRootExec)
      {
         wasRootExec = true;
         mRootExec = true;
      }
      
      if (mShouldReset)
      {
         lua_settop(mLuaState, 0);
      }
      
      int start = lua_gettop(mLuaState) - argc; // i.e. first parameter
      
      // Make sure obj is second parameter
      if (obj.index < start)
      {
         // Move obj to start
         lua_pushvalue(mLuaState, obj.index);
         lua_insert(mLuaState, start+1);
         lua_remove(mLuaState, obj.index);
         obj.index = start;
         argv[0].index -= 1;
      }
      else if (obj.index == lua_gettop(mLuaState))
      {
         lua_insert(mLuaState, start+1);
         obj.index = start+1;
      }
      
      // Resolve object field, replace arg 0
      lua_pushvalue(mLuaState, argv[0].index);
      const char *funcName = lua_tostring(mLuaState, -1);
      lua_gettable(mLuaState, obj.index);
      
      lua_replace(mLuaState, argv[0].index);
      
      //lua_debug_print_stack(mLuaState);
      
      if (lua_pcall(mLuaState, argc, 1, 0) != 0)
      {
         Con::errorf("Error calling %s (%s)", funcName, lua_tostring(mLuaState, -1));
         lua_settop(mLuaState, 0);
         lua_pushnil(mLuaState);
         ret.type = SCRIPTTYPE_NULL;
         ret.index = 1;
         
         if (wasRootExec)
         {
            mShouldReset = true;
         }
         
         return ret;
      }
      
      if (wasRootExec)
      {
         mShouldReset = true;
      }
      
      // Return result
      ret.index = lua_gettop(mLuaState);
      ret.type = mLuaStack.getTypeAtIndex(-1);
      return ret;
   }
   
   /// Evaluate an arbitrary chunk of code.
   ///
   /// @param  string   Buffer containing code to execute.
   /// @param  echo     Should we echo the string to the console?
   /// @param  fileName Indicate what file this code is coming from; used in error reporting and such.
   virtual ScriptStackValueRef evaluate(const char* string, bool echo = false, const char *fileName = NULL)
   {
      ScriptStackValueRef ret;
      ret.type = SCRIPTTYPE_NULL;
      ret.index = -1;
      
      int start = lua_gettop(mLuaState);
      
      bool wasRootExec = false;
      
      if (!mRootExec)
      {
         wasRootExec = true;
         mRootExec = true;
      }
      
      if (mShouldReset)
      {
         lua_settop(mLuaState, 0);
      }
      
      // Load the script
      if (luaL_loadbuffer(mLuaState, string, dStrlen(string), fileName) != 0)
      {
         Con::errorf("Error parsing script %s: %s\n", fileName ? fileName : "", lua_tostring(mLuaState, -1));
         lua_pop(mLuaState, 1);
         lua_pushnil(mLuaState);
         ret.type = SCRIPTTYPE_NULL;
         ret.index = lua_gettop(mLuaState);
         return ret;
      }
      
      //lua_debug_print_stack(mLuaState);
      
      if (lua_pcall(mLuaState, 0, 1, 0) != 0)
      {
         Con::errorf("Error running script: %s\n", lua_tostring(mLuaState, -1));
         lua_pop(mLuaState, 1);
         lua_pushnil(mLuaState);
         ret.type = SCRIPTTYPE_NULL;
         ret.index = lua_gettop(mLuaState);
         return ret;
      }
      
      if (wasRootExec)
      {
         mShouldReset = true;
      }
      
      // Return result
      ret.index = lua_gettop(mLuaState);
      ret.type = mLuaStack.getTypeAtIndex(-1);
      return ret;
   }
   
   static int doLuaWrite(lua_State *L,
                         const void* p,
                         size_t sz,
                         void* ud)
   {
      Stream *s = static_cast<Stream*>(ud);
      s->write(sz, p);
      return 0;
   }
   
   virtual bool compileFile(const char *filename)
   {
      char pathBuffer[1024];
      Con::expandPath(pathBuffer, sizeof(pathBuffer), filename);
      
      // Load the script
      Stream *s = ResourceManager->openStream(pathBuffer);
      if (!s)
      {
         Con::errorf(ConsoleLogEntry::Script, "Could not load script %s", pathBuffer);
         return false;
      }
      
      U32 size = s->getStreamSize();
      char *buffer = (char*)dMalloc(size+1);
      s->read(size, buffer);
      ResourceManager->closeStream(s);
      
      if (luaL_loadbuffer(mLuaState, buffer, size, pathBuffer) != 0)
      {
         Con::errorf(ConsoleLogEntry::Script, "Error parsing script %s: %s\n", pathBuffer, lua_tostring(mLuaState, -1));
         lua_pop(mLuaState, 1);
         return false;
      }
      
      dFree(buffer);
      
      // Add extension and dump to bytecode file
      char *ext = dStrrchr( pathBuffer, '.' );
      if (!ext) {
         Con::errorf(ConsoleLogEntry::Script, "No extension for script %s", pathBuffer);
         return false;
      }
      dStrcpy(ext, ".luc");
      
      FileStream outS;
      if (!ResourceManager->openFileForWrite(outS, pathBuffer))
      {
         Con::errorf(ConsoleLogEntry::Script, "Error opening %s for write", pathBuffer);
         lua_pop(mLuaState, 1);
         return false;
      }
      
      lua_dump(mLuaState, &doLuaWrite, &outS);
      lua_pop(mLuaState, 1);
      return true;
   }
   
   virtual bool executeFile(const char *filename)
   {
      char pathBuffer[1024];
      Con::expandPath(pathBuffer, sizeof(pathBuffer), filename);
      
      FileTime comModifyTime;
      FileTime scrModifyTime;
      ResourceObject *rScr = ResourceManager->find(pathBuffer);
      ResourceObject *rCom = NULL;
      
      // Check if .luc exists
      char *ext = dStrrchr( pathBuffer, '.' );
      if (!ext) {
         Con::errorf(ConsoleLogEntry::Script, "No extension for script %s", pathBuffer);
         return false;
      }
      dStrcpy(ext, ".luc");
      
      rCom = ResourceManager->find(pathBuffer);
      
      if(rCom)
         rCom->getFileTimes(NULL, &comModifyTime);
      if(rScr)
         rScr->getFileTimes(NULL, &scrModifyTime);
      
      // If com exists and its newer than the script, just load the compiled version.
      if (rCom && ( rScr == NULL || (rScr && Platform::compareFileTimes(comModifyTime, scrModifyTime) >= 0)) )
      {
         // Load the script
         Stream *s = ResourceManager->openStream(rCom);
         if (!s)
         {
            Con::errorf(ConsoleLogEntry::Script, "Could not load script %s", pathBuffer);
            return false;
         }
         
         U32 size = s->getStreamSize();
         char *buffer = (char*)dMalloc(size+1);
         s->read(size, buffer);
         ResourceManager->closeStream(s);
         
         // re-eval original filename
         Con::expandPath(pathBuffer, sizeof(pathBuffer), filename);
         
         if (luaL_loadbuffer(mLuaState, buffer, size, pathBuffer) != 0)
         {
            Con::errorf(ConsoleLogEntry::Script, "Error loading script %s: %s\n", pathBuffer, lua_tostring(mLuaState, -1));
            lua_pop(mLuaState, 1);
            return false;
         }
         
         dFree(buffer);
         
         if (lua_pcall(mLuaState, 0, 0, 0) != 0)
         {
            Con::errorf(ConsoleLogEntry::Script, "Error running script: %s\n", lua_tostring(mLuaState, -1));
            lua_pop(mLuaState, 1);
            return false;
         }
         
         lua_pop(mLuaState, 1);
         return true;
      }
      
      // Otherwise, compile script
      if (rScr)
      {
         // Load the script
         Stream *s = ResourceManager->openStream(rScr);
         if (!s)
         {
            Con::errorf(ConsoleLogEntry::Script, "Could not load script %s", pathBuffer);
            return false;
         }
         
         U32 size = s->getStreamSize();
         char *buffer = (char*)dMalloc(size+1);
         s->read(size, buffer);
         ResourceManager->closeStream(s);
         
         // Open compiled version file
         FileStream outF;
         Stream *outS = NULL;
         
         if (ResourceManager->openFileForWrite(outF, pathBuffer))
         {
            outS = &outF;
         }
         
         // re-eval original filename
         Con::expandPath(pathBuffer, sizeof(pathBuffer), filename);
         
         if (luaL_loadbuffer(mLuaState, buffer, size, pathBuffer) != 0)
         {
            Con::errorf(ConsoleLogEntry::Script, "Error parsing script %s: %s\n", pathBuffer, lua_tostring(mLuaState, -1));
            lua_pop(mLuaState, 1);
            return false;
         }
         
         dFree(buffer);
         
         // dump to compiled bytecode file
         if (outS)
         {
            lua_dump(mLuaState, &doLuaWrite, outS);
            outF.close();
         }
         
         if (lua_pcall(mLuaState, 0, 0, 0) != 0)
         {
            Con::errorf(ConsoleLogEntry::Script, "Error running script: %s\n", lua_tostring(mLuaState, -1));
            lua_pop(mLuaState, 1);
            return false;
         }
         
         lua_pop(mLuaState, 1);
         return true;
      }
      
      Con::expandPath(pathBuffer, sizeof(pathBuffer), filename);
      Con::errorf(ConsoleLogEntry::Script, "Could not execute script %s\n", pathBuffer);
      return false;
   }
};

extern ScriptEngine *sScriptInstance;

void SetupLuaInterpreter()
{
   ScriptEngine *script = ScriptEngine::getInstance();
   if (!script)
   {
      script = new LuaScriptEngine();
      
      script->registerNamespace(Namespace::global());
      
      AbstractClassRep *klass = AbstractClassRep::findClassRep("SimObject");
      script->registerClass(klass);
      
      klass = AbstractClassRep::findClassRep("AssetBase");
      script->registerClass(klass);
   }
}

void ShutdownLuaInterpreter()
{
   if (sScriptInstance)
      delete sScriptInstance;
   
   sScriptInstance = NULL;
}

ConsoleFunction(evalLua, const char*, 2, 2, "")
{
   return ScriptEngine::getInstance()->evaluate(argv[1], false, "evalLua.lua");
}

ConsoleFunction(evalLuaTF, const char*, 2, 2, "")
{
   SimObject *obj;
   if (Sim::findObject(argv[1], obj))
   {
      return ScriptEngine::getInstance()->executef(obj, "testFunction", 123);
   }
   
   return "";
}

ConsoleFunction(evalLuaF, const char*, 2, 12, "")
{
   ScriptStackValueRef args[MAX_THUNKARGS];
   for (int i=1; i<argc; i++)
   {
      args[i-1] = argv[i];
   }
   return ScriptEngine::getInstance()->execute(argc-1, args);
}

ConsoleFunction(evalLuaO, const char*, 2, 12, "")
{
   SimObject *obj;
   if (Sim::findObject(argv[1], obj))
   {
      ScriptStackValueRef args[MAX_THUNKARGS];
      for (int i=2; i<argc; i++)
      {
         args[i-2] = argv[i];
      }
      return ScriptEngine::getInstance()->executeOnObject(obj, argc-2, args);
   }
   
   return "";
}

ConsoleFunction(executeLua, bool, 2, 2, "")
{
   return ScriptEngine::getInstance()->executeFile(argv[1]);
}
