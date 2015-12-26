#include "platform/platform.h"
#include "console/console.h"

#include "console/codeblockEvalState.h"
#include "console/consoleNamespace.h"

#include "console/ast.h"
#include "collection/findIterator.h"
#include "io/resource/resourceManager.h"

#include "string/findMatch.h"
#include "io/fileStream.h"
#include "console/compiler.h"

#include "codeblockEvalState_ScriptBinding.h"

#include "debug/telnetDebugger.h"
#include "debug/remote/RemoteDebuggerBase.h"

CodeBlockEvalState gRootEvalState;
CodeBlockEvalState* gCurrentEvalState = NULL;

Vector<CodeBlockCoroutineState*> gCoroutineStack;

void CodeBlockEvalState::pushFunction(CodeBlockFunction* function, CodeBlock* code, Namespace::Entry* entry, U32 numParams)
{
   if (frames.size() == 0)
   {
      currentFrame.isRoot = true;
      currentFrame.localVars = NULL;
      cStackFrame = 0;
   }
   else
   {
      currentFrame.isRoot = false;
   }
   
   frames.push_back(currentFrame);
   
   currentFrame.localVars = NULL;
   currentFrame.function = function;
   currentFrame.filename = code->modPath;
   currentFrame.savedIP = function->ip;
   currentFrame.code   = CodeBlock::smCurrentCodeBlock = code;
   currentFrame.constants = code->mConstants.address();
   currentFrame.constantTop = 0; // resets to page 0 for new functions
   
   //Con::printf("Pushing codeblock %s %x", currentFrame.code->fullPath, currentFrame.code.getPtr());
   
   if (entry)
   {
      currentFrame.package = entry->mPackage;
      currentFrame.ns = entry->mNamespace->mName;
   }
   
   // As an optimization we only increment the stack to returnStart, and
   // blank out any unused vars. This means any register slots after will get
   // trashed (though there shouldn't be any anyway!).
   U32 usedVars = numParams > function->numArgs ? function->numArgs : numParams; //nsEntry->mMaxArgs ? nsEntry->mMaxArgs : numParams;
   
   if (currentFrame.isRoot)
   {
      currentFrame.stackTop = 0;
   }
   else
   {
      currentFrame.stackTop += currentFrame.returnReg+1; // skip function name, we dont need it
      currentFrame.isRoot = false;
   }
   
   U32 requiredStackSize = currentFrame.stackTop + function->maxStack;
   if (stack.size() < requiredStackSize)
   {
      U32 startSize = stack.size();
      stack.setSize(stack.size() + requiredStackSize);
      for (U32 i=startSize; i<startSize+requiredStackSize; i++)
      {
         stack[i].type = ConsoleValue::TypeInternalNull;
      }
   }
   
   ConsoleValuePtr *const base = stack.address() + currentFrame.stackTop;
   
   for (U32 j=numParams; j<function->numArgs; j++)
   {
      base[currentFrame.returnReg+j].setNull();
   }
   
   // Grab the state of the telenet debugger here once
   // so that the push and pop frames are always balanced.
   const bool telDebuggerOn = TelDebugger && TelDebugger->isConnected();
   if ( telDebuggerOn )
      TelDebugger->pushStackFrame();
   
   // Notify the remote debugger.
   RemoteDebuggerBase* pRemoteDebugger = RemoteDebuggerBase::getRemoteDebugger();
   if ( pRemoteDebugger != NULL )
      pRemoteDebugger->pushStackFrame();
   
   
   // Trace handler
   if(traceOn)
   {
      char traceBuffer[4096];
      traceBuffer[0] = 0;
      dStrcat(traceBuffer, "Entering ");
      if(currentFrame.package)
      {
         dStrcat(traceBuffer, "[");
         dStrcat(traceBuffer, currentFrame.package);
         dStrcat(traceBuffer, "]");
      }
      if(currentFrame.ns)
      {
         dSprintf(traceBuffer + dStrlen(traceBuffer), sizeof(traceBuffer) - dStrlen(traceBuffer),
                  "%s::%s(", currentFrame.ns, currentFrame.function->name);
      }
      else
      {
         dSprintf(traceBuffer + dStrlen(traceBuffer), sizeof(traceBuffer) - dStrlen(traceBuffer),
                  "%s(", currentFrame.function->name);
      }
      for(U32 i = 0; i < numParams; i++)
      {
         //dStrcat(traceBuffer, base[currentFrame.returnReg+i].getTempStringValue());
         //if(i != numParams - 1)
         //   dStrcat(traceBuffer, ", ");
      }
      dStrcat(traceBuffer, ")");
      Con::printf("%s", traceBuffer);
   }
}

