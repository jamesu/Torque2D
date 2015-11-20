//-----------------------------------------------------------------------------
// Copyright (c) 2012 GarageGames, LLC
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
#include "console/compiler.h"
#include "console/codeBlock.h"
#include "console/consoleSerialization.h"
#include "io/resource/resourceManager.h"
#include "math/mMath.h"

#include "console/codeblockEvalState.h"

#include "debug/telnetDebugger.h"

#ifndef _REMOTE_DEBUGGER_BASE_H_
#include "debug/remote/RemoteDebuggerBase.h"
#endif

using namespace Compiler;

bool           CodeBlock::smInFunction = false;
CodeBlock *    CodeBlock::smCodeBlockList = NULL;
CodeBlock *    CodeBlock::smCurrentCodeBlock = NULL;


FunctionDeclStmtNode* CodeBlock::smCurrentFunctionList = NULL;
FunctionDeclStmtNode* CodeBlock::smFunctionListHead = NULL;
CodeBlockFunction* CodeBlock::smCurrentFunction = NULL;
CodeBlockFunction* CodeBlock::smCurrentCodeblockFunction = NULL;

CodeBlockEvalState gNewEvalState;

//-------------------------------------------------------------------------

CodeBlock::CodeBlock()
{
    lineBreakPairs = NULL;
    breakList = NULL;
    breakListSize = 0;
    
    refCount = 0;
    name = NULL;
    fullPath = NULL;
    modPath = NULL;
    mRoot = StringTable->EmptyString;
}

CodeBlock::~CodeBlock()
{
    // Make sure we aren't lingering in the current code block...
    AssertFatal(smCurrentCodeBlock != this, "CodeBlock::~CodeBlock - Caught lingering in smCurrentCodeBlock!")
    
    if(name)
        removeFromCodeList();
}

//-------------------------------------------------------------------------

StringTableEntry CodeBlock::getCurrentCodeBlockName()
{
    if (CodeBlock::getCurrentBlock())
        return CodeBlock::getCurrentBlock()->name;
    else
        return NULL;
}

StringTableEntry CodeBlock::getCurrentCodeBlockFullPath()
{
    if (CodeBlock::getCurrentBlock())
        return CodeBlock::getCurrentBlock()->fullPath;
    else
        return NULL;
}

StringTableEntry CodeBlock::getCurrentCodeBlockModName()
{
    if (CodeBlock::getCurrentBlock())
        return CodeBlock::getCurrentBlock()->modPath;
    else
        return NULL;
}

CodeBlock *CodeBlock::find(StringTableEntry name)
{
    for(CodeBlock *walk = CodeBlock::getCodeBlockList(); walk; walk = walk->nextFile)
        if(walk->name == name)
            return walk;
    return NULL;
}

//-------------------------------------------------------------------------

void CodeBlock::addToCodeList()
{
    // remove any code blocks with my name
    for(CodeBlock **walk = &smCodeBlockList; *walk;walk = &((*walk)->nextFile))
    {
        if((*walk)->name == name)
        {
            *walk = (*walk)->nextFile;
            break;
        }
    }
    nextFile = smCodeBlockList;
    smCodeBlockList = this;
}

void CodeBlock::clearAllBreaks()
{
    if(!lineBreakPairs)
        return;
    for(U32 i = 0; i < lineBreakPairCount; i++)
    {
        U32 *p = lineBreakPairs + i * 2;
        code[p[1]] &= ~Compiler::OP_MASK;
        code[p[1]] |= p[0] & Compiler::OP_MASK;
    }
}

void CodeBlock::clearBreakpoint(U32 lineNumber)
{
    if(!lineBreakPairs)
        return;
    for(U32 i = 0; i < lineBreakPairCount; i++)
    {
        U32 *p = lineBreakPairs + i * 2; // [line | inst, ip]
        if((p[0] >> 8) == lineNumber)
        {
            code[p[1]] &= ~Compiler::OP_MASK;
            code[p[1]] |= p[0] & Compiler::OP_MASK;
           
            return;
        }
    }
}

