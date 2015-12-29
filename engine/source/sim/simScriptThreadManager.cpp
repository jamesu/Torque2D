//
//  simScriptThreadManager.cpp
//  Torque2D
//
//  Created by James Urquhart on 28/12/2015.
//

#include "platform/platform.h"
#include "sim/simBase.h"
#include "console/consoleTypes.h"
#include "console/consoleSerialization.h"
#include "console/codeblockEvalState.h"
#include "sim/simScriptThreadManager.h"

//-----------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(SimScriptThreadManager);

U32 SimScriptThreadManager::SAVE_VERSION = 1;
static U32 SimScriptFourCC = makeFourCCTag('T', 'S', 'T', 'S');

//-----------------------------------------------------------------------------

SimScriptThreadManager::SimScriptThreadManager()
{
   mThreadsToUpdate.setValue(ConsoleArrayValue::fromValues(0, NULL));
}

SimScriptThreadManager::~SimScriptThreadManager()
{
   cleanup();
}

void SimScriptThreadManager::cleanup()
{
   for (U32 i=0, sz=mThreads.size(); i<sz; i++)
   {
      mThreads[i].setNull();
   }
   
   mThreads.clear();
}

void SimScriptThreadManager::onRemove()
{
   cleanup();
   Parent::onRemove();
}

void SimScriptThreadManager::addThread(CodeBlockCoroutineState *thread)
{
   thread->waitTicks = 0;
   mThreads.push_back(thread);
}

void SimScriptThreadManager::removeThread(CodeBlockCoroutineState *thread)
{
   for (U32 i=0, sz=mThreads.size(); i<sz; i++)
   {
      if (mThreads[i].value.ptrValue == thread)
      {
         mThreads[i].setNull();
         mThreads.erase(i);
         return;
      }
   }
}

bool SimScriptThreadManager::saveThreads(Stream &s)
{
   s.write(SimScriptFourCC);
   s.write(SAVE_VERSION);
   s.write(DSO_VERSION);
   U32 codeblockOffsPos = s.getPosition();
   s.write((U32)0);
   
   ConsoleSerializationState serializationState;
   ConsoleValuePtr::writeStack(s, serializationState, mThreads);
   
   // Write codeblocks
   U32 offsBlocks = s.getStreamSize();
   s.write((U32)serializationState.loadedCodeblocks.size());
   
   for (U32 i=0, sz=serializationState.loadedCodeblocks.size(); i<sz; i++)
   {
      serializationState.loadedCodeblocks[i]->save(s);
   }
   
   s.setPosition(codeblockOffsPos);
   s.write(offsBlocks);
   
   return true;
}

bool SimScriptThreadManager::loadThreads(Stream &s)
{
   U32 v;
   cleanup();
   
   s.read(&v);
   if (v != SimScriptFourCC)
   {
      Con::errorf("Invalid file");
      return false;
   }
   
   s.read(&v);
   if (v != SAVE_VERSION)
   {
      Con::errorf("Invalid version");
      return false;
   }
   
   s.read(&v);
   if (v != DSO_VERSION)
   {
      Con::errorf("Invalid DSO version");
      return false;
   }
   
   U32 offs = 0;
   U32 sz = 0;
   ConsoleSerializationState serializationState;
   
   s.read(&offs);
   U32 offsStack = s.getStreamSize();
   
   s.setPosition(offs);
   s.read(&sz);
   
   for (U32 i=0; i<sz; i++)
   {
      char buffer[1024];
      CodeBlock *block = new CodeBlock();
      dSprintf(buffer, 1024, "[[COROUTINE-%x]]", block);
      block->read(s, buffer);
      
      serializationState.loadedCodeblocks.push_back(block);
   }
   
   s.setPosition(offsStack);
   
   ConsoleValuePtr::readStack(s, serializationState, mThreads);
   
   return true;
}

void SimScriptThreadManager::tick()
{
   static StringTableEntry stContinue = StringTable->insert("CONTINUE");
   ConsoleValuePtr continueValue;
   continueValue.setSTE(stContinue);
   
   Vector<ConsoleValuePtr> *values = &((ConsoleArrayValue*)mThreadsToUpdate.value.refValue)->mValues;
   values->clear();
   
   Vector<U32> threadsToRemove;
   
   for (U32 i=0, sz=mThreads.size(); i<sz; i++)
   {
      CodeBlockCoroutineState* state = static_cast<CodeBlockCoroutineState*>(mThreads[i].value.refValue);
      if (!state)
         continue;
      
      if (state->currentState == CodeBlockCoroutineState::DEAD)
      {
         //Con::printf("DBG: remove dead thread %i", i);
         threadsToRemove.push_back(i);
         continue;
      }
      
      if (state->waitTicks == S32_MIN) // special "ignore this" marker
         continue;
      
      state->waitTicks--;
      
      //Con::printf("Thread[%i/%x] waitTicks == %i", i, state, state->waitTicks);
      
      if (state->waitTicks <= 0)
      {
         values->push_back(state);
         state->waitTicks = 0;
      }
   }
   
   if (values->size() > 0)
   {
      Con::executef(this, "resumeRoutines", mThreadsToUpdate);
   }
   
   for (S32 i=threadsToRemove.size()-1; i >= 0; i--)
   {
      mThreads[threadsToRemove[i]].setNull();
      mThreads.erase(threadsToRemove[i]);
   }
}

void SimScriptThreadManager::processTick( void )
{
   tick();
}

void SimScriptThreadManager::interpolateTick( F32 delta )
{
}

void SimScriptThreadManager::advanceTime( F32 timeDelta )
{
   
}

ConsoleMethod(SimScriptThreadManager, addThread, void, 3, 3, "(thread)")
{
   if (ConsoleValue::isRefType(argv[2].type))
   {
      CodeBlockCoroutineState *state = dynamic_cast<CodeBlockCoroutineState*>(argv[2].value.refValue);
      if (state)
      {
         object->addThread(state);
      }
   }
}

ConsoleMethod(SimScriptThreadManager, removeThread, void, 3, 3, "(thread)")
{
   if (ConsoleValue::isRefType(argv[2].type))
   {
      CodeBlockCoroutineState *state = dynamic_cast<CodeBlockCoroutineState*>(argv[2].value.refValue);
      if (state)
      {
         object->removeThread(state);
      }
   }
}

ConsoleMethod(SimScriptThreadManager, saveThreads, bool, 3, 3, "(filename)")
{
   FileStream fs;
   if (fs.open(argv[2].getTempStringValue(), FileStream::Write))
   {
      return object->saveThreads(fs);
   }
   return false;
}

ConsoleMethod(SimScriptThreadManager, loadThreads, bool, 3, 3, "(filename)")
{
   FileStream fs;
   if (fs.open(argv[2].getTempStringValue(), FileStream::Read))
   {
      return object->loadThreads(fs);
   }
   return false;
}

ConsoleMethod(SimScriptThreadManager, tick, void, 2, 2, "")
{
   object->tick();
}
