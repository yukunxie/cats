
#include "node_integer.hpp"

NodeInteger::NodeInteger(long long value):m_value(value)
{}

void NodeInteger::codeGen()
{
	printf("%d", m_value);
}

NodeInteger::~NodeInteger()
{
}
