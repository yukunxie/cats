
#include "classinfo.h"
#include "codegen.h"

llvm::Type* ValueBase::getRealType(){
    return Type::getVoidTy(getGlobalContext());
}

llvm::Type* LongValue::getRealType(){
    return Type::getInt64Ty(getGlobalContext());
}


llvm::Type* DoubleValue::getRealType(){
    return Type::getDoubleTy(getGlobalContext());
}



