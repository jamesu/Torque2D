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

/// This is the backend for the ConsoleMethod()/ConsoleFunction() macros.
///
/// See the group ConsoleConstructor Innards for specifics on how this works.
///
/// @see @ref console_autodoc
/// @nosubgrouping
class ConsoleConstructor
{
public:
   /// @name Entry Type Fields
   ///
   /// One of these is set based on the type of entry we want
   /// inserted in the console.
   ///
   /// @ref console_autodoc
   /// @{
   StringCallback sc;   ///< A function/method that returns a string.
   IntCallback ic;      ///< A function/method that returns an int.
   FloatCallback fc;    ///< A function/method that returns a float.
   VoidCallback vc;     ///< A function/method that returns nothing.
   BoolCallback bc;     ///< A function/method that returns a bool.
   ValueCallback cc;    ///< A function/method that returns a ConsoleValuePtr.
   bool group;          ///< Indicates that this is a group marker.
   bool overload;       ///< Indicates that this is an overload marker.
   bool ns;             ///< Indicates that this is a namespace marker.
   ///  @deprecated Unused.
   /// @}
   
   /// Minimum/maximum number of arguments for the function.
   S32 mina, maxa;
   const char *usage;         ///< Usage string.
   const char *funcName;      ///< Function name.
   const char *className;     ///< Class name.
   
   /// @name ConsoleConstructer Innards
   ///
   /// The ConsoleConstructor class is used as the backend for the ConsoleFunction() and
   /// ConsoleMethod() macros. The way it works takes advantage of several properties of
   /// C++.
   ///
   /// The ConsoleFunction()/ConsoleMethod() macros wrap the declaration of a ConsoleConstructor.
   ///
   /// @code
   ///      // The definition of a ConsoleFunction using the macro
   ///      ConsoleFunction(ExpandPath, const char*, 2, 2, "(string filePath)")
   ///      {
   ///         argc;
   ///         char* ret = Con::getReturnBuffer( 1024 );
   ///         Con::expandPath(ret, 1024, argv[1]);
   ///         return ret;
   ///      }
   ///
   ///      // Resulting code
   ///      static const char *cExpandPath(SimObject *, S32, ConsoleValuePtr *argv);
   ///      static ConsoleConstructor
   ///            gExpandPathobj(NULL,"ExpandPath", cExpandPath,
   ///            "(string filePath)", 2, 2);
   ///      static const char *cExpandPath(SimObject *, S32 argc, ConsoleValuePtr *argv)
   ///      {
   ///         argc;
   ///         char* ret = Con::getReturnBuffer( 1024 );
   ///         Con::expandPath(ret, 1024, argv[1]);
   ///         return ret;
   ///      }
   ///
   ///      // A similar thing happens when you do a ConsoleMethod.
   /// @endcode
   ///
   /// As you can see, several global items are defined when you use the ConsoleFunction method.
   /// The macro constructs the name of these items from the parameters you passed it. Your
   /// implementation of the console function is is placed in a function with a name based on
   /// the actual name of the console funnction. In addition, a ConsoleConstructor is declared.
   ///
   /// Because it is defined as a global, the constructor for the ConsoleConstructor is called
   /// before execution of main() is started. The constructor is called once for each global
   /// ConsoleConstructor variable, in the order in which they were defined (this property only holds true
   /// within file scope).
   ///
   /// We have ConsoleConstructor create a linked list at constructor time, by storing a static
   /// pointer to the head of the list, and keeping a pointer to the next item in each instance
   /// of ConsoleConstructor. init() is a helper function in this process, automatically filling
   /// in commonly used fields and updating first and next as needed. In this way, a list of
   /// items to add to the console is assemble in memory, ready for use, before we start
   /// execution of the program proper.
   ///
   /// In Con::init(), ConsoleConstructor::setup() is called to process this prepared list. Each
   /// item in the list is iterated over, and the appropriate Con namespace functions (usually
   /// Con::addCommand) are invoked to register the ConsoleFunctions and ConsoleMethods in
   /// the appropriate namespaces.
   ///
   /// @see Namespace
   /// @see Con
   /// @{
   
   ConsoleConstructor *next;
   static ConsoleConstructor *first;
   
   void init(const char *cName, const char *fName, const char *usg, S32 minArgs, S32 maxArgs);
   static void setup();
   /// @}
   
   /// @name Basic Console Constructors
   /// @{
   
