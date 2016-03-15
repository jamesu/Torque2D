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
#include "platform/platformTLS.h"
#include "platform/threads/thread.h"
#include "console/console.h"
#include "console/consoleInternal.h"
#include "console/consoleObject.h"
#include "io/fileStream.h"
#include "io/resource/resourceManager.h"
#include "console/compiler/consoleAST.h"
#include "collection/findIterator.h"
#include "console/consoleTypes.h"
#include "debug/telnetDebugger.h"
#include "sim/simBase.h"
#include "console/compiler/consoleCompiler.h"
#include "string/stringStack.h"
#include "component/dynamicConsoleMethodComponent.h"
#include "memory/safeDelete.h"

#include "console/consoleOutput.h"
#include "console/consoleSerialization.h"
#include "console/codeblockEvalState.h"

namespace Con
{

static bool active = false;

#ifdef TORQUE_MULTITHREAD
static ThreadIdent gMainThreadID = -1;
#endif


void init()
{
   AssertFatal(active == false, "Con::init should only be called once.");

   // Set up general init values.
   active                        = true;
   initLog();
   initExec();

#ifdef TORQUE_MULTITHREAD
   // Note the main thread ID.
   gMainThreadID = ThreadManager::getCurrentThreadId();
#endif

   // Initialize subsystems.
   Namespace::init();
   ConsoleConstructor::setup();
   CodeBlockEvalState::initGlobal();

   // Variables
   setVariable("Con::prompt", "% ");
   addLogVariables();
   addExecVariables();

   // Setup the console types.
   ConsoleBaseType::initialize();

   // And finally, the ACR...
   AbstractClassRep::initialize();
}

//--------------------------------------

void shutdown()
{
   AssertFatal(active == true, "Con::shutdown should only be called once.");
   active = false;

   Namespace::shutdown();
   shutdownLog();
}

bool isActive()
{
   return active;
}

bool isMainThread()
{
#ifdef TORQUE_MULTITHREAD
   return ThreadManager::isCurrentThread(gMainThreadID);
#else
   // If we're single threaded we're always in the main thread.
   return true;
#endif
}

} // end of Console namespace



