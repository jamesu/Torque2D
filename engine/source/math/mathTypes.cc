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

#include "console/consoleTypes.h"
#include "console/console.h"
#include "console/consoleValue.h"
#include "math/mPoint.h"
#include "math/mMatrix.h"
#include "math/mRect.h"
#include "math/mBox.h"
#include "math/mathTypes.h"
#include "math/mRandom.h"

#include "vector_ScriptBinding.h"
#include "matrix_ScriptBinding.h"
#include "random_ScriptBinding.h"
#include "box_ScriptBinding.h"

//////////////////////////////////////////////////////////////////////////
// TypePoint2I
//////////////////////////////////////////////////////////////////////////
ConsoleType( Point2I, TypePoint2I, sizeof(Point2I), "" )
ConsoleUseDefaultReferenceType( TypePoint2I, Point2I )

ConsoleTypeToString( TypePoint2I )
{
   Point2I *pt = (Point2I *) dataPtr;
   char* returnBuffer = Con::getReturnBuffer(256);
   dSprintf(returnBuffer, 256, "%d %d", pt->x, pt->y);
   return returnBuffer;
}

ConsoleTypeFromConsoleValue( TypePoint2I )
{
   if (ConsoleValue::isRefType(value.type))
   {
      // We have three possibilities:
      // 1) it's a string
      // 2) it's a list
      // 3) it's some sort of hashtable
      
      ConsoleValuePtr arr;
      arr.type = ConsoleValue::TypeInternalInt;
      arr.value.ival = 0;
      
      ConsoleValuePtr xValue;
      ConsoleValuePtr yValue;
      
      ConsoleReferenceCountedType* refValue = value.value.refValue;
      if (refValue->isEnumerable())
      {
         const StringTableEntry stGetIndex = ConsoleBaseType::getFieldIndexName();
         if (refValue->getDataField(stGetIndex, arr, xValue))
         {
            arr.value.ival = 1;
            refValue->getDataField(stGetIndex, arr, yValue);
         }
         else
         {
            static StringTableEntry xName = StringTable->insert("x");
            static StringTableEntry yName = StringTable->insert("y");
            
            arr.type = ConsoleValue::TypeInternalNull;
            if (refValue->getDataField(xName, arr, xValue))
            {
               refValue->getDataField(yName, arr, yValue);
            }
            else
            {
               Con::errorf("Point2I must be a string, list, or hash with x,y");
            }
         }
         
         *((Point2I *) dataPtr) = Point2I(xValue.getIntValue(), yValue.getIntValue());
         return;
      }
   }
   
   S32 px = 0, py = 0;
   dSscanf(value.getTempStringValue(), "%d %d", &px, &py);
   *((Point2I *) dataPtr) = Point2I(px, py);
}

//////////////////////////////////////////////////////////////////////////
// TypePoint2F
//////////////////////////////////////////////////////////////////////////
ConsoleType( Point2F, TypePoint2F, sizeof(Point2F), "" )
ConsoleUseDefaultReferenceType( TypePoint2F, Point2F )

ConsoleTypeToString( TypePoint2F )
{
   Point2F *pt = (Point2F *) dataPtr;
   char* returnBuffer = Con::getReturnBuffer(256);
   dSprintf(returnBuffer, 256, "%.9g %9g", pt->x, pt->y);
   return returnBuffer;
}

