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
#include "console/console.h"
#include "console/consoleInternal.h"
#include "console/consoleObject.h"
#include "console/ast.h"
#include "io/resource/resourceManager.h"
#include "io/fileStream.h"
#include "console/compiler.h"
#include "console/codeblockUtil.h"

#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_OSX)
#include <ifaddrs.h>
#include <arpa/inet.h>
#endif

// Buffer for expanding script filenames.
static char pathBuffer[1024];

extern S32 QSORT_CALLBACK ACRCompare(const void *aptr, const void *bptr);

ConsoleFunctionGroupBegin(MetaScripting, "Functions that let you manipulate the scripting engine programmatically.");

/*! @defgroup MetaScriptingFunctions Meta-Scripting
      @ingroup TorqueScriptFunctions
      @{
*/

/*! Use the call function to dynamically build and call a function.
    @param funcName A string containing the unadorned name of a function to be executed.
    @param args .. - Any arguments that should be passed to the function.
    @return Returns a string containing the results from the function that is built and called.
    @sa eval
*/
ConsoleFunctionWithDocs(call, ConsoleValuePtr, 2, 0, ( funcName, [args ... ]?))
{
   return Con::execute(argc - 1, argv + 1);
}

/*! Returns the DSO path of the given filename
    @param scriptFileName A string contains the file to search for
    @return Returns either the requested pathname as a string, or an empty string if not found.
*/
ConsoleFunctionWithDocs(getDSOPath, ConsoleString, 2, 2, (scriptFileName))
{
   Con::expandPath(pathBuffer, sizeof(pathBuffer), argv[1]);  

   const char *filename = CodeBlock::getDSOPath(pathBuffer);
   if(filename == NULL || *filename == 0)
      return "";

   return filename;
}

/*! Use the compile function to pre-compile a script file without executing the contents.
    @param fileName A path to the script to compile.
    @return Returns 1 if the script compiled without errors and 0 if the file did not compile correctly or if the path is wrong. Also, ff the path is invalid, an error will print to the console.
    @sa exec
*/
ConsoleFunctionWithDocs(compile, ConsoleBool, 2, 2, ( fileName ))
{
   TORQUE_UNUSED( argc );
   return CodeblockUtil::compile(argv[1]);
}

/*! 
*/
ConsoleFunctionWithDocs(compilePath, ConsoleString, 2, 2, ( path ))
{
    if ( !Con::expandPath(pathBuffer, sizeof(pathBuffer), argv[1]) )
        return "-1 0";
   
    S32 failedScripts = 0;
    S32 totalScripts = 0;
    ResourceObject *match = NULL;
    const char* actualPath = NULL;
   
    while ( (match = ResourceManager->findMatch( pathBuffer, &actualPath, match )) )
    {
       if ( !CodeblockUtil::compile( actualPath ) )
            failedScripts++;
        
        totalScripts++;
    }
   
    char result[32];
    dSprintf( result, 32, "%d %d", failedScripts, totalScripts );
    return result;
}

bool scriptExecutionEcho = true;
/*! Whether to echo script file execution or not.
*/
ConsoleFunctionWithDocs(setScriptExecEcho, ConsoleVoid, 2, 2, (echo?))
{
    //scriptExecutionEcho = dAtob(argv[1]);
}

/*! Use the exec function to compile and execute a normal script, or a special journal script.
    If $Pref::ignoreDSOs is set to true, the system will use .cs before a .dso file if both are found.
    @param fileName A string containing a path to the script to be compiled and executed.
    @param nocalls A boolean value. If this value is set to true, then all function calls encountered while executing the script file will be skipped and not called. This allows us to re-define function definitions found in a script file, without re-executing other worker scripts in the same file.
    @param journalScript A boolean value. If this value is set tot true, and if a journal is being played, the engine will attempt to read this script from the journal stream. If no journal is playing, this field is ignored.
    @return Returns true if the file compiled and executed w/o errors, false otherwise.
    @sa compile
*/
#undef TORQUE_ALLOW_DSO_GENERATION
ConsoleFunctionWithDocs(exec, ConsoleBool, 2, 4, ( fileName, [nocalls]?, [journalScript ]?))
{
   bool journalScript = argc > 3 ? dAtob(argv[3]) : true;
   bool noCalls = argc > 2 ? dAtob(argv[2]) : false;
   return CodeblockUtil::execFile(argv[1], noCalls, journalScript, true);
}

