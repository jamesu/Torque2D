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
#include "console/consoleSerialization.h"

#include "console/consoleDictionary.h"
#include "console/consoleNamespace.h"
#include "console/codeblockEvalState.h"

#include "console/compiler/consoleAST.h"
#include "collection/findIterator.h"
#include "io/resource/resourceManager.h"

#include "string/findMatch.h"
#include "io/fileStream.h"
#include "console/compiler/consoleCompiler.h"
#include "string/stringStack.h"

#include "sim/simBase.h"

#define ST_INIT_SIZE 15

static S32 QSORT_CALLBACK varCompare(const void* a,const void* b)
{
   return dStricmp( (*((Dictionary::Entry **) a))->name, (*((Dictionary::Entry **) b))->name );
}

void Dictionary::exportVariables(const char *varString, const char *fileName, bool append)
{
   const char *searchStr = varString;
   Vector<Entry *> sortList(__FILE__, __LINE__);

   for(S32 i = 0; i < hashTable->size;i ++)
   {
      Entry *walk = hashTable->data[i];
      while(walk)
      {
         if(FindMatch::isMatch((char *) searchStr, (char *) walk->name))
            sortList.push_back(walk);

         walk = walk->nextEntry;
      }
   }

   if(!sortList.size())
      return;

   dQsort((void *)sortList.address(), sortList.size(), sizeof(Entry *), varCompare);

   Vector<Entry *>::iterator s;
   FileStream strm;

   if(fileName)
   {
      if(!ResourceManager->openFileForWrite(strm, fileName, append ? FileStream::ReadWrite : FileStream::Write))
      {
         Con::errorf(ConsoleLogEntry::General, "Unable to open file '%s for writing.", fileName);
         return;
      }
      if(append)
         strm.setPosition(strm.getStreamSize());
   }

   char buffer[1024];
   const char *cat = fileName ? "\r\n" : "";

   for(s = sortList.begin(); s != sortList.end(); s++)
   {
      if ((*s)->value.type > ConsoleValue::TypeReferenceCounted)
      {
         dSprintf(buffer, sizeof(buffer), "%s = \"%s\";%s", (*s)->name, (*s)->value.getTempStringValue(), cat);
      }
      else
      {
         if ((*s)->value.isString())
         {
            dSprintf(buffer, sizeof(buffer), "%s = \"%s\";%s", (*s)->name, (*s)->value.getTempStringValue(), cat);
         }
         else
         {
            dSprintf(buffer, sizeof(buffer), "%s = %s;%s", (*s)->name, (*s)->value.getTempStringValue(), cat);
         }
      }
      if(fileName)
         strm.write(dStrlen(buffer), buffer);
      else
         Con::printf("%s", buffer);
   }
   if(fileName)
      strm.close();
}

void Dictionary::deleteVariables(const char *varString)
{
   const char *searchStr = varString;

   for(S32 i = 0; i < hashTable->size; i++)
   {
      Entry *walk = hashTable->data[i];
      while(walk)
      {
         Entry *matchedEntry = (FindMatch::isMatch((char *) searchStr, (char *) walk->name)) ? walk : NULL;
         walk = walk->nextEntry;
         if (matchedEntry)
            remove(matchedEntry); // assumes remove() is a stable remove (will not reorder entries on remove)
      }
   }
}

U32 HashPointer(StringTableEntry ptr)
{
   return (U32)(((dsize_t)ptr) >> 2);
}

Dictionary::Entry *Dictionary::lookup(StringTableEntry name)
{
   Entry *walk = hashTable->data[HashPointer(name) % hashTable->size];
   while(walk)
   {
      if(walk->name == name)
         return walk;
      else
         walk = walk->nextEntry;
   }

   return NULL;
}

Dictionary::Entry *Dictionary::add(StringTableEntry name)
{
   Entry *walk = hashTable->data[HashPointer(name) % hashTable->size];
   while(walk)
   {
      if(walk->name == name)
         return walk;
      else
         walk = walk->nextEntry;
   }
   Entry *ret;
   hashTable->count++;

   if(hashTable->count > hashTable->size * 2)
   {
    Entry head(NULL), *walk;
    S32 i;
    walk = &head;
    walk->nextEntry = 0;
    for(i = 0; i < hashTable->size; i++) {
        while(walk->nextEntry) {
            walk = walk->nextEntry;
        }
        walk->nextEntry = hashTable->data[i];
    }
      delete[] hashTable->data;
      hashTable->size = hashTable->size * 4 - 1;
      hashTable->data = new Entry *[hashTable->size];
      for(i = 0; i < hashTable->size; i++)
         hashTable->data[i] = NULL;
      walk = head.nextEntry;
      while(walk)
      {
         Entry *temp = walk->nextEntry;
         S32 idx = HashPointer(walk->name) % hashTable->size;
         walk->nextEntry = hashTable->data[idx];
         hashTable->data[idx] = walk;
         walk = temp;
      }
   }

   ret = new Entry(name);
   S32 idx = HashPointer(name) % hashTable->size;
   ret->nextEntry = hashTable->data[idx];
   hashTable->data[idx] = ret;
   return ret;
}

// deleteVariables() assumes remove() is a stable remove (will not reorder entries on remove)
void Dictionary::remove(Dictionary::Entry *ent)
{
   Entry **walk = &hashTable->data[HashPointer(ent->name) % hashTable->size];
   while(*walk != ent)
      walk = &((*walk)->nextEntry);

   *walk = (ent->nextEntry);
   delete ent;
   hashTable->count--;
}