void CodeBlock::setAllBreaks()
{
    if(!lineBreakPairs)
        return;
    for(U32 i = 0; i < lineBreakPairCount; i++)
    {
       U32 *p = lineBreakPairs + i * 2;
       code[p[1]] &= ~Compiler::OP_MASK;
       code[p[1]] |= Compiler::OP_BREAK;
    }
}

bool CodeBlock::setBreakpoint(U32 lineNumber)
{
    if(!lineBreakPairs)
        return false;
    
    for(U32 i = 0; i < lineBreakPairCount; i++)
    {
        U32 *p = lineBreakPairs + i * 2;
        if((p[0] >> 8) == lineNumber)
        {
            code[p[1]] &= ~Compiler::OP_MASK;
            code[p[1]] |= Compiler::OP_BREAK;
            return true;
        }
    }
    
    return false;
}

U32 CodeBlock::findFirstBreakLine(U32 lineNumber)
{
    if(!lineBreakPairs)
        return 0;
    
    for(U32 i = 0; i < lineBreakPairCount; i++)
    {
        U32 *p = lineBreakPairs + i * 2;
        U32 line = (p[0] >> 8);
        
        if( lineNumber <= line )
            return line;
    }
    
    return 0;
}

struct LinePair
{
    U32 instLine;
    U32 ip;
};

void CodeBlock::findBreakLine(U32 ip, U32 &line, U32 &instruction)
{
    U32 min = 0;
    U32 max = lineBreakPairCount - 1;
    LinePair *p = (LinePair *) lineBreakPairs;
    
    U32 found;
    if(!lineBreakPairCount || p[min].ip > ip || p[max].ip < ip)
    {
        line = 0;
        instruction = Compiler::OP_BREAK; // NOP
        return;
    }
    else if(p[min].ip == ip)
        found = min;
    else if(p[max].ip == ip)
        found = max;
    else
    {
        for(;;)
        {
            if(min == max - 1)
            {
                found = min;
                break;
            }
            U32 mid = (min + max) >> 1;
            if(p[mid].ip == ip)
            {
                found = mid;
                break;
            }
            else if(p[mid].ip > ip)
                max = mid;
            else
                min = mid;
        }
    }
    instruction = p[found].instLine & Compiler::OP_MASK;
    line = p[found].instLine >> 8;
}

const char *CodeBlock::getFileLine(U32 ip)
{
    static char nameBuffer[256];
    U32 line, inst;
    findBreakLine(ip, line, inst);
    
    dSprintf(nameBuffer, sizeof(nameBuffer), "%s (%d)", name ? name : "<input>", line);
    return nameBuffer;
}

void CodeBlock::removeFromCodeList()
{
    for(CodeBlock **walk = &smCodeBlockList; *walk; walk = &((*walk)->nextFile))
    {
        if(*walk == this)
        {
            *walk = nextFile;
            
            // clear out all breakpoints
            clearAllBreaks();
            break;
        }
    }
    
    // Let the telnet debugger know that this code
    // block has been unloaded and that it needs to
    // remove references to it.
    if ( TelDebugger )
        TelDebugger->clearCodeBlockPointers( this );
}

void CodeBlock::calcBreakList()
{
    U32 size = 0;
    S32 line = -1;
    U32 seqCount = 0;
    U32 i;
    for(i = 0; i < lineBreakPairCount; i++)
    {
        U32 lineNumber = lineBreakPairs[i * 2];
        if(lineNumber == U32(line + 1))
            seqCount++;
        else
        {
            if(seqCount)
                size++;
            size++;
            seqCount = 1;
        }
        line = lineNumber;
    }
    if(seqCount)
        size++;
    
    breakList = new U32[size];
    breakListSize = size;
    line = -1;
    seqCount = 0;
    size = 0;
    
    for(i = 0; i < lineBreakPairCount; i++)
    {
        U32 lineNumber = lineBreakPairs[i * 2];
        
        if(lineNumber == U32(line + 1))
            seqCount++;
        else
        {
            if(seqCount)
                breakList[size++] = seqCount;
            breakList[size++] = lineNumber - getMax(0, line) - 1;
            seqCount = 1;
        }
        
        line = lineNumber;
    }
    
    if(seqCount)
        breakList[size++] = seqCount;
    
    for(i = 0; i < lineBreakPairCount; i++)
    {
        U32 *p = lineBreakPairs + i * 2;
        p[0] = (p[0] << 8) | (code[p[1]] & Compiler::OP_MASK);
    }
    
    // Let the telnet debugger know that this code
    // block has been loaded and that it can add break
    // points it has for it.
    if ( TelDebugger )
        TelDebugger->addAllBreakpoints( this );
}

