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

#include "2d/core/Vector2.h"

// Script bindings.
#include "Vector2_ScriptBinding.h"

//-----------------------------------------------------------------------------

ConsoleType( Vector2, TypeVector2, sizeof(Vector2), "" )
ConsoleUseDefaultReferenceType( TypeVector2, Vector2 )

ConsoleTypeToString( TypeVector2 )
{
    return ((Vector2*)dataPtr)->scriptThis();
}

ConsoleTypeFromConsoleValue( TypeVector2 )
{
   // Fetch vector.
   Vector2* pVector = (Vector2*)dataPtr;
   
   if (ConsoleValue::isRefType(value.type))
   {
      // We have three possibilities:
      // 1) it's a string
      // 2) it's a list
      // 3) it's some sort of hashtable
      
      ConsoleValue arr;
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
               Con::errorf("Vector2 must be set as { x, y } or \"x y\"");
            }
         }
         
         pVector->Set(xValue.getFloatValue(), yValue.getFloatValue());
         return;
      }
   }
   
   pVector->setString(value.getTempStringValue());
}
