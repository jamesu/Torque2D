//-----------------------------------------------------------------------------
// Copyright (c) 2012 GarageGames, LLC
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

#ifndef _AST_H_
#define _AST_H_

class ExprEvalState;
class Namespace;
class SimObject;
class SimGroup;
class CodeStream;

#define SET_VERIFY_TARGET_REG_SIZE() U32 _targetSize = codeStream.mTargetList.size();
#define VERIFY_TARGET_REG_SIZE(delta) AssertFatal(codeStream.mTargetList.size() == _targetSize, "Invalid target stack");

/// Enable this #define if you are seeing the message "precompile size mismatch" in the console.
/// This will help track down which node type is causing the error. It could be
/// due to incorrect compiler optimization.
//#define DEBUG_AST_NODES

enum TypeReq
{
   TypeReqNone,

   // Deprecated --
   TypeReqUInt,
   TypeReqFloat,
   TypeReqString,
   // Deprecated ^^

   TypeReqVar,

   TypeReqConditional, // Must emit a conditional jump op
   TypeReqFalseConditional,
   TypeReqTargetRegister, // Must pop and assign an input target register
};

/// Representation of a node for the scripting language parser.
///
/// When the scripting language is evaluated, it is turned from a string representation,
/// into a parse tree, thence into byte code, which is ultimately interpreted by the VM.
///
/// This is the base class for the nodes in the parse tree. There are a great many subclasses,
/// each representing a different language construct.
struct StmtNode
{
    StmtNode *next;   ///< Next entry in parse tree.
    
    StmtNode();
    virtual ~StmtNode() {}
    
    /// @name next Accessors
    /// @{
    
    ///
    void append(StmtNode *next);
    StmtNode *getNext() const { return next; }
    
    /// @}
    
    /// @name Debug Info
    /// @{
    
    StringTableEntry dbgFileName; ///< Name of file this node is associated with.
    S32 dbgLineNumber;            ///< Line number this node is associated with.
#ifdef DEBUG_AST_NODES
    virtual String dbgStmtType() const = 0;
#endif
    /// @}
    
    /// @name Breaking
    /// @{
    
    void addBreakLine(CodeStream &codeStream);
    /// @}
    
    /// @name Compilation
    /// @{
    
    virtual void compileStmt(CodeStream &codeStream) = 0;
    virtual void setPackage(StringTableEntry packageName);
    /// @}
};

/// Helper macro
#ifndef DEBUG_AST_NODES
#  define DBG_STMT_TYPE(s) virtual const char* dbgStmtType() const { return #s; }
#else
#  define DBG_STMT_TYPE(s)
#endif

struct BreakStmtNode : StmtNode
{
    static BreakStmtNode *alloc( S32 lineNumber );
    
    
    void compileStmt(CodeStream &codeStream);
    DBG_STMT_TYPE(BreakStmtNode);
};

struct ContinueStmtNode : StmtNode
{
    static ContinueStmtNode *alloc( S32 lineNumber );
    
    void compileStmt(CodeStream &codeStream);
    DBG_STMT_TYPE(ContinueStmtNode);
};

/// A mathematical expression.
struct ExprNode : StmtNode
{
    
    void compileStmt(CodeStream &codeStream);
    
    virtual void compile(CodeStream &codeStream, TypeReq type) = 0;
    virtual TypeReq getPreferredType() = 0;
};

struct ReturnStmtNode : StmtNode
{
    ExprNode *expr;
    
    static ReturnStmtNode *alloc( S32 lineNumber, ExprNode *expr );
    
    void compileStmt(CodeStream &codeStream);
    DBG_STMT_TYPE(ReturnStmtNode);
};

struct IfStmtNode : StmtNode
{
    ExprNode *testExpr;
    StmtNode *ifBlock, *elseBlock;
    U32 endifOffset;
    U32 elseOffset;
    bool integer;
    bool propagate;
    
    static IfStmtNode *alloc( S32 lineNumber, ExprNode *testExpr, StmtNode *ifBlock, StmtNode *elseBlock, bool propagateThrough );
    void propagateSwitchExpr(ExprNode *left, bool string);
    ExprNode *getSwitchOR(ExprNode *left, ExprNode *list, bool string);
    
    void compileStmt(CodeStream &codeStream);
    DBG_STMT_TYPE(IfStmtNode);
};

