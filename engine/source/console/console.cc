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
#include "console/ast.h"
#include "collection/findIterator.h"
#include "console/consoleTypes.h"
#include "debug/telnetDebugger.h"
#include "sim/simBase.h"
#include "console/compiler.h"
#include "string/stringStack.h"
#include "component/dynamicConsoleMethodComponent.h"
#include "memory/safeDelete.h"
#include <stdarg.h>

#include "output_ScriptBinding.h"
#include "expando_ScriptBinding.h"

#ifndef _HASHTABLE_H
#include "collection/hashTable.h"
#endif


#include "console/consoleSerialization.h"

static Mutex* sLogMutex;

extern StringStack STR;

StmtNode *gStatementList;
DocBlockNode *gCurrentDocBlock = NULL;

U32 gAnonFunctionID = 0;
ConsoleConstructor *ConsoleConstructor::first = NULL;
bool gWarnUndefinedScriptVariables;

static char scratchBuffer[4096];

static const char * prependDollar ( const char * name )
{
   if(name[0] != '$')
   {
      S32   len = dStrlen(name);
      AssertFatal(len < sizeof(scratchBuffer)-2, "CONSOLE: name too long");
      scratchBuffer[0] = '$';
      dMemcpy(scratchBuffer + 1, name, len + 1);
      name = scratchBuffer;
   }
   return name;
}

static const char * prependPercent ( const char * name )
{
   if(name[0] != '%')
   {
      S32   len = dStrlen(name);
      AssertFatal(len < sizeof(scratchBuffer)-2, "CONSOLE: name too long");
      scratchBuffer[0] = '%';
      dMemcpy(scratchBuffer + 1, name, len + 1);
      name = scratchBuffer;
   }
   return name;
}

//--------------------------------------
void ConsoleConstructor::init(const char *cName, const char *fName, const char *usg, S32 minArgs, S32 maxArgs)
{
   mina = minArgs;
   maxa = maxArgs;
   funcName = fName;
   usage = usg;
   className = cName;
   sc = 0; fc = 0; vc = 0; bc = 0; ic = 0;
   group = false;
   next = first;
   ns = false;
   first = this;
}

void ConsoleConstructor::setup()
{
   for(ConsoleConstructor *walk = first; walk; walk = walk->next)
   {
      if(walk->sc)
         Con::addCommand(walk->className, walk->funcName, walk->sc, walk->usage, walk->mina, walk->maxa);
      else if(walk->ic)
         Con::addCommand(walk->className, walk->funcName, walk->ic, walk->usage, walk->mina, walk->maxa);
      else if(walk->fc)
         Con::addCommand(walk->className, walk->funcName, walk->fc, walk->usage, walk->mina, walk->maxa);
      else if(walk->vc)
         Con::addCommand(walk->className, walk->funcName, walk->vc, walk->usage, walk->mina, walk->maxa);
      else if(walk->bc)
         Con::addCommand(walk->className, walk->funcName, walk->bc, walk->usage, walk->mina, walk->maxa);
      else if(walk->cc)
         Con::addCommand(walk->className, walk->funcName, walk->cc, walk->usage, walk->mina, walk->maxa);
      else if(walk->group)
         Con::markCommandGroup(walk->className, walk->funcName, walk->usage);
      else if(walk->overload)
         Con::addOverload(walk->className, walk->funcName, walk->usage);
      else if(walk->ns)
      {
         Namespace* ns = Namespace::find(StringTable->insert(walk->className));
         if( ns )
            ns->mUsage = walk->usage;
      }
      else
         AssertFatal(false, "Found a ConsoleConstructor with an indeterminate type!");
   }
}

ConsoleConstructor::ConsoleConstructor(const char *className, const char *funcName, ValueCallback vfunc, const char *usage, S32 minArgs, S32 maxArgs)
{
   init(className, funcName, usage, minArgs, maxArgs);
   cc = vfunc;
}

ConsoleConstructor::ConsoleConstructor(const char *className, const char *funcName, StringCallback sfunc, const char *usage, S32 minArgs, S32 maxArgs)
{
   init(className, funcName, usage, minArgs, maxArgs);
   sc = sfunc;
}

ConsoleConstructor::ConsoleConstructor(const char *className, const char *funcName, IntCallback ifunc, const char *usage, S32 minArgs, S32 maxArgs)
{
   init(className, funcName, usage, minArgs, maxArgs);
   ic = ifunc;
}

ConsoleConstructor::ConsoleConstructor(const char *className, const char *funcName, FloatCallback ffunc, const char *usage, S32 minArgs, S32 maxArgs)
{
   init(className, funcName, usage, minArgs, maxArgs);
   fc = ffunc;
}

ConsoleConstructor::ConsoleConstructor(const char *className, const char *funcName, VoidCallback vfunc, const char *usage, S32 minArgs, S32 maxArgs)
{
   init(className, funcName, usage, minArgs, maxArgs);
   vc = vfunc;
}

ConsoleConstructor::ConsoleConstructor(const char *className, const char *funcName, BoolCallback bfunc, const char *usage, S32 minArgs, S32 maxArgs)
{
   init(className, funcName, usage, minArgs, maxArgs);
   bc = bfunc;
}

ConsoleConstructor::ConsoleConstructor(const char* className, const char* groupName, const char* aUsage)
{
   init(className, groupName, usage, -1, -2);

   group = true;

   // Somewhere, the entry list is getting flipped, partially.
   // so we have to do tricks to deal with making sure usage
   // is properly populated.

   // This is probably redundant.
   static char * lastUsage = NULL;
   if(aUsage)
      lastUsage = (char *)aUsage;

   usage = lastUsage;
}

