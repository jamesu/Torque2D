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

#ifndef __Torque2D__ScriptEngine__
#define __Torque2D__ScriptEngine__

#include "platform/platform.h"
#include "sim/simFieldDictionary.h"
#include "console/console.h"

class Namespace;
class SimObject;
class AbstractClassRep;

// Basic type for a stack value
typedef enum
{
    SCRIPTTYPE_NULL=0,
    SCRIPTTYPE_STRING=1,
    SCRIPTTYPE_INTEGER=2,
    SCRIPTTYPE_NUMBER=3,
    SCRIPTTYPE_BOOL=4,
    SCRIPTTYPE_TABLE=5,    // i.e. a script Dictionary
    SCRIPTTYPE_OBJECT=6    // Instance of class
} ScriptValueBaseType;

class ScriptStack
{
public:
    ScriptStack();
    virtual ~ScriptStack();
    
    // Push values
    virtual int pushNull();
    virtual int pushBool(bool value);
    virtual int pushIndex(int value);
    virtual int pushInteger(U32 value);
    virtual int pushSignedInteger(S32 value);
    virtual int pushNumber(F32 value);
    virtual int pushSimFields(SimFieldDictionary *values);
    virtual int pushString(const char *value);
    virtual int pushSimObject(SimObject *value);
    
    // Get values
    virtual const char* getString(int index);
    virtual U32 getInteger(int index);
    virtual S32 getSignedInteger(int index);
    virtual F32 getNumber(int index);
    virtual void getSimFields(int index, SimFieldDictionary *outValues);
    virtual bool getBool(int index);
    
    virtual ScriptValueBaseType getTypeAtIndex(int index);
    
    // Set values
    virtual void setBool(int index, bool value);
    virtual void setString(int index, const char *value);
    virtual void setInteger(int index, U32 value);
    virtual void setSignedInteger(int index, S32 value);
    virtual void setNumber(int index, F32 value);
    virtual void setSimFields(int index, SimFieldDictionary *values);
    virtual void setSimObject(int index, SimObject *value);
};

// A value which has already been pushed to the stack
// e.g.
//   ScriptStackValueRef value1 = "carrots";
//   ScriptStackValueRef value2 = 123;
//   ScriptEngine::getInstance()->call();
// or:
//   ScriptEngine::getInstance()->executef("carrots", 123);

class ScriptStackValueRef
{
public:
    int index;
    ScriptValueBaseType type;
    
    ScriptStackValueRef() : index(-1), type(SCRIPTTYPE_NULL) {;}
    ~ScriptStackValueRef() {;}
    
    ScriptStackValueRef(const ScriptStackValueRef &ref) : index(ref.index), type(ref.type) { ; }
    ScriptStackValueRef(const char *value) : index(-1)
    {
        *this = value;
    }
    ScriptStackValueRef(U32 value) : index(-1)
    {
        *this = value;
    }
    ScriptStackValueRef(S32 value) : index(-1)
    {
        *this = value;
    }
    ScriptStackValueRef(F32 value) : index(-1)
    {
        *this = value;
    }
    ScriptStackValueRef(F64 value) : index(-1)
    {
        *this = value;
    }
    ScriptStackValueRef(SimObject *value) : index(-1)
    {
        *this = value;
    }
    ScriptStackValueRef(bool value) : index(-1)
    {
        *this = value;
    }
    
    ScriptStackValueRef clone();
    
    inline const char* getStringValue();
    inline U32 getIntValue();
    inline S32 getSignedIntValue();
    inline F32 getFloatValue();
    inline bool getBoolValue();
    
    inline operator const char*() { return getStringValue(); }
    inline operator U32() { return getIntValue(); }
    inline operator S32() { return getSignedIntValue(); }
    inline operator F32() { return getFloatValue(); }
    
    inline bool isString() { return type == SCRIPTTYPE_STRING; }
    inline bool isInt() { return type == SCRIPTTYPE_INTEGER; }
    inline bool isFloat() { return type == SCRIPTTYPE_NUMBER; }
    inline bool isTable() { return type == SCRIPTTYPE_TABLE; }
    inline bool isObject() { return type == SCRIPTTYPE_OBJECT; }
    
    // Note: operators replace value
    ScriptStackValueRef& operator=(const ScriptStackValueRef &other);
    ScriptStackValueRef& operator=(const char *newValue);
    ScriptStackValueRef& operator=(U32 newValue);
    ScriptStackValueRef& operator=(S32 newValue);
    ScriptStackValueRef& operator=(F32 newValue);
    ScriptStackValueRef& operator=(F64 newValue);
    ScriptStackValueRef& operator=(SimFieldDictionary *dict);
    ScriptStackValueRef& operator=(SimObject *object);
    ScriptStackValueRef& operator=(bool newValue);
    