bool CodeBlock::read(StringTableEntry fileName, Stream &st)
{
    const StringTableEntry exePath = Platform::getMainDotCsDir();
    const StringTableEntry cwd = Platform::getCurrentDirectory();
    
    name = fileName;
    
    if(fileName)
    {
        fullPath = NULL;
        
        if(Platform::isFullPath(fileName))
            fullPath = fileName;
        
        if(dStrnicmp(exePath, fileName, dStrlen(exePath)) == 0)
            name = StringTable->insert(fileName + dStrlen(exePath) + 1, true);
        else if(dStrnicmp(cwd, fileName, dStrlen(cwd)) == 0)
            name = StringTable->insert(fileName + dStrlen(cwd) + 1, true);
        
        if(fullPath == NULL)
        {
            char buf[1024];
            fullPath = StringTable->insert(Platform::makeFullPathName(fileName, buf, sizeof(buf)), true);
        }
        
        modPath = Con::getModNameFromPath(fileName);
    }
    
    //
    addToCodeList();
    
    U32 globalSize,size,i;
   
    U32 codeLength;
    st.read(&codeLength);
    st.read(&lineBreakPairCount);
    
    U32 totSize = codeLength + lineBreakPairCount * 2;
    code = new U32[totSize];
   
    st.read(codeLength * sizeof(U32), code);
   
   
    ConsoleSerializationState serializationState;
    ConsoleValuePtr::readStack(st, serializationState, constants);
    
    for(i = codeLength; i < totSize; i++)
        st.read(&code[i]);
    
    lineBreakPairs = code + codeLength;
    
    if(lineBreakPairCount)
        calcBreakList();
    
    return true;
}

extern void CMDSetScanBuffer(const char *sb, const char *fn);
extern void CMD_reset();
extern S32 CMDparse();

