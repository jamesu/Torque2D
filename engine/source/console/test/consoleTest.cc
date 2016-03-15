#include "platform/platform.h"
#include "platform/platformTLS.h"
#include "platform/threads/thread.h"
#include "console/console.h"
#include "console/consoleInternal.h"
#include "console/consoleObject.h"
#include "io/fileStream.h"
#include "io/resource/resourceManager.h"
#include "console/compiler/consoleAST.h"
#include "collection/findIterator.h"
#include "console/consoleTypes.h"
#include "console/consoleSerialization.h"
#include "debug/telnetDebugger.h"
#include "sim/simBase.h"
#include "console/compiler/consoleCompiler.h"
#include "component/dynamicConsoleMethodComponent.h"
#include "memory/safeDelete.h"



#include "io/memStream.h"

void testStackWrite()
{
	Vector<ConsoleValuePtr> stack;
	
	ConsoleValuePtr var1;
	ConsoleValuePtr var2;
	ConsoleValuePtr var3;
	
	ConsoleValuePtr var4;
	ConsoleValuePtr var5;
	ConsoleValuePtr var6;
	ConsoleValuePtr var7;
	ConsoleValuePtr var8;
	
	ConsoleSerializationState state;
	
	U8 buffer[16 * 1024];
	MemStream m(sizeof(buffer), buffer, true, true);
	
	ConsoleBaseType *type = ConsoleBaseType::getTypeByName("TypeS32Vector");
	var1.type = type->getTypeID();
	var1.value.refValue = type->createReferenceCountedValue();
	var1.AddRef();
	m.setPosition(0);
	m.writeLongString(4096, "1 2 3 4 5");
	m.setPosition(0);
	var1.value.refValue->read(m, state);
	
	type = ConsoleBaseType::getTypeByName("TypeString");
	var2.type = type->getTypeID();
	var2.value.refValue = type->createReferenceCountedValue();
	var2.AddRef();
	m.setPosition(0);
	m.writeLongString(4096, "This is a string");
	m.setPosition(0);
	var2.value.refValue->read(m, state);
	
	type = ConsoleBaseType::getTypeByName("TypeBufferString");
	var3.type = type->getTypeID();
	var3.value.refValue = type->createReferenceCountedValue();
	var3.AddRef();
	m.setPosition(0);
	const char *bufferStr = "Buffered string test";
	U32 len = dStrlen(bufferStr)+1;
	m.write(len);
	m.write(len, bufferStr);
	m.setPosition(0);
	var3.value.refValue->read(m, state);
	
	var4.type = ConsoleValue::TypeInternalNull;
	var5.type = ConsoleValue::TypeInternalInt;
	var5.value.ival = 4096;
	var6.type = ConsoleValue::TypeInternalFloat;
	var6.value.fval = 4.096;
	var7.type = ConsoleValue::TypeInternalStringTableEntry;
	var7.value.string = StringTable->insert("STEString");
	
	var8 = var3;
	
	stack.push_back(var1);
	stack.push_back(var2);
	stack.push_back(var3);
	stack.push_back(var4);
	stack.push_back(var5);
	stack.push_back(var6);
	stack.push_back(var7);
	stack.push_back(var8);
	
	ConsoleSerializationState serializationState;
	
	
	m.setPosition(0);
	ConsoleValuePtr::writeStack(m, serializationState, stack);
	stack.clearAndReset();
	serializationState.clear();
	
	m.setPosition(0);
	ConsoleValuePtr::readStack(m, serializationState, stack);
	
	for (U32 i=0; i<stack.size(); i++)
	{
		Con::printf("Stack[%u] type == %u value == %s refCount == %i", i, stack[i].type, stack[i].getTempStringValue(), ConsoleValue::isRefType(stack[i].type) ? stack[i].value.refValue->refCount : 0);
	}
	stack.clearAndReset();
	
	
	
}

ConsoleFunction(testExecute, void, 2, 2, "")
{
	SimObject* frodoObject = argv[1].getSimObject();
	if (frodoObject)
	{
		Con::executef(frodoObject, "testFunc", "string", 1, 2);
	}
}

ConsoleMethod(SimObject, testMethod, ConsoleValuePtr, 2, 2, "")
{
	ConsoleValuePtr ptr;
	ptr.setValue(ConsoleStringValuePtr(object->getName()));
	return ptr;
}

