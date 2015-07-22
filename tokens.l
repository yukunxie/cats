%{
#include <string>
#include <stdint.h>
#include "node.h"

#define SAVE_TOKEN  yylval.string = new std::string(yytext, yyleng)
#define TOKEN(t)    (yylval.token = t)

extern void yyerror(const char *msg);

typedef struct YYLTYPE  
{  
	int first_line;  
	int first_column;  
	int last_line;  
	int last_column;  
} YYLTYPE;

#define YYLTYPE_IS_DECLARED 1

int yycolumn = 1;

#include "parser.hpp"

void saveLocation()
{
	yylloc.first_line = yylloc.last_line;
	yylloc.last_line = yylineno;
	yylloc.first_column = yycolumn;
	yylloc.last_column = yycolumn + yyleng - 1;
	yycolumn += yyleng;
	//printf("%d, %d\n", yylineno, yycolumn);
}
#define YY_USER_ACTION saveLocation();

%}

%option noyywrap

%%

#.*\n							{yylineno += 1; yycolumn = 0;};
[\n]							{yylineno += 1; yycolumn = 0;};
[ \t]					        ;
"extern"                        return TOKEN(TEXTERN);
"return"				        return TOKEN(TRETURN);
"for"							return TOKEN(TFOR);
"while"							return TOKEN(TWHILE);
"if"							return TOKEN(TIF);
"else"							return TOKEN(TELSE);
[a-zA-Z_][a-zA-Z0-9_]*  SAVE_TOKEN; return TIDENTIFIER;
[0-9]+\.[0-9]* 			    SAVE_TOKEN; return TDOUBLE;
[0-9]+					        SAVE_TOKEN; return TINTEGER;

"="						          return TOKEN(TEQUAL);
"=="				          	return TOKEN(TCEQ);
"!="			          		return TOKEN(TCNE);
"<"				          		return TOKEN(TCLT);
"<="	          				return TOKEN(TCLE);
">"				          		return TOKEN(TCGT);
">="					          return TOKEN(TCGE);

"("	          					return '(';
")"					          	return ')';
"{"         						return TOKEN(TLBRACE);
"}"					          	return TOKEN(TRBRACE);

"."         						return TOKEN(TDOT);
","				          		return TOKEN(TCOMMA);

"+"				          		return TOKEN(TPLUS);
"-"		          				return TOKEN(TMINUS);
"*"		          				return TOKEN(TMUL);
"/"				          		return TOKEN(TDIV);

.                       printf("Unknown token!\n"); yyterminate();

%%