/*! Use the eval function to execute any valid script statement.
    If you choose to eval a multi-line statement, be sure that there are no comments or comment blocks embedded in the script string.
    @param script A string containing a valid script statement. This may be a single line statement or multiple lines concatenated together with new-line characters.
    @return Returns the result of executing the script statement.
    @sa call
*/
ConsoleFunctionWithDocs(eval, ConsoleValuePtr, 2, 2, ( script ))
{
   TORQUE_UNUSED( argc );
   return Con::evaluate(argv[1], false, NULL);
}

/*! Grabs the relevant data for the variable represented by the given string
    @param varName A String representing the variable to check
    @return Returns a string containing component data for the requested variable or an empty string if not found.
*/
ConsoleFunctionWithDocs(getVariable, ConsoleString, 2, 2, (string varName))
{
   return Con::getVariable(argv[1]);
}

/*! Checks whether given name represents a current valid function.
    @param funcName The name of the function to check.
    @return Returns either true if the string represents a valid function or false if not.
*/
ConsoleFunctionWithDocs(isFunction, ConsoleBool, 2, 2, (string funcName))
{
   return Con::isFunction(argv[1]);
}

/*! Checks whether the given method name represents a valid method within the given namespace.
    @param namespace A string representing the namespace in which the method should reside.
    @param method The name of the method in question.
    @return Returns a boolean value which is true if the given mathod name represents a valid method in the namespace and false otherwise.
*/
ConsoleFunctionWithDocs(isMethod, ConsoleBool, 3, 3, (string namespace, string method))
{
   Namespace* ns = Namespace::find( StringTable->insert( argv[1] ) );
   Namespace::Entry* nse = ns->lookup( StringTable->insert( argv[2] ) );
   if( !nse )
      return false;

   return true;
}

/*! Attempts to extract a mod directory from path. Returns empty string on failure.
*/
ConsoleFunctionWithDocs(getModNameFromPath, ConsoleString, 2, 2, (string path))
{
   StringTableEntry modPath = Con::getModNameFromPath(argv[1]);
   return modPath ? modPath : "";
}

//----------------------------------------------------------------

/*! 
*/
ConsoleFunctionWithDocs(getPrefsPath, ConsoleString, 1, 2, ([fileName]?))
{
   const char *filename = Platform::getPrefsPath(argc > 1 ? argv[1].getTempStringValue() : NULL);
   if(filename == NULL || *filename == 0)
      return "";
     
   return filename;
}

/*! 
*/
ConsoleFunctionWithDocs(execPrefs, ConsoleBool, 2, 4, (fileName, [nocalls]?, [journalScript]?))
{
   const char *filename = Platform::getPrefsPath(argv[1]);
   if (filename == NULL || *filename == 0)
      return false;

   if ( !Platform::isFile(filename) )
    return false;

   argv[0] = "exec";
   argv[1] = filename;
   return dAtob(Con::execute(argc, argv));
}

/*! Use the export function to save all global variables matching the specified name pattern in wildCard to a file, either appending to that file or over-writing it.
    @param wildCard A string identifying what variable(s) to export. All characters used to create a global are allowed and the special symbol \*\, meaning 0 or more instances of any character.
    @param fileName A string containing a path to a file in which to save the globals and their definitions.
    @param append A boolean value. If this value is true, the file will be appended to if it exists, otherwise it will be created/over-written.
    @return No return value
*/
ConsoleFunctionWithDocs(export, ConsoleVoid, 2, 4, ( wildCard, [fileName]?, [append]?))
{
    // Fetch the wildcard.
    const char* pWildcard = argv[1];

    // Fetch the filename.
    const char* pFilename = NULL;
    if ( argc >= 3 )
    {
        Con::expandPath( pathBuffer, sizeof(pathBuffer), argv[2] );
        pFilename = pathBuffer;
    }

    // Fetch append flag.
    const bool append = argc >= 4 ? dAtob(argv[3] ) : false;

    // Export the variables.
    CodeBlockEvalState::getCurrent()->globalVars->exportVariables( pWildcard, pFilename, append );
}

