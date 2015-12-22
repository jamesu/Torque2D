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

/*! @defgroup Callstack Call Stack
	@ingroup TorqueScriptFunctions
	@{
*/

/*! Use the backtrace function to print the current callstack to the console.

	This is used to trace functions called from withing functions and can help discover
	what functions were called (and not yet exited) before the current point in your scripts.
    @return No return value
*/
ConsoleFunctionWithDocs(backtrace, ConsoleVoid, 1, 1, ())
{
   U32 totalSize = 1;
	CodeBlockEvalState* evalState = CodeBlockEvalState::getCurrent();

   for(U32 i = 0; i < (U32)evalState->frames.size(); i++)
   {
      totalSize += dStrlen(evalState->frames[i].function->name) + 3;
      if(evalState->frames[i].ns)
         totalSize += dStrlen(evalState->frames[i].ns) + 2;
   }

   char *buf = Con::getReturnBuffer(totalSize);
   buf[0] = 0;
   for(U32 i = 0; i < (U32)evalState->frames.size(); i++)
   {
      dStrcat(buf, "->");
      if(evalState->frames[i].ns && evalState->frames[i].function->name)
      {
         dStrcat(buf, evalState->frames[i].ns);
         dStrcat(buf, "::");
      }
      dStrcat(buf, evalState->frames[i].function->name);
   }
   Con::printf("BackTrace: %s", buf);
}

/*! @} */ // group Callstack