    static ScriptStackValueRef valueAtIndex(int index);
};

class ScriptStackValueRef;

class ScriptEngine
{
public:
    ScriptEngine();
    virtual ~ScriptEngine();
    
    static ScriptEngine *getInstance();
    
    // register class
    virtual void registerClass(AbstractClassRep *rep);
    // register namespace functions
    virtual void registerNamespace(Namespace *ns);
    
    // register object instance
    virtual void registerObject(SimObject *object);
    // unregister object instance
    virtual void removeObject(SimObject *object);
    
    // gets current stack
    virtual ScriptStack *getStack();
    virtual ScriptStackValueRef execute(S32 argc, ScriptStackValueRef argv[]);
    virtual ScriptStackValueRef executeOnObject(ScriptStackValueRef obj, S32 argc, ScriptStackValueRef argv[]);
    
#define ARG ScriptStackValueRef
    // Plain versions
    inline ScriptStackValueRef executef( ARG);
    inline ScriptStackValueRef executef( ARG, ARG);
    inline ScriptStackValueRef executef( ARG, ARG, ARG);
    inline ScriptStackValueRef executef( ARG, ARG, ARG, ARG);
    inline ScriptStackValueRef executef( ARG, ARG, ARG, ARG, ARG);
    inline ScriptStackValueRef executef( ARG, ARG, ARG, ARG, ARG, ARG);
    inline ScriptStackValueRef executef( ARG, ARG, ARG, ARG, ARG, ARG, ARG);
    inline ScriptStackValueRef executef( ARG, ARG, ARG, ARG, ARG, ARG, ARG, ARG);
    inline ScriptStackValueRef executef( ARG, ARG, ARG, ARG, ARG, ARG, ARG, ARG, ARG);
    inline ScriptStackValueRef executef( ARG, ARG, ARG, ARG, ARG, ARG, ARG, ARG, ARG, ARG);
    
    // Object versions
    inline ScriptStackValueRef executef(SimObject *, ARG);
    inline ScriptStackValueRef executef(SimObject *, ARG, ARG);
    inline ScriptStackValueRef executef(SimObject *, ARG, ARG, ARG);
    inline ScriptStackValueRef executef(SimObject *, ARG, ARG, ARG, ARG);
    inline ScriptStackValueRef executef(SimObject *, ARG, ARG, ARG, ARG, ARG);
    inline ScriptStackValueRef executef(SimObject *, ARG, ARG, ARG, ARG, ARG, ARG);
    inline ScriptStackValueRef executef(SimObject *, ARG, ARG, ARG, ARG, ARG, ARG, ARG);
    inline ScriptStackValueRef executef(SimObject *, ARG, ARG, ARG, ARG, ARG, ARG, ARG, ARG);
    inline ScriptStackValueRef executef(SimObject *, ARG, ARG, ARG, ARG, ARG, ARG, ARG, ARG, ARG);
    inline ScriptStackValueRef executef(SimObject *, ARG, ARG, ARG, ARG, ARG, ARG, ARG, ARG, ARG, ARG);
    inline ScriptStackValueRef executef(SimObject *, ARG, ARG, ARG, ARG, ARG, ARG, ARG, ARG, ARG, ARG, ARG);
#undef ARG
    
    /// Evaluate an arbitrary chunk of code.
    ///
    /// @param  string   Buffer containing code to execute.
    /// @param  echo     Should we echo the string to the console?
    /// @param  fileName Indicate what file this code is coming from; used in error reporting and such.
    virtual ScriptStackValueRef evaluate(const char* string, bool echo = false, const char *fileName = NULL);
    
    /// Evaluate an arbitrary line of script.
    ///
    /// This wraps dVsprintf(), so you can substitute parameters into the code being executed.
    virtual ScriptStackValueRef evaluatef(const char* string, ...);
    
    // Compiles a file to a binary script
    virtual bool compileFile(const char *filename) = 0;
    
    // Executes a file to a binary script
    virtual bool executeFile(const char *filename) = 0;
};


