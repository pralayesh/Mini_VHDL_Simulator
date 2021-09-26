%{
#include<stdio.h>
#include "final_parse.tab.h"
%}


%%

"library ieee;"		{return LIBRARY_IEEE;}
"use ieee.std_logic_1164.all;" {return USE_IEEE;}
"entity"		{return ENTITY;}
"is"			{return IS;}
"port"			{return PORT;}
"in"			{return IN;}
"out"			{return OUT;}
"std_logic"		{return STD_LOGIC;}
"architecture"		{return ARCHITECTURE;}
"of"			{return OF;}
"begin"			{return BEG;}
"signal"		{return SIGNAL;}	
"end"			{return END;}
"process"		{return PROCESS;}
"after"			{return AFTER;}
"ns"			{return NS;}

"and"			{return AND;}
"or"			{return OR;}
"xor"      		{return XOR;}
"not"			{return NOT;}

"<="			{return SIGNAL_ASSIGN;}
":="			{return SIGNAL_INIT;}

[a-zA-Z_][a-zA-Z0-9_]*	{yylval.str = yytext; return IDEN;}
[1-9]?[0-9]*		{yylval.num = atoi(yytext); return NUM;}
['][01][']              {yylval.str = yytext; return LOGIC_VAL;}
[ \t\n]			{;}

"(" |
")" |
"," |
"'" |
":" |
"+" | 
"-" |
"." |
";" { return yytext[0];}

. {}

%%

int yywrap()
{
	return 1;
}