   ConsoleConstructor(const char *className, const char *funcName, ValueCallback vfunc, const char *usage,  S32 minArgs, S32 maxArgs);
   ConsoleConstructor(const char *className, const char *funcName, StringCallback sfunc, const char *usage,  S32 minArgs, S32 maxArgs);
   ConsoleConstructor(const char *className, const char *funcName, IntCallback    ifunc, const char *usage,  S32 minArgs, S32 maxArgs);
   ConsoleConstructor(const char *className, const char *funcName, FloatCallback  ffunc, const char *usage,  S32 minArgs, S32 maxArgs);
   ConsoleConstructor(const char *className, const char *funcName, VoidCallback   vfunc, const char *usage,  S32 minArgs, S32 maxArgs);
   ConsoleConstructor(const char *className, const char *funcName, BoolCallback   bfunc, const char *usage,  S32 minArgs, S32 maxArgs);
   /// @}
   
   /// @name Magic Console Constructors
   ///
   /// These perform various pieces of "magic" related to consoleDoc functionality.
   /// @ref console_autodoc
   /// @{
   
   /// Indicates a group marker. (A doxygen illusion)
   ///
   /// @see Con::markCommandGroup
   /// @ref console_autodoc
   ConsoleConstructor(const char *className, const char *groupName, const char *usage);
   
   /// Indicates a namespace usage string.
   ConsoleConstructor(const char *className, const char *usage);
   
   /// @}
};

/// @name Global Console Definition Macros
///
/// @note If TORQUE_DEBUG is defined, then we gather documentation information, and
///       do some extra sanity checks.
///
/// @see ConsoleConstructor
/// @ref console_autodoc
/// @{

// O hackery of hackeries
#define conmethod_return_const              return (const
#define conmethod_return_S32                return (S32
#define conmethod_return_F32                return (F32
#define conmethod_nullify(val)
#define conmethod_return_void               conmethod_nullify(void
#define conmethod_return_bool               return (bool
#define conmethod_return_ConsoleInt         conmethod_return_S32
#define conmethod_return_ConsoleFloat       conmethod_return_F32
#define conmethod_return_ConsoleVoid        conmethod_return_void
#define conmethod_return_ConsoleBool        conmethod_return_bool
#define conmethod_return_ConsoleString        return(ConsoleStringValuePtr
#define conmethod_return_ConsoleValuePtr      return (ConsoleValuePtr

#if !defined(TORQUE_SHIPPING)

// Console function return types
#define ConsoleString   ConsoleStringValuePtr
#define ConsoleInt      S32
#define ConsoleFloat   F32
#define ConsoleVoid      void
#define ConsoleBool      bool

// Console function macros
#  define ConsoleFunctionGroupBegin(groupName, usage) \
static ConsoleConstructor gConsoleFunctionGroup##groupName##__GroupBegin(NULL,#groupName,usage);

#  define ConsoleFunction(name,returnType,minArgs,maxArgs,usage1)                         \
static returnType c##name(SimObject *, S32, ConsoleValuePtr argv[]);                     \
static ConsoleConstructor g##name##obj(NULL,#name,c##name,usage1,minArgs,maxArgs);  \
static returnType c##name(SimObject *, S32 argc, ConsoleValuePtr argv[])

#  define ConsoleFunctionWithDocs(name,returnType,minArgs,maxArgs,argString)              \
static returnType c##name(SimObject *, S32, ConsoleValuePtr argv[]);                     \
static ConsoleConstructor g##name##obj(NULL,#name,c##name,#argString,minArgs,maxArgs);      \
static returnType c##name(SimObject *, S32 argc, ConsoleValuePtr argv[])

#  define ConsoleFunctionGroupEnd(groupName) \
static ConsoleConstructor gConsoleFunctionGroup##groupName##__GroupEnd(NULL,#groupName,NULL);

// Console method macros
#  define ConsoleNamespace(className, usage) \
static ConsoleConstructor className##__Namespace(#className, usage);

#  define ConsoleMethodGroupBegin(className, groupName, usage) \
static ConsoleConstructor className##groupName##__GroupBegin(#className,#groupName,usage);

// note: we would want to expand the following macro into (Doxygen) comments!
// we can not do that with a macro.  these are here just as a reminder until completion
#  define ConsoleMethodRootGroupBeginWithDocs(className)
#  define ConsoleMethodGroupBeginWithDocs(className, superclassName)

#  define ConsoleMethod(className,name,returnType,minArgs,maxArgs,usage1)                                                 \
static inline returnType c##className##name(className *, S32, ConsoleValuePtr *argv);                                   \
static returnType c##className##name##caster(SimObject *object, S32 argc, ConsoleValuePtr *argv) {                      \
   AssertFatal( dynamic_cast<className*>( object ), "Object passed to " #name " is not a " #className "!" );        \
   conmethod_return_##returnType ) c##className##name(static_cast<className*>(object),argc,argv);                   \
};                                                                                                                  \
static ConsoleConstructor className##name##obj(#className,#name,c##className##name##caster,usage1,minArgs,maxArgs); \
static inline returnType c##className##name(className *object, S32 argc, ConsoleValuePtr *argv)

