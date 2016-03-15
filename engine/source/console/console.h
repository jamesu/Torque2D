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

#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif
#ifndef _BITSET_H_
#include "collection/bitSet.h"
#endif
#ifndef _CONSOLEVALUE_H_
#include "console/consoleValue.h"
#endif

#include <stdarg.h>

#include "memory/safeDelete.h"

class SimObject;
struct EnumTable;
class Namespace;

enum StringTableConstants
{
   StringTagPrefixByte = 0x01 ///< Magic value prefixed to tagged strings.
};

/// Scripting engine representation of an enum.
///
/// This data structure is used by the scripting engine
/// to expose enumerations to the scripting language. It
/// acts to relate named constants to integer values, just
/// like an enum in C++.
struct EnumTable
{
   /// Number of enumerated items in the table.
   S32 size;

   /// This represents a specific item in the enumeration.
   struct Enums
   {
      S32 index;        ///< Index label maps to.
      const char *label;///< Label for this index.
   };

   Enums *table;

   /// Constructor.
   ///
   /// This sets up the EnumTable with predefined data.
   ///
   /// @param sSize  Size of the table.
   /// @param sTable Pointer to table of Enums.
   ///
   /// @see gLiquidTypeTable
   /// @see gAlignTable
   EnumTable(S32 sSize, Enums *sTable)
      { size = sSize; table = sTable; }
};

typedef const char *StringTableEntry;

/// @defgroup tsScripting TorqueScript Bindings
/// TorqueScrit bindings

/// @defgroup console_callbacks Scripting Engine Callbacks
/// @ingroup tsScripting
///
/// The scripting engine makes heavy use of callbacks to represent
/// function exposed to the scripting language. StringCallback,
/// IntCallback, FloatCallback, VoidCallback, and BoolCallback all
/// represent exposed script functions returning different types.
///
/// ConsumerCallback is used with the function Con::addConsumer; functions
/// registered with Con::addConsumer are called whenever something is outputted
/// to the console. For instance, the TelnetConsole registers itself with the
/// console so it can echo the console over the network.
///
/// @note Callbacks to the scripting language - for instance, onExit(), which is
///       a script function called when the engine is shutting down - are handled
///       using Con::executef() and kin.
/// @{

///
typedef ConsoleValuePtr (*ValueCallback)(SimObject *obj, S32 argc, ConsoleValuePtr argv[]);
typedef ConsoleStringValuePtr (*StringCallback)(SimObject *obj, S32 argc, ConsoleValuePtr argv[]);
typedef S32             (*IntCallback)(SimObject *obj, S32 argc, ConsoleValuePtr argv[]);
typedef F32           (*FloatCallback)(SimObject *obj, S32 argc, ConsoleValuePtr argv[]);
typedef void           (*VoidCallback)(SimObject *obj, S32 argc, ConsoleValuePtr argv[]); // We have it return a value so things don't break..
typedef bool           (*BoolCallback)(SimObject *obj, S32 argc, ConsoleValuePtr argv[]);

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
   /// Various configuration constants.
   enum Constants 
   {
      /// This is the version number associated with DSO files.
      ///
      /// If you make any changes to the way the scripting language works
      /// (such as DSO format changes, adding/removing op-codes) that would
      /// break compatibility, then you should increment this.
      ///
      /// If you make a really major change, increment it to the next multiple
      /// of ten.
      ///
      /// 12/29/04 - BJG - 33->34 Removed some opcodes, part of namespace upgrade.
      /// 12/30/04 - BJG - 34->35 Reordered some things, further general shuffling.
      /// 11/03/05 - BJG - 35->36 Integrated new debugger code.
      //  09/08/06 - THB - 36->37 New opcode for internal names
      //  09/15/06 - THB - 37->38 Added unit conversions
      //  11/23/06 - THB - 38->39 Added recursive internal name operator
      //  02/15/07 - THB - 39->40 Bumping to 40 for TGB since the console has been majorly hacked without the version number being bumped
      //  02/16/07 - THB - 40->41 newmsg operator
      //  02/16/07 - PAUP - 41->42 DSOs are read with a pointer before every string(ASTnodes changed). Namespace and HashTable revamped
      //  05/17/10 - Luma - 42-43 Adding proper sceneObject physics flags, fixes in general
      //  02/07/13 - JU   - 43->44 Expanded the width of stringtable entries to  64bits
      //  12/01/15 - JU   - 44->45 Complete revamp of bytecode
      DSOVersion = 45,
      MaxLineLength = 512,  ///< Maximum length of a line of console input.
      MaxDataTypes = 256    ///< Maximum number of registered data types.
   };

   /// @name Control Functions
   ///
   /// The console must be initialized and shutdown appropriately during the
   /// lifetime of the app. These functions are used to manage this behavior.
   ///
   /// @note Torque deals with this aspect of console management, so you don't need
   ///       to call these functions in normal usage of the engine.
   /// @{

   /// Initializes the console.
   ///
   /// This performs the following steps:
   ///   - Calls Namespace::init() to initialize the scripting namespace hierarchy.
   ///   - Calls ConsoleConstructor::setup() to initialize globally defined console
   ///     methods and functions.
   ///   - Registers some basic global script variables.
   ///   - Calls AbstractClassRep::init() to initialize Torque's class database.
   ///   - Registers some basic global script functions that couldn't usefully
   ///     be defined anywhere else.
   void init();

   /// Shuts down the console.
   ///
   /// This performs the following steps:
   ///   - Closes the console log file.
   ///   - Calls Namespace::shutdown() to shut down the scripting namespace hierarchy.
   void shutdown();

   /// Is the console active at this time?
   bool isActive();
   
   /// @}

   /// Returns true when called from the main thread, false otherwise
   bool isMainThread();
};

extern void expandEscape(char *dest, const char *src);
extern bool collapseEscape(char *buf);
extern U32 HashPointer(StringTableEntry ptr);


#include "console/consoleOutput.h"
#include "console/consoleTypes.h"
#include "console/consoleConstructor.h"
#include "console/consoleExec.h"
#include "console/consolePaths.h"



#endif