void CodeBlockEvalState::popFunction()
{
   AssertFatal(frames.size() > 0, "Stack misbalance");
   
   //Con::printf("Popping codeblock %s %x", currentFrame.code->fullPath, currentFrame.code.getPtr());
   
   // Copy locals to dictionary if applicable
   if (currentFrame.localVars)
   {
      if (!currentFrame.localVars->isRoot())
      {
         copyFrameToLocals(currentFrame.localVars, &currentFrame);
      }
      disposeLocals(currentFrame.localVars);
   }
   
   currentFrame = frames[frames.size()-1];
   frames.pop_back();
   //Con::printf("Codeblock now %s", currentFrame.code->fullPath);
   
   CodeBlock::smCurrentCodeBlock = currentFrame.code;
   
   const bool telDebuggerOn = TelDebugger && TelDebugger->isConnected();
   
   if ( telDebuggerOn )
      TelDebugger->popStackFrame();
   
   // Notify the remote debugger.
   RemoteDebuggerBase* pRemoteDebugger = RemoteDebuggerBase::getRemoteDebugger();
   if ( pRemoteDebugger != NULL )
      pRemoteDebugger->popStackFrame();
}

bool CodeBlockEvalState::createCoroutine(CodeBlockCoroutineState &outState, Namespace::Entry* nsRef)
{
   outState.reset();
   
   if (nsRef->mType != Namespace::Entry::ScriptFunctionType)
   {
      Con::errorf("Cant create a coroutine, must point to a script function.");
      return false;
   }
   
   outState.nsEntry = nsRef;
   return true;
}

bool CodeBlockEvalState::saveCoroutine(CodeBlockCoroutineState &outState)
{
   outState.currentState = CodeBlockCoroutineState::SUSPENDED;
   
   bool inStack = false;
   for (S32 i=gCoroutineStack.size()-1; i>=0; i--)
   {
      if (gCoroutineStack[i] == &outState)
      {
         inStack = true;
      }
   }
   
   if (!inStack)
   {
      Con::errorf("Couldn't save coroutine, not in stack");
      return false;
   }
   
   // Pop until we've got to the correct level
   for (S32 i=gCoroutineStack.size()-1; i>=0; i--)
   {
      if (gCoroutineStack[i] == &outState)
      {
         gCoroutineStack.pop_back();
         break;
      }
      else
      {
         gCoroutineStack[gCoroutineStack.size()-1]->currentState = CodeBlockCoroutineState::SUSPENDED;
         gCoroutineStack.pop_back();
      }
   }
   
   gCurrentEvalState = gCoroutineStack.size() > 0 ? &gCoroutineStack[gCoroutineStack.size()-1]->evalState : &gRootEvalState;
   return true;
}

bool CodeBlockEvalState::restoreCoroutine(CodeBlockCoroutineState &inState, S32 argc, ConsoleValuePtr *argv)
{
   // Check we're not already active
   if (gCurrentEvalState == &inState.evalState)
   {
      return false;
   }
   else
   {
      if (!inState.nsEntry->mCode)
      {
         Con::errorf("Coroutine: nsEntry is no longer valid");
         return false;
      }
      
      for (U32 i=0, sz=gCoroutineStack.size(); i<sz; i++)
      {
         if (gCoroutineStack[i] == &inState)
         {
            return false;
         }
      }
   }
   
   // Now active, so we can evaluate the nsEntry
   
   if (inState.currentState == CodeBlockCoroutineState::WAIT_INITIAL_CALL)
   {
      gCurrentEvalState = &inState.evalState;
      gCoroutineStack.push_back(&inState);
      if (inState.nsEntry->mType != Namespace::Entry::ScriptFunctionType)
      {
         Con::errorf("Cant restore coroutine, must point to a script function.");
         return false;
      }
      
      inState.currentState = CodeBlockCoroutineState::RUNNING;
      inState.nsEntry->mCode->prepCoroutine(inState.nsEntry->mFunctionOffset, inState.nsEntry->mFunctionName, inState.nsEntry->mNamespace, argc, argv, false, NULL);
   }
   else if (inState.currentState == CodeBlockCoroutineState::SUSPENDED)
   {
      inState.currentState = CodeBlockCoroutineState::RUNNING;
      gCurrentEvalState = &inState.evalState;
      gCoroutineStack.push_back(&inState);
      
      // 
   }
   else
   {
      Con::errorf("Coroutine is in an invalid state!");
      return false;
   }
   
   //const bool telDebuggerOn = TelDebugger && TelDebugger->isConnected();
   //RemoteDebuggerBase* pRemoteDebugger = RemoteDebuggerBase::getRemoteDebugger();
   
   return true;
}


