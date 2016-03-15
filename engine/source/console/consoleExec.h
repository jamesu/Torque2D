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
#ifndef _CONSOLEEXEC_H_
#define _CONSOLEEXEC_H_

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
   /// Indicates that warnings about undefined script variables should be displayed.
   ///
   /// @note This is set and controlled by script.
   extern bool gWarnUndefinedScriptVariables;
   
   /// Returns true if fn is a global scripting function.
   ///
   /// This looks in the global namespace. It also checks to see if fn
   /// is in the StringTable; if not, it returns false.
   bool isFunction(const char *fn);
   
   /// This is the basis for tab completion in the console.
   ///
   /// @note This is an internally used function. You probably don't
   ///       care much about how this works.
   ///
   /// This function does some basic parsing to try to ascertain the namespace in which
   /// we are attempting to do tab completion, then bumps control off to the appropriate
   /// tabComplete function, either in SimObject or Namespace.
   ///
   /// @param  inputBuffer     Pointer to buffer containing starting data, or last result.
   /// @param  cursorPos       Location of cursor in this buffer. This is used to indicate
   ///                         what part of the string should be kept and what part should
   ///                         be advanced to the next match if any.
   /// @param  maxResultLength Maximum amount of result data to put into inputBuffer. This
   ///                         is capped by MaxCompletionBufferSize.
   /// @param  forwardTab      Should we go forward to next match or backwards to previous
   ///                         match? True indicates forward.
   U32 tabComplete(char* inputBuffer, U32 cursorPos, U32 maxResultLength, bool forwardTab);
   
   /// @}
   
   
   /// @name Variable Management
   /// @{
   
   /// Add a console variable that references the value of a variable in C++ code.
   ///
   /// If a value is assigned to the console variable the C++ variable is updated,
   /// and vice-versa.
   ///
   /// @param name    Global console variable name to create
   /// @param type    The type of the C++ variable; see the ConsoleDynamicTypes enum for a complete list.
   /// @param pointer Pointer to the variable.
   /// @see ConsoleDynamicTypes
   bool addVariable(const char *name, S32 type, void *pointer);
   
   /// Remove a console variable.
   ///
   /// @param name   Global console variable name to remove
   /// @return       true if variable existed before removal.
   bool removeVariable(const char *name);
   
   /// Assign a string value to a locally scoped console variable
   ///
   /// @note The context of the variable is determined by gEvalState; that is,
   ///       by the currently executing code.
   ///
   /// @param name   Local console variable name to set
   /// @param value  String value to assign to name
   void setLocalVariable(const char *name, const char *value);
   
   /// Retrieve the string value to a locally scoped console variable
   ///
   /// @note The context of the variable is determined by gEvalState; that is,
   ///       by the currently executing code.
   ///
   /// @param name   Local console variable name to get
   ConsoleStringValuePtr getLocalVariable(const char *name);
   
   /// @}
   
   /// @name Global Variable Accessors
   /// @{
   /// Assign a string value to a global console variable
   /// @param name   Global console variable name to set
   /// @param value  String value to assign to this variable.
   void setVariable(const char *name, const char *value);
   
   /// Assign a ConsoleValuePtr value to a global console variable
   void setValueVariable(const char *name, const ConsoleValuePtr& value);
   
   /// Gets a ConsoleValuePtr value from a global console variable
   ConsoleValuePtr getConsoleValueVariable(const char *name);
   
   /// Retrieve the string value of a global console variable
   /// @param name   Global Console variable name to query
   /// @return       The string value of the variable or "" if the variable does not exist.
   ConsoleStringValuePtr getVariable(const char *name);
   
   /// Same as setVariable(), but for bools.
   void setBoolVariable (const char *name,bool var);
   
   /// Same as getVariable(), but for bools.
   ///
   /// @param  name  Name of the variable.
   /// @param  def   Default value to supply if no matching variable is found.
   bool getBoolVariable (const char *name,bool def = false);
   
   /// Same as setVariable(), but for ints.
   void setIntVariable  (const char *name,S32 var);
   
   /// Same as getVariable(), but for ints.
   ///
   /// @param  name  Name of the variable.
   /// @param  def   Default value to supply if no matching variable is found.
   S32  getIntVariable  (const char *name,S32 def = 0);
   
   /// Same as setVariable(), but for floats.
   void setFloatVariable(const char *name,F32 var);
   
   /// Same as getVariable(), but for floats.
   ///
   /// @param  name  Name of the variable.
   /// @param  def   Default value to supply if no matching variable is found.
   F32  getFloatVariable(const char *name,F32 def = .0f);
   
   /// @}
   
   /// @name Global Function Registration
   /// @{
   
   /// Register a C++ function with the console making it a global function callable from the scripting engine.
   ///
   /// @param name      Name of the new function.
   /// @param cb        Pointer to the function implementing the scripting call; a console callback function returning a specific type value.
   /// @param usage     Documentation for this function. @ref console_autodoc
   /// @param minArgs   Minimum number of arguments this function accepts
   /// @param maxArgs   Maximum number of arguments this function accepts
   void addCommand(const char *name, StringCallback cb, const char *usage, S32 minArgs, S32 maxArgs);
   
   void addCommand(const char *name, IntCallback    cb,    const char *usage, S32 minArgs, S32 maxArg); ///< @copydoc addCommand(const char *, StringCallback, const char *, S32, S32)
   void addCommand(const char *name, FloatCallback  cb,  const char *usage, S32 minArgs, S32 maxArgs); ///< @copydoc addCommand(const char *, StringCallback, const char *, S32, S32)
   void addCommand(const char *name, VoidCallback   cb,   const char *usage, S32 minArgs, S32 maxArgs); ///< @copydoc addCommand(const char *, StringCallback, const char *, S32, S32)
   void addCommand(const char *name, BoolCallback   cb,   const char *usage, S32 minArgs, S32 maxArgs); ///< @copydoc addCommand(const char *, StringCallback, const char *, S32, S32)
   void addCommand(const char *name, ValueCallback   cb,   const char *usage, S32 minArgs, S32 maxArgs); ///< @copydoc addCommand(const char *, ValueCallback, const char *, S32, S32)
   /// @}
   
   /// @name Namespace Function Registration
   /// @{
   
   /// Register a C++ function with the console making it callable
   /// as a method of the given namespace from the scripting engine.
   ///
   /// @param nameSpace Name of the namespace to associate the new function with; this is usually the name of a class.
   /// @param name      Name of the new function.
   /// @param cb        Pointer to the function implementing the scripting call; a console callback function returning a specific type value.
   /// @param usage     Documentation for this function. @ref console_autodoc
   /// @param minArgs   Minimum number of arguments this function accepts
   /// @param maxArgs   Maximum number of arguments this function accepts
   void addCommand(const char *nameSpace, const char *name,StringCallback cb, const char *usage, S32 minArgs, S32 maxArgs);
   void addCommand(const char *nameSpace, const char *name,IntCallback cb,    const char *usage, S32 minArgs, S32 maxArgs); ///< @copydoc addCommand(const char*, const char *, StringCallback, const char *, S32, S32)
   void addCommand(const char *nameSpace, const char *name,FloatCallback cb,  const char *usage, S32 minArgs, S32 maxArgs); ///< @copydoc addCommand(const char*, const char *, StringCallback, const char *, S32, S32)
   void addCommand(const char *nameSpace, const char *name,VoidCallback cb,   const char *usage, S32 minArgs, S32 maxArgs); ///< @copydoc addCommand(const char*, const char *, StringCallback, const char *, S32, S32)
   void addCommand(const char *nameSpace, const char *name,BoolCallback cb,   const char *usage, S32 minArgs, S32 maxArgs); ///< @copydoc addCommand(const char*, const char *, StringCallback, const char *, S32, S32)
   void addCommand(const char *nameSpace, const char *name,ValueCallback cb,   const char *usage, S32 minArgs, S32 maxArgs); ///< @copydoc addCommand(const char*, const char *, ValueCallback, const char *, S32, S32)
   
   /// @}
   
   /// @name Special Purpose Registration
   ///
   /// These are special-purpose functions that exist to allow commands to be grouped, so
   /// that when we generate console docs, they can be more meaningfully presented.
   ///
   /// @ref console_autodoc "Click here for more information about console docs and grouping."
   ///
   /// @{
   
   void markCommandGroup (const char * nsName, const char *name, const char *usage=NULL);
   void beginCommandGroup(const char * nsName, const char *name, const char *usage);
   void endCommandGroup  (const char * nsName, const char *name);
   
   /// @deprecated
   void addOverload      (const char * nsName, const char *name, const char *altUsage);
   
   /// @}
   
   /// Returns true when called from the main thread, false otherwise
   bool isMainThread();
   
   /// @name Console Execution
   ///
   /// These are functions relating to the execution of script code.
   ///
   /// @{
   
   /// Call a script function from C/C++ code.
   ///
   /// @param argc      Number of elements in the argv parameter
   /// @param argv      A character string array containing the name of the function
   ///                  to call followed by the arguments to that function.
   /// @code
   /// // Call a Torque script function called mAbs, having one parameter.
   /// char* argv[] = {"abs", "-9"};
   /// char* result = execute(2, argv);
   /// @endcode
   ConsoleValuePtr execute(S32 argc, ConsoleValuePtr argv[]);
   ConsoleStringValuePtr executeS(S32 argc, const char *argv[]);
   
   /// Call a Torque Script member function of a SimObject from C/C++ code.
   /// @param object    Object on which to execute the method call.
   /// @param argc      Number of elements in the argv parameter (must be >2, see argv)
   /// @param argv      A character string array containing the name of the member function
   ///                  to call followed by an empty parameter (gets filled with object ID)
   ///                  followed by arguments to that function.
   /// @code
   /// // Call the method setMode() on an object, passing it one parameter.
   ///
   /// char* argv[] = {"setMode", "", "2"};
   /// char* result = execute(mysimobject, 3, argv);
   /// @endcode
   // [neo, 5/10/2007 - #3010]
   // Added flag thisCallOnly to bypass dynamic method calls
   ConsoleValuePtr execute(SimObject *object, S32 argc, ConsoleValuePtr argv[], bool thisCallOnly = false);
   ConsoleStringValuePtr executeS(SimObject *object, S32 argc, const char *argv[], bool thisCallOnly = false);
   
   /// Evaluate an arbitrary chunk of code.
   ///
   /// @param  string   Buffer containing code to execute.
   /// @param  echo     Should we echo the string to the console?
   /// @param  fileName Indicate what file this code is coming from; used in error reporting and such.
   ConsoleValuePtr evaluate(const char *string, bool echo = false, const char *fileName = NULL);
   
   /// Evaluate an arbitrary line of script.
   ///
   /// This wraps dVsprintf(), so you can substitute parameters into the code being executed.
   ConsoleValuePtr evaluatef(const char *string, ...);
   
   /// @}
   
   /// @name Console Function Implementation Helpers
   ///
   /// The functions Con::getIntArg, Con::getFloatArg and Con::getArgBuffer(size) are used to
   /// allocate on the console stack string variables that will be passed into the next console
   //  function called.  This allows the console to avoid copying some data.
   ///
   /// getReturnBuffer lets you allocate stack space to return data in.
   /// @{
   
   ///
   char *getReturnBuffer(U32 bufferSize);
   char *getReturnBuffer(const char *stringToCopy);
   
   char *getArgBuffer(U32 bufferSize);
   char *getFloatArg(F64 arg);
   char *getIntArg  (S32 arg);
   char* getBoolArg(bool arg);
   /// @}
   
   /// @name Namespaces
   /// @{
   
   Namespace *lookupNamespace(const char *nsName);
   bool linkNamespaces(const char *parentName, const char *childName);
   bool unlinkNamespaces(const char *parentName, const char *childName);
   
   /// @note This should only be called from consoleObject.h
   bool classLinkNamespaces(Namespace *parent, Namespace *child);
   /// @}

   void initExec();
   void addExecVariables();
};


