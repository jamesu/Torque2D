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
#include <stdarg.h>

#include "console/consoleOutput.h"
#include "console/consoleSerialization.h"
#include "console/codeblockEvalState.h"
#include "console/codeblockUtil.h"

namespace Con
{
static const int MaxCompletionBufferSize = 4096;
static char completionBuffer[MaxCompletionBufferSize];
static char tabBuffer[MaxCompletionBufferSize] = {0};
static SimObjectPtr<SimObject> tabObject;
static U32 completionBaseStart;
static U32 completionBaseLen;
   
StringStack STR;

/// Current script file name and root, these are registered as
/// console variables.
/// @{

///
StringTableEntry gCurrentFile;
StringTableEntry gCurrentRoot;
/// @}

bool gWarnUndefinedScriptVariables;

void initExec()
{
   gWarnUndefinedScriptVariables = false;
}

void addExecVariables()
{
   // Current script file name and root
   Con::addVariable( "Con::File", TypeString, &gCurrentFile );
   Con::addVariable( "Con::Root", TypeString, &gCurrentRoot );
   
   Con::addVariable( "Con::warnUndefinedVariables", TypeBool, &gWarnUndefinedScriptVariables );
}

//--------------------------------------

U32 tabComplete(char* inputBuffer, U32 cursorPos, U32 maxResultLength, bool forwardTab)
{
   // Check for null input.
   if (!inputBuffer[0])
   {
      return cursorPos;
   }
   
   // Cap the max result length.
   if (maxResultLength > MaxCompletionBufferSize)
   {
      maxResultLength = MaxCompletionBufferSize;
   }
   
   // See if this is the same partial text as last checked.
   if (dStrcmp(tabBuffer, inputBuffer))
   {
      // If not...
      // Save it for checking next time.
      dStrcpy(tabBuffer, inputBuffer);
      // Scan backward from the cursor position to find the base to complete from.
      S32 p = cursorPos;
      while ((p > 0) && (inputBuffer[p - 1] != ' ') && (inputBuffer[p - 1] != '.') && (inputBuffer[p - 1] != '('))
      {
         p--;
      }
      completionBaseStart = p;
      completionBaseLen = cursorPos - p;
      // Is this function being invoked on an object?
      if (inputBuffer[p - 1] == '.')
      {
         // If so...
         if (p <= 1)
         {
            // Bail if no object identifier.
            return cursorPos;
         }
         
         // Find the object identifier.
         S32 objLast = --p;
         while ((p > 0) && (inputBuffer[p - 1] != ' ') && (inputBuffer[p - 1] != '('))
         {
            p--;
         }
         
         if (objLast == p)
         {
            // Bail if no object identifier.
            return cursorPos;
         }
         
         // Look up the object identifier.
         dStrncpy(completionBuffer, inputBuffer + p, objLast - p);
         completionBuffer[objLast - p] = 0;
         tabObject = Sim::findObject(completionBuffer);
         if (!bool(tabObject))
         {
            // Bail if not found.
            return cursorPos;
         }
      }
      else
      {
         // Not invoked on an object; we'll use the global namespace.
         tabObject = 0;
      }
   }
   
   // Chop off the input text at the cursor position.
   inputBuffer[cursorPos] = 0;
   
   // Try to find a completion in the appropriate namespace.
   const char *newText;
   
   if (bool(tabObject))
   {
      newText = tabObject->tabComplete(inputBuffer + completionBaseStart, completionBaseLen, forwardTab);
   }
   else
   {
      // In the global namespace, we can complete on global vars as well as functions.
      if (inputBuffer[completionBaseStart] == '$')
      {
         newText = CodeBlockEvalState::getCurrent()->globalVars->tabComplete(inputBuffer + completionBaseStart, completionBaseLen, forwardTab);
      }
      else
      {
         newText = Namespace::global()->tabComplete(inputBuffer + completionBaseStart, completionBaseLen, forwardTab);
      }
   }
   
   if (newText)
   {
      // If we got something, append it to the input text.
      S32 len = dStrlen(newText);
      if (len + completionBaseStart > maxResultLength)
      {
         len = maxResultLength - completionBaseStart;
      }
      dStrncpy(inputBuffer + completionBaseStart, newText, len);
      inputBuffer[completionBaseStart + len] = 0;
      // And set the cursor after it.
      cursorPos = completionBaseStart + len;
   }
   
   // Save the modified input buffer for checking next time.
   dStrcpy(tabBuffer, inputBuffer);
   
   // Return the new (maybe) cursor position.
   return cursorPos;
}

//---------------------------------------------------------------------------

void setVariable(const char *name, const char *value)
{
   if (name && name[0] == '$')
      name++;
   name = StringTable->insert(name);
   
   CodeBlockEvalState::getCurrent()->globalVars->setVariable(StringTable->insert(name), value);
}

void setValueVariable(const char *name, const ConsoleValuePtr& value)
{
   if (name && name[0] == '$')
      name++;
   name = StringTable->insert(name);
   
   CodeBlockEvalState::getCurrent()->globalVars->setValueVariable(StringTable->insert(name), value);
}

ConsoleValuePtr getConsoleValueVariable(const char *name)
{
   
   if (name && name[0] == '$')
      name++;
   name = StringTable->insert(name);
   return CodeBlockEvalState::getCurrent()->globalVars->getValueVariable(StringTable->insert(name));
}

void setLocalVariable(const char *name, const char *value)
{
   if (name && name[0] == '%')
      name++;
   name = StringTable->insert(name);
   CodeBlockEvalState *evalState = CodeBlockEvalState::getCurrent();
   
   //
   CodeBlockFunction* func = evalState->currentFrame.function;
   if (func)
   {
      for (Vector<CodeBlockFunction::Symbol>::iterator itr = func->vars.begin(), itrEnd = func->vars.end(); itr != itrEnd; itr++)
      {
         if (itr->varName == name)
         {
            evalState->globalVars->setVariable(name, value);
            return;
         }
      }
   }
}

void setBoolVariable(const char *varName, bool value)
{
   setVariable(varName, value ? "1" : "0");
}

void setIntVariable(const char *varName, S32 value)
{
   char scratchBuffer[32];
   dSprintf(scratchBuffer, sizeof(scratchBuffer), "%d", value);
   setVariable(varName, scratchBuffer);
}

void setFloatVariable(const char *varName, F32 value)
{
   char scratchBuffer[32];
   dSprintf(scratchBuffer, sizeof(scratchBuffer), "%.9g", value);
   setVariable(varName, scratchBuffer);
}

//---------------------------------------------------------------------------

ConsoleStringValuePtr getVariable(const char *name)
{
   if (name && name[0] == '$')
      name++;
   
   name = StringTable->insert(name);
   return CodeBlockEvalState::getCurrent()->globalVars->getVariable(StringTable->insert(name));
}

ConsoleStringValuePtr getLocalVariable(const char *name)
{
   if (name && name[0] == '%')
      name++;
   
   name = StringTable->insert(name);
   CodeBlockEvalState *evalState = CodeBlockEvalState::getCurrent();
   
   //
   CodeBlockFunction* func = evalState->currentFrame.function;
   if (func)
   {
      for (Vector<CodeBlockFunction::Symbol>::iterator itr = func->vars.begin(), itrEnd = func->vars.end(); itr != itrEnd; itr++)
      {
         if (itr->varName == name)
         {
            return evalState->globalVars->getVariable(name);
         }
      }
   }
   
   return "";
}

bool getBoolVariable(const char *varName, bool def)
{
   if (varName && varName[0] == '$')
      varName++;
   
   StringTableEntry name = StringTable->insert(varName);
   Dictionary::Entry* entry = CodeBlockEvalState::getCurrent()->globalVars->lookup(name);
   if (entry)
   {
      return dAtob(entry->getStringValue().c_str());
   }
   else
   {
      return def;
   }
}

S32 getIntVariable(const char *varName, S32 def)
{
   if (varName && varName[0] == '$')
      varName++;
   
   StringTableEntry name = StringTable->insert(varName);
   Dictionary::Entry* entry = CodeBlockEvalState::getCurrent()->globalVars->lookup(name);
   if (entry)
   {
      return (S32)(entry->getSignedIntValue());
   }
   else
   {
      return def;
   }
}

F32 getFloatVariable(const char *varName, F32 def)
{
   if (varName && varName[0] == '$')
      varName++;
   
   StringTableEntry name = StringTable->insert(varName);
   Dictionary::Entry* entry = CodeBlockEvalState::getCurrent()->globalVars->lookup(name);
   if (entry)
   {
      return entry->getFloatValue();
   }
   else
   {
      return def;
   }
}

//---------------------------------------------------------------------------

bool addVariable(const char *name, S32 t, void *dp)
{
   if (name && name[0] == '$')
      name++;
   
   CodeBlockEvalState::getCurrent()->globalVars->addVariable(name, t, dp);
   return true;
}

bool removeVariable(const char *name)
{
   if (name && name[0] == '$')
      name++;
   
   name = StringTable->lookup(name);
   return name!=0 && CodeBlockEvalState::getCurrent()->globalVars->removeVariable(name);
}

//---------------------------------------------------------------------------

void addCommand(const char *nsName, const char *name,StringCallback cb, const char *usage, S32 minArgs, S32 maxArgs)
{
   Namespace *ns = lookupNamespace(nsName);
   ns->addCommand(StringTable->insert(name), cb, usage, minArgs, maxArgs);
}

void addCommand(const char *nsName, const char *name,VoidCallback cb, const char *usage, S32 minArgs, S32 maxArgs)
{
   Namespace *ns = lookupNamespace(nsName);
   ns->addCommand(StringTable->insert(name), cb, usage, minArgs, maxArgs);
}

void addCommand(const char *nsName, const char *name,IntCallback cb, const char *usage, S32 minArgs, S32 maxArgs)
{
   Namespace *ns = lookupNamespace(nsName);
   ns->addCommand(StringTable->insert(name), cb, usage, minArgs, maxArgs);
}

void addCommand(const char *nsName, const char *name,FloatCallback cb, const char *usage, S32 minArgs, S32 maxArgs)
{
   Namespace *ns = lookupNamespace(nsName);
   ns->addCommand(StringTable->insert(name), cb, usage, minArgs, maxArgs);
}

void addCommand(const char *nsName, const char *name,BoolCallback cb, const char *usage, S32 minArgs, S32 maxArgs)
{
   Namespace *ns = lookupNamespace(nsName);
   ns->addCommand(StringTable->insert(name), cb, usage, minArgs, maxArgs);
}

void addCommand(const char *nsName, const char *name,ValueCallback cc, const char *usage, S32 minArgs, S32 maxArgs)
{
   Namespace *ns = lookupNamespace(nsName);
   ns->addCommand(StringTable->insert(name), cc, usage, minArgs, maxArgs);
}

void markCommandGroup(const char * nsName, const char *name, const char* usage)
{
   Namespace *ns = lookupNamespace(nsName);
   ns->markGroup(name,usage);
}

void beginCommandGroup(const char * nsName, const char *name, const char* usage)
{
   markCommandGroup(nsName, name, usage);
}

void endCommandGroup(const char * nsName, const char *name)
{
   markCommandGroup(nsName, name, NULL);
}

void addOverload(const char * nsName, const char * name, const char * altUsage)
{
   Namespace *ns = lookupNamespace(nsName);
   ns->addOverload(name,altUsage);
}

void addCommand(const char *name,StringCallback cb,const char *usage, S32 minArgs, S32 maxArgs)
{
   Namespace::global()->addCommand(StringTable->insert(name), cb, usage, minArgs, maxArgs);
}

void addCommand(const char *name,VoidCallback cb,const char *usage, S32 minArgs, S32 maxArgs)
{
   Namespace::global()->addCommand(StringTable->insert(name), cb, usage, minArgs, maxArgs);
}

void addCommand(const char *name,IntCallback cb,const char *usage, S32 minArgs, S32 maxArgs)
{
   Namespace::global()->addCommand(StringTable->insert(name), cb, usage, minArgs, maxArgs);
}

void addCommand(const char *name,FloatCallback cb,const char *usage, S32 minArgs, S32 maxArgs)
{
   Namespace::global()->addCommand(StringTable->insert(name), cb, usage, minArgs, maxArgs);
}

void addCommand(const char *name,BoolCallback cb,const char *usage, S32 minArgs, S32 maxArgs)
{
   Namespace::global()->addCommand(StringTable->insert(name), cb, usage, minArgs, maxArgs);
}

ConsoleValuePtr evaluate(const char *string, bool echo, const char *fileName)
{
   if (echo)
      Con::printf("%s%s", getVariable( "$Con::Prompt" ).c_str(), string);
   
   if(fileName)
      fileName = StringTable->insert(fileName);
   
   CodeBlockPtr newCodeBlock = new CodeBlock();
   ConsoleValuePtr ret = newCodeBlock->compileExec(fileName, string, false, fileName ? -1 : 0);
   return ret;
}

//------------------------------------------------------------------------------
ConsoleValuePtr evaluatef(const char *string, ...)
{
   const char * result = NULL;
   char * buffer = new char[4096];
   if (buffer != NULL)
   {
      va_list args;
      va_start(args, string);
      dVsprintf(buffer, 4096, string, args);
      va_end (args);
      
      CodeBlockPtr newCodeBlock = new CodeBlock();
      result = newCodeBlock->compileExec(NULL, buffer, false, 0);
      
      delete [] buffer;
      buffer = NULL;
   }
   
   return result;
}

ConsoleValuePtr execute(S32 argc, ConsoleValuePtr argv[])
{
   StringTableEntry funcName = argv[0].getSTEStringValue();
   Namespace* ns = Namespace::global();
   Namespace::Entry* entry = ns->lookup(funcName);
   
   if (entry)
   {
      return entry->execute(argc, argv, CodeBlockEvalState::getCurrent());
   }
   else
   {
      Con::warnf("Couldn't find global function %s.", funcName);
      return ConsoleValuePtr();
   }
}

//------------------------------------------------------------------------------
ConsoleValuePtr execute(SimObject *object, S32 argc, ConsoleValuePtr argv[],bool thisCallOnly)
{
   StringTableEntry funcName = argv[0].getSTEStringValue();
   Namespace* ns = object->getNamespace();
   Namespace::Entry* entry = ns->lookup(funcName);
   
   if (entry)
   {
      return entry->execute(argc, argv, CodeBlockEvalState::getCurrent());
   }
   else
   {
      Con::warnf("Couldn't find function %s in object %s.", funcName, ns->mName);
      return ConsoleValuePtr();
   }
}

//------------------------------------------------------------------------------
ConsoleStringValuePtr executeS(S32 argc, const char *argv[])
{
   StringTableEntry funcName = StringTable->insert(argv[0]);
   Namespace* ns = Namespace::global();
   Namespace::Entry* entry = ns->lookup(funcName);
   
   if (entry)
   {
      // Copy argv to temp list
      ConsoleValuePtr argvValue[128];
      for (U32 i=0; i<argc; i++)
      {
         argvValue[i].setString(argv[i]);
      }
      return entry->execute(argc, argvValue, CodeBlockEvalState::getCurrent()).getStringValue();
   }
   else
   {
      Con::warnf("Couldn't find function %s in object %s.", funcName, ns->mName);
      return "";
   }
}

//------------------------------------------------------------------------------
ConsoleStringValuePtr executeS(SimObject *object, S32 argc, const char *argv[],bool thisCallOnly)
{
   StringTableEntry funcName = StringTable->insert(argv[0]);
   Namespace* ns = object->getNamespace();
   Namespace::Entry* entry = ns->lookup(funcName);
   
   if (entry)
   {
      // Copy argv to temp list
      ConsoleValuePtr argvValue[128];
      for (U32 i=0; i<argc; i++)
      {
         argvValue[i].setString(argv[i]);
      }
      return entry->execute(argc, argvValue, CodeBlockEvalState::getCurrent()).getStringValue();
   }
   else
   {
      Con::warnf("Couldn't find function %s in object %s.", funcName, ns->mName);
      return "";
   }
}

//------------------------------------------------------------------------------
bool isFunction(const char *fn)
{
   const char *string = StringTable->lookup(fn);
   if(!string)
      return false;
   else
      return Namespace::global()->lookup(string) != NULL;
}

//------------------------------------------------------------------------------

Namespace *lookupNamespace(const char *ns)
{
   if(!ns)
      return Namespace::global();
   return Namespace::find(StringTable->insert(ns));
}

bool linkNamespaces(const char *parent, const char *child)
{
   Namespace *pns = lookupNamespace(parent);
   Namespace *cns = lookupNamespace(child);
   if(pns && cns)
      return cns->classLinkTo(pns);
   return false;
}

bool unlinkNamespaces(const char *parent, const char *child)
{
   Namespace *pns = lookupNamespace(parent);
   Namespace *cns = lookupNamespace(child);
   if(pns && cns)
      return cns->unlinkClass(pns);
   return false;
}

bool classLinkNamespaces(Namespace *parent, Namespace *child)
{
   if(parent && child)
      return child->classLinkTo(parent);
   return false;
}

char *getReturnBuffer(U32 bufferSize)
{
   return STR.getReturnBuffer(bufferSize);
}

char *getReturnBuffer( const char *stringToCopy )
{
   char *ret = STR.getReturnBuffer( dStrlen( stringToCopy ) + 1 );
   dStrcpy( ret, stringToCopy );
   ret[dStrlen( stringToCopy )] = '\0';
   return ret;
}

char *getArgBuffer(U32 bufferSize)
{
   return STR.getArgBuffer(bufferSize);
}

char *getFloatArg(F64 arg)
{
   char *ret = STR.getArgBuffer(32);
   dSprintf(ret, 32, "%g", arg);
   return ret;
}

char *getIntArg(S32 arg)
{
   char *ret = STR.getArgBuffer(32);
   dSprintf(ret, 32, "%d", arg);
   return ret;
}

char *getBoolArg(bool arg)
{
   char *ret = STR.getArgBuffer(32);
   dSprintf(ret, 32, "%d", arg);
   return ret;
}

} // end of Console namespace


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