ConsoleTypeFromConsoleValue( TypePoint2F )
{
   if (ConsoleValue::isRefType(value.type))
   {
      // We have three possibilities:
      // 1) it's a string
      // 2) it's a list
      // 3) it's some sort of hashtable
      
      ConsoleValuePtr arr;
      arr.type = ConsoleValue::TypeInternalInt;
      arr.value.ival = 0;
      
      ConsoleValuePtr xValue;
      ConsoleValuePtr yValue;
      
      ConsoleReferenceCountedType* refValue = value.value.refValue;
      if (refValue->isEnumerable())
      {
         const StringTableEntry stGetIndex = ConsoleBaseType::getFieldIndexName();
         if (refValue->getDataField(stGetIndex, arr, xValue))
         {
            arr.value.ival = 1;
            refValue->getDataField(stGetIndex, arr, yValue);
         }
         else
         {
            static StringTableEntry xName = StringTable->insert("x");
            static StringTableEntry yName = StringTable->insert("y");
            
            arr.type = ConsoleValue::TypeInternalNull;
            if (refValue->getDataField(xName, arr, xValue))
            {
               refValue->getDataField(yName, arr, yValue);
            }
            else
            {
               Con::errorf("Point2F must be a string, list, or hash with x,y");
            }
         }
         
         *((Point2F *) dataPtr) = Point2F(xValue.getFloatValue(), yValue.getFloatValue());
         return;
      }
   }
   
   F32 px = 0, py = 0;
   dSscanf(value.getTempStringValue(), "%f %f", &px, &py);
   *((Point2F *) dataPtr) = Point2F(px, py);
}

//////////////////////////////////////////////////////////////////////////
// TypePoint2FVector
//////////////////////////////////////////////////////////////////////////
ConsoleType( point2FList, TypePoint2FVector, sizeof(Vector<Point2F>), "" )
ConsoleUseDefaultReferenceType( TypePoint2FVector, Vector<Point2F> )

ConsoleTypeToString( TypePoint2FVector )
{
   Vector<Point2F> *vec = (Vector<Point2F> *)dataPtr;
   ConsoleValuePtr out;
   
   ConsoleArrayValue* list = new ConsoleArrayValue();
   list->mValues.reserve(vec->size());
   
   for (Vector<Point2F>::iterator itr = vec->begin(); itr != vec->end(); itr++)
   {
      ConsoleValuePtr xValue;
      ConsoleValuePtr yValue;
      
      xValue.type = ConsoleValue::TypeInternalFloat;
      xValue.value.fval = itr->x;
      yValue.type = ConsoleValue::TypeInternalFloat;
      xValue.value.fval = itr->y;
      
      list->mValues.push_back(xValue);
      list->mValues.push_back(yValue);
   }
   
   out.setValue(list);
   return out;
}

ConsoleTypeFromConsoleValue( TypePoint2FVector )
{
   Vector<Point2F> *vec = (Vector<Point2F> *)dataPtr;
   // we assume the vector should be cleared first (not just appending)
   vec->clear();
   
   if (ConsoleValue::isRefType(value.type))
   {
      if (value.value.refValue->isEnumerable())
      {
         S32 length = value.value.refValue->getIteratorLength();
         ConsoleValuePtr iterator;
         iterator.type = ConsoleValue::TypeInternalInt;
         iterator.value.ival = 1;
         
         ConsoleValuePtr outValue;
         for (S32 i=0; i<length; i++)
         {
            if (!value.value.refValue->advanceIterator(iterator, outValue))
               break;
            
            Point2F pointVal(0,0);
            Con::setData(TypePoint2F, &pointVal, 0, outValue);
            vec->push_back(pointVal);
         }
         return;
      }
   }
   
   // Fallback: use string
   const char* arg = value.getTempStringValue();
   const char *values = arg;
   const char *endValues = values + dStrlen(values);
   Point2F pointValue(0,0);
   // advance through the string, pulling off S32's and advancing the pointer
   while (values < endValues && dSscanf(values, "%g %g", &pointValue.x, &pointValue.y) != 0)
   {
      vec->push_back(pointValue);
      pointValue = Point2F(0,0);
      const char *nextSeperator = dStrchr(values, ' ');
      if( !nextSeperator )
         break;
      const char *nextValues = dStrchr(nextSeperator + 1, ' ');
      if (nextValues != 0 && nextValues < endValues)
         values = nextValues + 1;
      else
         break;
   }
}

//////////////////////////////////////////////////////////////////////////
// TypePoint3F
//////////////////////////////////////////////////////////////////////////
ConsoleType( Point3F, TypePoint3F, sizeof(Point3F), "" )
ConsoleUseDefaultReferenceType( TypePoint3F, Point3F )

ConsoleTypeToString( TypePoint3F )
{
   Point3F *pt = (Point3F *) dataPtr;
   char* returnBuffer = Con::getReturnBuffer(256);
   dSprintf(returnBuffer, 256, "%.9g %.9g %.9g", pt->x, pt->y);
   return returnBuffer;
}