struct LoopStmtNode : StmtNode
{
    ExprNode *testExpr;
    ExprNode *initExpr;
    ExprNode *endLoopExpr;
    StmtNode *loopBlock;
    bool isDoLoop;
    U32 breakOffset;
    U32 continueOffset;
    U32 loopBlockStartOffset;
    bool integer;
    
    static LoopStmtNode *alloc( S32 lineNumber, ExprNode *testExpr, ExprNode *initExpr, ExprNode *endLoopExpr, StmtNode *loopBlock, bool isDoLoop );
    
    void compileStmt(CodeStream &codeStream);
    DBG_STMT_TYPE(LoopStmtNode);
};

/// A "foreach" statement.
struct IterStmtNode : StmtNode
{
    /// Local variable name to use for the container element.
    StringTableEntry varName;
    
    /// Expression evaluating to a SimSet object.
    ExprNode* containerExpr;
    
    /// The statement body.
    StmtNode* body;
    
    /// If true, this is a 'foreach$'.
    bool isStringIter;
    
    /// Bytecode size of body statement.  Set by precompileStmt.
    U32 bodySize;
    
    static IterStmtNode* alloc( S32 lineNumber, StringTableEntry varName, ExprNode* containerExpr, StmtNode* body, bool isStringIter );
    
    void compileStmt(CodeStream &codeStream);
};

/// A binary mathematical expression (ie, left op right).
struct BinaryExprNode : ExprNode
{
    S32 op;
    ExprNode *left;
    ExprNode *right;
};

struct FloatBinaryExprNode : BinaryExprNode
{
    static FloatBinaryExprNode *alloc( S32 lineNumber, S32 op, ExprNode *left, ExprNode *right );
    
    void compile(CodeStream &codeStream, TypeReq type);
    TypeReq getPreferredType();
    DBG_STMT_TYPE(FloatBinaryExprNode);
};

struct ConditionalExprNode : ExprNode
{
    ExprNode *testExpr;
    ExprNode *trueExpr;
    ExprNode *falseExpr;
    bool integer;
    static ConditionalExprNode *alloc( S32 lineNumber, ExprNode *testExpr, ExprNode *trueExpr, ExprNode *falseExpr );
    
    virtual void compile(CodeStream &codeStream, TypeReq type);
    virtual TypeReq getPreferredType();
    DBG_STMT_TYPE(ConditionalExprNode);
};

struct IntBinaryExprNode : BinaryExprNode
{
    TypeReq subType;
    U32 operand;
   
   U32 conditionalValue;
    U32 cmpValue;
   
    
    static IntBinaryExprNode *alloc( S32 lineNumber, S32 op, ExprNode *left, ExprNode *right );
    
    void getSubTypeOperand();
    
    void compile(CodeStream &codeStream, TypeReq type);
    TypeReq getPreferredType();
    DBG_STMT_TYPE(IntBinaryExprNode);
};

struct StreqExprNode : BinaryExprNode
{
    bool eq;
    static StreqExprNode *alloc( S32 lineNumber, ExprNode *left, ExprNode *right, bool eq );
    
    void compile(CodeStream &codeStream, TypeReq type);
    TypeReq getPreferredType();
    DBG_STMT_TYPE(StreqExprNode);
};

struct StrcatExprNode : BinaryExprNode
{
    S32 appendChar;
    static StrcatExprNode *alloc( S32 lineNumber, ExprNode *left, ExprNode *right, S32 appendChar );
    
    void compile(CodeStream &codeStream, TypeReq type);
    TypeReq getPreferredType();
    DBG_STMT_TYPE(StrcatExprNode);
};

struct CommaCatExprNode : BinaryExprNode
{
    static CommaCatExprNode *alloc( S32 lineNumber, ExprNode *left, ExprNode *right );
    
    
    void compile(CodeStream &codeStream, TypeReq type);
    TypeReq getPreferredType();
    DBG_STMT_TYPE(CommaCatExprNode);
};

struct IntUnaryExprNode : ExprNode
{
    S32 op;
    ExprNode *expr;
    bool integer;
    
    static IntUnaryExprNode *alloc( S32 lineNumber, S32 op, ExprNode *expr );
    
    void compile(CodeStream &codeStream, TypeReq type);
    TypeReq getPreferredType();
    DBG_STMT_TYPE(IntUnaryExprNode);
};

struct FloatUnaryExprNode : ExprNode
{
    S32 op;
    ExprNode *expr;
    
    static FloatUnaryExprNode *alloc( S32 lineNumber, S32 op, ExprNode *expr );
    
