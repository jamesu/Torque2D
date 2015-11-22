#ifndef _CONSOLE_CODEBLOCKEVALSTATE_H_
#define _CONSOLE_CODEBLOCKEVALSTATE_H_

#ifndef _CONSOLE_DICTIONARY_H_
#include "console/consoleDictionary.h"
#endif
#ifndef _CONSOLE_NAMESPACE_H
#include "console/consoleNamespace.h"
#endif

//-----------------------------------------------------------------------------


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
      ConsoleValue* constants;
      U32 stackTop;
      U32 constantTop;

      /// Current function
      CodeBlockFunction* function;
      CodeBlock* code;

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

   InternalState currentFrame;

   Vector<ConsoleValuePtr> stack;
   Vector<InternalState> frames;

   bool traceOn;
   S32 execDepth;
   S32 journalDepth;
   /// }
    
   CodeBlockEvalState() : execDepth(0), journalDepth(0), traceOn(false)
   {
      globalVars = new Dictionary(this, NULL);
   }
   
   ~CodeBlockEvalState()
   {
      SAFE_DELETE(globalVars);
   }
   
   U32 getFrameEnd()
   {
      if (currentFrame.isRoot)
         return 0;
      
      return currentFrame.function ? currentFrame.stackTop + currentFrame.function->maxStack : 0;
   }
   
   void pushFunction(CodeBlockFunction* function, CodeBlock* code, Namespace::Entry* entry, U32 numParams);
   void popFunction();
};

#endif // _CONSOLE_EXPREVALSTATE_H_
