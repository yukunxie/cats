
#include "classinfo.h"

ClassInfo::ClassInfo(const std::string& name, llvm::Type* llvmType):
	name(name), llvmType(llvmType)
{}

bool ClassInfo::isBoolType(){
	return name == "bool";
}

bool ClassInfo::isLongType(){
	return name == "long";
}

bool ClassInfo::isDoubleType(){
	return name == "double";
}

bool ClassInfo::isCharType(){
	return name == "char";
}