ConsoleTypeFromConsoleValue( TypePoint3F )
{
   if (ConsoleValue::isRefType(value.type))
   {
      // We have three possibilities:
      // 1) it's a string
      // 2) it's a list
      // 3) it's some sort of hashtable
      
      ConsoleValuePtr arr;
      arr.type = ConsoleValue::TypeInternalInt;
      arr.value.ival = 0;
      
      ConsoleValuePtr xValue;
      ConsoleValuePtr yValue;
      ConsoleValuePtr zValue;
      
      ConsoleReferenceCountedType* refValue = value.value.refValue;
      if (refValue->isEnumerable())
      {
         const StringTableEntry stGetIndex = ConsoleBaseType::getFieldIndexName();
         if (refValue->getDataField(stGetIndex, arr, xValue))
         {
            arr.value.ival = 1;
            refValue->getDataField(stGetIndex, arr, yValue);
            arr.value.ival = 2;
            refValue->getDataField(stGetIndex, arr, zValue);
         }
         else
         {
            static StringTableEntry xName = StringTable->insert("x");
            static StringTableEntry yName = StringTable->insert("y");
            static StringTableEntry zName = StringTable->insert("z");
            
            arr.type = ConsoleValue::TypeInternalNull;
            if (refValue->getDataField(xName, arr, xValue))
            {
               refValue->getDataField(yName, arr, yValue);
               refValue->getDataField(zName, arr, zValue);
            }
            else
            {
               Con::errorf("Point3F must be a string, list, or hash with x,y");
            }
         }
         
         *((Point3F *) dataPtr) = Point3F(xValue.getFloatValue(), yValue.getFloatValue(), zValue.getFloatValue());
         return;
      }
   }
   
   F32 px = 0, py = 0, pz = 0;
   dSscanf(value.getTempStringValue(), "%f %f %f", &px, &py, &pz);
   *((Point3F *) dataPtr) = Point3F(px, py, pz);
}

//////////////////////////////////////////////////////////////////////////
// TypePoint4F
//////////////////////////////////////////////////////////////////////////
ConsoleType( Point4F, TypePoint4F, sizeof(Point4F), "" )
ConsoleUseDefaultReferenceType( TypePoint4F, Point4F )

ConsoleTypeToString( TypePoint4F )
{
   Point4F *pt = (Point4F *) dataPtr;
   char* returnBuffer = Con::getReturnBuffer(256);
   dSprintf(returnBuffer, 256, "%.9g %.9g %.9g %.9g", pt->x, pt->y, pt->z, pt->w);
   return returnBuffer;
}

ConsoleTypeFromConsoleValue( TypePoint4F )
{
   if (ConsoleValue::isRefType(value.type))
   {
      // We have three possibilities:
      // 1) it's a string
      // 2) it's a list
      // 3) it's some sort of hashtable
      
      ConsoleValuePtr arr;
      arr.type = ConsoleValue::TypeInternalInt;
      arr.value.ival = 0;
      
      ConsoleValuePtr xValue;
      ConsoleValuePtr yValue;
      ConsoleValuePtr zValue;
      ConsoleValuePtr wValue;
      
      ConsoleReferenceCountedType* refValue = value.value.refValue;
      if (refValue->isEnumerable())
      {
         const StringTableEntry stGetIndex = ConsoleBaseType::getFieldIndexName();
         
         if (refValue->getDataField(stGetIndex, arr, xValue))
         {
            arr.value.ival = 1;
            refValue->getDataField(stGetIndex, arr, yValue);
            arr.value.ival = 2;
            refValue->getDataField(stGetIndex, arr, zValue);
            arr.value.ival = 3;
            refValue->getDataField(stGetIndex, arr, wValue);
         }
         else
         {
            static StringTableEntry xName = StringTable->insert("x");
            static StringTableEntry yName = StringTable->insert("y");
            static StringTableEntry zName = StringTable->insert("z");
            static StringTableEntry wName = StringTable->insert("w");
            
            arr.type = ConsoleValue::TypeInternalNull;
            if (refValue->getDataField(xName, arr, xValue))
            {
               refValue->getDataField(yName, arr, yValue);
               refValue->getDataField(zName, arr, zValue);
               refValue->getDataField(wName, arr, wValue);
            }
            else
            {
               Con::errorf("Point3F must be a string, list, or hash with x,y");
            }
         }
         
         *((Point4F *) dataPtr) = Point4F(xValue.getIntValue(), yValue.getIntValue(), zValue.getIntValue(), wValue.getIntValue());
         return;
      }
   }
   
   F32 px = 0, py = 0, pz = 0, pw = 0;
   dSscanf(value.getTempStringValue(), "%f %f %f %f", &px, &py, &pz, &pw);
   *((Point4F *) dataPtr) = Point4F(px, py, pz, pw);
}