Dictionary::Dictionary()
   :  hashTable( NULL ),
      exprState( NULL ),
      scopeName( NULL ),
      scopeNamespace( NULL ),
      code( NULL ),
      ip( 0 )
{
}

Dictionary::Dictionary(CodeBlockEvalState *state, Dictionary* ref)
   :  hashTable( NULL ),
      exprState( NULL ),
      scopeName( NULL ),
      scopeNamespace( NULL ),
      code( NULL ),
      ip( 0 )
{
   setState(state,ref);
}

void Dictionary::setState(CodeBlockEvalState *state, Dictionary* ref)
{
   exprState = state;

   if (ref)
      hashTable = ref->hashTable;
   else
   {
      hashTable = new HashTableData;
      hashTable->owner = this;
      hashTable->count = 0;
      hashTable->size = ST_INIT_SIZE;
      hashTable->data = new Entry *[hashTable->size];
   
      for(S32 i = 0; i < hashTable->size; i++)
         hashTable->data[i] = NULL;
   }
}

Dictionary::~Dictionary()
{
   if ( hashTable->owner == this ) 
   {
      reset();
      delete [] hashTable->data;
      delete hashTable;
   }
}

void Dictionary::reset()
{
   S32 i;
   Entry *walk, *temp;

   for(i = 0; i < hashTable->size; i++)
   {
      walk = hashTable->data[i];
      while(walk)
      {
         temp = walk->nextEntry;
         delete walk;
         walk = temp;
      }
      hashTable->data[i] = NULL;
   }
   hashTable->size = ST_INIT_SIZE;
   hashTable->count = 0;
}




const char *Dictionary::tabComplete(const char *prevText, S32 baseLen, bool fForward)
{
   S32 i;

   const char *bestMatch = NULL;
   for(i = 0; i < hashTable->size; i++)
   {
      Entry *walk = hashTable->data[i];
      while(walk)
      {
         if(Namespace::canTabComplete(prevText, bestMatch, walk->name, baseLen, fForward))
            bestMatch = walk->name;
         walk = walk->nextEntry;
      }
   }
   return bestMatch;
}


char *typeValueEmpty = (char*)"";

Dictionary::Entry::Entry(StringTableEntry in_name) : name(in_name), type(-1)
{
   value.setValue(ConsoleValuePtr());
}

Dictionary::Entry::~Entry()
{
}

ConsoleStringValuePtr Dictionary::getVariable(StringTableEntry name, bool *entValid)
{
   Entry *ent = lookup(name);
   if(ent)
   {
      if(entValid)
         *entValid = true;
      return ent->getStringValue();
   }
   if(entValid)
      *entValid = false;

   // Warn users when they access a variable that isn't defined.
	if(Con::gWarnUndefinedScriptVariables)
      Con::warnf(" *** Accessed undefined variable '%s'", name);

   return "";
}

ConsoleValuePtr Dictionary::getValueVariable(StringTableEntry name, bool *entValid)
{
   Entry *ent = lookup(name);
   if(ent)
   {
      if(entValid)
         *entValid = true;
      return ent->getValue();
   }
   if(entValid)
      *entValid = false;
   
   // Warn users when they access a variable that isn't defined.
	if(Con::gWarnUndefinedScriptVariables)
      Con::warnf(" *** Accessed undefined variable '%s'", name);
   
   return ConsoleValuePtr();
}

void Dictionary::setVariable(StringTableEntry name, const char *value)
{
   Entry *ent = add(name);
   if(!value)
      value = "";
   ent->setStringValue(value);
}

void Dictionary::setValueVariable(StringTableEntry name, const ConsoleValuePtr& variable)
{
   Entry *ent = add(name);
   ent->value.setValue(variable);
}

void Dictionary::addVariable(const char *name, S32 type, void *dataPtr)
{
	char scratchBuffer[1024];
   if(name[0] != '$')
   {
      scratchBuffer[0] = '$';
      dStrcpy(scratchBuffer + 1, name);
      name = scratchBuffer;
   }
   Entry *ent = add(StringTable->insert(name));
   ent->type = type;
   ent->dataPtr = dataPtr;
}

bool Dictionary::removeVariable(StringTableEntry name)
{
   if( Entry *ent = lookup(name) ){
      remove( ent );
      return true;
   }
   return false;
}

void Dictionary::setFrame(const CodeBlockFunction* function, ConsoleValuePtr *outStack) const
{
   const Vector<CodeBlockFunction::Symbol> &vars = function->vars;
   //CodeBlockEvalState *evalState = CodeBlockEvalState::getCurrent();
   
   for(S32 i = 0; i < hashTable->size;i ++)
   {
      Entry *walk = hashTable->data[i];
      while(walk)
      {
         for (U32 j=0; j<vars.size(); j++)
         {
            if (vars[j].varName == walk->name)
            {
               //Con::printf("setFrame(%s) REG %i abs %i", walk->name, vars[j].registerIdx, vars[j].registerIdx + (outStack - evalState->stack.address()));
               outStack[vars[j].registerIdx].setValue(walk->value);
               break;
            }
         }
         
         walk = walk->nextEntry;
      }
   }
}