ConsoleConstructor::ConsoleConstructor(const char* className, const char* usage)
{
   init(className, NULL, usage, -1, -2);
   ns = true;
}

namespace Con
{

static Vector<ConsumerCallback> gConsumers(__FILE__, __LINE__);
static DataChunker consoleLogChunker;
static Vector<ConsoleLogEntry> consoleLog(__FILE__, __LINE__);
static bool consoleLogLocked;
static bool logBufferEnabled=true;
static S32 printLevel = 10;
static FileStream consoleLogFile;
static const char *defLogFileName = "console.log";
static S32 consoleLogMode = 0;
static bool active = false;
static bool newLogFile;
static const char *logFileName;

static const int MaxCompletionBufferSize = 4096;
static char completionBuffer[MaxCompletionBufferSize];
static char tabBuffer[MaxCompletionBufferSize] = {0};
static SimObjectPtr<SimObject> tabObject;
static U32 completionBaseStart;
static U32 completionBaseLen;

#ifdef TORQUE_MULTITHREAD
static ThreadIdent gMainThreadID = -1;
#endif

/// Current script file name and root, these are registered as
/// console variables.
/// @{

///
StringTableEntry gCurrentFile;
StringTableEntry gCurrentRoot;
/// @}

void init()
{
   AssertFatal(active == false, "Con::init should only be called once.");

   // Set up general init values.
   active                        = true;
   logFileName                   = NULL;
   newLogFile                    = true;
   gWarnUndefinedScriptVariables = false;
   sLogMutex                     = new Mutex;

#ifdef TORQUE_MULTITHREAD
   // Note the main thread ID.
   gMainThreadID = ThreadManager::getCurrentThreadId();
#endif

   // Initialize subsystems.
   Namespace::init();
   ConsoleConstructor::setup();

   // Variables
   setVariable("Con::prompt", "% ");
   addVariable("Con::logBufferEnabled", TypeBool, &logBufferEnabled);
   addVariable("Con::printLevel", TypeS32, &printLevel);
   addVariable("Con::warnUndefinedVariables", TypeBool, &gWarnUndefinedScriptVariables);

   // Current script file name and root
   Con::addVariable( "Con::File", TypeString, &gCurrentFile );
   Con::addVariable( "Con::Root", TypeString, &gCurrentRoot );

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

   consoleLogFile.close();
   Namespace::shutdown();

   SAFE_DELETE( sLogMutex );
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

//--------------------------------------

void getLockLog(ConsoleLogEntry *&log, U32 &size)  
{  
   consoleLogLocked = true;
   log = consoleLog.address();
   size = consoleLog.size();  
}

void unlockLog()
{
   consoleLogLocked = false;
}

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
         newText = gNewEvalState.globalVars->tabComplete(inputBuffer + completionBaseStart, completionBaseLen, forwardTab);
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

//------------------------------------------------------------------------------
static void log(const char *string)
{
   // Lock.
   MutexHandle mutex;
   if( sLogMutex )
      mutex.lock( sLogMutex, true );

   // Bail if we ain't logging.
   if (!consoleLogMode) 
   {
      return;
   }

   // In mode 1, we open, append, close on each log write.
   if ((consoleLogMode & 0x3) == 1) 
   {
      consoleLogFile.open(defLogFileName, FileStream::ReadWrite);
   }

   // Write to the log if its status is hunky-dory.
   if ((consoleLogFile.getStatus() == Stream::Ok) || (consoleLogFile.getStatus() == Stream::EOS)) 
   {
      consoleLogFile.setPosition(consoleLogFile.getStreamSize());
      // If this is the first write...
      if (newLogFile) 
      {
         // Make a header.
         Platform::LocalTime lt;
         Platform::getLocalTime(lt);
         char buffer[128];
         dSprintf(buffer, sizeof(buffer), "//-------------------------- %d/%d/%d -- %02d:%02d:%02d -----\r\n",
               lt.month + 1,
               lt.monthday,
               lt.year + 1900,
               lt.hour,
               lt.min,
               lt.sec);
         consoleLogFile.write(dStrlen(buffer), buffer);
         newLogFile = false;
         if (consoleLogMode & 0x4) 
         {
            // Dump anything that has been printed to the console so far.
            consoleLogMode -= 0x4;
            U32 size, line;
            ConsoleLogEntry *log;
            getLockLog(log, size);
            for (line = 0; line < size; line++) 
            {
               consoleLogFile.write(dStrlen(log[line].mString), log[line].mString);
               consoleLogFile.write(2, "\r\n");
            }
            unlockLog();
         }
      }
      // Now write what we came here to write.
      consoleLogFile.write(dStrlen(string), string);
      consoleLogFile.write(2, "\r\n");
   }

   if ((consoleLogMode & 0x3) == 1) 
   {
      consoleLogFile.close();
   }
}

//------------------------------------------------------------------------------

void cls( void )
{
   if(consoleLogLocked)
      return;
   consoleLogChunker.freeBlocks();
   consoleLog.setSize(0);
};

//------------------------------------------------------------------------------

#if defined( _MSC_VER )  
#include <windows.h>  

static void _outputDebugString(char* pString)  
{  
    // Format string.
    char* pBuffer = pString;  
    S32 stringLength = dStrlen(pString);  
    pBuffer += stringLength;  
    *pBuffer++ = '\r';  
    *pBuffer++ = '\n';  
    *pBuffer   = '\0';  
    stringLength = strlen(pString) + 1;  
    wchar_t *wstr = new wchar_t[stringLength];  
    dMemset( wstr, 0, stringLength );

    // Convert to wide string.
    Con::MultiByteToWideChar( CP_ACP, NULL, pString, -1, wstr, stringLength );  

    // Output string.
    Con::OutputDebugStringW( wstr );
    delete [] wstr;  
}
#endif

//------------------------------------------------------------------------------

static void _printf(ConsoleLogEntry::Level level, ConsoleLogEntry::Type type, const char* fmt)
{
   Con::active = false; 

   char buffer[4096];
   U32 offset = 0;
   if(gNewEvalState.traceOn && gNewEvalState.frames.size() > 0)
   {
      offset = gNewEvalState.frames.size() * 3;
      for(U32 i = 0; i < offset; i++)
         buffer[i] = ' ';
   }
   dSprintf(buffer + offset, sizeof(buffer) - offset, "%s", fmt);

   for(U32 i = 0; i < (U32)gConsumers.size(); i++)
      gConsumers[i](level, buffer);

   Platform::cprintf(buffer);

   if(logBufferEnabled || consoleLogMode)
   {
      char *pos = buffer;
      while(*pos)
      {
         if(*pos == '\t')
            *pos = '^';
         pos++;
      }
      pos = buffer;

      for(;;)
      {
         char *eofPos = dStrchr(pos, '\n');
         if(eofPos)
            *eofPos = 0;

         log(pos);
         if(logBufferEnabled && !consoleLogLocked)
         {
            ConsoleLogEntry entry;
            entry.mLevel  = level;
            entry.mType   = type;
            entry.mString = (const char *)consoleLogChunker.alloc(dStrlen(pos) + 1);
            dStrcpy(const_cast<char*>(entry.mString), pos);
            consoleLog.push_back(entry);
         }
         if(!eofPos)
            break;
         pos = eofPos + 1;
      }
   }

   Con::active = true;

#if defined( _MSC_VER )  
   _outputDebugString( buffer );  
#endif
}

//------------------------------------------------------------------------------

class ConPrinfThreadedEvent : public SimEvent
{
   ConsoleLogEntry::Level mLevel;
   ConsoleLogEntry::Type mType;
   char *mBuf;
public:
   ConPrinfThreadedEvent(ConsoleLogEntry::Level level = ConsoleLogEntry::Normal, ConsoleLogEntry::Type type = ConsoleLogEntry::General, const char *buf = NULL)
   {
      mLevel = level;
      mType = type;
      if(buf)
      {
         mBuf = (char*)dMalloc(dStrlen(buf)+1);
         dMemcpy((void*)mBuf, (void*)buf, dStrlen(buf));
         mBuf[dStrlen(buf)] = 0;
      }
      else
         mBuf = NULL;
   }
   ~ConPrinfThreadedEvent()
   {
      SAFE_FREE(mBuf);
   }
   virtual void process(SimObject *object)
   {
      if(mBuf)
      {
         switch(mLevel)
         {
         case ConsoleLogEntry::Normal :
            Con::printf(mBuf);
         break;
         case ConsoleLogEntry::Warning :
            Con::warnf(mType, mBuf);
         break;
         case ConsoleLogEntry::Error :
            Con::errorf(mType, mBuf);
         break;
         case ConsoleLogEntry::NUM_CLASS :
            Con::errorf("Unhandled case NUM_CLASS");
         break;
                 
         }
      }
   }
};

//------------------------------------------------------------------------------
void printf(const char* fmt,...)
{
   va_list argptr;
   va_start(argptr, fmt);
   char buf[8192];
   dVsprintf(buf, sizeof(buf), fmt, argptr);
   if(!isMainThread())
      Sim::postEvent(Sim::getRootGroup(), new ConPrinfThreadedEvent(ConsoleLogEntry::Normal, ConsoleLogEntry::General, buf), Sim::getTargetTime());
   else
      _printf(ConsoleLogEntry::Normal, ConsoleLogEntry::General, buf);
   va_end(argptr);
}

void warnf(ConsoleLogEntry::Type type, const char* fmt,...)
{
   va_list argptr;
   va_start(argptr, fmt);
   char buf[8192];
   dVsprintf(buf, sizeof(buf), fmt, argptr);
   if(!isMainThread())
      Sim::postEvent(Sim::getRootGroup(), new ConPrinfThreadedEvent(ConsoleLogEntry::Warning, type, buf), Sim::getTargetTime());
   else
      _printf(ConsoleLogEntry::Warning, type, buf);
   va_end(argptr);
}

void errorf(ConsoleLogEntry::Type type, const char* fmt,...)
{
   va_list argptr;
   va_start(argptr, fmt);
   char buf[8192];
   dVsprintf(buf, sizeof(buf), fmt, argptr);
   if(!isMainThread())
      Sim::postEvent(Sim::getRootGroup(), new ConPrinfThreadedEvent(ConsoleLogEntry::Error, type, buf), Sim::getTargetTime());
   else
      _printf(ConsoleLogEntry::Error, type, buf);
   va_end(argptr);
}

void warnf(const char* fmt,...)
{
   va_list argptr;
   va_start(argptr, fmt);
   char buf[8192];
   dVsprintf(buf, sizeof(buf), fmt, argptr);
   if(!isMainThread())
      Sim::postEvent(Sim::getRootGroup(), new ConPrinfThreadedEvent(ConsoleLogEntry::Warning, ConsoleLogEntry::General, buf), Sim::getTargetTime());
   else
      _printf(ConsoleLogEntry::Warning, ConsoleLogEntry::General, buf);
   va_end(argptr);
}

void errorf(const char* fmt,...)
{
   va_list argptr;
   va_start(argptr, fmt);
   char buf[8192];
   dVsprintf(buf, sizeof(buf), fmt, argptr);
   if(!isMainThread())
      Sim::postEvent(Sim::getRootGroup(), new ConPrinfThreadedEvent(ConsoleLogEntry::Error, ConsoleLogEntry::General, buf), Sim::getTargetTime());
   else
      _printf(ConsoleLogEntry::Error, ConsoleLogEntry::General, buf);
   va_end(argptr);
}

//---------------------------------------------------------------------------

void setVariable(const char *name, const char *value)
{
   if (name && name[0] == '$')
      name++;
   name = StringTable->insert(name);
   
   gNewEvalState.globalVars->setVariable(StringTable->insert(name), value);
}

void setValueVariable(const char *name, const ConsoleValuePtr& value)
{
   if (name && name[0] == '$')
      name++;
   name = StringTable->insert(name);
   
   gNewEvalState.globalVars->setValueVariable(StringTable->insert(name), value);
}

ConsoleValuePtr getConsoleValueVariable(const char *name)
{
   
   if (name && name[0] == '$')
      name++;
   name = StringTable->insert(name);
   return gNewEvalState.globalVars->getValueVariable(StringTable->insert(name));
}

void setLocalVariable(const char *name, const char *value)
{
   if (name && name[0] == '%')
      name++;
   name = StringTable->insert(name);
   
   //
   CodeBlockFunction* func = gNewEvalState.currentFrame.function;
   if (func)
   {
      for (Vector<CodeBlockFunction::Symbol>::iterator itr = func->vars.begin(), itrEnd = func->vars.end(); itr != itrEnd; itr++)
      {
         if (itr->varName == name)
         {
            gNewEvalState.globalVars->setVariable(name, value);
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
void addConsumer(ConsumerCallback consumer)
{
   gConsumers.push_back(consumer);
}

// dhc - found this empty -- trying what I think is a reasonable impl.
void removeConsumer(ConsumerCallback consumer)
{
   for(U32 i = 0; i < (U32)gConsumers.size(); i++)
      if (gConsumers[i] == consumer)
      { // remove it from the list.
         gConsumers.erase(i);
         break;
      }
}

void stripColorChars(char* line)
{
   char* c = line;
   char cp = *c;
   while (cp) 
   {
      if (cp < 18) 
      {
         // Could be a color control character; let's take a closer look.
         if ((cp != 8) && (cp != 9) && (cp != 10) && (cp != 13)) 
         {
            // Yep... copy following chars forward over this.
            char* cprime = c;
            char cpp;
            do 
            {
               cpp = *++cprime;
               *(cprime - 1) = cpp;
            } 
            while (cpp);
            // Back up 1 so we'll check this position again post-copy.
            c--;
         }
      }
      cp = *++c;
   }
}

ConsoleStringValuePtr getVariable(const char *name)
{
   if (name && name[0] == '$')
      name++;
   
   name = StringTable->insert(name);
   return gNewEvalState.globalVars->getVariable(StringTable->insert(name));
}

ConsoleStringValuePtr getLocalVariable(const char *name)
{
   if (name && name[0] == '%')
      name++;
   
   name = StringTable->insert(name);
   
   //
   CodeBlockFunction* func = gNewEvalState.currentFrame.function;
   if (func)
   {
      for (Vector<CodeBlockFunction::Symbol>::iterator itr = func->vars.begin(), itrEnd = func->vars.end(); itr; itr++)
      {
         if (itr->varName == name)
         {
            return gNewEvalState.globalVars->getVariable(name);
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
   Dictionary::Entry* entry = gNewEvalState.globalVars->lookup(name);
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
   Dictionary::Entry* entry = gNewEvalState.globalVars->lookup(name);
   if (entry)
   {
      return entry->getIntValue();
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
   Dictionary::Entry* entry = gNewEvalState.globalVars->lookup(name);
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
   
   gNewEvalState.globalVars->addVariable(name, t, dp);
   return true;
}

bool removeVariable(const char *name)
{
   if (name && name[0] == '$')
      name++;
   
   name = StringTable->lookup(name);
   return name!=0 && gNewEvalState.globalVars->removeVariable(name);
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

   CodeBlock *newCodeBlock = new CodeBlock();
   return newCodeBlock->compileExec(fileName, string, false, fileName ? -1 : 0);
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

      CodeBlock *newCodeBlock = new CodeBlock();
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
      return entry->execute(argc, argv, &gNewEvalState);
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
      return entry->execute(argc, argv, &gNewEvalState);
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
      return entry->execute(argc, argvValue, &gNewEvalState).getStringValue();
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
      return entry->execute(argc, argvValue, &gNewEvalState).getStringValue();
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

void setLogMode(S32 newMode)
{
   if ((newMode & 0x3) != (consoleLogMode & 0x3))
   {
      if (newMode && !consoleLogMode)
      {
         // Enabling logging when it was previously disabled.
         newLogFile = true;
      }
      if ((consoleLogMode & 0x3) == 2)
      {
         // Changing away from mode 2, must close logfile.
         consoleLogFile.close();
      }
      else if ((newMode & 0x3) == 2)
      {
         // Starting mode 2, must open logfile.
         consoleLogFile.open(defLogFileName, FileStream::Write);
      }
      consoleLogMode = newMode;
   }
}

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

void setData(S32 type, void *dataPtr, S32 index, const char* value, EnumTable *tbl)
{
   ConsoleBaseType *cbt = ConsoleBaseType::getType(type);
   AssertFatal(cbt, "Con::setData - could not resolve type ID!");
   cbt->setData((void *) (((const char *)dataPtr) + index * cbt->getTypeSize()), value, tbl);
}

ConsoleStringValuePtr getData(S32 type, void *dataPtr, S32 index, EnumTable *tbl)
{
   ConsoleBaseType *cbt = ConsoleBaseType::getType(type);
   AssertFatal(cbt, "Con::getData - could not resolve type ID!");
   return cbt->getData((void *) (((const char *)dataPtr) + index * cbt->getTypeSize()), tbl);
}
   
void setDataFromValue(S32 type, void *dataPtr, S32 index, const ConsoleValuePtr &value, EnumTable *tbl)
{
   ConsoleBaseType *cbt = ConsoleBaseType::getType(type);
   AssertFatal(cbt, "Con::setData - could not resolve type ID!");
   cbt->setDataFromValue((void *) (((const char *)dataPtr) + index * cbt->getTypeSize()), value, tbl);
}

ConsoleValuePtr getDataValue(S32 type, void *dataPtr, S32 index, EnumTable *tbl)
{
   ConsoleBaseType *cbt = ConsoleBaseType::getType(type);
   AssertFatal(cbt, "Con::getData - could not resolve type ID!");
   return cbt->getDataValue((void *) (((const char *)dataPtr) + index * cbt->getTypeSize()), tbl);
}

//------------------------------------------------------------------------------

StringTableEntry getModNameFromPath(const char *path)
{
   if(path == NULL || *path == 0)
      return NULL;

   char buf[1024];
   buf[0] = 0;

   if(path[0] == '/' || path[1] == ':')
   {
      // It's an absolute path
      const StringTableEntry exePath = Platform::getMainDotCsDir();
      U32 len = dStrlen(exePath);
      if(dStrnicmp(exePath, path, len) == 0)
      {
         const char *ptr = path + len + 1;
         const char *slash = dStrchr(ptr, '/');
         if(slash)
         {
            dStrncpy(buf, ptr, slash - ptr);
            buf[slash - ptr] = 0;
         }
         else
            return NULL;
      }
      else
         return NULL;
   }
   else
   {
      const char *slash = dStrchr(path, '/');
      if(slash)
      {
         dStrncpy(buf, path, slash - path);
         buf[slash - path] = 0;
      }
      else
         return NULL;
   }

   return StringTable->insert(buf);
}

//-----------------------------------------------------------------------------

typedef HashMap<StringTableEntry, StringTableEntry> typePathExpandoMap;
static typePathExpandoMap PathExpandos;

//-----------------------------------------------------------------------------

void addPathExpando( const char* pExpandoName, const char* pPath )
{
    // Sanity!
    AssertFatal( pExpandoName != NULL, "Expando name cannot be NULL." );
    AssertFatal( pPath != NULL, "Expando path cannot be NULL." );

    // Fetch expando name.
    StringTableEntry expandoName = StringTable->insert( pExpandoName );

    // Fetch the length of the path.
    S32 pathLength = dStrlen(pPath);

    char pathBuffer[1024];

    // Sanity!
    if ( pathLength == 0 || pathLength >= sizeof(pathBuffer) )
    {
        Con::warnf( "Cannot add path expando '%s' with path '%s' as the path is an invalid length.", pExpandoName, pPath );
        return;
    }

    // Strip repeat slashes.
    if ( !Con::stripRepeatSlashes(pathBuffer, pPath, sizeof(pathBuffer) ) )
    {
        Con::warnf( "Cannot add path expando '%s' with path '%s' as the path is an invalid length.", pExpandoName, pPath );
        return;
    }

    // Fetch new path length.
    pathLength = dStrlen(pathBuffer);

    // Sanity!
    if ( pathLength == 0 )
    {
        Con::warnf( "Cannot add path expando '%s' with path '%s' as the path is an invalid length.", pExpandoName, pPath );
        return;
    }

    // Remove any terminating slash.
    if (pathBuffer[pathLength-1] == '/')
        pathBuffer[pathLength-1] = 0;

    // Fetch expanded path.
    StringTableEntry expandedPath = StringTable->insert( pathBuffer );

    // Info.
    #if defined(TORQUE_DEBUG)
    Con::printf("Adding path expando of '%s' as '%s'.", expandoName, expandedPath );
    #endif

    // Find any existing path expando.
    typePathExpandoMap::iterator expandoItr = PathExpandos.find( pExpandoName );

    // Does the expando exist?
    if( expandoItr != PathExpandos.end() )
    {
        // Yes, so modify the path.
        expandoItr->value = expandedPath;
        return;
    }

    // Insert expando.
    PathExpandos.insert( expandoName, expandedPath );
}

//-----------------------------------------------------------------------------

StringTableEntry getPathExpando( const char* pExpandoName )
{
    // Sanity!
    AssertFatal( pExpandoName != NULL, "Expando name cannot be NULL." );

    // Fetch expando name.
    StringTableEntry expandoName = StringTable->insert( pExpandoName );

    // Find any existing path expando.
    typePathExpandoMap::iterator expandoItr = PathExpandos.find( expandoName );

    // Does the expando exist?
    if( expandoItr != PathExpandos.end() )
    {
        // Yes, so return it.
        return expandoItr->value;
    }

    // Not found.
    return NULL;
}

//-----------------------------------------------------------------------------

void removePathExpando( const char* pExpandoName )
{    
    // Sanity!
    AssertFatal( pExpandoName != NULL, "Expando name cannot be NULL." );

    // Fetch expando name.
    StringTableEntry expandoName = StringTable->insert( pExpandoName );

    // Find any existing path expando.
    typePathExpandoMap::iterator expandoItr = PathExpandos.find( expandoName );

    // Does the expando exist?
    if ( expandoItr == PathExpandos.end() )
    {
        // No, so warn.
        #if defined(TORQUE_DEBUG)
        Con::warnf("Removing path expando of '%s' but it does not exist.", expandoName );
        #endif
        return;
    }

    // Info.
    #if defined(TORQUE_DEBUG)
    Con::printf("Removing path expando of '%s' as '%s'.", expandoName, expandoItr->value );
    #endif
    // Remove expando.
    PathExpandos.erase( expandoItr );
}

//-----------------------------------------------------------------------------

bool isPathExpando( const char* pExpandoName )
{
    // Sanity!
    AssertFatal( pExpandoName != NULL, "Expando name cannot be NULL." );

    // Fetch expando name.
    StringTableEntry expandoName = StringTable->insert( pExpandoName );

    return PathExpandos.contains( expandoName );
}

//-----------------------------------------------------------------------------

U32 getPathExpandoCount( void )
{
    return PathExpandos.size();
}

//-----------------------------------------------------------------------------

StringTableEntry getPathExpandoKey( U32 expandoIndex )
{
    // Finish if index is out of range.
    if ( expandoIndex >= PathExpandos.size() )
        return NULL;

    // Find indexed iterator.
    typePathExpandoMap::iterator expandoItr = PathExpandos.begin();
    while( expandoIndex > 0 ) { ++expandoItr; --expandoIndex; }

    return expandoItr->key;
}

//-----------------------------------------------------------------------------

StringTableEntry getPathExpandoValue( U32 expandoIndex )
{
    // Finish if index is out of range.
    if ( expandoIndex >= PathExpandos.size() )
        return NULL;

    // Find indexed iterator.
    typePathExpandoMap::iterator expandoItr = PathExpandos.begin();
    while( expandoIndex > 0 ) { ++expandoItr; --expandoIndex; }

    return expandoItr->value;
}

//-----------------------------------------------------------------------------

bool expandPath( char* pDstPath, U32 size, const char* pSrcPath, const char* pWorkingDirectoryHint, const bool ensureTrailingSlash )
{ 
    char pathBuffer[2048];
    const char* pSrc = pSrcPath;
    char* pSlash;

    // Fetch leading character.
    const char leadingToken = *pSrc;

    // Fetch following token.
    const char followingToken = leadingToken != 0 ? pSrc[1] : 0;

    // Expando.
    if ( leadingToken == '^' )
    {
        // Initial prefix search.
        const char* pPrefixSrc = pSrc+1;
        char* pPrefixDst = pathBuffer;

        // Search for end of expando.
        while( *pPrefixSrc != '/' && *pPrefixSrc != 0 )
        {
            // Copy prefix character.
            *pPrefixDst++ = *pPrefixSrc++;
        }

        // Yes, so terminate the expando string.
        *pPrefixDst = 0;

        // Fetch the expando path.
        StringTableEntry expandoPath = getPathExpando(pathBuffer);
               
        // Does the expando exist?
        if( expandoPath == NULL )
        {
            // No, so error.
            Con::errorf("expandPath() : Could not find path expando '%s' for path '%s'.", pathBuffer, pSrcPath );

            // Are we ensuring the trailing slash?
            if ( ensureTrailingSlash )
            {
                // Yes, so ensure it.
                Con::ensureTrailingSlash( pDstPath, pSrcPath );
            }
            else
            {
                // No, so just use the source path.
                dStrcpy( pDstPath, pSrcPath );
            }

            return false;
        }

        // Skip the expando and the following slash.
        pSrc += dStrlen(pathBuffer) + 1;

        // Format the output path.
        dSprintf( pathBuffer, sizeof(pathBuffer), "%s/%s", expandoPath, pSrc );

        // Are we ensuring the trailing slash?
        if ( ensureTrailingSlash )
        {
            // Yes, so ensure it.
            Con::ensureTrailingSlash( pathBuffer, pathBuffer );
        }

        // Strip repeat slashes.
        Con::stripRepeatSlashes( pDstPath, pathBuffer, size );

        return true;
    }

    // Script-Relative.
    if ( leadingToken == '.' )
    {
        // Fetch the code-block file-path.
       StringTableEntry codeblockFullPath = NULL;
       CodeBlockEvalState *state = &gNewEvalState;
       if (state->currentFrame.code)
       {
          codeblockFullPath = state->currentFrame.code->fullPath;
       }

        // Do we have a code block full path?
        if( codeblockFullPath == NULL )
        {
            // No, so error.
            Con::errorf("expandPath() : Could not find relative path from code-block for path '%s'.", pSrcPath );

            // Are we ensuring the trailing slash?
            if ( ensureTrailingSlash )
            {
                // Yes, so ensure it.
                Con::ensureTrailingSlash( pDstPath, pSrcPath );
            }
            else
            {
                // No, so just use the source path.
                dStrcpy( pDstPath, pSrcPath );
            }

            return false;
        }

        // Yes, so use it as the prefix.
        dStrncpy(pathBuffer, codeblockFullPath, sizeof(pathBuffer) - 1);

        // Find the final slash in the code-block.
        pSlash = dStrrchr(pathBuffer, '/');

        // Is this a parent directory token?
        if ( followingToken == '.' )
        {
            // Yes, so terminate after the slash so we include it.
            pSlash[1] = 0;
        }
        else
        {
            // No, it's a current directory token so terminate at the slash so we don't include it.
            pSlash[0] = 0;

            // Skip the current directory token.
            pSrc++;
        }
           
        // Format the output path.
        dStrncat(pathBuffer, "/", sizeof(pathBuffer) - 1 - strlen(pathBuffer));
        dStrncat(pathBuffer, pSrc, sizeof(pathBuffer) - 1 - strlen(pathBuffer));

        // Are we ensuring the trailing slash?
        if ( ensureTrailingSlash )
        {
            // Yes, so ensure it.
            Con::ensureTrailingSlash( pathBuffer, pathBuffer );
        }

        // Strip repeat slashes.
        Con::stripRepeatSlashes( pDstPath, pathBuffer, size );

        return true;
    }

    // All else.

    //Using a special case here because the code below barfs on trying to build a full path for apk reading
 #ifdef TORQUE_OS_ANDROID
       if (leadingToken == '/' || strstr(pSrcPath, "/") == NULL)
          Platform::makeFullPathName( pSrcPath, pathBuffer, sizeof(pathBuffer), pWorkingDirectoryHint );
       else
          dSprintf(pathBuffer, sizeof(pathBuffer), "/%s", pSrcPath);
#else
      Platform::makeFullPathName( pSrcPath, pathBuffer, sizeof(pathBuffer), pWorkingDirectoryHint );
#endif

    // Are we ensuring the trailing slash?
    if ( ensureTrailingSlash )
    {
        // Yes, so ensure it.
        Con::ensureTrailingSlash( pathBuffer, pathBuffer );
    }

    // Strip repeat slashes.
    Con::stripRepeatSlashes( pDstPath, pathBuffer, size );

    return true;
}

//-----------------------------------------------------------------------------

bool isBasePath( const char* SrcPath, const char* pBasePath )
{
    char expandBuffer[1024];
    Con::expandPath( expandBuffer, sizeof(expandBuffer), SrcPath );
    return dStrnicmp(pBasePath, expandBuffer, dStrlen( pBasePath ) ) == 0;
}

//-----------------------------------------------------------------------------

void collapsePath( char* pDstPath, U32 size, const char* pSrcPath, const char* pWorkingDirectoryHint )
{
    // Check path against expandos.  If there are multiple matches, choose the
    // expando that produces the shortest relative path.

    char pathBuffer[2048];

    // Fetch expando count.
    const U32 expandoCount = getPathExpandoCount();

    // Iterate expandos.
    U32 expandoRelativePathLength = U32_MAX;
    for( U32 expandoIndex = 0; expandoIndex < expandoCount; ++expandoIndex )
    {
        // Fetch expando value (path).
        StringTableEntry expandoValue = getPathExpandoValue( expandoIndex );

        // Skip if not the base path.
        if ( !isBasePath( pSrcPath, expandoValue ) )
            continue;

        // Fetch path relative to expando path.
        StringTableEntry relativePath = Platform::makeRelativePathName( pSrcPath, expandoValue );

        // If the relative path is simply a period
        if ( relativePath[0] == '.' )
            relativePath++;

        if ( dStrlen(relativePath) > expandoRelativePathLength )
        {
            // This expando covers less of the path than any previous one found.
            // We will keep the previous one.
            continue;
        }

        // Keep track of the relative path length
        expandoRelativePathLength = dStrlen(relativePath);

        // Fetch expando key (name).
        StringTableEntry expandoName = getPathExpandoKey( expandoIndex );

        // Format against expando.
        dSprintf( pathBuffer, sizeof(pathBuffer), "^%s/%s", expandoName, relativePath );
    }

    // Check if we've found a suitable expando
    if ( expandoRelativePathLength != U32_MAX )
    {
        // Strip repeat slashes.
        Con::stripRepeatSlashes( pDstPath, pathBuffer, size );

        return;
    }

    // Fetch the working directory.
    StringTableEntry workingDirectory = pWorkingDirectoryHint != NULL ? pWorkingDirectoryHint : Platform::getCurrentDirectory();

    // Fetch path relative to current directory.
    StringTableEntry relativePath = Platform::makeRelativePathName( pSrcPath, workingDirectory );

    // If the relative path is simply a period
    if ( relativePath[0] == '.'  && relativePath[1] != '.' )
        relativePath++;

    // Format against expando.
    dSprintf( pathBuffer, sizeof(pathBuffer), "%s/%s", workingDirectory, relativePath );

    // Strip repeat slashes.
    Con::stripRepeatSlashes( pDstPath, pathBuffer, size );
}

//-----------------------------------------------------------------------------

void ensureTrailingSlash( char* pDstPath, const char* pSrcPath )
{
    // Copy to target.
    dStrcpy( pDstPath, pSrcPath );

    // Find trailing character index.
    S32 trailIndex = dStrlen(pDstPath);

    // Ignore if empty string.
    if ( trailIndex == 0 )
        return;

    // Finish if the trailing slash already exists.
    if ( pDstPath[trailIndex-1] == '/' )
        return;

    // Add trailing slash.
    pDstPath[trailIndex++] = '/';
    pDstPath[trailIndex] = 0;
}

//-----------------------------------------------------------------------------

bool stripRepeatSlashes( char* pDstPath, const char* pSrcPath, S32 dstSize )
{
    // Note original destination.
    char* pOriginalDst = pDstPath;

    // Reset last source character.
    char lastSrcChar = 0;

    // Search source...
    while ( dstSize > 0 )
    {
        // Fetch characters.
        const char srcChar = *pSrcPath++;

        // Do we have a repeat slash?
        if ( srcChar == '/' && lastSrcChar == '/' )
        {
            // Yes, so skip it.
            continue;
        }

        // No, so copy character.
        *pDstPath++ = srcChar;

        // Finish if end of source.
        if ( srcChar == 0 )
            return true;

        // Reduce room left in destination.
        dstSize--;

        // Set last character.
        lastSrcChar = srcChar;
    }

    // Terminate the destination string as we ran out of room.
    *pOriginalDst = 0;

    // Fail!
    return false;
}

} // end of Console namespace


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
            ptr.value.refValue->read(s, serializationState);
            ptr.AddRef();
        }
        serializationState.loadedValues.push_back(&ptr);
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
           Con::printf("Var[%i] is %s", i, ptr.value.refValue->getType()->getTypeName());
            S32 idx = serializationState.getSavedObjectIdx(ptr.value.refValue);
            if (idx == -1)
            {
               s.write(varTypeIds[i]);
               ptr.value.refValue->write(s, serializationState);
               serializationState.addSavedObject(ptr.value.refValue);
            }
            else
            {
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
         return Sim::findObject(value.ival);
      case TypeInternalFloat:
         return Sim::findObject((S32)value.fval);
      default:
         return Sim::findObject(getTempStringValue());
   }
}

#include "io/memStream.h"

void testStackWrite()
{
    Vector<ConsoleValuePtr> stack;
    
    ConsoleValuePtr var1;
    ConsoleValuePtr var2;
    ConsoleValuePtr var3;
    
    ConsoleValuePtr var4;
    ConsoleValuePtr var5;
    ConsoleValuePtr var6;
   ConsoleValuePtr var7;
   ConsoleValuePtr var8;
   
   ConsoleSerializationState state;
   
    U8 buffer[16 * 1024];
    MemStream m(sizeof(buffer), buffer, true, true);
    
    ConsoleBaseType *type = ConsoleBaseType::getTypeByName("TypeS32Vector");
    var1.type = type->getTypeID();
    var1.value.refValue = type->createReferenceCountedValue();
    var1.AddRef();
    m.setPosition(0);
    m.writeLongString(4096, "1 2 3 4 5");
    m.setPosition(0);
    var1.value.refValue->read(m, state);
    
    type = ConsoleBaseType::getTypeByName("TypeString");
    var2.type = type->getTypeID();
    var2.value.refValue = type->createReferenceCountedValue();
    var2.AddRef();
    m.setPosition(0);
    m.writeLongString(4096, "This is a string");
    m.setPosition(0);
    var2.value.refValue->read(m, state);
    
    type = ConsoleBaseType::getTypeByName("TypeBufferString");
    var3.type = type->getTypeID();
    var3.value.refValue = type->createReferenceCountedValue();
    var3.AddRef();
    m.setPosition(0);
    const char *bufferStr = "Buffered string test";
    U32 len = dStrlen(bufferStr)+1;
    m.write(len);
    m.write(len, bufferStr);
    m.setPosition(0);
    var3.value.refValue->read(m, state);
    
    var4.type = ConsoleValue::TypeInternalNull;
    var5.type = ConsoleValue::TypeInternalInt;
    var5.value.ival = 4096;
    var6.type = ConsoleValue::TypeInternalFloat;
    var6.value.fval = 4.096;
    var7.type = ConsoleValue::TypeInternalStringTableEntry;
    var7.value.string = StringTable->insert("STEString");
   
    var8 = var3;
    
    stack.push_back(var1);
    stack.push_back(var2);
    stack.push_back(var3);
    stack.push_back(var4);
    stack.push_back(var5);
    stack.push_back(var6);
   stack.push_back(var7);
   stack.push_back(var8);
   
   ConsoleSerializationState serializationState;
    
    
    m.setPosition(0);
    ConsoleValuePtr::writeStack(m, serializationState, stack);
    stack.clearAndReset();
    serializationState.clear();
    
    m.setPosition(0);
    ConsoleValuePtr::readStack(m, serializationState, stack);
    
    for (U32 i=0; i<stack.size(); i++)
    {
       Con::printf("Stack[%u] type == %u value == %s refCount == %i", i, stack[i].type, stack[i].getTempStringValue(), ConsoleValue::isRefType(stack[i].type) ? stack[i].value.refValue->refCount : 0);
    }
    stack.clearAndReset();
    
    
    
}


ConsoleFunction(testExecute, void, 2, 2, "")
{
   SimObject* frodoObject = argv[1].getSimObject();
   if (frodoObject)
   {
      Con::executef(frodoObject, "testFunc", "string", 1, 2);
   }
}

ConsoleFunction(createArray, ConsoleValuePtr, 1, 1, "")
{
   return ConsoleValuePtr(ConsoleArrayValue::fromValues(0, NULL));
}

ConsoleMethod(SimObject, testMethod, ConsoleValuePtr, 2, 2, "")
{
   ConsoleValuePtr ptr;
   ptr.setValue(ConsoleStringValuePtr(object->getName()));
   return ptr;
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
            returnValue.setValue(entry->execute(mArgc, mArgv, &gNewEvalState));
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
         returnValue.setValue(entry->execute(mArgc, mArgv, &gNewEvalState));
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

ConsoleStringValuePtr::ConsoleStringValuePtr(const char *str)
{
   value = ConsoleStringValue::fromString(str);
   value->addRef();
}

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


