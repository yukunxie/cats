#include "node.h"
#include "codegen.h"
#include "parser.hpp"

using namespace std;

IRBuilder<> builder(getGlobalContext());

/* Compile the AST into a module */
void CodeGenContext::generateCode(NBlock& root)
{
	//std::cout << "Generating code...\n";
	
	/* Create the top level interpreter function to call as entry */
	vector<Type*> argTypes;
	FunctionType *ftype = FunctionType::get(Type::getVoidTy(getGlobalContext()), makeArrayRef(argTypes), false);
	mainFunction = Function::Create(ftype, GlobalValue::InternalLinkage, "main", module);
	BasicBlock *bblock = BasicBlock::Create(getGlobalContext(), "entry", mainFunction, 0);
	
	currentFunction = mainFunction;
	
	/* Push a new variable/block context */
	pushBlock(bblock);
	root.codeGen(*this); /* emit bytecode for the toplevel block */
	ReturnInst::Create(getGlobalContext(), bblock);
	popBlock();
	
	/* Print the bytecode in a human-readable format 
	   to see if our program compiled properly
	 */
	//std::cout << "Code is generated.\n";
	PassManager pm;
	pm.add(createPrintModulePass(outs()));
	pm.run(*module);
}

/* Executes the AST by running the main function */
GenericValue CodeGenContext::runCode() {
	//std::cout << "Running code...\n";
	ExecutionEngine *ee = EngineBuilder(module).create();
	vector<GenericValue> noargs;
	GenericValue v = ee->runFunction(mainFunction, noargs);
	//std::cout << "Code was run.\n";
	return v;
}

/* Returns an LLVM type based on the identifier */
static Type *typeOf(const NIdentifier& type) 
{
	if (type.name.compare("int") == 0) {
		return Type::getInt64Ty(getGlobalContext());
	}
	else if (type.name.compare("double") == 0) {
		return Type::getDoubleTy(getGlobalContext());
	}
	return Type::getVoidTy(getGlobalContext());
}

/* -- Code Generation -- */

ValueBase* NInteger::codeGen(CodeGenContext& context)
{
	//std::cout << "Creating integer: " << value << endl;
	return new LongValue(ConstantInt::get(Type::getInt64Ty(getGlobalContext()), value, true));
}

ValueBase* NDouble::codeGen(CodeGenContext& context)
{
	//std::cout << "Creating double: " << value << endl;
	return new DoubleValue(ConstantFP::get(Type::getDoubleTy(getGlobalContext()), value));
}

ValueBase* NIdentifier::codeGen(CodeGenContext& context)
{
	//std::cout << "Creating identifier reference: " << name << endl;
	ValueBase *value = context.getVar(name);
	if (value == NULL) {
		return NULL;
	}
	//if (context.locals().find(name) == context.locals().end()) {
	//	std::cerr << "undeclared variable " << name << endl;
	//	return NULL;
	//}
	//std::cout << value <<" " << context.locals()[name] << endl;
	return new LongValue(new LoadInst(value->getValue(), "", false, context.currentBlock()));
}

ValueBase* NMethodCall::codeGen(CodeGenContext& context)
{
    //std::cout <<"method call: " << id.name << endl;
    ValueBase * functionInfo = context.getVar(id.name);
    if (functionInfo == NULL){
        return this->callCompilerFunc(context);
    }
    else{
        return this->callScriptFunc(context);
    }
}

ValueBase* NMethodCall::callCompilerFunc(CodeGenContext& context)
{
    Function *function = context.module->getFunction(id.name.c_str());
    if (function == NULL) {
        std::cerr << "no such function " << id.name << endl;
    }
    std::vector<Value*> args;
    ExpressionList::const_iterator it;
    for (it = arguments.begin(); it != arguments.end(); it++) {
        args.push_back((**it).codeGen(context)->getValue());
    }
    CallInst *call = CallInst::Create(function, makeArrayRef(args), "", context.currentBlock());
    //std::cout << "Creating method call: " << id.name << endl;
    return new LongValue(call);
}

