//
//  codeblockUtil.cpp
//  Torque2D
//
//  Created by James Urquhart on 21/11/2015.
//

#include "console/console.h"
#include "console/codeBlock.h"
#include "console/codeblockEvalState.h"
#include "console/codeblockUtil.h"
#include "io/resource/resourceManager.h"

#define TORQUE_ALLOW_DSO_GENERATION

extern bool scriptExecutionEcho;

namespace CodeblockUtil
{
   CodeBlock* loadCompiledScriptFromStream(Stream &s)
   {
      return NULL;
   }
   
   CodeBlock* loadScriptFromStream(Stream &s)
   {
      return NULL;
   }
   
   void getDSOPathForScript(const char *pathBuffer, char *outBuffer, U32 bufferSize, StringTableEntry rootDSOPath)
   {
      const char *filenameOnly = dStrrchr(pathBuffer, '/');
      if(filenameOnly)
         ++filenameOnly;
      else
         filenameOnly = pathBuffer;
      
      // we could skip this step and rid ourselves of a bunch of nonsense but we can't be
      // certain the dso path is the same as the path given to use in scriptFileNameBuffer
      char pathAndFilename[4096];
      Platform::makeFullPathName(filenameOnly, pathAndFilename, sizeof(pathAndFilename), rootDSOPath);
      dStrcpyl(outBuffer, bufferSize, pathAndFilename, ".dso", NULL);
   }
   
