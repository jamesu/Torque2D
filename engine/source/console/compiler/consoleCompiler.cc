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
#include "console/console.h"
#include "debug/telnetDebugger.h"
#include "platform/event.h"

#include "console/compiler/consoleAST.h"
#include "collection/findIterator.h"
#include "io/resource/resourceManager.h"

#include "string/findMatch.h"
#include "console/consoleInternal.h"
#include "io/fileStream.h"
#include "console/compiler/consoleCompiler.h"

#include "sim/simBase.h"

#include "network/netStringTable.h"

StmtNode *gStatementList;

namespace Compiler
{

   F64 consoleStringToNumber(const char *str, StringTableEntry file, U32 line)
   {
      F64 val = dAtof(str);
      if(val != 0)
         return val;
      else if(!dStricmp(str, "true"))
         return 1;
      else if(!dStricmp(str, "false"))
         return 0;
      else if(file)
      {
         Con::warnf(ConsoleLogEntry::General, "%s (%d): string always evaluates to 0.", file, line);
         return 0;
      }
      return 0;
   }

   //------------------------------------------------------------
    
   CompilerConstantsTable gGlobalConstantTable;
    
   DataChunker          gConsoleAllocator;
   CodeBlock           *gCurBreakBlock;

   //------------------------------------------------------------


   CodeBlock *getBreakCodeBlock()         { return gCurBreakBlock; }
   void setBreakCodeBlock(CodeBlock *cb)  { gCurBreakBlock = cb;   }

   //------------------------------------------------------------

   bool gSyntaxError = false;

   //------------------------------------------------------------
   
   CompilerConstantsTable& getGlobalConstantsTable() { return gGlobalConstantTable; }

   void resetTables()
   {
      getGlobalConstantsTable().reset();
   }

   void *consoleAlloc(U32 size) { U8* ptr = (U8*)gConsoleAllocator.alloc(size); dMemset(ptr, '\0', size); return ptr;  }
   void consoleAllocReset()     { gConsoleAllocator.freeBlocks(); }

}

//-------------------------------------------------------------------------

using namespace Compiler;

//-------------------------------------------------------------------------


void Compiler::CompilerConstantsTable::emitNewPage()
{
   if (currentPage == numPages-1)
   {
      U32 newPageCount = numPages + 16;
      Entry** newList = (Entry**)consoleAlloc(sizeof(Entry**) * newPageCount);
      dMemset(newList, '\0', sizeof(Entry**) * newPageCount);
      dMemcpy(newList, list, sizeof(Entry**) * (currentPage+1));
      list = newList;
      numPages = newPageCount;
   }
   
   currentPage++;
   currentPageCount = 0;
   
#ifdef DEBUG_COMPILER
   Con::printf("  Emitted page %i", currentPage-1);
#endif
}

void Compiler::CompilerConstantsTable::debugPrintPage(U32 pageId)
{
   Entry** walk;
   U32 i = 0;
   Con::printf("Page[%i]...", pageId);
   for(walk = &list[pageId]; *walk; walk = &((*walk)->next), i++)
   {
      ConsoleValue *value = &(*walk)->value;
      
      Con::printf("  K[%i] = %s [%i]", i, value->getTempStringValue(), value->type);
   }
   
}

extern S32 TypeNetString;

Compiler::CompilerConstantRef Compiler::CompilerConstantsTable::addString(const char *str, bool caseSens, bool tag)
{
    // Is it already in?
    Entry** walk;
    Entry** endCurrentPage;
   
    for (S32 pageI=currentPage; pageI >= 0; --pageI)
    {
       U32 i = 0;
       for(walk = &list[pageI]; *walk; walk = &((*walk)->next), i++)
       {
           if(!tag && (*walk)->value.type != ConsoleValue::TypeInternalStringTableEntry)
               continue;
           else if (tag && (*walk)->value.type != TypeNetString)
               continue;

           if(caseSens)
           {
               if(!dStrcmp((*walk)->value.value.string, str))
                   return CompilerConstantRef(i, pageI);
           }
           else
           {
               if(!dStricmp((*walk)->value.value.string, str))
                   return CompilerConstantRef(i, pageI);
           }
       }
       
       if (pageI == currentPage)
       {
          endCurrentPage = walk;
       }
    }
   
    // Make sure we have enough constants in this page
    if (currentPageCount == Compiler::CONSTANTS_PER_PAGE)
    {
       emitNewPage();
       endCurrentPage = &list[currentPage];
    }
    walk = endCurrentPage;
   
#ifdef DEBUG_COMPILER
   Con::printf("CompilerConstantsTable::addString(%s) [page %i]", str, currentPage);
#endif
    
    // Write it out to current page
    Entry *newStr = (Entry *) consoleAlloc(sizeof(Entry));
    *walk = newStr;
    newStr->next = NULL;
    //newStr->start = totalLen;
    U32 len = dStrlen(str) + 1;
    if(tag && len < 7) // alloc space for the numeric tag 1 for tag, 5 for # and 1 for nul
        len = 7;
    totalLen += len;

    char* allocStr = (char *) consoleAlloc(len);
    dStrcpy(allocStr, str);
    newStr->value.value.string = allocStr;

    if (!tag)
    {
       newStr->value.type = ConsoleValue::TypeInternalStringTableEntry;
    }
    else
    {
       newStr->value.type = TypeNetString;
    }

    currentPageCount++;
    count++;
   
#ifdef DEBUG_COMPILER
   debugPrintPage(currentPage);
#endif
    return CompilerConstantRef(currentPageCount-1, currentPage);
}

