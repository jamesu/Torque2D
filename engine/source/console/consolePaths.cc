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
#include "console/console.h"
#include "console/codeblockEvalState.h"

namespace Con
{

//------------------------------------------------------------------------------

StringTableEntry getModNameFromPath(const char *path)
{
   if(path == NULL || *path == 0)
      return NULL;
   
   char buf[1024];
   buf[0] = 0;
   
   if(path[0] == '/' || path[1] == ':')
   {
      // It's an absolute path
      const StringTableEntry exePath = Platform::getMainDotCsDir();
      U32 len = dStrlen(exePath);
      if(dStrnicmp(exePath, path, len) == 0)
      {
         const char *ptr = path + len + 1;
         const char *slash = dStrchr(ptr, '/');
         if(slash)
         {
            dStrncpy(buf, ptr, slash - ptr);
            buf[slash - ptr] = 0;
         }
         else
            return NULL;
      }
      else
         return NULL;
   }
   else
   {
      const char *slash = dStrchr(path, '/');
      if(slash)
      {
         dStrncpy(buf, path, slash - path);
         buf[slash - path] = 0;
      }
      else
         return NULL;
   }
   
   return StringTable->insert(buf);
}

//-----------------------------------------------------------------------------

typedef HashMap<StringTableEntry, StringTableEntry> typePathExpandoMap;
static typePathExpandoMap PathExpandos;

//-----------------------------------------------------------------------------

void addPathExpando( const char* pExpandoName, const char* pPath )
{
   // Sanity!
   AssertFatal( pExpandoName != NULL, "Expando name cannot be NULL." );
   AssertFatal( pPath != NULL, "Expando path cannot be NULL." );
   
   // Fetch expando name.
   StringTableEntry expandoName = StringTable->insert( pExpandoName );
   
   // Fetch the length of the path.
   S32 pathLength = dStrlen(pPath);
   
   char pathBuffer[1024];
   
   // Sanity!
   if ( pathLength == 0 || pathLength >= sizeof(pathBuffer) )
   {
      Con::warnf( "Cannot add path expando '%s' with path '%s' as the path is an invalid length.", pExpandoName, pPath );
      return;
   }
   
   // Strip repeat slashes.
   if ( !Con::stripRepeatSlashes(pathBuffer, pPath, sizeof(pathBuffer) ) )
   {
      Con::warnf( "Cannot add path expando '%s' with path '%s' as the path is an invalid length.", pExpandoName, pPath );
      return;
   }
   
   // Fetch new path length.
   pathLength = dStrlen(pathBuffer);
   
   // Sanity!
   if ( pathLength == 0 )
   {
      Con::warnf( "Cannot add path expando '%s' with path '%s' as the path is an invalid length.", pExpandoName, pPath );
      return;
   }
   
   // Remove any terminating slash.
   if (pathBuffer[pathLength-1] == '/')
      pathBuffer[pathLength-1] = 0;
   
   // Fetch expanded path.
   StringTableEntry expandedPath = StringTable->insert( pathBuffer );
   
   // Info.
#if defined(TORQUE_DEBUG)
   Con::printf("Adding path expando of '%s' as '%s'.", expandoName, expandedPath );
#endif
   
   // Find any existing path expando.
   typePathExpandoMap::iterator expandoItr = PathExpandos.find( pExpandoName );
   
   // Does the expando exist?
   if( expandoItr != PathExpandos.end() )
   {
      // Yes, so modify the path.
      expandoItr->value = expandedPath;
      return;
   }
   
   // Insert expando.
   PathExpandos.insert( expandoName, expandedPath );
}

//-----------------------------------------------------------------------------

StringTableEntry getPathExpando( const char* pExpandoName )
{
   // Sanity!
   AssertFatal( pExpandoName != NULL, "Expando name cannot be NULL." );
   
   // Fetch expando name.
   StringTableEntry expandoName = StringTable->insert( pExpandoName );
   
   // Find any existing path expando.
   typePathExpandoMap::iterator expandoItr = PathExpandos.find( expandoName );
   
   // Does the expando exist?
   if( expandoItr != PathExpandos.end() )
   {
      // Yes, so return it.
      return expandoItr->value;
   }
   
   // Not found.
   return NULL;
}

//-----------------------------------------------------------------------------

void removePathExpando( const char* pExpandoName )
{
   // Sanity!
   AssertFatal( pExpandoName != NULL, "Expando name cannot be NULL." );
   
   // Fetch expando name.
   StringTableEntry expandoName = StringTable->insert( pExpandoName );
   
   // Find any existing path expando.
   typePathExpandoMap::iterator expandoItr = PathExpandos.find( expandoName );
   
   // Does the expando exist?
   if ( expandoItr == PathExpandos.end() )
   {
      // No, so warn.
#if defined(TORQUE_DEBUG)
      Con::warnf("Removing path expando of '%s' but it does not exist.", expandoName );
#endif
      return;
   }
   
   // Info.
#if defined(TORQUE_DEBUG)
   Con::printf("Removing path expando of '%s' as '%s'.", expandoName, expandoItr->value );
#endif
   // Remove expando.
   PathExpandos.erase( expandoItr );
}

//-----------------------------------------------------------------------------

bool isPathExpando( const char* pExpandoName )
{
   // Sanity!
   AssertFatal( pExpandoName != NULL, "Expando name cannot be NULL." );
   
   // Fetch expando name.
   StringTableEntry expandoName = StringTable->insert( pExpandoName );
   
   return PathExpandos.contains( expandoName );
}

//-----------------------------------------------------------------------------

U32 getPathExpandoCount( void )
{
   return PathExpandos.size();
}

//-----------------------------------------------------------------------------

StringTableEntry getPathExpandoKey( U32 expandoIndex )
{
   // Finish if index is out of range.
   if ( expandoIndex >= PathExpandos.size() )
      return NULL;
   
   // Find indexed iterator.
   typePathExpandoMap::iterator expandoItr = PathExpandos.begin();
   while( expandoIndex > 0 ) { ++expandoItr; --expandoIndex; }
   
   return expandoItr->key;
}

//-----------------------------------------------------------------------------

StringTableEntry getPathExpandoValue( U32 expandoIndex )
{
   // Finish if index is out of range.
   if ( expandoIndex >= PathExpandos.size() )
      return NULL;
   
   // Find indexed iterator.
   typePathExpandoMap::iterator expandoItr = PathExpandos.begin();
   while( expandoIndex > 0 ) { ++expandoItr; --expandoIndex; }
   
   return expandoItr->value;
}

//-----------------------------------------------------------------------------

bool expandPath( char* pDstPath, U32 size, const char* pSrcPath, const char* pWorkingDirectoryHint, const bool ensureTrailingSlash )
{
   char pathBuffer[2048];
   const char* pSrc = pSrcPath;
   char* pSlash;
   
   // Fetch leading character.
   const char leadingToken = *pSrc;
   
   // Fetch following token.
   const char followingToken = leadingToken != 0 ? pSrc[1] : 0;
   
   // Expando.
   if ( leadingToken == '^' )
   {
      // Initial prefix search.
      const char* pPrefixSrc = pSrc+1;
      char* pPrefixDst = pathBuffer;
      
      // Search for end of expando.
      while( *pPrefixSrc != '/' && *pPrefixSrc != 0 )
      {
         // Copy prefix character.
         *pPrefixDst++ = *pPrefixSrc++;
      }
      
      // Yes, so terminate the expando string.
      *pPrefixDst = 0;
      
      // Fetch the expando path.
      StringTableEntry expandoPath = getPathExpando(pathBuffer);
      
      // Does the expando exist?
      if( expandoPath == NULL )
      {
         // No, so error.
         Con::errorf("expandPath() : Could not find path expando '%s' for path '%s'.", pathBuffer, pSrcPath );
         
         // Are we ensuring the trailing slash?
         if ( ensureTrailingSlash )
         {
            // Yes, so ensure it.
            Con::ensureTrailingSlash( pDstPath, pSrcPath );
         }
         else
         {
            // No, so just use the source path.
            dStrcpy( pDstPath, pSrcPath );
         }
         
         return false;
      }
      
      // Skip the expando and the following slash.
      pSrc += dStrlen(pathBuffer) + 1;
      
      // Format the output path.
      dSprintf( pathBuffer, sizeof(pathBuffer), "%s/%s", expandoPath, pSrc );
      
      // Are we ensuring the trailing slash?
      if ( ensureTrailingSlash )
      {
         // Yes, so ensure it.
         Con::ensureTrailingSlash( pathBuffer, pathBuffer );
      }
      
      // Strip repeat slashes.
      Con::stripRepeatSlashes( pDstPath, pathBuffer, size );
      
      return true;
   }
   
   // Script-Relative.
   if ( leadingToken == '.' )
   {
      // Fetch the code-block file-path.
      StringTableEntry codeblockFullPath = NULL;
      CodeBlockEvalState *state = CodeBlockEvalState::getCurrent();
      if (state->currentFrame.code)
      {
         codeblockFullPath = state->currentFrame.code->fullPath;
      }
      
      // Do we have a code block full path?
      if( codeblockFullPath == NULL )
      {
         // No, so error.
         Con::errorf("expandPath() : Could not find relative path from code-block for path '%s'.", pSrcPath );
         
         // Are we ensuring the trailing slash?
         if ( ensureTrailingSlash )
         {
            // Yes, so ensure it.
            Con::ensureTrailingSlash( pDstPath, pSrcPath );
         }
         else
         {
            // No, so just use the source path.
            dStrcpy( pDstPath, pSrcPath );
         }
         
         return false;
      }
      
      // Yes, so use it as the prefix.
      dStrncpy(pathBuffer, codeblockFullPath, sizeof(pathBuffer) - 1);
      
      // Find the final slash in the code-block.
      pSlash = dStrrchr(pathBuffer, '/');
      
      // Is this a parent directory token?
      if ( followingToken == '.' )
      {
         // Yes, so terminate after the slash so we include it.
         pSlash[1] = 0;
      }
      else
      {
         // No, it's a current directory token so terminate at the slash so we don't include it.
         pSlash[0] = 0;
         
         // Skip the current directory token.
         pSrc++;
      }
      
      // Format the output path.
      dStrncat(pathBuffer, "/", sizeof(pathBuffer) - 1 - strlen(pathBuffer));
      dStrncat(pathBuffer, pSrc, sizeof(pathBuffer) - 1 - strlen(pathBuffer));
      
      // Are we ensuring the trailing slash?
      if ( ensureTrailingSlash )
      {
         // Yes, so ensure it.
         Con::ensureTrailingSlash( pathBuffer, pathBuffer );
      }
      
      // Strip repeat slashes.
      Con::stripRepeatSlashes( pDstPath, pathBuffer, size );
      
      return true;
   }
   
   // All else.
   
   //Using a special case here because the code below barfs on trying to build a full path for apk reading
#ifdef TORQUE_OS_ANDROID
   if (leadingToken == '/' || strstr(pSrcPath, "/") == NULL)
      Platform::makeFullPathName( pSrcPath, pathBuffer, sizeof(pathBuffer), pWorkingDirectoryHint );
   else
      dSprintf(pathBuffer, sizeof(pathBuffer), "/%s", pSrcPath);
#else
   Platform::makeFullPathName( pSrcPath, pathBuffer, sizeof(pathBuffer), pWorkingDirectoryHint );
#endif
   
   // Are we ensuring the trailing slash?
   if ( ensureTrailingSlash )
   {
      // Yes, so ensure it.
      Con::ensureTrailingSlash( pathBuffer, pathBuffer );
   }
   
   // Strip repeat slashes.
   Con::stripRepeatSlashes( pDstPath, pathBuffer, size );
   
   return true;
}

//-----------------------------------------------------------------------------

bool isBasePath( const char* SrcPath, const char* pBasePath )
{
   char expandBuffer[1024];
   Con::expandPath( expandBuffer, sizeof(expandBuffer), SrcPath );
   return dStrnicmp(pBasePath, expandBuffer, dStrlen( pBasePath ) ) == 0;
}

//-----------------------------------------------------------------------------

void collapsePath( char* pDstPath, U32 size, const char* pSrcPath, const char* pWorkingDirectoryHint )
{
   // Check path against expandos.  If there are multiple matches, choose the
   // expando that produces the shortest relative path.
   
   char pathBuffer[2048];
   
   // Fetch expando count.
   const U32 expandoCount = getPathExpandoCount();
   
   // Iterate expandos.
   U32 expandoRelativePathLength = U32_MAX;
   for( U32 expandoIndex = 0; expandoIndex < expandoCount; ++expandoIndex )
   {
      // Fetch expando value (path).
      StringTableEntry expandoValue = getPathExpandoValue( expandoIndex );
      
      // Skip if not the base path.
      if ( !isBasePath( pSrcPath, expandoValue ) )
         continue;
      
      // Fetch path relative to expando path.
      StringTableEntry relativePath = Platform::makeRelativePathName( pSrcPath, expandoValue );
      
      // If the relative path is simply a period
      if ( relativePath[0] == '.' )
         relativePath++;
      
      if ( dStrlen(relativePath) > expandoRelativePathLength )
      {
         // This expando covers less of the path than any previous one found.
         // We will keep the previous one.
         continue;
      }
      
      // Keep track of the relative path length
      expandoRelativePathLength = dStrlen(relativePath);
      
      // Fetch expando key (name).
      StringTableEntry expandoName = getPathExpandoKey( expandoIndex );
      
      // Format against expando.
      dSprintf( pathBuffer, sizeof(pathBuffer), "^%s/%s", expandoName, relativePath );
   }
   
   // Check if we've found a suitable expando
   if ( expandoRelativePathLength != U32_MAX )
   {
      // Strip repeat slashes.
      Con::stripRepeatSlashes( pDstPath, pathBuffer, size );
      
      return;
   }
   
   // Fetch the working directory.
   StringTableEntry workingDirectory = pWorkingDirectoryHint != NULL ? pWorkingDirectoryHint : Platform::getCurrentDirectory();
   
   // Fetch path relative to current directory.
   StringTableEntry relativePath = Platform::makeRelativePathName( pSrcPath, workingDirectory );
   
   // If the relative path is simply a period
   if ( relativePath[0] == '.'  && relativePath[1] != '.' )
      relativePath++;
   
   // Format against expando.
   dSprintf( pathBuffer, sizeof(pathBuffer), "%s/%s", workingDirectory, relativePath );
   
   // Strip repeat slashes.
   Con::stripRepeatSlashes( pDstPath, pathBuffer, size );
}

//-----------------------------------------------------------------------------

void ensureTrailingSlash( char* pDstPath, const char* pSrcPath )
{
   // Copy to target.
   dStrcpy( pDstPath, pSrcPath );
   
   // Find trailing character index.
   S32 trailIndex = dStrlen(pDstPath);
   
   // Ignore if empty string.
   if ( trailIndex == 0 )
      return;
   
   // Finish if the trailing slash already exists.
   if ( pDstPath[trailIndex-1] == '/' )
      return;
   
   // Add trailing slash.
   pDstPath[trailIndex++] = '/';
   pDstPath[trailIndex] = 0;
}

//-----------------------------------------------------------------------------

bool stripRepeatSlashes( char* pDstPath, const char* pSrcPath, S32 dstSize )
{
   // Note original destination.
   char* pOriginalDst = pDstPath;
   
   // Reset last source character.
   char lastSrcChar = 0;
   
   // Search source...
   while ( dstSize > 0 )
   {
      // Fetch characters.
      const char srcChar = *pSrcPath++;
      
      // Do we have a repeat slash?
      if ( srcChar == '/' && lastSrcChar == '/' )
      {
         // Yes, so skip it.
         continue;
      }
      
      // No, so copy character.
      *pDstPath++ = srcChar;
      
      // Finish if end of source.
      if ( srcChar == 0 )
         return true;
      
      // Reduce room left in destination.
      dstSize--;
      
      // Set last character.
      lastSrcChar = srcChar;
   }
   
   // Terminate the destination string as we ran out of room.
   *pOriginalDst = 0;
   
   // Fail!
   return false;
}

}