   bool execFile(const char *filename, bool noCalls, bool journal, bool allowCompiled)
   {
      CodeBlockEvalState* state = CodeBlockEvalState::getCurrent();
      
      bool doJournal = false;
      
#ifdef TORQUE_ALLOW_JOURNALING
      if(state->journalDepth >= state->execDepth)
         state->journalDepth = state->execDepth + 1;
      else
         doJournal = true;
      
      if( doJournal && !journal )
      {
         journal = true;
         state->journalDepth = state->execDepth;
      }
#else
      doJournal = false;
#endif //TORQUE_ALLOW_JOURNALING
      
      bool ret = false;
      
      if (doJournal)
      {
         state->journalDepth = state->execDepth;
      }
      
      // If in journal mode and playing back, directly execute script
      // from journal.
#ifdef TORQUE_ALLOW_JOURNALING
      if(doJournal && Game->isJournalReading())
      {
         char fileNameBuf[1024];
         bool fileRead;
         U32 fileSize;
         
         Game->getJournalStream()->readString(fileNameBuf);
         Game->getJournalStream()->read(&fileRead);
         if(!fileRead)
         {
            Con::errorf(ConsoleLogEntry::Script, "Journal script read (failed) for %s", fileNameBuf);
            execDepth--;
            return false;
         }
         Game->journalRead(&fileSize);
         
         CodeBlock *newCodeBlock = new CodeBlock();
         newCodeBlock->incRefCount();
         
         const char *ext = dStrrchr(filenameBuf, '.');
         char *script = new char[fileSize + 1];
         Game->journalRead(fileSize, script);
         script[fileSize] = 0;
         
         bool didLoadFromJournal = false;
         
         if (dStricmp(ext, ".dso") == 0)
         {
            MemStream s(fileSize, script, true, false);
            didLoadFromJournal = newCodeBlock->read(s);
         }
         else
         {
            newCodeBlock->compile(fileNameBuf, script);
         }
         
         scriptFileName = StringTable->insert(fileNameBuf);
         
         Con::printf("Executing (journal-read) %s.", scriptFileName);
         newCodeBlock->compileExec(scriptFileName, script, noCalls, -1);
         newCodeBlock->decRefCount();
         
         delete [] script;
         execDepth--;
         return true;
      }
#endif //TORQUE_ALLOW_JOURNALING
      
      // Determine the filename we actually want...
      char pathBuffer[4096];
      char nameBuffer[4096];
      Con::expandPath(pathBuffer, sizeof(pathBuffer), filename);
      const char *ext = dStrrchr(pathBuffer, '.');
      
      if(!ext)
      {
         // We need an extension!
         Con::errorf(ConsoleLogEntry::Script, "exec: invalid script file name %s.", pathBuffer);
         state->execDepth--;
         return false;
      }
      
      // Figure out where to put DSOs
      StringTableEntry dsoPath = CodeBlock::getDSOPath(pathBuffer);
      StringTableEntry scriptFileName = StringTable->EmptyString;
      
#ifndef TORQUE_ALLOW_DSO_GENERATION
      allowCompiled = false;
#endif
      
      if(!ResourceManager->find(pathBuffer) && allowCompiled)
      {
         // Check if we have a DSO instead, falling back to using a cs in the prefs path
         getDSOPathForScript(pathBuffer, nameBuffer, sizeof(nameBuffer), dsoPath);
         
         if(!ResourceManager->find(nameBuffer))
            scriptFileName = Platform::getPrefsPath(Platform::stripBasePath(pathBuffer));
         else
            scriptFileName = StringTable->insert(pathBuffer);
      }
      else
      {
         scriptFileName = StringTable->insert(pathBuffer);
      }
      
      if(*scriptFileName == 0)
      {
         state->execDepth--;
         return false;
      }
      
      bool doCompile = allowCompiled && !Con::getBoolVariable("Scripts::ignoreDSOs");
      
      // Avoid compiling stuff if it's in the prefs path
      StringTableEntry prefsPath = Platform::getPrefsPath();
      if( dStrlen(prefsPath) > 0 && dStrnicmp(scriptFileName, prefsPath, dStrlen(prefsPath)) == 0)
      {
         doCompile = false;
      }
      
      // Ok, we let's try to load and compile the script.
      ResourceObject *rScr = ResourceManager->find(scriptFileName);
      ResourceObject *rCom = NULL;
      
      char* script = NULL;
      U32 scriptSize = 0;
      
      Stream *compiledStream = NULL;
      FileTime comModifyTime, scrModifyTime;
      
      // If we're supposed to be compiling this file, check to see if there's a DSO
      if(doCompile)
      {
         getDSOPathForScript(pathBuffer, nameBuffer, sizeof(nameBuffer), dsoPath);
         
         rCom = ResourceManager->find(nameBuffer);
         
         if(rCom)
            rCom->getFileTimes(NULL, &comModifyTime);
         if(rScr)
            rScr->getFileTimes(NULL, &scrModifyTime);
      }
      
      // If we had a DSO, let's check to see if we should be reading from it.
      if((doCompile && rCom) &&
         (!rScr || Platform::compareFileTimes(comModifyTime, scrModifyTime) >= 0))
      {
         compiledStream = ResourceManager->openStream(nameBuffer);
      }
      
      CodeBlockPtr runtimeBlock = NULL;
      
      if(rScr && !compiledStream)
      {
         Stream *scriptStream = ResourceManager->openStream(scriptFileName);
         
         if(scriptStream)
         {
            scriptSize = ResourceManager->getSize(scriptFileName);
            script = new char [scriptSize+1];
            scriptStream->read(scriptSize, script);
            
            ResourceManager->closeStream(scriptStream);
            script[scriptSize] = 0;
         }
         
#ifdef   TORQUE_ALLOW_JOURNALING
         if(journal && Game->isJournalWriting())
         {
            Game->getJournalStream()->write(bool(s != NULL && scriptSize > 0));
            if (s != NULL && scriptSize > 0)
            {
               Game->getJournalStream()->writeString(scriptFileName);
            }
         }
#endif
         
         if (!scriptSize || !script)
         {
            delete[] script;
            Con::errorf(ConsoleLogEntry::Script, "exec: invalid script file %s.", scriptFileName);
            state->execDepth--;
            return false;
         }
         
#ifdef   TORQUE_ALLOW_JOURNALING
         if(doJournal && Game->isJournalWriting())
         {
            Game->journalWrite(scriptSize);
            Game->journalWrite(scriptSize, script);
         }
#endif
         
         // Compile script
         runtimeBlock = new CodeBlock();
         runtimeBlock->compile(scriptFileName, script);
         
         if(doCompile)
         {
            // compile this baddie.
#if defined(TORQUE_DEBUG)
            Con::printf("Compiling %s...", scriptFileName);
#endif
            FileStream fs;
            if (fs.open(nameBuffer, FileStream::Write))
            {
               runtimeBlock->save(fs);
            }
         }
         
         delete[] script;
         script = 0;
         
      }
      else
      {
#ifdef   TORQUE_ALLOW_JOURNALING
         if(doJournal && Game->isJournalWriting())
         {
            if (compiledStream)
            {
               scriptSize = ResourceManager->getSize(nameBuffer);
               Game->getJournalStream()->write(bool(scriptSize > 0));
               if (scriptSize > 0)
               {
                  script = new char [scriptSize+1];
                  compiledStream->read(scriptSize, script);
                  compiledStream->setPosition(0);
                  
                  Game->getJournalStream()->writeString(nameBuffer);
                  Game->journalWrite(scriptSize);
                  Game->journalWrite(scriptSize, script);
                  
                  delete[] script;
                  script = 0;
               }
            }
            else
            {
               Game->getJournalStream()->write(bool(false));
            }
         }
#endif   //TORQUE_ALLOW_JOURNALING
         
         if (compiledStream)
         {
            runtimeBlock = new CodeBlock;
            runtimeBlock->read(*compiledStream, scriptFileName);
            doCompile = false;
            ResourceManager->closeStream(compiledStream);
         }
      }
      
      // If we have a codeblock, we can now exec!
      if(runtimeBlock)
      {
         // Log this as an execution if we're not reling on a DSO
         if (!doCompile)
         {
#if defined(TORQUE_DEBUG)
            Con::printf("Executing %s...", scriptFileName);
#endif
         }
         
         // We're all doCompile, so let's run it.
         F32 st1 = (F32)Platform::getRealMilliseconds();
         {
            runtimeBlock->execRoot(noCalls, -1);
         }
         F32 et1 = (F32)Platform::getRealMilliseconds();
         
         F32 etf = et1 - st1;
         
         if ( scriptExecutionEcho )
            Con::printf("Loaded compiled script %s. Took %.0f ms", scriptFileName, etf);
         
         ret = true;
      }
      else
      {
         // Don't have anything.
         Con::warnf(ConsoleLogEntry::Script, "Missing file: %s!", pathBuffer);
         ret = false;
      }
      
      state->execDepth--;
      return ret;
   }
   
   
   bool compile(const char *fileName)
   {
      char scriptFilenameBuffer[4096];
      char dsoFilenameBuffer[4096];
      FileTime comModifyTime, scrModifyTime;
      
      Con::expandPath( scriptFilenameBuffer, sizeof( scriptFilenameBuffer ), fileName );
      
      // Figure out where to put DSOs
      StringTableEntry dsoPath = CodeBlock::getDSOPath(scriptFilenameBuffer);
      if(dsoPath && *dsoPath == 0)
         return false;
      
      // Check if we have a DSO instead, falling back to using a cs in the prefs path
      getDSOPathForScript(scriptFilenameBuffer, dsoFilenameBuffer, sizeof(dsoFilenameBuffer), dsoPath);
      
      ResourceObject *rScr = ResourceManager->find(scriptFilenameBuffer);
      ResourceObject *rCom = ResourceManager->find(dsoFilenameBuffer);
      
      if(rCom)
         rCom->getFileTimes(NULL, &comModifyTime);
      if(rScr)
         rScr->getFileTimes(NULL, &scrModifyTime);
      
      Stream *scriptStream = ResourceManager->openStream(dsoFilenameBuffer);
      U32 scriptSize = 0;
      char* script = NULL;
      if(scriptStream)
      {
         scriptSize = ResourceManager->getSize(scriptFilenameBuffer);
         script = new char [scriptSize+1];
         scriptStream->read(scriptSize, script);
         ResourceManager->closeStream(scriptStream);
         script[scriptSize] = 0;
      }
      
      if (!scriptSize || !script)
      {
         delete [] script;
         Con::errorf(ConsoleLogEntry::Script, "compile: invalid script file %s.", scriptFilenameBuffer);
         return false;
      }
      
#if defined(TORQUE_DEBUG)
      Con::printf("Compiling %s...", scriptFilenameBuffer);
#endif
      
      CodeBlock *code = new CodeBlock();
      bool ret = code->compile(scriptFilenameBuffer, script);
      delete code;
      code = NULL;
      
      delete[] script;
      return ret;
   }
   
};