Compiler::CompilerConstantRef Compiler::CompilerConstantsTable::addIntString(U32 value)
{
    dSprintf(buf, sizeof(buf), "%u", value);
    return addString(buf);
}

Compiler::CompilerConstantRef Compiler::CompilerConstantsTable::addFloatString(F64 value)
{
    dSprintf(buf, sizeof(buf), "%.9g", value);
    return addString(buf);
}

Compiler::CompilerConstantRef Compiler::CompilerConstantsTable::addNull()
{
   // Is it already in?
   Entry **walk;
   Entry **endCurrentPage;
   
   for (S32 pageI=currentPage; pageI >= 0; --pageI)
   {
      U32 i = 0;
      for(walk = &list[pageI]; *walk; walk = &((*walk)->next), i++)
      {
         if((*walk)->value.type != ConsoleValue::TypeInternalNull)
            continue;
         
         return CompilerConstantRef(i, pageI);
      }
      
      if (pageI == currentPage)
      {
         endCurrentPage = walk;
      }
   }
   
   // Make sure we have enough constants in this page
   if (currentPageCount == Compiler::CONSTANTS_PER_PAGE)
   {
      emitNewPage();
      endCurrentPage = &list[currentPage];
   }
   walk = endCurrentPage;
   
   
#ifdef DEBUG_COMPILER
   Con::printf("CompilerConstantsTable::addNull() [page %i]", currentPage);
#endif
   
   // Write it out.
   Entry *newStr = (Entry *)consoleAlloc(sizeof(Entry));
   dMemset(newStr, '\0', sizeof(Entry));
   *walk = newStr;
   newStr->next = NULL;
   //newStr->start = totalLen;
   
   newStr->value.type = ConsoleValue::TypeInternalNull;
   newStr->value.value.ival = 0;
   currentPageCount++;
   count++;
   
#ifdef DEBUG_COMPILER
   debugPrintPage(currentPage);
#endif
   
   return CompilerConstantRef(currentPageCount-1, currentPage);
}

CompilerConstantRef Compiler::CompilerConstantsTable::addNamespace(const char *str)
{
   // Is it already in?
   Entry** walk;
   Entry** endCurrentPage;
   
   for (S32 pageI=currentPage; pageI >= 0; --pageI)
   {
      U32 i = 0;
      for(walk = &list[pageI]; *walk; walk = &((*walk)->next), i++)
      {
         if((*walk)->value.type != ConsoleValue::TypeInternalNamespaceName)
            continue;
         
         if(!dStricmp((*walk)->value.value.string, str))
            return CompilerConstantRef(i, pageI);
      }
      
      if (pageI == currentPage)
      {
         endCurrentPage = walk;
      }
   }
   
   // Make sure we have enough constants in this page
   if (currentPageCount == Compiler::CONSTANTS_PER_PAGE)
   {
      emitNewPage();
      endCurrentPage = &list[currentPage];
   }
   walk = endCurrentPage;
   
#ifdef DEBUG_COMPILER
   Con::printf("CompilerConstantsTable::addNamespace(%s) [page %i]", str, currentPage);
#endif
   
   // Write it out to current page
   Entry *newStr = (Entry *) consoleAlloc(sizeof(Entry));
   *walk = newStr;
   newStr->next = NULL;
   //newStr->start = totalLen;
   U32 len = dStrlen(str) + 1;
   totalLen += len;
   
   char* allocStr = (char *) consoleAlloc(len);
   //newStr->len = len;
   //newStr->tag = tag;
   dStrcpy(allocStr, str);
   newStr->value.type = ConsoleValue::TypeInternalNamespaceName;
   newStr->value.value.string = StringTable->insert(allocStr);
   currentPageCount++;
   count++;
   
#ifdef DEBUG_COMPILER
   debugPrintPage(currentPage);
#endif
   
   return CompilerConstantRef(currentPageCount-1, currentPage);
}

