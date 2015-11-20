#include "platform/platform.h"
#include "console/console.h"
#include "console/consoleSerialization.h"

#include "console/consoleDictionary.h"
#include "console/consoleNamespace.h"

#include "console/ast.h"
#include "collection/findIterator.h"
#include "io/resource/resourceManager.h"

#include "string/findMatch.h"
#include "io/fileStream.h"
#include "console/compiler.h"
#include "string/stringStack.h"

#include "sim/simBase.h"

static char tempBuffer[256];
static char tempBuffer2[256];
static bool bValue = false;

const char* ConsoleValue::getTempStringValue() const
{
   char* destBuffer = bValue ? tempBuffer : tempBuffer2;
   bValue = !bValue;
   
    destBuffer[0] = '\0';
    getInternalStringValue(destBuffer, 256);
    return destBuffer;
}

StringTableEntry ConsoleValue::getSTEStringValue() const
{
    switch (type)
    {
        case TypeInternalNull:
            return NULL;
        case TypeInternalStringTableEntry:
        case TypeInternalNamespaceName:
            return value.string;
          break;
        default:
            return StringTable->insert(getTempStringValue());
            break;
    }
}

void ConsoleValue::getInternalStringValue(char* outBuffer, U32 bufferSize) const
{
    switch (type)
    {
        case TypeInternalNull:
            *outBuffer = '\0';
          break;
        case TypeInternalInt:
            dSprintf(outBuffer, bufferSize, "%u", value.ival);
            break;
        case TypeInternalFloat:
            dSprintf(outBuffer, bufferSize, "%.5g", value.fval);
            break;
        case TypeInternalStringTableEntry:
        case TypeInternalNamespaceName:
            dSprintf(outBuffer, bufferSize, "%s", value.string);
            break;
       case TypeInternalNamespaceEntry:
          dSprintf(outBuffer, bufferSize, "%s", static_cast<Namespace::Entry*>(value.ptrValue)->mFunctionName);
          break;
        default:
            dSprintf(outBuffer, bufferSize, "%s", value.refValue->getString().c_str());
            break;
    }
}

ConsoleStringValuePtr ConsoleValuePtr::getStringValue()  const
{
    switch (type)
    {
        case TypeInternalNull:
            return "";
        case TypeInternalInt:
       {
          ConsoleStringValuePtr newStr = ConsoleStringValue::fromInt(value.ival);
          return newStr;
       }
        case TypeInternalFloat:
       {
          ConsoleStringValuePtr newStr = ConsoleStringValue::fromFloat(value.fval);
          return newStr;
       }
        case TypeInternalStringTableEntry:
        case TypeInternalNamespaceName:
            return value.string;
        default:
            return value.refValue->getString();
    }
}

