#include <iostream>
#include <vector>
#include <llvm/IR/Value.h>

#include "classinfo.h"

class CodeGenContext;
class NStatement;
class NExpression;
class NVariableDeclaration;
class NIdentifier;

typedef std::vector<NStatement*> StatementList;
typedef std::vector<NExpression*> ExpressionList;
typedef std::vector<NIdentifier*> VariableList;

class Node {
public:
	virtual ~Node() {}
	virtual ValueBase* codeGen(CodeGenContext& context) { return NULL; }
};

class NExpression : public Node {
};

class NStatement : public Node {
};

class NInteger : public NExpression {
public:
	long long value;
	NInteger(long long value) : value(value) { }
	virtual ValueBase* codeGen(CodeGenContext& context);
};

class NDouble : public NExpression {
public:
	double value;
	NDouble(double value) : value(value) { }
	virtual ValueBase* codeGen(CodeGenContext& context);
};

class NIdentifier : public NExpression {
public:
	std::string name;
	NIdentifier(const std::string& name) : name(name) { }
	virtual ValueBase* codeGen(CodeGenContext& context);
};

class NMethodCall : public NExpression {
public:
	const NIdentifier& id;
	ExpressionList arguments;
	NMethodCall(const NIdentifier& id, ExpressionList& arguments) :
		id(id), arguments(arguments) { }
	NMethodCall(const NIdentifier& id) : id(id) { }
	virtual ValueBase* codeGen(CodeGenContext& context);
    
    ValueBase* callCompilerFunc(CodeGenContext& context);
    ValueBase* callScriptFunc(CodeGenContext& context);
    
};

class NBinaryOperator : public NExpression {
public:
	int op;
	NExpression& lhs;
	NExpression& rhs;
	NBinaryOperator(NExpression& lhs, int op, NExpression& rhs) :
		lhs(lhs), rhs(rhs), op(op) { }
	virtual ValueBase* codeGen(CodeGenContext& context);

private:
	ValueBase* doArithmeticalOps(CodeGenContext& context);
	ValueBase* doArithmeticalDoubleOps(CodeGenContext& context, ValueBase* p1, ValueBase* p2);
	ValueBase* doArithmeticalIntOps(CodeGenContext& context, ValueBase* p1, ValueBase* p2);
	ValueBase* doLogicalOps(CodeGenContext& context);
};

class NAssignment : public NExpression {
public:
	NIdentifier& lhs;
	NExpression& rhs;
	NAssignment(NIdentifier& lhs, NExpression& rhs) : 
		lhs(lhs), rhs(rhs) { }
	virtual ValueBase* codeGen(CodeGenContext& context);
};

class NBlock : public NExpression {
public:
	StatementList statements;
	NBlock() { }
	virtual ValueBase* codeGen(CodeGenContext& context);
};

class NExpressionStatement : public NStatement {
public:
	NExpression& expression;
	NExpressionStatement(NExpression& expression) : 
		expression(expression) { }
	virtual ValueBase* codeGen(CodeGenContext& context);
};

class NReturnStatement : public NStatement {
public:
	NExpression& expression;
	NReturnStatement(NExpression& expression) : 
		expression(expression) { }
	virtual ValueBase* codeGen(CodeGenContext& context);
};

class NExternDeclaration : public NStatement {
public:
    const NIdentifier& type;
    const NIdentifier& id;
    VariableList arguments;
    NExternDeclaration(const NIdentifier& type, const NIdentifier& id,
            const VariableList& arguments) :
        type(type), id(id), arguments(arguments) {}
    virtual ValueBase* codeGen(CodeGenContext& context);
};

class NFunctionDeclaration : public NStatement {
public:
	const NIdentifier& type;
	const NIdentifier& id;
	VariableList arguments;
	NBlock& block;
	NFunctionDeclaration(const NIdentifier& type, const NIdentifier& id, 
			const VariableList& arguments, NBlock& block) :
		type(type), id(id), arguments(arguments), block(block) { }
    virtual ValueBase* codeGen(CodeGenContext& context);
    ValueBase* codeGen(CodeGenContext& context, const std::vector<llvm::Type*> &argTypes);
    
};

class NIfElseStatement : public NStatement {
public:
	NExpression* condExpr;
	NBlock* thenBlock;
	NBlock*  elseBlock;
	NIfElseStatement(NExpression* condExpr, NBlock* thenBlock, NBlock* elseBlock = NULL) :
		condExpr(condExpr), thenBlock(thenBlock), elseBlock(elseBlock) {}
	virtual ValueBase* codeGen(CodeGenContext& context);
};