struct _EngineConsoleCallbackHelper;
template<typename P1> struct _EngineConsoleExecCallbackHelper;

namespace Con
{
   /// @name Console Execution - executef
   /// {
   ///
   /// Implements a script function thunk which automatically converts parameters to relevant console types.
   /// Can be used as follows:
   /// - Con::executef("functionName", ...);
   /// - Con::executef(mySimObject, "functionName", ...);
   ///
   /// NOTE: if you get a rather cryptic template error coming through here, most likely you are trying to
   /// convert a parameter which EngineMarshallType does not have a specialization for.
   /// Another problem can occur if you do not include "console/simBase.h" and "console/engineAPI.h"
   /// since _EngineConsoleExecCallbackHelper and SimConsoleThreadExecCallback are required.
   ///
   /// @see _EngineConsoleExecCallbackHelper
   ///
   template<typename A> ConsoleValuePtr executef(A a) { _EngineConsoleExecCallbackHelper<A> callback( a ); return callback.template call<ConsoleValuePtr>(); }
   template<typename A, typename B> ConsoleValuePtr executef(A a, B b) { _EngineConsoleExecCallbackHelper<A> callback( a ); return callback.template call<ConsoleValuePtr>(b); }
   template<typename A, typename B, typename C> ConsoleValuePtr executef(A a, B b, C c) { _EngineConsoleExecCallbackHelper<A> callback( a ); return callback.template call<ConsoleValuePtr>(b, c); }
   template<typename A, typename B, typename C, typename D> ConsoleValuePtr executef(A a, B b, C c, D d) { _EngineConsoleExecCallbackHelper<A> callback( a ); return callback.template call<ConsoleValuePtr>(b, c, d); }
   template<typename A, typename B, typename C, typename D, typename E> ConsoleValuePtr executef(A a, B b, C c, D d, E e) { _EngineConsoleExecCallbackHelper<A> callback( a ); return callback.template call<ConsoleValuePtr>(b, c, d, e); }
   template<typename A, typename B, typename C, typename D, typename E, typename F> ConsoleValuePtr executef(A a, B b, C c, D d, E e, F f) { _EngineConsoleExecCallbackHelper<A> callback( a ); return callback.template call<ConsoleValuePtr>(b, c, d, e, f); }
   template<typename A, typename B, typename C, typename D, typename E, typename F, typename G> ConsoleValuePtr executef(A a, B b, C c, D d, E e, F f, G g) { _EngineConsoleExecCallbackHelper<A> callback( a ); return callback.template call<ConsoleValuePtr>(b, c, d, e, f, g); }
   template<typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H> ConsoleValuePtr executef(A a, B b, C c, D d, E e, F f, G g, H h) { _EngineConsoleExecCallbackHelper<A> callback( a ); return callback.template call<ConsoleValuePtr>(b, c, d, e, f, g, h); }
   template<typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H, typename I> ConsoleValuePtr executef(A a, B b, C c, D d, E e, F f, G g, H h, I i) { _EngineConsoleExecCallbackHelper<A> callback( a ); return callback.template call<ConsoleValuePtr>(b, c, d, e, f, g, h, i); }
   template<typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H, typename I, typename J> ConsoleValuePtr executef(A a, B b, C c, D d, E e, F f, G g, H h, I i, J j) { _EngineConsoleExecCallbackHelper<A> callback( a ); return callback.template call<ConsoleValuePtr>(b, c, d, e, f, g, h, i, j); }
   template<typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H, typename I, typename J, typename K> ConsoleValuePtr executef(A a, B b, C c, D d, E e, F f, G g, H h, I i, J j, K k) { _EngineConsoleExecCallbackHelper<A> callback( a ); return callback.template call<ConsoleValuePtr>(b, c, d, e, f, g, h, i, j, k); }
   template<typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H, typename I, typename J, typename K, typename L> ConsoleValuePtr executef(A a, B b, C c, D d, E e, F f, G g, H h, I i, J j, K k, L l) { _EngineConsoleExecCallbackHelper<A> callback( a ); return callback.template call<ConsoleValuePtr>(b, c, d, e, f, g, h, i, j, k, l); }
   /// }
};


#include "console/consoleTypes.h"
#include "console/consoleCallbackHelper.h"

#endif