//////////////////////////////////////////////////////////////////////////
// TypeRectI
//////////////////////////////////////////////////////////////////////////
ConsoleType( RectI, TypeRectI, sizeof(RectI), "" )
ConsoleUseDefaultReferenceType( TypeRectI, RectI )

ConsoleTypeToString( TypeRectI )
{
   RectF *rect = (RectF *) dataPtr;
   char* returnBuffer = Con::getReturnBuffer(256);
   dSprintf(returnBuffer, 256, "%d %d %d %d", rect->point.x, rect->point.y, rect->extent.x, rect->extent.y);
   return returnBuffer;
}

ConsoleTypeFromConsoleValue( TypeRectI )
{
   if (ConsoleValue::isRefType(value.type))
   {
      // We have three possibilities:
      // 1) it's a string
      // 2) it's a list
      // 3) it's some sort of hashtable
      
      ConsoleValuePtr arr;
      arr.type = ConsoleValue::TypeInternalInt;
      arr.value.ival = 0;
      
      ConsoleValuePtr xValue;
      ConsoleValuePtr yValue;
      ConsoleValuePtr wValue;
      ConsoleValuePtr hValue;
      
      ConsoleReferenceCountedType* refValue = value.value.refValue;
      if (refValue->isEnumerable())
      {
         const StringTableEntry stGetIndex = ConsoleBaseType::getFieldIndexName();
         if (refValue->getDataField(stGetIndex, arr, xValue))
         {
            arr.value.ival = 1;
            refValue->getDataField(stGetIndex, arr, yValue);
            arr.value.ival = 2;
            refValue->getDataField(stGetIndex, arr, wValue);
            arr.value.ival = 3;
            refValue->getDataField(stGetIndex, arr, hValue);
         }
         else
         {
            static StringTableEntry xName = StringTable->insert("x");
            static StringTableEntry yName = StringTable->insert("y");
            static StringTableEntry wName = StringTable->insert("w");
            static StringTableEntry hName = StringTable->insert("h");
            
            arr.type = ConsoleValue::TypeInternalNull;
            if (refValue->getDataField(xName, arr, xValue))
            {
               refValue->getDataField(yName, arr, yValue);
               refValue->getDataField(wName, arr, wValue);
               refValue->getDataField(hName, arr, hValue);
            }
            else
            {
               Con::errorf("RectI must be a string, list, or hash with x,y,w,h");
            }
         }
         
         *((RectI *) dataPtr) = RectI(Point2I(xValue.getIntValue(), yValue.getIntValue()), Point2I(wValue.getIntValue(), hValue.getIntValue()));
         return;
      }
   }
   
   S32 px = 0, py = 0, pw = 0, ph = 0;
   dSscanf(value.getTempStringValue(), "%d %d %d %d", &px, &py, &pw, &ph);
   *((RectI *) dataPtr) = RectI(Point2I(px, py), Point2I(pw, ph));
}

//////////////////////////////////////////////////////////////////////////
// TypeRectF
//////////////////////////////////////////////////////////////////////////
ConsoleType( RectF, TypeRectF, sizeof(RectF), "" )
ConsoleUseDefaultReferenceType( TypeRectF, RectF )

