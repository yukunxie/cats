
objects = main.o node_integer.o
		  
cats: $(objects)
	g++ -o cats $(objects)
	
main.o: main.cpp ast/ast_node.hpp ast/node_integer.hpp
	g++ -c main.cpp
	
node_integer.o: ast/node_integer.cpp ast/node_integer.hpp ast/ast_node.hpp
	g++ -c ast/node_integer.cpp

clean:
	rm cats main.o node_integer.o
