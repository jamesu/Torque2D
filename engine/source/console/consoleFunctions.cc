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

#include "platform/platform.h"
#include "platform/Tickable.h"
#include "console/console.h"
#include "io/resource/resourceManager.h"
#include "platform/platformInput.h"
#include "console/codeblockEvalState.h"
#include "console/compiler/consoleAST.h"

#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_OSX)
#include <ifaddrs.h>
#include <arpa/inet.h>
#endif

#include "inputManagement_ScriptBinding.h"

#ifdef TORQUE_ALLOW_JOURNALING
   static U32 journalDepth = 1;
#endif

//Luma:   Console function to tell if this is a TORQUE_OS_IOS build
ConsoleFunction(isiPhoneBuild, bool, 1, 1, "Returns true if this is a iPhone build, false otherwise")
{
#ifdef   TORQUE_OS_IOS
   return true;
#else
   return false;
#endif   //TORQUE_OS_IOS
}

extern CodeBlockEvalState gRootEvalState;

// Creates a coroutine, sets it in the initial state.
ConsoleStaticMethod(Coroutine, create, ConsoleValuePtr, 2, 2, "Create a coroutine calling func")
{
   ConsoleValuePtr &nse = argv[1];
   Namespace::Entry *callEntry = NULL;
   if (nse.type == ConsoleValue::TypeInternalNamespaceEntry)
   {
      callEntry = (Namespace::Entry*)nse.value.ptrValue;
   }
   else
   {
      callEntry = Namespace::global()->lookup(nse.getSTEStringValue());
   }
   
   if (!callEntry)
   {
      Con::errorf("Could not find coroutine function %s", nse.getSTEStringValue());
      return ConsoleValuePtr();
   }
   
   ConsoleValuePtr ret;
   CodeBlockCoroutineState* state = new CodeBlockCoroutineState();
   state->nsEntry = callEntry;
   state->currentState = CodeBlockCoroutineState::WAIT_INITIAL_CALL;
   ret.setValue(state);
   return ret;
}

// Creates a coroutine, sets it in the initial state.
ConsoleMethod(SimObject, createCoroutine, ConsoleValuePtr, 2, 2, "Create a coroutine calling func")
{
   ConsoleValuePtr &nse = argv[2];
   Namespace::Entry *callEntry = NULL;
   if (nse.type == ConsoleValue::TypeInternalNamespaceEntry)
   {
      callEntry = (Namespace::Entry*)nse.value.ptrValue;
   }
   else
   {
      callEntry = object->getNamespace()->lookup(nse.getSTEStringValue());
   }
   
   if (!callEntry)
   {
      Con::errorf("Could not find coroutine function %s in object %s", nse.getSTEStringValue(), object->getIdString());
      return ConsoleValuePtr();
   }
   
   ConsoleValuePtr ret;
   CodeBlockCoroutineState* state = new CodeBlockCoroutineState();
   state->nsEntry = callEntry;
   ret.setValue(state);
   return ret;
}

// Resumes a coroutine, passing in argv to the restoreCoroutine func.
// @note this function will return the value intended to be yielded to the coroutine
ConsoleStaticMethod(Coroutine, resume, ConsoleValuePtr, 2, 0, "Resume a coroutine")
{
   ConsoleValuePtr &cvalue = argv[1];
   ConsoleValuePtr returnValue;
   CodeBlockCoroutineState* state = ConsoleValue::isRefType(cvalue.type) ? dynamic_cast<CodeBlockCoroutineState*>(cvalue.value.refValue) : NULL;
   
   // Rewrite args so we get nsEntry a b c instead of resume coroutine a b c
   ConsoleValuePtr realArgs[32];
   realArgs[0].type = ConsoleValue::TypeInternalNamespaceEntry;
   realArgs[0].value.ptrValue = state->nsEntry;
   
   for (U32 i=2; i<argc; i++)
   {
      realArgs[i-1].setValue(argv[i]);
   }
   
   if (argc > 2)
   {
      returnValue.setValue(argv[2]);
   }
   
   // we'll essentially switch to this when exiting
   if (!CodeBlockEvalState::getCurrent()->restoreCoroutine(*state, argc-1, realArgs))
   {
      Con::errorf("Couldn't resume coroutine");
      return ConsoleValuePtr();
   }
   
   return returnValue;
}