ValueBase* NMethodCall::callScriptFunc(CodeGenContext& context)
{
    ValueBase * functionInfo = context.getVar(id.name);
    if (functionInfo->getType() != ValueType::VT_FUNCTION){
        std::cerr<< id.name <<" is not a function" <<endl;
        return NULL;
    }
    
    std::vector<Value*> args;
    std::vector<Type*> argsTypes;
    ExpressionList::const_iterator it;
    for (it = arguments.begin(); it != arguments.end(); it++) {
        ValueBase* arg = (**it).codeGen(context);
        argsTypes.push_back(arg->getRealType());
        args.push_back(arg->getValue());
    }
    
    ValueBase *value = ((FunctionValue*)functionInfo)->getFunctionInfo()->codeGen(context, argsTypes);
    CallInst *call = CallInst::Create(value->getValue(), makeArrayRef(args), "", context.currentBlock());
    //std::cout << "Creating method call: " << id.name << endl;
    return new LongValue(call);
}

ValueBase* NBinaryOperator::codeGen(CodeGenContext& context)
{
	//std::cout << "Creating binary operation " << op << endl;
	Instruction::BinaryOps instr;
	CmpInst::Predicate cmpInst;
	switch (op) {
		case TPLUS: 	instr = Instruction::Add; goto math;
		case TMINUS: 	instr = Instruction::Sub; goto math;
		case TMUL: 		instr = Instruction::Mul; goto math;
		case TDIV: 		instr = Instruction::SDiv; goto math;
				
		/* TODO comparison */
		case TCEQ:		cmpInst = CmpInst::ICMP_EQ; goto cmp;
		case TCNE:		cmpInst = CmpInst::ICMP_NE; goto cmp;
		case TCLT:		cmpInst = CmpInst::ICMP_ULT; goto cmp;
		case TCLE:		cmpInst = CmpInst::ICMP_ULE; goto cmp;
		case TCGT:		cmpInst = CmpInst::ICMP_UGT; goto cmp;
		case TCGE:		cmpInst = CmpInst::ICMP_UGE; goto cmp;
		//TCEQ | TCNE | TCLT | TCLE | TCGT | TCGE;
	}

	return NULL;
math:
	return new LongValue(BinaryOperator::Create(instr, lhs.codeGen(context)->getValue(),
		rhs.codeGen(context)->getValue(), "", context.currentBlock()));

cmp:
	return new BoolValue(CmpInst::Create(Instruction::ICmp, cmpInst,lhs.codeGen(context)->getValue(), rhs.codeGen(context)->getValue(), "", context.currentBlock()));
}

ValueBase* NAssignment::codeGen(CodeGenContext& context)
{
	//std::cout << "Creating assignment for " << lhs.name << endl;
	ValueBase * value = context.getVar(lhs.name);
	if (value == NULL) {
        auto type = rhs.codeGen(context)->getRealType();
        AllocaInst *alloc = new AllocaInst(type, lhs.name.c_str(), context.currentBlock());
        value = new LongValue(alloc);
        context.locals()[lhs.name] = value;
	}
    
    ValueBase* _target = rhs.codeGen(context);
    new StoreInst(_target->getValue(), value->getValue(), false, context.currentBlock());
    return _target;
}

ValueBase* NBlock::codeGen(CodeGenContext& context)
{
	StatementList::const_iterator it;
	ValueBase *last = NULL;
	for (it = statements.begin(); it != statements.end(); it++) {
		//std::cout << "Generating code for " << typeid(**it).name() << endl;
		last = (**it).codeGen(context);
		if (strcmp(typeid(**it).name(), "16NReturnStatement") == 0){
			BasicBlock *retStmt = BasicBlock::Create(getGlobalContext(), "retStmt", context.currentFunction);    
			Value *value = last->getValue();
    		ReturnInst::Create(getGlobalContext(), value, retStmt);
			llvm::BranchInst::Create(retStmt,context.currentBlock());
			return (ValueBase*)0xff;
		}

	}
	//std::cout << "Creating block" << endl;
	return NULL;
}