ConsoleTypeToString( TypeRectF )
{
   RectF *rect = (RectF *) dataPtr;
   char* returnBuffer = Con::getReturnBuffer(256);
   dSprintf(returnBuffer, 256, "%.9g %.9g %.9g %.9g", rect->point.x, rect->point.y, rect->extent.x, rect->extent.y);
   return returnBuffer;
}

ConsoleTypeFromConsoleValue( TypeRectF )
{
   if (ConsoleValue::isRefType(value.type))
   {
      // We have three possibilities:
      // 1) it's a string
      // 2) it's a list
      // 3) it's some sort of hashtable
      
      ConsoleValuePtr arr;
      arr.type = ConsoleValue::TypeInternalInt;
      arr.value.ival = 0;
      
      ConsoleValuePtr xValue;
      ConsoleValuePtr yValue;
      ConsoleValuePtr wValue;
      ConsoleValuePtr hValue;
      
      ConsoleReferenceCountedType* refValue = value.value.refValue;
      if (refValue->isEnumerable())
      {
         const StringTableEntry stGetIndex = ConsoleBaseType::getFieldIndexName();
         if (refValue->getDataField(stGetIndex, arr, xValue))
         {
            arr.value.ival = 1;
            refValue->getDataField(stGetIndex, arr, yValue);
            arr.value.ival = 2;
            refValue->getDataField(stGetIndex, arr, wValue);
            arr.value.ival = 3;
            refValue->getDataField(stGetIndex, arr, hValue);
         }
         else
         {
            static StringTableEntry xName = StringTable->insert("x");
            static StringTableEntry yName = StringTable->insert("y");
            static StringTableEntry wName = StringTable->insert("w");
            static StringTableEntry hName = StringTable->insert("h");
            
            arr.type = ConsoleValue::TypeInternalNull;
            if (refValue->getDataField(xName, arr, xValue))
            {
               refValue->getDataField(yName, arr, yValue);
               refValue->getDataField(wName, arr, wValue);
               refValue->getDataField(hName, arr, hValue);
            }
            else
            {
               Con::errorf("RectI must be a string, list, or hash with x,y,w,h");
            }
         }
         
         *((RectF *) dataPtr) = RectF(Point2F(xValue.getFloatValue(), yValue.getFloatValue()), Point2F(wValue.getFloatValue(), hValue.getFloatValue()));
         return;
      }
   }
   
   F32 px = 0, py = 0, pw = 0, ph = 0;
   dSscanf(value.getTempStringValue(), "%f %f %f %f", &px, &py, &pw, &ph);
   *((RectF *) dataPtr) = RectF(Point2F(px, py), Point2F(pw, ph));
}

//////////////////////////////////////////////////////////////////////////
// TypeMatrixPosition
//////////////////////////////////////////////////////////////////////////
ConsoleType( MatrixPosition, TypeMatrixPosition, sizeof(4*sizeof(F32)), "" )
ConsoleUseDefaultReferenceType( TypeMatrixPosition, MatrixF )

ConsoleTypeToString( TypeMatrixPosition )
{
   F32 *col = (F32 *) dataPtr + 3;
   
   if (col[12] == 1.f)
   {
      char* returnBuffer = Con::getReturnBuffer(256);
      dSprintf(returnBuffer, 256, "%.9g %.9g %.9g", col[0], col[4], col[8]);
      return returnBuffer;
   }
   else
   {
      char* returnBuffer = Con::getReturnBuffer(256);
      dSprintf(returnBuffer, 256, "%.9g %.9g %.9g %.9g", col[0], col[4], col[8], col[12]);
      return returnBuffer;
   }
}