ConsoleStaticMethod(Coroutine, getCurrent, ConsoleValuePtr, 1, 1, "Gets current active coroutine")
{
   ConsoleValuePtr returnValue;
   returnValue.setValue(CodeBlockEvalState::getCurrent()->coroutine);
   return returnValue;
}

// Resumes a coroutine, passing in argv to the restoreCoroutine func.
// @note this function will return the value intended to be yielded to the
// function which calls resume
ConsoleStaticMethod(Coroutine, yield, ConsoleValuePtr, 1, 3, "Yield current coroutine")
{
   ConsoleValuePtr returnValue;
   if (argc > 2)
   {
      returnValue.setValue(argv[2]);
   }
   
   CodeBlockEvalState* evalState = CodeBlockEvalState::getCurrent();
   if (evalState->coroutine)
   {
      if (!CodeBlockEvalState::getCurrent()->saveCoroutine(*evalState->coroutine))
      {
         Con::errorf("Coroutine::yield: yield failed.");
      }
   }
   else
   {
      Con::errorf("Coroutine::yield: no coroutine is active.");
   }
   return returnValue;
}

ConsoleStaticMethod(Coroutine, status, ConsoleValuePtr, 2, 2, "Get status of a coroutine")
{
   ConsoleValuePtr &cvalue = argv[1];
   ConsoleValuePtr returnValue;
   CodeBlockCoroutineState* state = ConsoleValue::isRefType(cvalue.type) ? dynamic_cast<CodeBlockCoroutineState*>(cvalue.value.refValue) : NULL;
   
   if (state)
   {
      ConsoleValuePtr retValue;
      static StringTableEntry steDEAD = StringTable->insert("DEAD");
      static StringTableEntry steRUNNING = StringTable->insert("RUNNING");
      static StringTableEntry steSUSPENDED = StringTable->insert("SUSPENDED");
      
      switch(state->currentState)
      {
         case CodeBlockCoroutineState::WAIT_INITIAL_CALL:
         case CodeBlockCoroutineState::SUSPENDED:
            retValue.setSTE(steSUSPENDED);
            break;
         case CodeBlockCoroutineState::RUNNING:
            retValue.setSTE(steRUNNING);
            break;
         case CodeBlockCoroutineState::DEAD:
            retValue.setSTE(steDEAD);
            break;
      }
      
      return retValue;
   }
   else
   {
      return ConsoleValuePtr();
   }
}

ConsoleStaticMethod(Coroutine, setWaitTicks, void, 3, 3, "Sets next tick time if threaded")
{
   ConsoleValuePtr &cvalue = argv[1];
   ConsoleValuePtr returnValue;
   CodeBlockCoroutineState* state = ConsoleValue::isRefType(cvalue.type) ? dynamic_cast<CodeBlockCoroutineState*>(cvalue.value.refValue) : NULL;
   
   if (state)
   {
      state->waitTicks = (S32)argv[2].getFloatValue();
      //Con::printf("setWaitTicks[%x] ticks == %i", state, state->waitTicks);
   }
}

ConsoleStaticMethod(Coroutine, setWaitMS, void, 3, 3, "Sets next tick time if threaded")
{
   ConsoleValuePtr &cvalue = argv[1];
   ConsoleValuePtr returnValue;
   CodeBlockCoroutineState* state = ConsoleValue::isRefType(cvalue.type) ? dynamic_cast<CodeBlockCoroutineState*>(cvalue.value.refValue) : NULL;
   
   if (state)
   {
      state->waitTicks = (S32)(argv[2].getFloatValue() / Tickable::smTickMs);
   }
}

ConsoleStaticMethod(Coroutine, setNoTicks, void, 2, 2, "Disables ticking")
{
   ConsoleValuePtr &cvalue = argv[1];
   ConsoleValuePtr returnValue;
   CodeBlockCoroutineState* state = ConsoleValue::isRefType(cvalue.type) ? dynamic_cast<CodeBlockCoroutineState*>(cvalue.value.refValue) : NULL;
   
   if (state)
   {
      state->waitTicks = S32_MIN;
   }
}
