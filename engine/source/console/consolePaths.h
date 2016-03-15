#ifndef _CONSOLEPATHS_H_
#define _CONSOLEPATHS_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif
#ifndef _CONSOLEVALUE_H_
#include "console/consoleValue.h"
#endif
#ifndef _CONSOLE_H_
#include "console/console.h"
#endif

namespace Con
{
/// Convert from a relative path to an absolute path.
///
/// This is used in (among other places) the exec() script function, which
/// takes a parameter indicating a script file and executes it. Script paths
/// can be one of:
///      - <b>Absolute:</b> <i>fps/foo/bar.cs</i> Paths of this sort are passed
///        through.
///      - <b>Mod-relative:</b> <i>~/foo/bar.cs</i> Paths of this sort have their
///        replaced with the name of the current mod.
///      - <b>File-relative:</b> <i>./baz/blip.cs</i> Paths of this sort are
///        calculated relative to the path of the current scripting file.
///      - <b>Expando:</b> <i>^Project/image/happy.png</I> Paths of this sort are
///        relative to the path defined by the expando, in this case the "Project"
///        expando.
///
/// @param  pDstPath    Pointer to string buffer to fill with absolute path.
/// @param  size        Size of buffer pointed to by pDstPath.
/// @param  pSrcPath    Original, possibly relative path.
bool expandPath( char* pDstPath, U32 size, const char *pSrcPath, const char *pWorkingDirectoryHint = NULL, const bool ensureTrailingSlash = false );
void collapsePath( char* pDstPath, U32 size, const char *pSrcPath, const char *pWorkingDirectoryHint = NULL );
bool isBasePath( const char *SrcPath, const char *pBasePath );
void ensureTrailingSlash( char* pDstPath, const char *pSrcPath );
bool stripRepeatSlashes( char* pDstPath, const char *pSrcPath, S32 dstSize );

void addPathExpando( const char *pExpandoName, const char *pPath );
void removePathExpando( const char *pExpandoName );
bool isPathExpando( const char *pExpandoName );
StringTableEntry getPathExpando( const char *pExpandoName );
U32 getPathExpandoCount( void );
StringTableEntry getPathExpandoKey( U32 expandoIndex );
StringTableEntry getPathExpandoValue( U32 expandoIndex );

StringTableEntry getModNameFromPath(const char *path);
}

#endif
