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

#ifndef _ASSET_FIELD_TYPES_H_
#include "assetFieldTypes.h"
#endif

#ifndef _ASSET_PTR_H_
#include "assetPtr.h"
#endif

#ifndef _ASSET_BASE_H_
#include "assetBase.h"
#endif

#ifndef _IMAGE_ASSET_H_
#include "2d/assets/ImageAsset.h"
#endif

#ifndef _ANIMATION_ASSET_H_
#include "2d/assets/AnimationAsset.h"
#endif

#ifndef _AUDIO_ASSET_H_
#include "audio/AudioAsset.h"
#endif

#ifndef _STRINGUNIT_H_
#include "string/stringUnit.h"
#endif

//-----------------------------------------------------------------------------

StringTableEntry assetLooseIdSignature = StringTable->insert( ASSET_ID_SIGNATURE );
StringTableEntry assetLooseFileSignature = StringTable->insert( ASSET_LOOSEFILE_SIGNATURE );

//-----------------------------------------------------------------------------

ConsoleType( assetLooseFilePath, TypeAssetLooseFilePath, sizeof(StringTableEntry), ASSET_LOOSE_FILE_FIELD_PREFIX )
ConsoleUseDefaultReferenceType( TypeAssetLooseFilePath, StringTableEntry )

ConsoleType( assetIdString, TypeAssetId, sizeof(StringTableEntry), ASSET_ID_FIELD_PREFIX )
ConsoleUseDefaultReferenceType( TypeAssetId, StringTableEntry )


//-----------------------------------------------------------------------------

ConsoleTypeToString( TypeAssetLooseFilePath )
{
    // Fetch asset loose file-path.
    return *((StringTableEntry*)dataPtr);
}

//-----------------------------------------------------------------------------

ConsoleTypeFromConsoleValue( TypeAssetLooseFilePath )
{
	// Check we have the right sort of value here
	if (ConsoleValue::isRefType(value.type))
	{
		if (value.value.refValue->isEnumerable())
		{
			Con::warnf( "(TypeAssetLooseFilePath) - Cannot set multiple args to a single asset loose-file." );
			return;
		}
	}
	
	// Was a single argument specified?
	// Yes, so fetch field value.
	const char* pFieldValue = value.getTempStringValue();

	// Fetch asset loose file-path.
	StringTableEntry* assetLooseFilePath = (StringTableEntry*)(dataPtr);

	// Update asset loose file-path value.
	*assetLooseFilePath = StringTable->insert(pFieldValue);
}

//-----------------------------------------------------------------------------

ConsoleTypeToString( TypeAssetId )
{
    // Fetch asset Id.
    return *((StringTableEntry*)dataPtr);
}

//-----------------------------------------------------------------------------

ConsoleTypeFromConsoleValue( TypeAssetId )
{
	// Check we have the right sort of value here
	if (ConsoleValue::isRefType(value.type))
	{
		if (value.value.refValue->isEnumerable())
		{
			Con::warnf( "(TypeAssetId) - Cannot set multiple args to a single asset." );
			return;
		}
	}
	
	// Was a single argument specified?
	// Yes, so fetch field value.
	const char* pFieldValue = value.getTempStringValue();
	
	// Fetch asset Id.
	StringTableEntry* assetId = (StringTableEntry*)(dataPtr);
	
	// Update asset value.
	*assetId = StringTable->insert(pFieldValue);
	
	
	// Warn.
	Con::warnf( "(TypeAssetId) - Cannot set multiple args to a single asset." );
}