ConsoleTypeFromConsoleValue( TypeMatrixPosition )
{
   F32 *col = ((F32 *) dataPtr) + 3;
   
   
   if (ConsoleValue::isRefType(value.type))
   {
      // We have three possibilities:
      // 1) it's a string
      // 2) it's a list
      // 3) it's some sort of hashtable
      
      ConsoleValuePtr arr;
      arr.type = ConsoleValue::TypeInternalInt;
      arr.value.ival = 0;
      
      ConsoleValuePtr value1;
      ConsoleValuePtr value2;
      ConsoleValuePtr value3;
      ConsoleValuePtr value4;
      
      ConsoleReferenceCountedType* refValue = value.value.refValue;
      if (refValue->isEnumerable())
      {
         const StringTableEntry stGetIndex = ConsoleBaseType::getFieldIndexName();
         if (refValue->getDataField(stGetIndex, arr, value1))
         {
            arr.value.ival = 1;
            refValue->getDataField(stGetIndex, arr, value2);
            arr.value.ival = 2;
            refValue->getDataField(stGetIndex, arr, value2);
            arr.value.ival = 3;
            refValue->getDataField(stGetIndex, arr, value3);
         }
         else
         {
            Con::errorf("MatrixPosition must be a string or a list");
         }
         
         col[0] = value1.getFloatValue();
         col[4] = value2.getFloatValue();
         col[8] = value3.getFloatValue();
         col[12] = value4.getFloatValue();
         return;
      }
   }
   
   col[0] = col[4] = col[8] = 0.f;
   col[12] = 1.f;
   dSscanf(value.getTempStringValue(), "%g %g %g %g", &col[0], &col[4], &col[8], &col[12]);
}

//////////////////////////////////////////////////////////////////////////
// TypeMatrixRotation
//////////////////////////////////////////////////////////////////////////
ConsoleType( MatrixRotation, TypeMatrixRotation, sizeof(MatrixF), "" )
ConsoleUseDefaultReferenceType( TypeMatrixRotation, MatrixF )

ConsoleTypeToString( TypeMatrixRotation )
{
   AngAxisF aa(*(MatrixF *) dataPtr);
   aa.axis.normalize();
   
   char* returnBuffer = Con::getReturnBuffer(256);
   dSprintf(returnBuffer, 256, "%.9g %.9g %.9g %.9g", aa.axis.x, aa.axis.y, aa.axis.z, mRadToDeg(aa.angle));
   return returnBuffer;
}

ConsoleTypeFromConsoleValue( TypeMatrixRotation )
{
   AngAxisF aa(Point3F(0,0,0),0);
   
   if (ConsoleValue::isRefType(value.type))
   {
      // We have three possibilities:
      // 1) it's a string
      // 2) it's a list
      // 3) it's some sort of hashtable
      
      ConsoleValuePtr arr;
      arr.type = ConsoleValue::TypeInternalInt;
      arr.value.ival = 0;
      
      ConsoleValuePtr value1;
      ConsoleValuePtr value2;
      ConsoleValuePtr value3;
      ConsoleValuePtr value4;
      
      ConsoleReferenceCountedType* refValue = value.value.refValue;
      if (refValue->isEnumerable())
      {
         const StringTableEntry stGetIndex = ConsoleBaseType::getFieldIndexName();
         if (refValue->getDataField(stGetIndex, arr, value1))
         {
            arr.value.ival = 1;
            refValue->getDataField(stGetIndex, arr, value2);
            arr.value.ival = 2;
            refValue->getDataField(stGetIndex, arr, value2);
            arr.value.ival = 3;
            refValue->getDataField(stGetIndex, arr, value3);
         }
         else
         {
            Con::errorf("Matrix rotation must be set as { x, y, z, angle } or \"x y z angle\"");
         }
         
         aa = AngAxisF(Point3F(value1.getFloatValue(),
                               value2.getFloatValue(),
                               value3.getFloatValue()),
                       mDegToRad(value4.getFloatValue()));
         
         
         //
         MatrixF temp;
         aa.setMatrix(&temp);
         
         F32* pDst = *(MatrixF *)dataPtr;
         const F32* pSrc = temp;
         for (U32 i = 0; i < 3; i++)
            for (U32 j = 0; j < 3; j++)
               pDst[i*4 + j] = pSrc[i*4 + j];
         return;
      }
   }
   
   dSscanf(value.getTempStringValue(), "%g %g %g %g", &aa.axis.x, &aa.axis.y, &aa.axis.z, &aa.angle);
   aa.angle = mDegToRad(aa.angle);
   
   //
   MatrixF temp;
   aa.setMatrix(&temp);
   
   F32* pDst = *(MatrixF *)dataPtr;
   const F32* pSrc = temp;
   for (U32 i = 0; i < 3; i++)
      for (U32 j = 0; j < 3; j++)
         pDst[i*4 + j] = pSrc[i*4 + j];
}



