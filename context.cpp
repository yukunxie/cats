
#include "codegen.h"

CodeGenBlock::CodeGenBlock(): 
	block(NULL), returnValue(NULL), currentFunction(NULL), parentBlock(NULL)
{
}
	

Value* CodeGenBlock::getVarValue(const std::string &varName){
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

void CodeGenBlock::setVarValue(const std::string &varName, Value *value){
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

///////////////////CodeGenContext//////////////////////////

CodeGenContext::CodeGenContext() {
	module = new Module("main", getGlobalContext());
}


Value* CodeGenContext::getVar(const std::string &varName){
	CodeGenBlock *pBlock = blocks.top();
	return pBlock->getVarValue(varName);
}

std::map<std::string, Value*>& CodeGenContext::locals() {
	return blocks.top()->locals;
}

Value* CodeGenContext::getVarValue(std::string varName)
{
	blocks.top()->getVarValue(varName);
}

void CodeGenContext::setVarValue(std::string varName, Value *value)
{
	blocks.top()->setVarValue(varName, value);
}

BasicBlock* CodeGenContext::currentBlock() {
	return blocks.top()->block;
}

void CodeGenContext::pushBlock(BasicBlock *block) {
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

void CodeGenContext::popBlock() {
	CodeGenBlock *top = blocks.top();
	blocks.pop();
	delete top;
}

void CodeGenContext::setCurrentReturnValue(Value *value) {
	blocks.top()->returnValue = value;
}

Value* CodeGenContext::getCurrentReturnValue() {
	return blocks.top()->returnValue;
}

