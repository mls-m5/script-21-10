#include "code/ast.h"
#include "context.h"

namespace llvmapi {

llvm::Type *generateStructDeclaration(Ast &ast, CodegenContext &context);
llvm::AllocaInst *generateStructInitializer(Ast &ast, CodegenContext &context);
llvm::Value *generateMemberAccessor(Ast &ast, CodegenContext &context);

} // namespace llvmapi