//////////////////////////////////////////////////////////////////////////
// TypeBox3F
//////////////////////////////////////////////////////////////////////////
ConsoleType( Box3F, TypeBox3F, sizeof(Box3F), "" )
ConsoleUseDefaultReferenceType( TypeBox3F, Box3F )

ConsoleTypeToString( TypeBox3F )
{
   Box3F *pBox = (Box3F *) dataPtr;
   
   char* returnBuffer = Con::getReturnBuffer(256);
   dSprintf(returnBuffer, 256, "%.9g %.9g %.9g %.9g %.9g %.9g",
            pBox->mMin.x, pBox->mMin.y, pBox->mMin.z,
            pBox->mMax.x, pBox->mMax.y, pBox->mMax.z);
   return returnBuffer;
}

ConsoleTypeFromConsoleValue( TypeBox3F )
{
   if (ConsoleValue::isRefType(value.type))
   {
      // We have three possibilities:
      // 1) it's a string
      // 2) it's a list
      // 3) it's some sort of hashtable
      
      ConsoleValuePtr arr;
      arr.type = ConsoleValue::TypeInternalInt;
      arr.value.ival = 0;
      
      ConsoleValuePtr minxValue;
      ConsoleValuePtr minyValue;
      ConsoleValuePtr minzValue;
      ConsoleValuePtr maxxValue;
      ConsoleValuePtr maxyValue;
      ConsoleValuePtr maxzValue;
      
      ConsoleReferenceCountedType* refValue = value.value.refValue;
      if (refValue->isEnumerable())
      {
         const StringTableEntry stGetIndex = ConsoleBaseType::getFieldIndexName();
         if (refValue->getDataField(stGetIndex, arr, minxValue))
         {
            arr.value.ival = 1;
            refValue->getDataField(stGetIndex, arr, minyValue);
            arr.value.ival = 2;
            refValue->getDataField(stGetIndex, arr, minzValue);
            arr.value.ival = 3;
            refValue->getDataField(stGetIndex, arr, maxxValue);
            arr.value.ival = 4;
            refValue->getDataField(stGetIndex, arr, maxyValue);
            arr.value.ival = 5;
            refValue->getDataField(stGetIndex, arr, maxzValue);
         }
         else
         {
            static StringTableEntry xminName = StringTable->insert("xmin");
            static StringTableEntry yminName = StringTable->insert("ymin");
            static StringTableEntry zminName = StringTable->insert("zmin");
            static StringTableEntry xmaxName = StringTable->insert("xmax");
            static StringTableEntry ymaxName = StringTable->insert("ymax");
            static StringTableEntry zmaxName = StringTable->insert("zmax");
            
            arr.type = ConsoleValue::TypeInternalNull;
            if (refValue->getDataField(xminName, arr, minxValue))
            {
               refValue->getDataField(yminName, arr, minyValue);
               refValue->getDataField(zminName, arr, minzValue);
               refValue->getDataField(xmaxName, arr, maxxValue);
               refValue->getDataField(ymaxName, arr, maxyValue);
               refValue->getDataField(zmaxName, arr, maxzValue);
            }
            else
            {
               Con::errorf("Box3F must be a string, list, or hash with xmin,ymin,xmax,ymax");
            }
         }
         
         *((Box3F *) dataPtr) = Box3F(Point3F(minxValue.getFloatValue(), minyValue.getFloatValue(), minzValue.getFloatValue()), Point3F(maxxValue.getFloatValue(), maxyValue.getFloatValue(), maxzValue.getFloatValue()));
         return;
      }
   }
   
   F32 minx = 0, miny = 0, minz = 0, maxx = 0, maxy = 0, maxz = 0;
   dSscanf(value.getTempStringValue(), "%f %f %f %f %f %f", &minx, &miny, &minz, &maxx, &maxy, &maxz);
   *((Box3F *) dataPtr) = Box3F(Point3F(minx, miny, minz), Point3F(maxx, maxy, maxz));
}

//------------------------------------------------------------------------------