bool CodeBlock::compile(const char *codeFileName, StringTableEntry fileName, const char *script)
{
   AssertFatal(Con::isMainThread(), "Compiling code on a secondary thread");
   
   gSyntaxError = false;
   
   consoleAllocReset();
   
   gStatementList = NULL;
   gCodeblockFunctionList = NULL;
   
   gCurrentLocalVariables = NULL;
   gLocalVariableStackIdx = 0;

   
   // Now do some parsing.
   CMDSetScanBuffer(script, fileName);
   CMD_reset();
   CMDparse();
   
   if(gSyntaxError || !gStatementList)
   {
      consoleAllocReset();
      return false;
   }
   
   FileStream st;
   if(!ResourceManager->openFileForWrite(st, codeFileName))
      return false;
   st.write(DSO_VERSION);
   
   resetTables();
   
   smInFunction = false;
   
   CodeBlock::smCurrentCodeBlock = this;
   CodeBlock::smCurrentCodeblockFunction = new CodeBlockFunction;
   CodeBlock::smCurrentCodeblockFunction->ip = 0;
   functions.push_back(smCurrentCodeblockFunction);
   Compiler::CompilerConstantsTable constants;
   constants.reset();
   
   CodeStream codeStream;
   codeStream.setConstantsTable(&constants);
   
   // Add actual local variables for the codeblock scope
   AssertFatal(gLocalVariableStackIdx == 0, "Unbalanced variable stack");
   if (gLocalVariableStackIdx == 0)
   {
      for (ReferencedVariableNode* walk = gCurrentLocalVariables; walk; walk = walk->next)
      {
         codeStream.addLocalVariable(walk->varName);
      }
   }
   
   U32 lastIp = compileBlock(gStatementList, codeStream, 0);
   codeStream.emitOpcodeABC(Compiler::OP_RETURN, 0, 0, 0);
   
   functions[0]->name = StringTable->insert("");
   functions[0]->numArgs = 0;
   functions[0]->maxStack = codeStream.getMaxStack();
   functions[0]->stmt = NULL;
   
   // Append additional functions in the order they are defined at the end
   codeStream.mLastKonstPage = 0; // reset konst page, it's always 0 at the start
   for (U32 i=1; i<functions.size(); i++)
   {
      functions[i]->ip = codeStream.tell();
      CodeBlock::smCurrentFunction = functions[i];
      functions[i]->stmt->compileFunction(codeStream, codeStream.tell());
   }
   
   lineBreakPairCount = codeStream.getNumLineBreaks();
   
   Vector<ConsoleValuePtr> outConstants;
   constants.build(outConstants);
   
   codeStream.emitCodeStream(&codeSize, &code, &lineBreakPairs);
   
   ConsoleSerializationState serializationState;
   st.write(outConstants.size());
   ConsoleValuePtr::writeStack(st, serializationState, outConstants);
   
   st.write(functions.size());
   st.write(codeSize);
   st.write(lineBreakPairCount);
   
   for (U32 i=0; i<functions.size(); i++)
   {
      CodeBlockFunction* func = functions[i];
      func->write(st, serializationState);
   }
   
   st.write(sizeof(U32) * codeSize, code);
   
   consoleAllocReset();
   st.close();
   
   return true;
}

void testStackWrite();

