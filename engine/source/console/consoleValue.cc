#include "platform/platform.h"
#include "console/console.h"
#include "console/consoleSerialization.h"

#include "console/consoleDictionary.h"
#include "console/consoleNamespace.h"

#include "console/compiler/consoleAST.h"
#include "collection/findIterator.h"
#include "io/resource/resourceManager.h"

#include "string/findMatch.h"
#include "io/fileStream.h"
#include "console/compiler/consoleCompiler.h"
#include "string/stringStack.h"

#include "sim/simBase.h"
#include "console/codeblockEvalState.h"

static char tempBuffer[256];
static char tempBuffer2[256];
static bool bValue = false;

const char* ConsoleValue::getTempStringValue() const
{
   char* destBuffer = bValue ? tempBuffer : tempBuffer2;
   bValue = !bValue;
   
    destBuffer[0] = '\0';
    getInternalStringValue(destBuffer, 256);
    return destBuffer;
}

StringTableEntry ConsoleValue::getSTEStringValue() const
{
    switch (type)
    {
        case TypeInternalNull:
            return NULL;
        case TypeInternalStringTableEntry:
        case TypeInternalNamespaceName:
            return value.string;
          break;
        default:
            return StringTable->insert(getTempStringValue());
            break;
    }
}

void ConsoleValue::getInternalStringValue(char* outBuffer, U32 bufferSize) const
{
    switch (type)
    {
        case TypeInternalNull:
            *outBuffer = '\0';
          break;
        case TypeInternalInt:
            dSprintf(outBuffer, bufferSize, "%u", value.ival);
            break;
        case TypeInternalFloat:
            dSprintf(outBuffer, bufferSize, "%.5g", value.fval);
            break;
        case TypeInternalStringTableEntry:
        case TypeInternalNamespaceName:
            dSprintf(outBuffer, bufferSize, "%s", value.string);
            break;
       case TypeInternalNamespaceEntry:
          dSprintf(outBuffer, bufferSize, "%s", static_cast<Namespace::Entry*>(value.ptrValue)->mFunctionName);
          break;
        default:
            dSprintf(outBuffer, bufferSize, "%s", value.refValue->getString().c_str());
            break;
    }
}

ConsoleStringValuePtr ConsoleValuePtr::getStringValue()  const
{
    switch (type)
    {
        case TypeInternalNull:
            return "";
        case TypeInternalInt:
       {
          ConsoleStringValuePtr newStr = ConsoleStringValue::fromInt(value.ival);
          return newStr;
       }
        case TypeInternalFloat:
       {
          ConsoleStringValuePtr newStr = ConsoleStringValue::fromFloat(value.fval);
          return newStr;
       }
        case TypeInternalStringTableEntry:
        case TypeInternalNamespaceName:
            return value.string;
        case TypeInternalNamespaceEntry:
          return ((Namespace::Entry*)value.ptrValue)->mFunctionName;
          break;
        default:
            return value.refValue->getString();
    }
}


void ConsoleValuePtr::readStack(Stream &s, ConsoleSerializationState &serializationState, Vector<ConsoleValuePtr> &stack)
{
   S16 numTypes = 0;
   s.read(&numTypes);
   Vector<StringTableEntry> varTypes;
   varTypes.setSize(numTypes);
   for (U32 i=0; i<numTypes; i++)
   {
      varTypes[i] = s.readSTString();
   }
   
   U32 numStackValues = 0;
   s.read(&numStackValues);
   stack.setSize(numStackValues);
   
   for (U32 i=0; i<numStackValues; i++)
   {
      U16 refType = 0;
      ConsoleValuePtr &ptr = stack[i];
      s.read(&refType);
      
      // TODO: ConsoleValue itelf should handle I/O i.e.
      // ConsoleValue::read(Stream &s, S32 typeId)
      
      // Internal value, just copy
      if (refType < TypeReferenceCounted)
      {
         ptr.type = refType;
         
         if (refType == TypeInternalStringTableEntry || refType == TypeInternalNamespaceName)
         {
            ptr.value.string = s.readSTString();
         }
         else if (refType == TypeSavedReference)
         {
            S32 refIdx = 0;
            s.read(&refIdx);
            ptr.type = serializationState.loadedValues[refIdx]->type;
            ptr.value = serializationState.loadedValues[refIdx]->value;
            ptr.AddRef();
         }
         else
         {
            // Can just copy as-is
            s.read(&ptr.value.ival);
         }
         continue;
      }
      
      StringTableEntry varType = varTypes[refType-TypeReferenceCounted];
      ConsoleBaseType * type = ConsoleBaseType::getTypeByName(varType);
      
      ptr.type = type->getTypeID();
      ptr.value.refValue = type->createReferenceCountedValue();
      if (ptr.value.refValue)
      {
         serializationState.loadedValues.push_back(&ptr);
         ptr.value.refValue->read(s, serializationState);
         ptr.AddRef();
      }
      else
      {
         serializationState.loadedValues.push_back(&ptr);
      }
   }
}

