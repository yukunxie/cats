#include "ast/ast_node.hpp"
#include "ast/node_integer.hpp"
#include <stdio.h>

int main()
{
	ASTNode * node = new NodeInteger(12323);
	node->codeGen();
	printf("\n");
	printf("fffffffffffff\n");
	return 0;
}
