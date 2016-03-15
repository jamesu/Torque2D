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
#include "console/codeblockEvalState.h"
#include "sim/simBase.h"
#include "io/fileStream.h"

namespace Con
{
   
static Vector<ConsumerCallback> gConsumers(__FILE__, __LINE__);
static DataChunker consoleLogChunker;
static Vector<ConsoleLogEntry> consoleLog(__FILE__, __LINE__);
static bool consoleLogLocked;

static FileStream consoleLogFile;
static const char *defLogFileName = "console.log";
static S32 consoleLogMode = 0;
static Mutex* sLogMutex;
static bool newLogFile;
static const char *logFileName;
static bool logBufferEnabled=true;


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
   char buffer[4096];
   U32 offset = 0;
   CodeBlockEvalState* evalState = CodeBlockEvalState::getCurrent();
   if(evalState->traceOn && evalState->frames.size() > 0)
   {
      offset = evalState->frames.size() * 3;
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

//------------------------------------------------------------------------------

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
   
//------------------------------------------------------------------------------
   
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
   
void initLog()
{
   logFileName                   = NULL;
   newLogFile                    = true;
   sLogMutex                     = new Mutex;
}
   
void shutdownLog()
{
   consoleLogFile.close();
   SAFE_DELETE(sLogMutex);
}
   
void addLogVariables()
{
   addVariable("Con::logBufferEnabled", TypeBool, &logBufferEnabled);
}

}
