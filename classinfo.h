
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

enum ValueType{
	VT_NULL,
	VT_LONG,
	VT_BOOL,
	VT_CHAR,
	VT_DOUBLE,
};

class ClassInfo{
public:
	std::string name;
	llvm::Type * llvmType;

public:
	ClassInfo(const std::string& name, llvm::Type* llvmType);

public:
	bool isBoolType();
	bool isLongType();
	bool isDoubleType();
	bool isCharType();
};

class ValueBase{
private:
	ValueType type;
	llvm::Value * value;

public:
	ValueBase(ValueType type, llvm::Value * value):
		type(type), value(value)
	{}

	ValueType getType(){
		return this->type;
	}

	llvm::Value* getValue(){
		return this->value;
	}
	
	void setValue(llvm::Value* value){
		this->value = value;
	}

	virtual ~ValueBase(){
	}
};

class LongValue: public ValueBase{
public:
	LongValue(llvm::Value* value):
		ValueBase(ValueType::VT_LONG, value)
	{}
};

class DoubleValue: public ValueBase{
public:
	DoubleValue(llvm::Value* value):
		ValueBase(ValueType::VT_DOUBLE, value)
	{}
};

class CharValue: public ValueBase{
public:
	CharValue(llvm::Value* value):
		ValueBase(ValueType::VT_CHAR, value)
	{}
};

class BoolValue: public ValueBase{
public:
	BoolValue(llvm::Value* value):
		ValueBase(ValueType::VT_BOOL, value)
	{}
};

class NullValue: public ValueBase{
public:
	NullValue(llvm::Value* value):
		ValueBase(ValueType::VT_NULL, value)
	{}
};

#endif