ValueBase* NExpressionStatement::codeGen(CodeGenContext& context)
{
	//std::cout << "Generating code for " << typeid(expression).name() << endl;
	return expression.codeGen(context);
}

ValueBase* NReturnStatement::codeGen(CodeGenContext& context)
{
	ValueBase *returnValue = expression.codeGen(context);
    //ReturnInst::Create(getGlobalContext(), returnValue->getValue(), context.currentBlock());
	return returnValue;
}

ValueBase* NExternDeclaration::codeGen(CodeGenContext& context)
{
//    vector<Type*> argTypes;
//    VariableList::const_iterator it;
//    for (it = arguments.begin(); it != arguments.end(); it++) {
//        argTypes.push_back(typeOf((**it).type));
//    }
//    FunctionType *ftype = FunctionType::get(typeOf(type), makeArrayRef(argTypes), false);
//    Function *function = Function::Create(ftype, GlobalValue::ExternalLinkage, id.name.c_str(), context.module);
    return NULL;
}

ValueBase* NFunctionDeclaration::codeGen(CodeGenContext& context){
    context.locals()[id.name] = new FunctionValue(this);
}

ValueBase* NFunctionDeclaration::codeGen(CodeGenContext& context, const std::vector<llvm::Type*> &argTypes)
{
	FunctionType *ftype = FunctionType::get(typeOf(type), makeArrayRef(argTypes), false);
	Function *function = Function::Create(ftype, GlobalValue::InternalLinkage, id.name.c_str(), context.module);
	BasicBlock *bblock = BasicBlock::Create(getGlobalContext(), "entry", function, 0);
	Function *oldFunction = context.currentFunction;
	context.currentFunction = function;
	auto oldBlock = context.currentBlock();

	context.pushBlock(bblock);

	Function::arg_iterator argsValues = function->arg_begin();
    Value* argumentValue;
    
    VariableList::const_iterator it;
    std::vector<llvm::Type*>::const_iterator itType = argTypes.begin();
	for (it = arguments.begin(); it != arguments.end(); it++) {
        argumentValue = argsValues++;
        itType = itType ++;
        
        AllocaInst *alloc = new AllocaInst(*itType, (*it)->name.c_str(), context.currentBlock());
        ValueBase* value = new LongValue(alloc);
        context.locals()[(*it)->name] = value;
        
        new StoreInst(argumentValue, value->getValue(), false, context.currentBlock());
	}
	
	block.codeGen(context);
	
	
	while (context.currentBlock() != oldBlock)
		context.popBlock();
	context.currentFunction = oldFunction;
	return new FunctionValue(function);
}

ValueBase* NIfElseStatement::codeGen(CodeGenContext& context)
{
	ValueBase* test = condExpr->codeGen( context );
	
	BasicBlock *btrue = BasicBlock::Create(getGlobalContext(), "thenBlock", context.currentFunction);
	BasicBlock *bfalse = BasicBlock::Create(getGlobalContext(), "elseBlock", context.currentFunction);
	BasicBlock *bmerge = BasicBlock::Create(getGlobalContext(), "mergeStmt", context.currentFunction);    
	auto ret = llvm::BranchInst::Create(btrue,bfalse,test->getValue(), context.currentBlock());
	
	auto oldBlock = context.currentBlock();
	context.pushBlock(btrue);
	auto retThen = thenBlock->codeGen(context);
	if (retThen == NULL){
		llvm::BranchInst::Create(bmerge,context.currentBlock());
	}
	while (context.currentBlock() != oldBlock){
		context.popBlock();
	}
	oldBlock = context.currentBlock();
	context.pushBlock(bfalse);
	ValueBase * retElse = NULL;
	if (elseBlock != NULL)
	{
		retElse = elseBlock->codeGen(context);
	}
	if (retElse == NULL){
		llvm::BranchInst::Create(bmerge,context.currentBlock());
	}
	while (context.currentBlock() != oldBlock){
		context.popBlock();
	}
	context.pushBlock(bmerge);
	
	return NULL;
}