Compiler::CompilerConstantRef Compiler::CompilerConstantsTable::addInt(S64 value)
{
   Entry** walk;
   Entry** endCurrentPage;
   
   for (S32 pageI=currentPage; pageI >= 0; --pageI)
   {
      U32 i = 0;
      for(walk = &list[pageI]; *walk; walk = &((*walk)->next), i++)
      {
        if((*walk)->value.type == ConsoleValue::TypeInternalInt && value == (*walk)->value.value.ival)
            return CompilerConstantRef(i, pageI);
      }
      
      if (pageI == currentPage)
      {
         endCurrentPage = walk;
      }
   }
   
   
   // Make sure we have enough constants in this page
   if (currentPageCount == Compiler::CONSTANTS_PER_PAGE)
   {
      emitNewPage();
      endCurrentPage = &list[currentPage];
   }
   walk = endCurrentPage;
   
#ifdef DEBUG_COMPILER
   Con::printf("CompilerConstantsTable::addInt(%i) [page %i]", value, currentPage);
#endif
   
   Entry *newFloat = (Entry *) consoleAlloc(sizeof(Entry));
   dMemset(newFloat, '\0', sizeof(Entry));
    newFloat->value.type = ConsoleValue::TypeInternalInt;
    newFloat->value.value.ival = value;
    newFloat->next = NULL;
    count++;
    currentPageCount++;
    *walk = newFloat;
   
#ifdef DEBUG_COMPILER
   debugPrintPage(currentPage);
#endif
   
    return CompilerConstantRef(currentPageCount-1, currentPage);
}

Compiler::CompilerConstantRef Compiler::CompilerConstantsTable::addFloat(F64 value)
{
   Entry** walk;
   Entry** endCurrentPage;
   for (S32 pageI=currentPage; pageI >= 0; --pageI)
   {
      U32 i = 0;
      for(walk = &list[pageI]; *walk; walk = &((*walk)->next), i++)
      {
        if((*walk)->value.type == ConsoleValue::TypeInternalFloat && value == (*walk)->value.value.fval)
            return CompilerConstantRef(i, pageI);
      }
      
      if (pageI == currentPage)
      {
         endCurrentPage = walk;
      }
   }
   
   // Make sure we have enough constants in this page
   if (currentPageCount == Compiler::CONSTANTS_PER_PAGE)
   {
      emitNewPage();
      endCurrentPage = &list[currentPage];
   }
   walk = endCurrentPage;
   
#ifdef DEBUG_COMPILER
   Con::printf("CompilerConstantsTable::addFloat(%f) [page %i]", value, currentPage);
#endif
   
   Entry *newFloat = (Entry *) consoleAlloc(sizeof(Entry));
   dMemset(newFloat, '\0', sizeof(Entry));
    newFloat->value.type = ConsoleValue::TypeInternalFloat;
    newFloat->value.value.fval = value;
    newFloat->next = NULL;
    count++;
    currentPageCount++;
    *walk = newFloat;
   
#ifdef DEBUG_COMPILER
   debugPrintPage(currentPage);
#endif

    return CompilerConstantRef(currentPageCount-1, currentPage);
}

void Compiler::CompilerConstantsTable::checkFlushPage(U32 requiredConstants)
{
   if (currentPageCount + requiredConstants > Compiler::CONSTANTS_PER_PAGE)
   {
      S32 itemsLeft = Compiler::CONSTANTS_PER_PAGE - currentPageCount;
      
      Entry **walk;
      
      U32 i = 0;
      for(walk = &list[currentPage]; *walk; walk = &((*walk)->next), i++)
      {
      }
      
#ifdef DEBUG_COMPILER
      Con::warnf("checkFlushPage %i items currently", i);
#endif

      for (i=0; i<itemsLeft; i++)
      {
         // Write it out.
         Entry *newStr = (Entry *)consoleAlloc(sizeof(Entry));
         dMemset(newStr, '\0', sizeof(Entry));
         *walk = newStr;
         newStr->next = NULL;
         
         newStr->value.type = ConsoleValue::TypeInternalNull;
         newStr->value.value.ival = 0;
         currentPageCount++;
         count++;
         
         walk = &((*walk)->next);
      }
      
      emitNewPage();
   }
}

