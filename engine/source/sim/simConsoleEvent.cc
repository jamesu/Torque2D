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
#include "sim/simConsoleEvent.h"
#include "platform/platform.h"
#include "console/consoleInternal.h"

extern CodeBlockEvalState gNewEvalState;

//-----------------------------------------------------------------------------

SimConsoleEvent::SimConsoleEvent(S32 argc, ConsoleValuePtr argv[], bool onObject)
{
   mOnObject = onObject;
   mArgc = argc;
   U32 totalSize = 0;
   mArgv = new ConsoleValuePtr[argc];

   for(U32 i = 0; i < argc; i++)
   {
      mArgv[i].setValue(argv[i]);
   }
}

SimConsoleEvent::~SimConsoleEvent()
{
   delete[] mArgv;
}

void SimConsoleEvent::process(SimObject* object)
{
// #ifdef DEBUG
//    Con::printf("Executing schedule: %d", sequenceCount);
// #endif
   
    if(mOnObject)
      Con::execute(object, mArgc, mArgv);
   else
   {
      // Grab the function name. If '::' doesn't exist, then the schedule is
      // on a global function.
      StringTableEntry v = mArgv[0].getSTEStringValue();
      char* func = dStrstr( v, (char*)"::" );
      if( func )
      {
         // Set the first colon to NULL, so we can reference the namespace.
         // This is okay because events are deleted immediately after
         // processing. Maybe a bad idea anyway?
         func[0] = '\0';

         // Move the pointer forward to the function name.
         func += 2;

         // Lookup the namespace and function entry.
         Namespace* ns = Namespace::find( v );
         if( ns )
         {
            Namespace::Entry* nse = ns->lookup( StringTable->insert( func ) );
            if( nse )
               // Execute.
               nse->execute( mArgc, mArgv, &gNewEvalState );
         }
      }
      else
         Con::execute( mArgc, mArgv );
   }
}