void ConsoleValuePtr::writeStack(Stream &s, ConsoleSerializationState &serializationState, Vector<ConsoleValuePtr> &stack)
{
   U32 numStackValues = stack.size();
   
   // Collate together type names
   Vector<StringTableEntry> varTypes;
   Vector<U16> varTypeIds;
   
   for (U32 i=0; i<numStackValues; i++)
   {
      ConsoleValuePtr &ptr = stack[i];
      
      if (isRefType(ptr.type))
      {
         ConsoleBaseType *type = ptr.value.refValue->getType();
         StringTableEntry te = StringTable->insert(type->getTypeName());
         U32 idx = varTypes.indexOf(te);
         
         if (idx == (U32)-1)
         {
            varTypes.push_back(te);
            varTypeIds.push_back((varTypes.size()-1)+TypeReferenceCounted);
         }
         else
         {
            varTypeIds.push_back(idx+TypeReferenceCounted);
         }
      }
      else
      {
         varTypeIds.push_back(ptr.type);
      }
   }
   
   S16 numTypes = varTypes.size();
   s.write(numTypes);
   
   for (U32 i=0; i<numTypes; i++)
   {
      s.writeString(varTypes[i]);
   }
   
   s.write(numStackValues);
   
   for (U32 i=0; i<numStackValues; i++)
   {
      ConsoleValuePtr &ptr = stack[i];
      
      
      if (isRefType(ptr.type))
      {
         //Con::printf("Var[%i] is %s", i, ptr.value.refValue->getType()->getTypeName());
         S32 idx = serializationState.getSavedObjectIdx(ptr.value.refValue);
         if (idx == -1)
         {
            //Con::printf(" ^^ New Copy");
            s.write(varTypeIds[i]);
            serializationState.addSavedObject(ptr.value.refValue);
            ptr.value.refValue->write(s, serializationState);
         }
         else
         {
            //Con::printf(" ^^ Referenced");
            s.write((U16)TypeSavedReference);
            s.write(idx);
         }
      }
      else
      {
         s.write(varTypeIds[i]);
         if (ptr.type == TypeInternalStringTableEntry || ptr.type == TypeInternalNamespaceName)
         {
            s.writeString(ptr.value.string);
         }
         else
         {
            // Raw write, easy!
            s.write(ptr.value.ival);
         }
      }
   }
}

SimObject* ConsoleValue::getSimObject()
{
   switch (type)
   {
      case TypeInternalNull:
         return NULL;
      case TypeInternalInt:
      case TypeInternalSimObjectId:
         return Sim::findObject((SimObjectId)value.ival);
      case TypeInternalFloat:
         return Sim::findObject((S32)value.fval);
      case TypeInternalStringTableEntry:
         return Sim::findObject(value.string);
      case TypeInternalNamespaceName:
         return Sim::findObject(value.stringStackPtr);
      case TypeInternalNamespaceEntry:
         return NULL;
      default:
         return value.refValue->getSimObject();
   }
}

//------------------------------------------------------------------------------