    void compile(CodeStream &codeStream, TypeReq type);
    TypeReq getPreferredType();
    DBG_STMT_TYPE(FloatUnaryExprNode);
};

struct VarNode : ExprNode
{
    StringTableEntry varName;
    ExprNode *arrayIndex;
    
    static VarNode *alloc( S32 lineNumber, StringTableEntry varName, ExprNode *arrayIndex );
    
    void compile(CodeStream &codeStream, TypeReq type);
    TypeReq getPreferredType();
    DBG_STMT_TYPE(VarNode);
};

struct IntNode : ExprNode
{
    S32 value;
    U32 index; // if it's converted to float/string
    
    static IntNode *alloc( S32 lineNumber, S32 value );
    
    void compile(CodeStream &codeStream, TypeReq type);
    TypeReq getPreferredType();
    DBG_STMT_TYPE(IntNode);
};

struct FloatNode : ExprNode
{
    F64 value;
    U32 index;
    
    static FloatNode *alloc( S32 lineNumber, F64 value );
    
    void compile(CodeStream &codeStream, TypeReq type);
    TypeReq getPreferredType();
    DBG_STMT_TYPE(FloatNode);
};

struct StrConstNode : ExprNode
{
    char *str;
    F64 fVal;
    U32 index;
    bool tag;
    bool doc; // Specifies that this string is a documentation block.
    
    static StrConstNode *alloc( S32 lineNumber, char *str, bool tag, bool doc = false );
    
    void compile(CodeStream &codeStream, TypeReq type);
    TypeReq getPreferredType();
    DBG_STMT_TYPE(StrConstNode);
};

struct ConstantNode : ExprNode
{
    StringTableEntry value;
    F64 fVal;
    U32 index;
    
    static ConstantNode *alloc( S32 lineNumber, StringTableEntry value );
    
    void compile(CodeStream &codeStream, TypeReq type);
    TypeReq getPreferredType();
    DBG_STMT_TYPE(ConstantNode);
};

struct AssignExprNode : ExprNode
{
    StringTableEntry varName;
    ExprNode *expr;
    ExprNode *arrayIndex;
    TypeReq subType;
    
    static AssignExprNode *alloc( S32 lineNumber, StringTableEntry varName, ExprNode *arrayIndex, ExprNode *expr );
    
    void compile(CodeStream &codeStream, TypeReq type);
    TypeReq getPreferredType();
    DBG_STMT_TYPE(AssignExprNode);
};

struct AssignDecl
{
    S32 lineNumber;
    S32 token;
    ExprNode *expr;
    bool integer;
};

struct AssignOpExprNode : ExprNode
{
    StringTableEntry varName;
    ExprNode *expr;
    ExprNode *arrayIndex;
    S32 op;
    U32 operand;
    TypeReq subType;
    
    static AssignOpExprNode *alloc( S32 lineNumber, StringTableEntry varName, ExprNode *arrayIndex, ExprNode *expr, S32 op );
    
    void compile(CodeStream &codeStream, TypeReq type);
    TypeReq getPreferredType();
    DBG_STMT_TYPE(AssignOpExprNode);
};

struct FuncCallExprNode : ExprNode
{
    StringTableEntry funcName;
    StringTableEntry nameSpace;
    ExprNode *args;
    U32 callType;
    enum {
        FunctionCall,
        MethodCall,
        ParentCall
    };
    
    static FuncCallExprNode *alloc( S32 lineNumber, StringTableEntry funcName, StringTableEntry nameSpace, ExprNode *args, bool dot );
    
    void compile(CodeStream &codeStream, TypeReq type);
    TypeReq getPreferredType();
    DBG_STMT_TYPE(FuncCallExprNode);
};

struct AssertCallExprNode : ExprNode
{
    ExprNode *testExpr;
    const char *message;
    U32 messageIndex;
    
    static AssertCallExprNode *alloc( S32 lineNumber, ExprNode *testExpr, const char *message );
    
    void compile(CodeStream &codeStream, TypeReq type);
    TypeReq getPreferredType();
    DBG_STMT_TYPE(AssertCallExprNode);
};

struct SlotDecl
{
    S32              lineNumber;
    ExprNode         *object;
    StringTableEntry slotName;
    ExprNode         *array;
};

struct SlotAccessNode : ExprNode
{
    ExprNode *objectExpr, *arrayExpr;
    StringTableEntry slotName;
    
    static SlotAccessNode *alloc( S32 lineNumber, ExprNode *objectExpr, ExprNode *arrayExpr, StringTableEntry slotName );
    
