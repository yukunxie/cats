
// this the define a an abstract syntax tree node
//

#ifndef __AST_NODE_H__
#define __AST_NODE_H__

class ASTNode
{
public:
	virtual ~ASTNode(){}
	virtual void codeGen() = 0;
};

#endif
