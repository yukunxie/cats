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

using namespace llvm;

class NBlock;

class CodeGenBlock {
public:
    BasicBlock *block;
    Value *returnValue;
    Function * currentFunction;
    CodeGenBlock * parentBlock;
    std::map<std::string, Value*> locals;
    
public:
    CodeGenBlock(): block(NULL), returnValue(NULL), currentFunction(NULL), parentBlock(NULL)
    {
    }
    
public:
    Value * getVarValue(const std::string &varName){
        CodeGenBlock *pBlock = this;
        while(pBlock != NULL && pBlock->currentFunction == this->currentFunction){
            std::map<std::string, Value *>::iterator it = pBlock->locals.find(varName);
            if (it != pBlock->locals.end()){
                return it->second;
            }
            pBlock = pBlock->parentBlock;
        }
		printf("undefined variable:%s\n", varName.c_str());
        return NULL;
    }
    
    void setVarValue(const std::string &varName, Value *value){
        CodeGenBlock *pBlock = this;
        while(pBlock != NULL && pBlock->currentFunction == this->currentFunction){
            std::map<std::string, Value *>::iterator it = pBlock->locals.find(varName);
            if (it != pBlock->locals.end()){
                it->second = value;
				break;
            }
            pBlock = pBlock->parentBlock;
        }
        pBlock->locals[varName] = value;
    }

};

class CodeGenContext {
    std::stack<CodeGenBlock *> blocks;
    
public:
    Function *mainFunction;
    Function *currentFunction;
    
public:
    Module *module;
    CodeGenContext() {
        module = new Module("main", getGlobalContext());
    }
    
    void generateCode(NBlock& root);
    GenericValue runCode();

	Value * getVar(const std::string &varName){
		CodeGenBlock *pBlock = blocks.top();
		return pBlock->getVarValue(varName);
	}
    
    std::map<std::string, Value*>& locals() {
        return blocks.top()->locals;
    }
    
    Value * getVarValue(std::string varName)
    {
        blocks.top()->getVarValue(varName);
    }
    
    void setVarValue(std::string varName, Value *value)
    {
        blocks.top()->setVarValue(varName, value);
    }
    
    BasicBlock *currentBlock() {
        return blocks.top()->block;
    }
    void pushBlock(BasicBlock *block) {
        CodeGenBlock * parent = NULL;
        if (blocks.size() > 0){
            parent = blocks.top();
        }
        CodeGenBlock * cgBlock = new CodeGenBlock();
        blocks.push(cgBlock);
        cgBlock->returnValue = NULL;
        cgBlock->block = block;
        cgBlock->parentBlock = parent;
        cgBlock->currentFunction = currentFunction;
    }
    void popBlock() {
        CodeGenBlock *top = blocks.top();
        blocks.pop();
        delete top;
    }
    void setCurrentReturnValue(Value *value) {
        blocks.top()->returnValue = value;
    }
    Value* getCurrentReturnValue() {
        return blocks.top()->returnValue;
    }
};