ConsoleValuePtr _BaseEngineConsoleCallbackHelper::_exec()
{
   ConsoleValuePtr returnValue;
   if( mThis )
   {
      mArgv[1].setValue(ConsoleSimObjectPtr::fromObject(mThis));
      
      // Cannot invoke callback until object has been registered
      if (mThis->isProperlyAdded()) {
         
         Namespace* ns = mThis->getNamespace();
         Namespace::Entry* entry = ns->lookup(mCallbackName);
         
         if (entry)
         {
            returnValue.setValue(entry->execute(mArgc, mArgv, CodeBlockEvalState::getCurrent()));
         }
         else
         {
            Con::warnf("Couldn't find function %s::%s.", ns->mName, mCallbackName);
            returnValue.setNull();
         }
         
      } else {
         returnValue.setNull();
      }
   }
   else
   {
      Namespace* ns = Namespace::global();
      Namespace::Entry* entry = ns->lookup(mCallbackName);
      
      if (entry)
      {
         returnValue.setValue(entry->execute(mArgc, mArgv, CodeBlockEvalState::getCurrent()));
      }
      else
      {
         Con::warnf("Couldn't find global function %s::%s.", ns->mName, mCallbackName);
         returnValue.setNull();
      }
   }
   
   mArgc = mInitialArgc; // reset args
   return returnValue;
}

ConsoleValuePtr _BaseEngineConsoleCallbackHelper::_execLater(SimConsoleThreadExecEvent *evt)
{
   mArgc = mInitialArgc; // reset args
   Sim::postEvent((SimObject*)Sim::getRootGroup(), evt, Sim::getCurrentTime());
   return evt->getCB().waitForResult();
}

void _BaseEngineConsoleCallbackHelper::postEvent(SimConsoleThreadExecEvent *evt)
{
   Sim::postEvent((SimObject*)Sim::getRootGroup(), evt, Sim::getCurrentTime());
}

void ConsoleValuePtr::setValue(const ConsoleStringValuePtr &other)
{
   DecRef();
   
   type = other.value->getInternalType();
   value.refValue = other.value;
   value.refValue->addRef();
}

void ConsoleValuePtr::setString(const char* other)
{
   DecRef();
   
   value.refValue = ConsoleStringValue::fromString(other);
   type = value.refValue->getInternalType();
   value.refValue->addRef();
}

// ConsoleStringValue stuff

ConsoleStringValuePtr::ConsoleStringValuePtr(const char *str)
{
   if (!str || str[0] == '\0')
   {
      value = NULL;
      return;
   }
   
   value = ConsoleStringValue::fromString(str);
   value->addRef();
}
/*
 ConsoleStringValuePtr::ConsoleStringValuePtr(StringTableEntry str)
 {
 value = (ConsoleStringValue*)((uintptr_t)str & ConsoleStringValuePtr::TAGGED);
 }*/

ConsoleStringValuePtr::ConsoleStringValuePtr(SimpleString* str)
{
   value = ConsoleStringValue::fromSimpleString(str);
   value->addRef();
}

ConsoleStringValuePtr::ConsoleStringValuePtr(ConsoleStringValuePtr* other)
{
   value = other->value;
   value->addRef();
}

ConsoleStringValuePtr::ConsoleStringValuePtr(ConsoleStringValue* newValue)
{
   value = newValue;
   value->addRef();
}

ConsoleStringValuePtr ConsoleStringValuePtr::fromSTE(StringTableEntry ste)
{
   ConsoleStringValuePtr value;
   value = (ConsoleStringValue*)((uintptr_t)ste & ConsoleStringValuePtr::TAGGED);
   return value;
}

void ConsoleStringValue::read(Stream &s, ConsoleSerializationState &state)
{
   U32 len = 0;
   s.read(&len);
   
   value.buffer = (char*)dMalloc(len);
   value.bufferLen = len;
   s.read(len, value.buffer);
}

void ConsoleStringValue::write(Stream &s, ConsoleSerializationState &state)
{
   s.write(value.bufferLen);
   s.write(value.bufferLen, value.buffer);
}

SimObject* ConsoleStringValue::getSimObject()
{
   return Sim::findObject(value.getString());
}

// Array value stuff


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
      return false;
   }
   else
   {
      ((ConsoleValuePtr&)iteratorValue).setValue(mValues[iterValue]);
      ((ConsoleValuePtr&)iterator).setValue(iterValue+1);
   }
   
   return true;
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