/*! Use the deleteVariables function to delete any global variable matching the wildCard statement.
    @param wildCard A string identifying what variable(s) to delete. All characters used to create a global are allowed and the special symbol \*\, meaning 0 or more instances of any character.
    @return No return value
*/
ConsoleFunctionWithDocs(deleteVariables, ConsoleVoid, 2, 2, ( wildCard ))
{
   TORQUE_UNUSED( argc );
   CodeBlockEvalState::getCurrent()->globalVars->deleteVariables(argv[1]);
}

//----------------------------------------------------------------

/*! Use the trace function to enable (or disable) function call tracing. If enabled, tracing will print a message every time a function is entered, showing what arguments it received, and it will print a message every time a function is exited, showing the return value (or last value of last statement) for that function.
    @param enable A boolean value. If set to true, tracing is enabled, otherwise it is disabled.
    @return No return value
*/
ConsoleFunctionWithDocs(trace, ConsoleVoid, 2, 2, ( enable ))
{
   TORQUE_UNUSED( argc );
   CodeBlockEvalState *evalState = CodeBlockEvalState::getCurrent();
   evalState->traceOn = dAtob(argv[1]);
   Con::printf("Console trace is %s", evalState->traceOn ? "on." : "off.");
}

//----------------------------------------------------------------

#if defined(TORQUE_DEBUG) || defined(INTERNAL_RELEASE)
/*! Use the debug function to cause the engine to issue a debug break and to break into an active debugger.
    For this to work, the engine must have been compiled with either TORQUE_DEBUG, or INTERNAL_RELEASE defined
    @return No return value.
*/
ConsoleFunctionWithDocs(debug, ConsoleVoid, 1, 1, ())
{
   TORQUE_UNUSED( argc );
   TORQUE_UNUSED( argv );
   Platform::debugBreak();
}
#endif

#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_OSX)
//Some code for allowing torsion to connect, this code returns the ipads local ip address
//code was obtained from http://blog.zachwaugh.com/post/309927273/programmatically-retrieving-ip-address-of-iphone
//adapted to iT2D by me
/*! Gets the Apple hardware local IP on wifi. Should work on OS X and iOS
*/
ConsoleFunctionWithDocs(getAppleDeviceIPAddress, ConsoleString, 1, 1, ())
{
    char *address = Con::getReturnBuffer(32);
    dStrcpy(address, "error");
    struct ifaddrs *interfaces = NULL;
    struct ifaddrs *temp_addr = NULL;
    int success = 0;

    // retrieve the current interfaces - returns 0 on success
    success = getifaddrs(&interfaces);
    if (success == 0)
    {
        // Loop through linked list of interfaces
        temp_addr = interfaces;
        while(temp_addr != NULL)
        {
            if(temp_addr->ifa_addr->sa_family == AF_INET)
            {
                // Check if interface is en0 which is the wifi connection on the iPhone
                // Note: Could be different on MacOSX and simulator and may need modifying
                if(dStrcmp(temp_addr->ifa_name, "en0") == 0)
                {
                    dStrcpy(address, inet_ntoa(((struct sockaddr_in *)temp_addr->ifa_addr)->sin_addr));
                }
            }

            temp_addr = temp_addr->ifa_next;
        }
    }

    // Free memory
    freeifaddrs(interfaces);

    return address;
}
#endif


//----------------------------------------------------------------

/*! 
*/
ConsoleFunctionWithDocs( enumerateConsoleClasses, ConsoleString, 1, 2, ([baseClass]?))
{

   AbstractClassRep *base = NULL;    
   if(argc > 1)
   {
      base = AbstractClassRep::findClassRep(argv[1]);
      if(!base)
         return "";
   }
   
   Vector<AbstractClassRep*> classes;
   U32 bufSize = 0;
   for(AbstractClassRep *rep = AbstractClassRep::getClassList(); rep; rep = rep->getNextClass())
   {
      if( !base || rep->isClass(base))
      {
         classes.push_back(rep);
         bufSize += dStrlen(rep->getClassName()) + 1;
      }
   }
   
   if(!classes.size())
      return "";

   dQsort(classes.address(), classes.size(), sizeof(AbstractClassRep*), ACRCompare);

   char* ret = Con::getReturnBuffer(bufSize);
   dStrcpy( ret, classes[0]->getClassName());
   for( U32 i=0; i< (U32)classes.size(); i++)
   {
      dStrcat( ret, "\t" );
      dStrcat( ret, classes[i]->getClassName() );
   }
   
   return ret;
}

/*! @} */ // group MetaScriptFunctions
