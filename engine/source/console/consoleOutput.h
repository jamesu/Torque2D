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

#ifndef _CONSOLEOUTPUT_H_
#define _CONSOLEOUTPUT_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif
#ifndef _CONSOLEVALUE_H_
#include "console/consoleValue.h"
#endif
#ifndef _CONSOLE_H_
#include "console/console.h"
#endif

/// Represents an entry in the log.
struct ConsoleLogEntry
{
   /// This field indicates the severity of the log entry.
   ///
   /// Log entries are filtered and displayed differently based on
   /// their severity. Errors are highlighted red, while normal entries
   /// are displayed as normal text. Often times, the engine will be
   /// configured to hide all log entries except warnings or errors,
   /// or to perform a special notification when it encounters an error.
   enum Level
   {
      Normal = 0,
      Warning,
      Error,
      NUM_CLASS
   } mLevel;
   
   /// Used to associate a log entry with a module.
   ///
   /// Log entries can come from different sources; for instance,
   /// the scripting engine, or the network code. This allows the
   /// logging system to be aware of where different log entries
   /// originated from.
   enum Type
   {
      General = 0,
      Assert,
      Script,
      GUI,
      Network,
      NUM_TYPE
   } mType;
   
   /// Indicates the actual log entry.
   ///
   /// This contains a description of the event being logged.
   /// For instance, "unable to access file", or "player connected
   /// successfully", or nearly anything else you might imagine.
   ///
   /// Typically, the description should contain a concise, descriptive
   /// string describing whatever is being logged. Whenever possible,
   /// include useful details like the name of the file being accessed,
   /// or the id of the player or GuiControl, so that if a log needs
   /// to be used to locate a bug, it can be done as painlessly as
   /// possible.
   const char *mString;
};

typedef const char *StringTableEntry;

typedef void (*ConsumerCallback)(ConsoleLogEntry::Level level, const char *consoleLine);
/// @}

/// This namespace contains the core of the console functionality.
///
/// @section con_intro Introduction
///
/// The console is a key part of Torque's architecture. It allows direct run-time control
/// of many aspects of the engine.
///
/// @nosubgrouping
namespace Con
{
   /// @name Console Consumers
   ///
   /// The console distributes its output through Torque by using
   /// consumers. Every time a new line is printed to the console,
   /// all the ConsumerCallbacks registered using addConsumer are
   /// called, in order.
   ///
   /// @note The GuiConsole control, which provides the on-screen
   ///       in-game console, uses a different technique to render
   ///       the console. It calls getLockLog() to lock the Vector
   ///       of on-screen console entries, then it renders them as
   ///       needed. While the Vector is locked, the console will
   ///       not change the Vector. When the GuiConsole control is
   ///       done with the console entries, it calls unlockLog()
   ///       to tell the console that it is again safe to modify
   ///       the Vector.
   ///
   /// @see TelnetConsole
   /// @see TelnetDebugger
   /// @see WinConsole
   /// @see MacCarbConsole
   /// @see iPhoneConsole
   /// @see StdConsole
   /// @see ConsoleLogger
   ///
   /// @{
   void addConsumer(ConsumerCallback cb);
   void removeConsumer(ConsumerCallback cb);
   /// @}
   
   /// @name Miscellaneous
   /// @{
   
   /// Remove color marking information from a string.
   ///
   /// @note It does this in-place, so be careful! It may
   ///       potentially blast data if you're not careful.
   ///       When in doubt, make a copy of the string first.
   void stripColorChars(char* line);
   
   /// @name Console Output
   ///
   /// These functions process the formatted string and pass it to all the ConsumerCallbacks that are
   /// currently registered. The console log file and the console window callbacks are installed by default.
   ///
   /// @see addConsumer()
   /// @see removeConsumer()
   /// @{
   
   /// @param _format   A stdlib printf style formatted out put string
   /// @param ...       Variables to be written
   void printf(const char *_format, ...);
   
   /// @note The console window colors warning text as LIGHT GRAY.
   /// @param _format   A stdlib printf style formatted out put string
   /// @param ...       Variables to be written
   void warnf(const char *_format, ...);
   
   /// @note The console window colors warning text as RED.
   /// @param _format   A stdlib printf style formatted out put string
   /// @param ...       Variables to be written
   void errorf(const char *_format, ...);
   
   /// @note The console window colors warning text as LIGHT GRAY.
   /// @param type      Allows you to associate the warning message with an internal module.
   /// @param _format   A stdlib printf style formatted out put string
   /// @param ...       Variables to be written
   /// @see Con::warnf()
   void warnf(ConsoleLogEntry::Type type, const char *_format, ...);
   
   /// @note The console window colors warning text as RED.
   /// @param type      Allows you to associate the warning message with an internal module.
   /// @param _format   A stdlib printf style formatted out put string
   /// @param ...       Variables to be written
   /// @see Con::errorf()
   void errorf(ConsoleLogEntry::Type type, const char *_format, ...);
   
   /// clear the console log
   void cls( void );
   
   /// Prints a separator to the console.
   inline void printSeparator( void ) { printf("--------------------------------------------------------------------------------"); }
   
   /// Prints a separator to the console.
   inline void printBlankLine( void ) { printf(""); }
   
   /// @}
   
   /// @name Logging
   /// @{
   
   void getLockLog(ConsoleLogEntry * &log, U32 &size);
   void unlockLog(void);
   void setLogMode(S32 mode);
   
   void initLog();
   void shutdownLog();
   void addLogVariables();
   
   /// @}
};

#endif
