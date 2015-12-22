#ifndef _CONSOLE_CODEBLOCKEVALSTATE_H_
#define _CONSOLE_CODEBLOCKEVALSTATE_H_

#ifndef _CONSOLE_DICTIONARY_H_
#include "console/consoleDictionary.h"
#endif
#ifndef _CONSOLE_NAMESPACE_H
#include "console/consoleNamespace.h"
#endif
#ifndef _CODEBLOCK_H_
#include "console/codeBlock.h"
#endif

//-----------------------------------------------------------------------------

class CodeBlockCoroutineState;


class CodeBlockEvalState
{
public:
   
   struct InternalState
   {
      /// @name Global state
      /// {
      Dictionary* globalVars;
      /// }
      
      /// @name Execution state
      /// {
      const char* filename;
      const char* package;
      const char* ns;
      ConsoleValuePtr* constants;
      U32 stackTop;
      U32 constantTop;

      /// Current function
      CodeBlockFunction* function;
      CodeBlockPtr code;
      
      /// Copy of local vars for exec
      Dictionary* localVars;

      U32 savedIP;
      U32 returnReg;

      bool isRoot;
      bool noCalls;

      InternalState() : globalVars(NULL), stackTop(0), constantTop(0), filename(NULL), package(NULL), ns(NULL), function(NULL), code(NULL), returnReg(0), noCalls(false) {;}
   };

   /// Global registered variables
   Dictionary* globalVars;

   /// Last returned or yielded value
   ConsoleValuePtr yieldValue;
   
   /// Set if we are a coroutine
   CodeBlockCoroutineState* coroutine;

   InternalState currentFrame;

   Vector<ConsoleValuePtr> stack;
   Vector<InternalState> frames;

   bool traceOn;
   S32 execDepth;
   S32 journalDepth;
   
   S32 cStackFrame; // execBlock nesting count
   /// }
    
   CodeBlockEvalState() : execDepth(0), journalDepth(0), traceOn(false), coroutine(NULL)
   {
      globalVars = new Dictionary(this, NULL);
   }
   
   ~CodeBlockEvalState()
   {
      SAFE_DELETE(globalVars);
   }
   
   U32 getFrameEnd()
   {
      if (frames.size() == 0)
         return 0;
      
      return currentFrame.function ? currentFrame.stackTop + currentFrame.function->maxStack : 0;
   }
   
   void pushFunction(CodeBlockFunction* function, CodeBlock* code, Namespace::Entry* entry, U32 numParams);
   void popFunction();
   
   /// @name Coroutine handling
   /// {
   /// Creates an empty coroutine (currentFrame needs to be changed by caller)
   bool createCoroutine(CodeBlockCoroutineState &outState, Namespace::Entry* nsRef);
   
   /// Saves current execution state to coroutine
   bool saveCoroutine(CodeBlockCoroutineState &outState);
   
   /// Restores stack state from saved state
   /// @note this merely plonks the saved state on top of the current stack like in a function call
   bool restoreCoroutine(CodeBlockCoroutineState &inState, S32 argc, ConsoleValuePtr *argv);
   /// }
   
   /// @name Local var handling
   /// {
   /// Create locals dict for free-floating vars
   Dictionary* createLocals(Dictionary* base);
   
   /// Copy frame locals to dictionary
   void copyFrameToLocals(Dictionary* locals, InternalState* srcFrame);
   
   /// Copy locals from dictionary to a frame
   void copyLocalsToFrame(Dictionary* locals, InternalState* destFrame);
   
   /// Gets rid of a free-floating variable dictionary
   void disposeLocals(Dictionary* locals);
   /// }
   
   static CodeBlockEvalState* getCurrent();
};

class CodeBlockCoroutineState : public ConsoleReferenceCountedType
{
public:
   enum State
   {
      WAIT_INITIAL_CALL,
      SUSPENDED,
      RUNNING,
      DEAD
   };
   
   CodeBlockEvalState evalState; // current state of script stack
   State currentState; //
   
   Namespace::Entry *nsEntry; // function to call
   
   CodeBlockCoroutineState();
   ~CodeBlockCoroutineState();
   
   void reset(); ///< clears state
   
   virtual ConsoleStringValuePtr getString();
   virtual ConsoleBaseType *getType();
   
   virtual bool getDataField(StringTableEntry slotName, const ConsoleValuePtr &array, ConsoleValuePtr &outValue);
   virtual void setDataField(StringTableEntry slotName, const ConsoleValuePtr &array, const ConsoleValuePtr &newValue);
   virtual Namespace *getNamespace();
   
   virtual void read(Stream &s, ConsoleSerializationState &state);
   virtual void write(Stream &s, ConsoleSerializationState &state);
   
   virtual bool stringCompare(const char *other)
   {
      return dStricmp(getString().c_str(), other) == 0;
   }
   
   virtual bool refCompare(ConsoleReferenceCountedType *other)
   {
      return other == this;
   }
};

#endif // _CONSOLE_EXPREVALSTATE_H_