Compiler::CompilerConstantsTable::Entry** Compiler::CompilerConstantsTable::getConstantEntry(U32 idx, U32 pageIdx)
{
   Entry **walk;
   U32 i = 0;
   
   for(walk = &list[pageIdx]; *walk; walk = &((*walk)->next), i++)
   {
      if(i == idx)
      {
         AssertFatal(walk != NULL, "errm wtf");
         return walk;
      }
   }
   
   return NULL;
}

Compiler::CompilerConstantRef Compiler::CompilerConstantsTable::duplicateConstant(CompilerConstantRef idx)
{
   Entry** dupWalk = getConstantEntry(idx.idx, idx.page);
   Entry** walk = NULL;
   
   AssertFatal(dupWalk != NULL, "Invalid constant");
   ConsoleValuePtr valueCheck = (*dupWalk)->value;
   
   // Check to see if we already have the constant (might happen)
   U32 i = 0;
   for(walk = &list[currentPage]; *walk; walk = &((*walk)->next), i++)
   {
      Entry* constCheck = *walk;
      if (constCheck->value.type == valueCheck.type &&
          dMemcmp(&constCheck->value.value, &valueCheck.value, sizeof(ConsoleValueBase)) == 0)
      {
         return Compiler::CompilerConstantRef(i, currentPage);
      }
   }
   
   // Make sure we have enough constants in this page
   if (currentPageCount == Compiler::CONSTANTS_PER_PAGE)
   {
      emitNewPage();
      walk = &list[currentPage];
   }
   
#ifdef DEBUG_COMPILER
   Con::printf("CompilerConstantsTable::duplicate(%i,%i) [page %i]", idx.idx, idx.page, currentPage);
#endif

   Entry *dupConst = (Entry *) consoleAlloc(sizeof(Entry));
   dMemset(dupConst, '\0', sizeof(Entry));
   dupConst->value.setValue(valueCheck);
   dupConst->next = NULL;
   count++;
   currentPageCount++;
   *walk = dupConst;
   
   return CompilerConstantRef(currentPageCount-1, currentPage);
}

void Compiler::CompilerConstantsTable::reset()
{
    list = NULL;
    totalLen = 0;
    count = 0;
    numPages = 16;
    currentPage = 0;
    currentPageCount = 0;
   
    list = (Entry**)consoleAlloc(sizeof(Entry**) * numPages);
    dMemset(list, '\0', sizeof(Entry**) * numPages);
}

void Compiler::CompilerConstantsTable::build(Vector<ConsoleValuePtr> &outConstants)
{
    Entry **walk;
    outConstants.setSize(count);
   
#ifdef DEBUG_COMPILER
   Con::printf("K %i constants", count);
#endif
   
   U32 ocount = 0;
   for (S32 pageI=0; pageI < currentPage+1; pageI++)
   {
      U32 i = 0;
      for(walk = &list[pageI]; *walk; walk = &((*walk)->next), i++)
      {
         ConsoleValue *value = &(*walk)->value;
         if (value->type == ConsoleValue::TypeInternalStringTableEntry)
         {
            value->value.string = StringTable->insert(value->value.string);
         }
         else if (value->type == TypeNetString)
         {
            value->value.refValue = NetStringHandle::asReferenceCountedType(value->value.string);
         }
         
         outConstants[ocount].type = ConsoleValue::TypeInternalNull;
         outConstants[ocount++].setValue(*value);
         
#ifdef DEBUG_COMPILER
         Con::printf("K[%i,%i(%i)] = %s [%i]", i, pageI, ocount-1, value->getTempStringValue(), value->type);
#endif
      }
   }
}

//-------------------------------------------------------------------------

U8 *CodeStream::allocCode(U32 sz)
{
    U8 *ptr = NULL;
    if (mCodeHead)
    {
        const U32 bytesLeft = BlockSize - mCodeHead->size;
        if (bytesLeft > sz)
        {
            ptr = mCodeHead->data + mCodeHead->size;
            mCodeHead->size += sz;
            return ptr;
        }
    }
    
    CodeData *data = new CodeData;
    data->data = (U8*)dMalloc(BlockSize);
    data->size = sz;
    data->next = NULL;
    
    if (mCodeHead)
        mCodeHead->next = data;
    mCodeHead = data;
    if (mCode == NULL)
        mCode = data;
    return data->data;
}