/*! @defgroup PathExpandoFunctions Path Expando
   @ingroup TorqueScriptFunctions
   @{
 */

/*! Expands an expando or relative path into a full path.
 */
ConsoleFunctionWithDocs(expandPath, ConsoleString, 2, 2, (string path))
{
   char* ret = Con::getReturnBuffer( 1024 );
   Con::expandPath(ret, 1024, argv[1]);
   return ret;
}

//-----------------------------------------------------------------------------

/*! Collapses a path into either an expando path or a relative path.
 */
ConsoleFunctionWithDocs(collapsePath, ConsoleString, 2, 2, (string path))
{
   char* ret = Con::getReturnBuffer( 1024 );
   Con::collapsePath(ret, 1024, argv[1]);
   return ret;
}

//-----------------------------------------------------------------------------

/*! Adds the expando to the path.  If it already exists then it is replaced.
 */
ConsoleFunctionWithDocs(addPathExpando, ConsoleVoid, 3, 3, (string expando, string path))
{
   Con::addPathExpando(argv[1], argv[2]);
}

//-----------------------------------------------------------------------------

/*! Removes the specified path expando.
 */
ConsoleFunctionWithDocs(removePathExpando, ConsoleVoid, 2, 2, (string expando))
{
   Con::removePathExpando(argv[1]);
}