#define A ScriptStackValueRef
#define OBJ SimObject* obj
inline ScriptStackValueRef ScriptEngine::executef(OBJ, A a)                                    { ScriptStackValueRef params[] = {a}; return executeOnObject(obj, 1, params); }
inline ScriptStackValueRef ScriptEngine::executef(OBJ, A a, A b)                               { ScriptStackValueRef params[] = {a,b}; return executeOnObject(obj, 2, params); }
inline ScriptStackValueRef ScriptEngine::executef(OBJ, A a, A b, A c)                          { ScriptStackValueRef params[] = {a,b,c}; return executeOnObject(obj, 3, params); }
inline ScriptStackValueRef ScriptEngine::executef(OBJ, A a, A b, A c, A d)                     { ScriptStackValueRef params[] = {a,b,c,d}; return executeOnObject(obj, 4, params); }
inline ScriptStackValueRef ScriptEngine::executef(OBJ, A a, A b, A c, A d, A e)                { ScriptStackValueRef params[] = {a,b,c,d,e}; return executeOnObject(obj, 5, params); }
inline ScriptStackValueRef ScriptEngine::executef(OBJ, A a, A b, A c, A d, A e, A f)           { ScriptStackValueRef params[] = {a,b,c,d,e,f}; return executeOnObject(obj, 6, params); }
inline ScriptStackValueRef ScriptEngine::executef(OBJ, A a, A b, A c, A d, A e, A f, A g)      { ScriptStackValueRef params[] = {a,b,c,d,e,f,g}; return executeOnObject(obj, 7, params); }
inline ScriptStackValueRef ScriptEngine::executef(OBJ, A a, A b, A c, A d, A e, A f, A g, A h) { ScriptStackValueRef params[] = {a,b,c,d,e,f,g,h}; return executeOnObject(obj, 8, params); }
inline ScriptStackValueRef ScriptEngine::executef(OBJ, A a, A b, A c, A d, A e, A f, A g, A h, A i) { ScriptStackValueRef params[] = {a,b,c,d,e,f,g,h,i}; return executeOnObject(obj, 9, params); }
inline ScriptStackValueRef ScriptEngine::executef(OBJ, A a, A b, A c, A d, A e, A f, A g, A h, A i, A j) { ScriptStackValueRef params[] = {a,b,c,d,e,f,g,h,i,j}; return executeOnObject(obj, 10, params); }
inline ScriptStackValueRef ScriptEngine::executef(OBJ, A a, A b, A c, A d, A e, A f, A g, A h, A i, A j, A k) { ScriptStackValueRef params[] = {a,b,c,d,e,f,g,h,i,j,k}; return executeOnObject(obj, 11, params); }

inline ScriptStackValueRef ScriptEngine::executef(A a)                                    { ScriptStackValueRef params[] = {a}; return execute(1, params); }
inline ScriptStackValueRef ScriptEngine::executef(A a, A b)                               { ScriptStackValueRef params[] = {a,b}; return execute(2, params); }
inline ScriptStackValueRef ScriptEngine::executef(A a, A b, A c)                          { ScriptStackValueRef params[] = {a,b,c}; return execute(3, params); }
inline ScriptStackValueRef ScriptEngine::executef(A a, A b, A c, A d)                     { ScriptStackValueRef params[] = {a,b,c,d}; return execute(4, params); }
inline ScriptStackValueRef ScriptEngine::executef(A a, A b, A c, A d, A e)                { ScriptStackValueRef params[] = {a,b,c,d,e}; return execute(5, params); }
inline ScriptStackValueRef ScriptEngine::executef(A a, A b, A c, A d, A e, A f)           { ScriptStackValueRef params[] = {a,b,c,d,e,f}; return execute(6, params); }
inline ScriptStackValueRef ScriptEngine::executef(A a, A b, A c, A d, A e, A f, A g)      { ScriptStackValueRef params[] = {a,b,c,d,e,f,g}; return execute(7, params); }
inline ScriptStackValueRef ScriptEngine::executef(A a, A b, A c, A d, A e, A f, A g, A h) { ScriptStackValueRef params[] = {a,b,c,d,e,f,g,h}; return execute(8, params); }
inline ScriptStackValueRef ScriptEngine::executef(A a, A b, A c, A d, A e, A f, A g, A h, A i) { ScriptStackValueRef params[] = {a,b,c,d,e,f,g,h,i}; return execute(9, params); }
inline ScriptStackValueRef ScriptEngine::executef(A a, A b, A c, A d, A e, A f, A g, A h, A i, A j) { ScriptStackValueRef params[] = {a,b,c,d,e,f,g,h,i,j}; return execute(10, params); }
#undef A
#undef OBJ

inline const char* ScriptStackValueRef::getStringValue() { return ScriptEngine::getInstance()->getStack()->getString(index); }
inline U32 ScriptStackValueRef::getIntValue() { return ScriptEngine::getInstance()->getStack()->getInteger(index); }
inline S32 ScriptStackValueRef::getSignedIntValue() { return ScriptEngine::getInstance()->getStack()->getSignedInteger(index); }
inline F32 ScriptStackValueRef::getFloatValue() { return ScriptEngine::getInstance()->getStack()->getNumber(index); }
inline bool ScriptStackValueRef::getBoolValue() { return ScriptEngine::getInstance()->getStack()->getInteger(index); }

#endif /* defined(__Torque2D__ScriptEngine__) */
