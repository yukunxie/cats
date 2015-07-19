all: parser

OBJS = parser.o  \
       codegen.o \
       main.o    \
       tokens.o  \
       corefn.o  \
	   native.o  \

LIB=-L/usr/local/Cellar/llvm/3.5.0_2/lib
INC=-I/usr/local/Cellar/llvm/3.5.0_2/include
LLVMCONFIG = llvm-config
CPPFLAGS = `$(LLVMCONFIG) --cppflags` -lstdc++  -std=c++11 -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS
LDFLAGS = `$(LLVMCONFIG) --ldflags` -lpthread -ll -lz -lncurses -rdynamic
LIBS = `$(LLVMCONFIG) --libs`

clean:
	$(RM) -rf parser.cpp parser.hpp parser tokens.cpp $(OBJS)

parser.cpp: parser.y
	bison -d -o $@ $^
	
parser.hpp: parser.cpp

tokens.cpp: tokens.l parser.hpp
	flex -o $@ $^ 

%.o: %.cpp
	g++ -c $(CPPFLAGS) -o $@ $< $(INC) $(LIB) -g


parser: $(OBJS)
	g++ -o $@ $(OBJS) $(LIBS) $(LDFLAGS) $(LIB) -g

test: parser example.txt
	cat example.txt | ./parser
