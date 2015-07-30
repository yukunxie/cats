
#include "classinfo.h"
#include "codegen.h"

extern IRBuilder<> builder;

ValueType binaryOpTypeCast(ValueType type1, ValueType type2){
	ValueType types[] = {ValueType::VT_CHAR, ValueType::VT_LONG, ValueType::VT_DOUBLE};
	int size = sizeof(types) / sizeof(ValueType::VT_CHAR);
	int t1(-1), t2(-1);
	for (int i = 0; i < size; ++i){
		if (types[i] == type1) t1 = i;
		if (types[i] == type2) t2 = i;
	}
	assert(t1 >= 0 and t2 >= 0);
	return types[t1 >= t2 ? t1 : t2];
}

ValueBase* createValue(ValueType type, llvm::Value *value){
	switch(type){
	case VT_CHAR:
		return new CharValue(value);
	case VT_LONG:
		return new LongValue(value);
	case VT_DOUBLE:
		return new DoubleValue(value);
	}
	std::cout <<"createValue: undefined type"<<std::endl;
	return NULL;
}

llvm::Type* ValueBase::getRealType(){
    return Type::getVoidTy(getGlobalContext());
}

llvm::Type* LongValue::getRealType(){
    return Type::getInt64Ty(getGlobalContext());
}

ValueBase* LongValue::castTo(ValueType targetType, CodeGenContext& context){
	switch(targetType){
	case ValueType::VT_LONG:{
		return this;
	}

	case ValueType::VT_DOUBLE:{
		AddrSpaceCastInst::CastOps ops = AddrSpaceCastInst::CastOps::SIToFP;
		Type* targetType = Type::getDoubleTy(getGlobalContext());
		Value* target = CastInst::Create(ops, this->getValue(), targetType,  "", context.currentBlock());
		return new DoubleValue(target);
	}
	default:
		break;
	}
	return NULL;
}

llvm::Type* DoubleValue::getRealType(){
    return Type::getDoubleTy(getGlobalContext());
}

ValueBase* DoubleValue::castTo(ValueType targetType, CodeGenContext& context){
	switch(targetType){
	case ValueType::VT_LONG:{
		AddrSpaceCastInst::CastOps ops = AddrSpaceCastInst::CastOps::FPToSI;
		Type* targetType = Type::getInt64Ty(getGlobalContext());
		Value* target = CastInst::Create(ops, this->getValue(), targetType,  "", context.currentBlock());
		return new LongValue(target);
	}
	case ValueType::VT_DOUBLE:{
		return this;
	}
	
	default:
		break;
	}
	return NULL;
}

ValueBase* CharValue::castTo(ValueType targetType, CodeGenContext& context){
	return NULL;
}

ValueBase* NullValue::castTo(ValueType targetType, CodeGenContext& context){
	return NULL;
}

ValueBase* FunctionValue::castTo(ValueType targetType, CodeGenContext& context){
	return NULL;
}

ValueBase* BoolValue::castTo(ValueType targetType, CodeGenContext& context){
	return NULL;
}