//-------------------------------------------------------------------------

CodeStream::CodeData *CodeStream::allocCodeData()
{
   CodeData *data = new CodeData;
   data->data = NULL;
   data->size = BlockSize;
   data->next = NULL;
   
   if (mCodeHead)
      mCodeHead->next = data;
   mCodeHead = data;
   if (mCode == NULL)
      mCode = data;
   return data;
}

//-------------------------------------------------------------------------

void CodeStream::fixLoop(U32 loopBlockStart, U32 breakPoint, U32 continuePoint)
{
    AssertFatal(mFixStack.size() > 0, "Fix stack mismatch");
    
    U32 fixStart = mFixStack[mFixStack.size()-1];
    for (U32 i=fixStart; i<mFixList.size(); i += 2)
    {
        FixType type = (FixType)mFixList[i+1];
        
        S32 fixedIp = 0;
        bool valid = true;
        
        switch (type)
        {
           case FIXTYPE_LOOPBLOCKSTART:
                //Con::printf("FIXTYPE_LOOPBLOCKSTART point %i @ %i", loopBlockStart, mFixList[i]);
                fixedIp = (S32)loopBlockStart - (mFixList[i]+1);
                break;
            case FIXTYPE_BREAK:
                //Con::printf("FIXTYPE_BREAK point %i @ %i", breakPoint, mFixList[i]);
                fixedIp = (S32)breakPoint - (mFixList[i]+1);
                break;
           case FIXTYPE_CONTINUE:
                //Con::printf("FIXTYPE_CONTINUE point %i @ %i", continuePoint, mFixList[i]);
                fixedIp = (S32)continuePoint - (mFixList[i]+1);
                break;
            default:
                //Con::warnf("Address %u fixed as %u", mFixList[i], mFixList[i+1]);
                valid = false;
                break;
        }
        
        if (valid)
        {
            fixedIp = TS2_OP_ENC_A_Bx(Compiler::OP_JMP, 0, TS2_OP_MAKE_sBX(fixedIp));
           
           
            patch(mFixList[i], fixedIp);
        }
    }
}

//-------------------------------------------------------------------------

void CodeStream::emitCodeStream(U32 *size, U32 **stream, U32 **lineBreaks)
{
    // Alloc stream
    U32 numLineBreaks = getNumLineBreaks();
    *stream = new U32[mCodePos + (numLineBreaks * 2)];
    dMemset(*stream, '\0', mCodePos + (numLineBreaks * 2));
    *size = mCodePos;
    
    // Dump chunks & line breaks
    U32 outBytes = mCodePos * sizeof(U32);
    U8 *outPtr = *((U8**)stream);
    for (CodeData *itr = mCode; itr != NULL; itr = itr->next)
    {
        U32 bytesToCopy = itr->size > outBytes ? outBytes : itr->size;
        dMemcpy(outPtr, itr->data, bytesToCopy);
        outPtr += bytesToCopy;
        outBytes -= bytesToCopy;
    }
    
    *lineBreaks = *stream + mCodePos;
    dMemcpy(*lineBreaks, mBreakLines.address(), sizeof(U32) * mBreakLines.size());
    
    // Apply patches on top
    for (U32 i=0; i<mPatchList.size(); i++)
    {
        PatchEntry &e = mPatchList[i];
        (*stream)[e.addr] = e.value;
    }
}

//-------------------------------------------------------------------------

void CodeStream::reset()
{
    mLastKonstPage = 0;
    mKonstPageVersion = 0;
    mCodePos = 0;
    mFixStack.clear();
    mFixLoopStack.clear();
    mFixList.clear();
    mBreakLines.clear();
    mTargetList.clear();
    mVarInfo.clear();
    mTempStackSize = 0;
    mMaxStackSize = 0;
    
    // Pop down to one code block
    CodeData *itr = mCode ? mCode->next : NULL;
    while (itr != NULL)
    {
        CodeData *next = itr->next;
        dFree(itr->data);
        delete(itr);
        itr = next;
    }
    
    if (mCode)
    {
        mCode->size = 0;
        mCode->next = NULL;
        mCodeHead = mCode;
    }
}