#  define ConsoleMethodWithDocs(className,name,returnType,minArgs,maxArgs,argString)                                  \
static inline returnType c##className##name(className *, S32, ConsoleValuePtr *argv);                               \
static returnType c##className##name##caster(SimObject *object, S32 argc, ConsoleValuePtr *argv) {                  \
   AssertFatal( dynamic_cast<className*>( object ), "Object passed to " #name " is not a " #className "!" );    \
   conmethod_return_##returnType ) c##className##name(static_cast<className*>(object),argc,argv);               \
};                                                                                                              \
static ConsoleConstructor className##name##obj(#className,#name,c##className##name##caster,#argString,minArgs,maxArgs); \
static inline returnType c##className##name(className *object, S32 argc, ConsoleValuePtr *argv)

#  define ConsoleStaticMethod(className,name,returnType,minArgs,maxArgs,usage1)                       \
static inline returnType c##className##name(S32, ConsoleValuePtr*);                                \
static returnType c##className##name##caster(SimObject *object, S32 argc, ConsoleValuePtr *argv) {  \
   conmethod_return_##returnType ) c##className##name(argc,argv);                               \
};                                                                                              \
static ConsoleConstructor                                                                       \
className##name##obj(#className,#name,c##className##name##caster,usage1,minArgs,maxArgs);    \
static inline returnType c##className##name(S32 argc, ConsoleValuePtr *argv)

#  define ConsoleStaticMethodWithDocs(className,name,returnType,minArgs,maxArgs,argString)            \
static inline returnType c##className##name(S32, ConsoleValuePtr*);                                \
static returnType c##className##name##caster(SimObject *object, S32 argc, ConsoleValuePtr *argv) {  \
   conmethod_return_##returnType ) c##className##name(argc,argv);                               \
};                                                                                              \
static ConsoleConstructor                                                                       \
className##name##obj(#className,#name,c##className##name##caster,#argString,minArgs,maxArgs);        \
static inline returnType c##className##name(S32 argc, ConsoleValuePtr *argv)

#  define ConsoleMethodGroupEnd(className, groupName) \
static ConsoleConstructor className##groupName##__GroupEnd(#className,#groupName,NULL);

#  define ConsoleMethodRootGroupEndWithDocs(className)
#  define ConsoleMethodGroupEndWithDocs(className)

#else

// These do nothing if we don't want doc information.
#  define ConsoleFunctionGroupBegin(groupName, usage)
#  define ConsoleFunctionGroupEnd(groupName)
#  define ConsoleNamespace(className, usage)
#  define ConsoleMethodGroupBegin(className, groupName, usage)
#  define ConsoleMethodGroupEnd(className, groupName)

// These are identical to what's above, we just want to null out the usage strings.
#  define ConsoleFunction(name,returnType,minArgs,maxArgs,usage1)                   \
static returnType c##name(SimObject *, S32, const char **);                   \
static ConsoleConstructor g##name##obj(NULL,#name,c##name,"",minArgs,maxArgs);\
static returnType c##name(SimObject *, S32 argc, ConsoleValuePtr *argv)

#  define ConsoleMethod(className,name,returnType,minArgs,maxArgs,usage1)                             \
static inline returnType c##className##name(className *, S32, ConsoleValuePtr *argv);               \
static returnType c##className##name##caster(SimObject *object, S32 argc, ConsoleValuePtr *argv) {  \
   conmethod_return_##returnType ) c##className##name(static_cast<className*>(object),argc,argv);              \
};                                                                                              \
static ConsoleConstructor                                                                       \
className##name##obj(#className,#name,c##className##name##caster,"",minArgs,maxArgs);        \
static inline returnType c##className##name(className *object, S32 argc, ConsoleValuePtr *argv)

#  define ConsoleStaticMethod(className,name,returnType,minArgs,maxArgs,usage1)                       \
static inline returnType c##className##name(S32, const char **);                                \
static returnType c##className##name##caster(SimObject *object, S32 argc, ConsoleValuePtr *argv) {  \
   conmethod_return_##returnType ) c##className##name(argc,argv);                                                        \
};                                                                                              \
static ConsoleConstructor                                                                       \
className##name##obj(#className,#name,c##className##name##caster,"",minArgs,maxArgs);        \
static inline returnType c##className##name(S32 argc, ConsoleValuePtr *argv)


#endif

/// @}
