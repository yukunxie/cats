#include <stack>
#include <typeinfo>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/PassManager.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/CallingConv.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/ValueSymbolTable.h>

#include "classinfo.h"

using namespace llvm;

class NBlock;

class CodeGenBlock {
public:
	BasicBlock *block;
	ValueBase *returnValue;
	Function * currentFunction;
	CodeGenBlock * parentBlock;
	std::map<std::string, ValueBase*> locals;
	
public:
	CodeGenBlock();
	ValueBase* getVarValue(const std::string &varName);
	void setVarValue(const std::string &varName, ValueBase *value);
};

class CodeGenContext {
private:
	std::stack<CodeGenBlock *> blocks;
	
public:
	Function *mainFunction;
	Function *currentFunction;
	
public:
	Module *module;
	CodeGenContext(); 	
	void generateCode(NBlock& root);
	GenericValue runCode();

	BasicBlock *currentBlock() ;
	ValueBase * getVar(const std::string &varName);
	std::map<std::string, ValueBase*>& locals() ;
	ValueBase* getVarValue(std::string varName);
	ValueBase* getCurrentReturnValue();

	void setVarValue(std::string varName, ValueBase *value);
	void pushBlock(BasicBlock *block);
	void popBlock();
	void setCurrentReturnValue(ValueBase *value);
};