ConsoleValuePtr CodeBlock::compileExec(StringTableEntry fileName, const char *script, bool noCalls, S32 setFrame)
{
    AssertFatal(Con::isMainThread(), "Compiling code on a secondary thread");
    
    consoleAllocReset();
    
    name = fileName;
    
    if(fileName)
    {
        const StringTableEntry exePath = Platform::getMainDotCsDir();
        const StringTableEntry cwd = Platform::getCurrentDirectory();
        
        fullPath = NULL;
        
        if(Platform::isFullPath(fileName))
            fullPath = fileName;
        
        if(dStrnicmp(exePath, fileName, dStrlen(exePath)) == 0)
            name = StringTable->insert(fileName + dStrlen(exePath) + 1, true);
        else if(dStrnicmp(cwd, fileName, dStrlen(cwd)) == 0)
            name = StringTable->insert(fileName + dStrlen(cwd) + 1, true);
        
        if(fullPath == NULL)
        {
            char buf[1024];
            fullPath = StringTable->insert(Platform::makeFullPathName(fileName, buf, sizeof(buf)), true);
        }
        
        modPath = Con::getModNameFromPath(fileName);
    }
    
    //
    if (name)
    {
        if (const char *slash = dStrchr(this->name, '/'))
        {
            char root[512];
            dStrncpy(root, this->name, slash-this->name);
            root[slash-this->name] = 0;
            mRoot = StringTable->insert(root);
        }
    }
    
    if(name)
        addToCodeList();
    
    gStatementList = NULL;
    gCodeblockFunctionList = NULL;
   
   
   gCurrentLocalVariables = NULL;
   gLocalVariableStackIdx = 0;
   
   // Now do some parsing.
   CMDSetScanBuffer(script, fileName);
   CMD_reset();
   CMDparse();
   
    if(!gStatementList)
    {
        delete this;
        return "";
    }
    
    resetTables();
    
    smInFunction = false;
   
    CodeBlock::smCurrentCodeBlock = this;
    CodeBlock::smCurrentCodeblockFunction = new CodeBlockFunction;
    CodeBlock::smCurrentCodeblockFunction->ip = 0;
    functions.push_back(smCurrentCodeblockFunction);
    Compiler::CompilerConstantsTable constants;
   constants.reset();
    
    CodeStream codeStream;
   
    codeStream.setConstantsTable(&constants);
   
   
   // Add actual local variables for the codeblock scope
   AssertFatal(gLocalVariableStackIdx == 0, "Unbalanced variable stack");
   if (gLocalVariableStackIdx == 0)
   {
      for (ReferencedVariableNode* walk = gCurrentLocalVariables; walk; walk = walk->next)
      {
         codeStream.addLocalVariable(walk->varName);
      }
   }
   
    U32 lastIp = compileBlock(gStatementList, codeStream, 0);
    codeStream.emitOpcodeABC(Compiler::OP_RETURN, 0, 0, 0);
   
   functions[0]->name = StringTable->insert("");
   functions[0]->numArgs = 0;
   functions[0]->maxStack = codeStream.getMaxStack();
   functions[0]->stmt = NULL;
   
   // Append additional functions in the order they are defined at the end
   codeStream.mLastKonstPage = 0; // reset konst page, it's always 0 at the start
   for (U32 i=1; i<functions.size(); i++)
   {
      functions[i]->ip = codeStream.tell();
      CodeBlock::smCurrentFunction = functions[i];
      functions[i]->stmt->compileFunction(codeStream, codeStream.tell());
   }
    
    lineBreakPairCount = codeStream.getNumLineBreaks();
    
    Vector<ConsoleValuePtr> outConstants;
    constants.build(outConstants);
   
    codeStream.emitCodeStream(&codeSize, &code, &lineBreakPairs);
    
    consoleAllocReset();
    
    if(lineBreakPairCount && fileName)
        calcBreakList();
    
    if(lastIp+1 != codeSize)
        Con::warnf(ConsoleLogEntry::General, "precompile size mismatch, precompile: %d compile: %d", codeSize, lastIp);
    
    
    // jamesu - new exec function
   
   CodeBlockFunction* newFunction = functions[0];
   
   gNewEvalState.currentFrame.globalVars = gNewEvalState.globalVars;
   gNewEvalState.currentFrame.constants = outConstants.address();
   gNewEvalState.currentFrame.code = NULL;
   gNewEvalState.pushFunction(newFunction, this, NULL, 0);
   
   
   U32 currentTop = gNewEvalState.currentFrame.stackTop;
   // Reset local stack for function
   for (U32 i=currentTop; i<currentTop+newFunction->maxStack; i++)
   {
      ((ConsoleValuePtr*)(gNewEvalState.stack.address()+i))->setNull();
   }
   
   /*
   
   CodeBlockFunction* oldFunction = gNewEvalState.function;
   CodeBlockFunction* newFunction = functions[0];
   
   // Merge in exec block
   if (newFunction)
   {
      mergeLocalVars(oldFunction, newFunction, false);
   }
   
    S32 currentTop = gNewEvalState.stackTop;
    U32 currentIP = gNewEvalState.savedIP;
    ConsoleValue* currentConstants = gNewEvalState.constants;
   const char* filename = currentFilename;
   
    gNewEvalState.filename = fileName;
    gNewEvalState.globalVars = &gEvalState.globalVars;
   
    // Reset local stack for function
    for (U32 i=currentTop; i<currentTop+newFunction->maxStack; i++)
    {
       ((ConsoleValuePtr*)(gNewEvalState.stack.address()+i))->setNull();
    }
   
    gNewEvalState.constants = outConstants.address();
    gNewEvalState.stackTop = currentTop;
    gNewEvalState.function = functions[0];
   */
   
   
    CodeBlock::execBlock(&gNewEvalState);
   
   /*
    // Merge in exec block
    if (oldFunction)
    {
       mergeLocalVars(newFunction, oldFunction, true);
    }*/
   
    return gNewEvalState.yieldValue;
}

//-------------------------------------------------------------------------

void CodeBlock::incRefCount()
{
    refCount++;
}

void CodeBlock::decRefCount()
{
    refCount--;
    if(!refCount)
        delete this;
}

