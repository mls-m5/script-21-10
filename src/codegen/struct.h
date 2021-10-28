#include "code/ast.h"
#include "context.h"

llvm::Type *generateStructDeclaration(Ast &ast, CodegenContext &context);
llvm::AllocaInst *generateStructInitializer(Ast &ast, CodegenContext &context);
llvm::Value *generateMemberAccessor(Ast &ast, CodegenContext &context);