Dictionary *CodeBlockEvalState::createLocals(Dictionary* base)
{
   return new Dictionary(this, base);
}

void CodeBlockEvalState::copyFrameToLocals(Dictionary* locals, InternalState* srcFrame)
{
   CodeBlockFunction* func = srcFrame->function;
   if (!func)
      return;
   
   for (U32 i=0, sz=func->vars.size(); i<sz; i++)
   {
      CodeBlockFunction::Symbol &symbol = func->vars[i];
      //Con::printf("copyFrame(%s) = REG %i (abs %i)", symbol.varName, symbol.registerIdx, srcFrame->stackTop + symbol.registerIdx);
      locals->setValueVariable(symbol.varName, this->stack[srcFrame->stackTop + symbol.registerIdx]);
   }
}

void CodeBlockEvalState::copyLocalsToFrame(Dictionary* locals, InternalState* dstFrame)
{
   CodeBlockFunction* func = dstFrame->function;
   if (!func)
      return;
   
   locals->setFrame(func, stack.address()+dstFrame->stackTop);
}

void CodeBlockEvalState::disposeLocals(Dictionary* locals)
{
   delete locals;
}

CodeBlockEvalState* CodeBlockEvalState::getCurrent()
{
   if (gCurrentEvalState == NULL)
   {
      gCurrentEvalState = &gRootEvalState;
   }
   return gCurrentEvalState;
}

//


//////////////////////////////////////////////////////////////////////////
// TypeCoroutineRef
//////////////////////////////////////////////////////////////////////////
ConsoleType( TypeCoroutineRef, TypeCoroutineRef, sizeof(ConsoleValuePtr), "" )
ConsoleSetReferenceType( TypeCoroutineRef, CodeBlockCoroutineState )

ConsoleTypeFromConsoleValue( TypeCoroutineRef )
{
   if (ConsoleValue::isRefType(value.type))
   {
      if (value.value.refValue->isEnumerable())
      {
         Con::warnf( "(TypeCoroutineRef) Cannot set multiple args to a single SimObject." );
         return;
      }
   }
   
   ConsoleValuePtr *obj = (ConsoleValuePtr*)dataPtr;
   obj->setValue(value);
}

ConsoleTypeToString( TypeCoroutineRef )
{
   return "[[COROUTINE]]";
}

CodeBlockCoroutineState::CodeBlockCoroutineState()
{
   reset();
}

CodeBlockCoroutineState::~CodeBlockCoroutineState()
{
   
}

void CodeBlockCoroutineState::reset()
{
   ConsoleValuePtr nullValue;
   
   for (U32 i=0, sz=evalState.stack.size(); i<sz; i++)
   {
      evalState.stack[i].setValue(nullValue);
   }
   
   for (U32 i=0, sz=evalState.frames.size(); i<sz; i++)
   {
      evalState.frames[i].code = NULL;
   }
   
   evalState.frames.clear();
   evalState.yieldValue.setValue(nullValue);
   evalState.coroutine = this;
   currentState = CodeBlockCoroutineState::DEAD;
}

ConsoleStringValuePtr CodeBlockCoroutineState::getString()
{
   return "[[COROUTINE]]";
}

ConsoleBaseType *CodeBlockCoroutineState::getType()
{
   return ConsoleTypeTypeCoroutineRef::getInstance();
}

bool CodeBlockCoroutineState::getDataField(StringTableEntry slotName, const ConsoleValuePtr &array, ConsoleValuePtr &outValue)
{
   outValue.setNull();
   return false;
}

void CodeBlockCoroutineState::setDataField(StringTableEntry slotName, const ConsoleValuePtr &array, const ConsoleValuePtr &newValue)
{
   return;
}

Namespace *CodeBlockCoroutineState::getNamespace()
{
   return Namespace::find(StringTable->insert("Coroutine"));
}

void CodeBlockCoroutineState::read(Stream &s, ConsoleSerializationState &state)
{
   ConsoleValuePtr::readStack(s, state, evalState.stack);
   // TODO
}

void CodeBlockCoroutineState::write(Stream &s, ConsoleSerializationState &state)
{
   ConsoleValuePtr::writeStack(s, state, evalState.stack);
   // TODO
}



