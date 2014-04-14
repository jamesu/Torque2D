//
// Copyright (c) 2014 James S Urquhart. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//

#include "ScriptEngine.h"

ScriptStackValueRef ScriptStackValueRef::valueAtIndex(int index)
{
    ScriptStackValueRef newValue;
    newValue.type = ScriptEngine::getInstance()->getStack()->getTypeAtIndex(index);
    newValue.index = ScriptEngine::getInstance()->getStack()->pushIndex(index);
    return newValue;
}

ScriptStack::ScriptStack()
{
    
}

ScriptStack::~ScriptStack()
{
    
}

int ScriptStack::pushNull()
{
    
}

int ScriptStack::pushIndex(int value)
{
    
}

int ScriptStack::pushInteger(U32 value)
{
    
}

int ScriptStack::pushSignedInteger(S32 value)
{
    
}

int ScriptStack::pushNumber(F32 value)
{
    
}

int ScriptStack::pushSimFields(SimFieldDictionary *values)
{
    
}

int ScriptStack::pushString(const char *value)
{
    
}

int ScriptStack::pushSimObject(SimObject *value)
{
    
}

int ScriptStack::pushBool(bool value)
{
    
}


// Get values
const char* ScriptStack::getString(int index)
{
    
}

U32 ScriptStack::getInteger(int index)
{
    
}

S32 ScriptStack::getSignedInteger(int index)
{
    
}

F32 ScriptStack::getNumber(int index)
{
    
}

bool ScriptStack::getBool(int index)
{
    
}

void ScriptStack::getSimFields(int index, SimFieldDictionary *outValues)
{
    
}


ScriptValueBaseType ScriptStack::getTypeAtIndex(int index)
{
    
}


void ScriptStack::setString(int index, const char *value)
{
    
}

void ScriptStack::setInteger(int index, U32 value)
{
    
}

void ScriptStack::setSignedInteger(int index, S32 value)
{
    
}

void ScriptStack::setNumber(int index, F32 value)
{
    
}

void ScriptStack::setSimFields(int index, SimFieldDictionary *values)
{
    
}

void ScriptStack::setSimObject(int index, SimObject *value)
{
}

void ScriptStack::setBool(int index, bool value)
{
    
}

ScriptEngine *sScriptInstance = NULL;

ScriptEngine::ScriptEngine()
{
    sScriptInstance = this;
}

ScriptEngine::~ScriptEngine()
{
    
}

ScriptEngine *ScriptEngine::getInstance()
{
    return sScriptInstance;
}

void ScriptEngine::registerClass(AbstractClassRep *rep)
{
}

void ScriptEngine::registerNamespace(Namespace *ns)
{
    
}

void ScriptEngine::registerObject(SimObject *object)
{
}

void ScriptEngine::removeObject(SimObject *object)
{
}

ScriptStack *ScriptEngine::getStack()
{
    return NULL;
}

ScriptStackValueRef ScriptEngine::execute(S32 argc, ScriptStackValueRef argv[])
{
    return NULL;
}

ScriptStackValueRef ScriptEngine::executeOnObject(ScriptStackValueRef obj, S32 argc, ScriptStackValueRef argv[])
{
    return NULL;
}

ScriptStackValueRef ScriptEngine::evaluate(const char* string, bool echo, const char *fileName)
{
    
}

ScriptStackValueRef ScriptEngine::evaluatef(const char* string, ...)
{
    
}



// Note: operators replace value
ScriptStackValueRef& ScriptStackValueRef::operator=(const ScriptStackValueRef &other)
{
    index = other.index;
    type = other.type;
    return *this;
}

ScriptStackValueRef& ScriptStackValueRef::operator=(bool newValue)
{
    if (index == -1)
        index = ScriptEngine::getInstance()->getStack()->pushBool(newValue);
    else
        ScriptEngine::getInstance()->getStack()->setBool(index, newValue);
    return *this;
}

ScriptStackValueRef& ScriptStackValueRef::operator=(const char *newValue)
{
    if (index == -1)
        index = ScriptEngine::getInstance()->getStack()->pushString(newValue);
    else
        ScriptEngine::getInstance()->getStack()->setString(index, newValue);
    return *this;
}

ScriptStackValueRef& ScriptStackValueRef::operator=(U32 newValue)
{
    if (index == -1)
        index = ScriptEngine::getInstance()->getStack()->pushInteger(newValue);
    else
        ScriptEngine::getInstance()->getStack()->setInteger(index, newValue);
    return *this;
}

ScriptStackValueRef& ScriptStackValueRef::operator=(S32 newValue)
{
    if (index == -1)
        index = ScriptEngine::getInstance()->getStack()->pushSignedInteger(newValue);
    else
        ScriptEngine::getInstance()->getStack()->setSignedInteger(index, newValue);
    return *this;
}

ScriptStackValueRef& ScriptStackValueRef::operator=(F32 newValue)
{
    if (index == -1)
        index = ScriptEngine::getInstance()->getStack()->pushNumber(newValue);
    else
        ScriptEngine::getInstance()->getStack()->setNumber(index, newValue);
    return *this;
}

ScriptStackValueRef& ScriptStackValueRef::operator=(F64 newValue)
{
    if (index == -1)
        index = ScriptEngine::getInstance()->getStack()->pushNumber(newValue);
    else
        ScriptEngine::getInstance()->getStack()->setNumber(index, newValue);
    return *this;
}

ScriptStackValueRef& ScriptStackValueRef::operator=(SimFieldDictionary *newValue)
{
    if (index == -1)
        index = ScriptEngine::getInstance()->getStack()->pushSimFields(newValue);
    else
        ScriptEngine::getInstance()->getStack()->setSimFields(index, newValue);
    return *this;
}

ScriptStackValueRef& ScriptStackValueRef::operator=(SimObject *newValue)
{
    if (index == -1)
        index = ScriptEngine::getInstance()->getStack()->pushSimObject(newValue);
    else
        ScriptEngine::getInstance()->getStack()->setSimObject(index, newValue);
    return *this;
}


ScriptStackValueRef ScriptStackValueRef::clone()
{
    ScriptStackValueRef newValue;
    newValue.type = type;
    newValue.index = index == -1 ? ScriptEngine::getInstance()->getStack()->pushNull() : ScriptEngine::getInstance()->getStack()->pushIndex(index);
    return newValue;
}