    void compile(CodeStream &codeStream, TypeReq type);
    TypeReq getPreferredType();
    DBG_STMT_TYPE(SlotAccessNode);
};

struct InternalSlotDecl
{
    S32              lineNumber;
    ExprNode         *object;
    ExprNode         *slotExpr;
    bool             recurse;
};

struct InternalSlotAccessNode : ExprNode
{
    ExprNode *objectExpr, *slotExpr;
    bool recurse;
    
    static InternalSlotAccessNode *alloc( S32 lineNumber, ExprNode *objectExpr, ExprNode *slotExpr, bool recurse );
    
    void compile(CodeStream &codeStream, TypeReq type);
    TypeReq getPreferredType();
    DBG_STMT_TYPE(InternalSlotAccessNode);
};

struct SlotAssignNode : ExprNode
{
    ExprNode *objectExpr, *arrayExpr;
    StringTableEntry slotName;
    ExprNode *valueExpr;
    U32 typeID;
    
    static SlotAssignNode *alloc( S32 lineNumber, ExprNode *objectExpr, ExprNode *arrayExpr, StringTableEntry slotName, ExprNode *valueExpr, U32 typeID = -1 );
    
    void compile(CodeStream &codeStream, TypeReq type);
    TypeReq getPreferredType();
    DBG_STMT_TYPE(SlotAssignNode);
};

struct SlotAssignOpNode : ExprNode
{
    ExprNode *objectExpr, *arrayExpr;
    StringTableEntry slotName;
    S32 op;
    ExprNode *valueExpr;
    U32 operand;
    TypeReq subType;
    
    static SlotAssignOpNode *alloc( S32 lineNumber, ExprNode *objectExpr, StringTableEntry slotName, ExprNode *arrayExpr, S32 op, ExprNode *valueExpr );
    
    void compile(CodeStream &codeStream, TypeReq type);
    TypeReq getPreferredType();
    DBG_STMT_TYPE(SlotAssignOpNode);
};

struct ObjectDeclNode : ExprNode
{
    ExprNode *classNameExpr;
    StringTableEntry parentObject;
    ExprNode *objectNameExpr;
    ExprNode *argList;
    SlotAssignNode *slotDecls;
    ObjectDeclNode *subObjects;
    bool isDatablock;
    U32 failOffset;
    bool isClassNameInternal;
    bool isSingleton;
    
    static ObjectDeclNode *alloc( S32 lineNumber, ExprNode *classNameExpr, ExprNode *objectNameExpr, ExprNode *argList, StringTableEntry parentObject, SlotAssignNode *slotDecls, ObjectDeclNode *subObjects, bool isDatablock, bool classNameInternal, bool isSingleton );
   
    void compile(CodeStream &codeStream, TypeReq type);
    void compileSubObject(CodeStream &codeStream, U32 targetRegister, S32 parentObject);
   
    TypeReq getPreferredType();
    DBG_STMT_TYPE(ObjectDeclNode);
};

struct ObjectBlockDecl
{
    SlotAssignNode *slots;
    ObjectDeclNode *decls;
};

struct ReferencedVariableNode
{
   StringTableEntry varName;
   ReferencedVariableNode* next;
};

struct FunctionDeclStmtNode : StmtNode
{
    StringTableEntry fnName;
    VarNode *args;
    StmtNode *stmts;
    StringTableEntry nameSpace;
    StringTableEntry package;
    U32 endOffset;
    U32 argc;
    
    VarNode *vars;
    ReferencedVariableNode* localVars;
    
    static FunctionDeclStmtNode *alloc( S32 lineNumber, StringTableEntry fnName, StringTableEntry nameSpace, VarNode *args, StmtNode *stmts );
   
    static void referenceLocalVariable(StringTableEntry name);
    static void pushVariableScope();
    static void popVariableScope();
   
    void compileStmt(CodeStream &codeStream);
    void compileFunction(CodeStream& codeStream);
    void setPackage(StringTableEntry packageName);
    DBG_STMT_TYPE(FunctionDeclStmtNode);
};

class CodeBlockEvalState;
extern StmtNode *gStatementList;
extern ReferencedVariableNode *gCurrentLocalVariables;
extern ReferencedVariableNode *gLocalVariableStack[16];
extern U32 gLocalVariableStackIdx;

extern U32 gAnonFunctionID;
extern CodeBlockEvalState gNewEvalState;

#endif