//-----------------------------------------------------------------------------

/*! Checks whether the specified path expando is current set or not.
 */
ConsoleFunctionWithDocs(isPathExpando, ConsoleBool, 2, 2, (string expando))
{
   return Con::isPathExpando(argv[1]);
}

//-----------------------------------------------------------------------------

/*! Gets the expando path count.
 */
ConsoleFunctionWithDocs(getPathExpandoCount, ConsoleInt, 1, 1, ())
{
   return Con::getPathExpandoCount();
}

//-----------------------------------------------------------------------------

/*! Gets the path expando key (the expando name) at the specified index.
 */
ConsoleFunctionWithDocs(getPathExpandoKey, ConsoleString, 2, 2, (int expandoIndex))
{
   // Fetch expando index.
   const S32 expandoIndex = dAtoi(argv[1]);
   
   // Is the expando index in range?
   if ( expandoIndex < 0 || expandoIndex >= (S32) Con::getPathExpandoCount() )
   {
      // No, so warn.
      Con::warnf("getPathExpandoKey() - Expando index of '%d' is out of bounds.  Current expando count is '%d'.",
                 expandoIndex,
                 Con::getPathExpandoCount() );
      return StringTable->EmptyString;
   }
   
   // Fetch path expando key.
   return Con::getPathExpandoKey( expandoIndex );
}

//-----------------------------------------------------------------------------

/*! Gets the path expando value (the expando path) at the specified index.
 */
ConsoleFunctionWithDocs(getPathExpandoValue, ConsoleString, 2, 2, (int expandoIndex))
{
   // Fetch expando index.
   const S32 expandoIndex = dAtoi(argv[1]);
   
   // Is the expando index in range?
   if ( expandoIndex < 0 || expandoIndex >= (S32) Con::getPathExpandoCount() )
   {
      // No, so warn.
      Con::warnf("getPathExpandoValue() - Expando index of '%d' is out of bounds.  Current expando count is '%d'.",
                 expandoIndex,
                 Con::getPathExpandoCount() );
      return StringTable->EmptyString;
   }
   
   // Fetch path expando value.
   return Con::getPathExpandoValue( expandoIndex );
}

/*! @} */ // group PathExpandoFunctions
