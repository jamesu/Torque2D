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

#ifndef _MATHTYPES_H_
#define _MATHTYPES_H_

#ifndef _CONSOLE_BASE_TYPE_H_
#include "console/consoleBaseType.h"
#endif

#ifndef _MRECT_H_
#include "math/mRect.h"
#endif

#ifndef _MBOX_H_
#include "math/mBox.h"
#endif

void RegisterMathFunctions(void);

// Define Math Console Types
DefineNativeConsoleType( TypePoint2I, Point2I )
DefineNativeConsoleType( TypePoint2F, Point2F )
DefineNativeConsoleType( TypePoint3F, Point3F )
DefineNativeConsoleType( TypePoint4F, Point4F )
DefineNativeConsoleType( TypePoint2FVector, Vector<Point2F> )
DefineNativeConsoleType( TypeRectI, RectI )
DefineNativeConsoleType( TypeRectF, RectF )
DefineConsoleType( TypeMatrixPosition )
DefineConsoleType( TypeMatrixRotation )
DefineNativeConsoleType( TypeBox3F, Box3F )

#endif
