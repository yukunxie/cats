
#ifndef __NODE_INTEGER_HPP__
#define __NODE_INTEGER_HPP__

#include "ast_node.hpp"
#include <stdio.h>

class NodeInteger : public ASTNode
{
private:
	long long m_value;
public:
	NodeInteger(long long value);
	~NodeInteger();
	virtual void codeGen();
};

#endif
