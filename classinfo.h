
#ifndef __CLASS_INFO_H_
#define __CLASS_INFO_H_

#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <map>
#include <vector>
#include <string>
#include <iostream>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Type.h>

using namespace llvm;

class NFunctionDeclaration;
class CodeGenContext;

enum ValueType{
	VT_NULL,
	VT_LONG,
	VT_BOOL,
	VT_CHAR,
	VT_DOUBLE,
	VT_FUNCTION,
};

ValueType binaryOpTypeCast(ValueType type1, ValueType type2);


class ValueBase{
private:
	ValueType type;
	llvm::Value * value;

public:
	ValueBase(ValueType type, llvm::Value * value):
		type(type), value(value)
	{}
    
    virtual llvm::Type* getRealType();
    
	ValueType getType(){
		return this->type;
	}

	llvm::Value* getValue(){
		return this->value;
	}
	
	void setValue(llvm::Value* value){
		this->value = value;
	}

	virtual ValueBase* castTo(ValueType targetType, CodeGenContext& context) = 0;

	virtual ~ValueBase(){
	}
};

ValueBase* createValue(ValueType type, llvm::Value *value);

class LongValue: public ValueBase{
public:
	LongValue(llvm::Value* value):
		ValueBase(ValueType::VT_LONG, value)
	{}
	
	virtual ValueBase* castTo(ValueType targetType, CodeGenContext& context);
    llvm::Type* getRealType();
};

class DoubleValue: public ValueBase{
public:
	DoubleValue(llvm::Value* value):
		ValueBase(ValueType::VT_DOUBLE, value)
	{}
    
	virtual ValueBase* castTo(ValueType targetType, CodeGenContext& context);
    virtual llvm::Type* getRealType();
};

class CharValue: public ValueBase{
public:
	CharValue(llvm::Value* value):
		ValueBase(ValueType::VT_CHAR, value)
	{}
	virtual ValueBase* castTo(ValueType targetType, CodeGenContext& context);
};

class BoolValue: public ValueBase{
public:
	BoolValue(llvm::Value* value):
		ValueBase(ValueType::VT_BOOL, value)
	{}
	virtual ValueBase* castTo(ValueType targetType, CodeGenContext& context);
};

class NullValue: public ValueBase{
public:
	NullValue(llvm::Value* value):
		ValueBase(ValueType::VT_NULL, value)
	{}
	virtual ValueBase* castTo(ValueType targetType, CodeGenContext& context);
};

class FunctionValue: public ValueBase{
private:
    NFunctionDeclaration * functionInfo;
public:
	FunctionValue(llvm::Value* value):
		ValueBase(ValueType::VT_FUNCTION, value)
	{}
    
    FunctionValue(NFunctionDeclaration* function):
    ValueBase(ValueType::VT_FUNCTION, NULL), functionInfo(function)
    {}
    NFunctionDeclaration* getFunctionInfo(){return functionInfo;}
    
	virtual ValueBase* castTo(ValueType targetType, CodeGenContext& context);
    
};
#endif
