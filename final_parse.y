%{
#include<stdio.h>
#include<string.h>
#include "main.h"
int yylex();
void yyerror(const char* s);
#define YYDEBUG 1
int countId = 0, isProcess = 1;
char first[5];
char op;
%}

%union{
	int   num;
	char* str;
}

%token LIBRARY_IEEE
%token USE_IEEE
%token ENTITY
%token ARCHITECTURE
%token BEG
%token END
%token PORT
%token PROCESS
%token STD_LOGIC
%token SIGNAL
%token IS 
%token OF
%token IN
%token OUT
%token NUM
%token IDEN
%token LOGIC_VAL

%token SIGNAL_ASSIGN
%token SIGNAL_INIT
%token NOT
%token XOR
%token AND
%token OR
%token AFTER
%token NS

%type <num> NUM
%type <str> IDEN
%type <str> LOGIC_VAL

%%

vhdl_code : library entity_decl architecture_decl
;

library : LIBRARY_IEEE USE_IEEE
;

entity_decl : ENTITY IDEN IS port_decl END IDEN ';'
;

port_decl :
|PORT '(' portlist_decl ')' ';'
;


portlist_decl : id_list ':' in_out_decl
| portlist_decl ';' id_list ':' in_out_decl
;

in_out_decl : IN STD_LOGIC 
| OUT STD_LOGIC
| IN STD_LOGIC SIGNAL_INIT LOGIC_VAL
| OUT STD_LOGIC SIGNAL_INIT LOGIC_VAL
;

architecture_decl : ARCHITECTURE IDEN OF IDEN IS arch_decl_region BEG arch_body END IDEN ';'
;

arch_decl_region : signal_decl
| signal_decl arch_decl_region
;

signal_decl : SIGNAL id_list ':' STD_LOGIC ';'
{
	countId = 0;
}
| SIGNAL id_list ':' STD_LOGIC SIGNAL_INIT logic_init_val ';'
;

logic_init_val : LOGIC_VAL
{
	action_sig_init((char*)$1, countId); 
	countId = 0;
}
;

id_list : IDEN
{
	action_sig_decl($1);
	++countId;
}
| id_list ',' IDEN
{
        action_sig_decl($3);
	++countId;
}
;

arch_body : concurrent_assignment
| process_statement
| arch_body concurrent_assignment
| arch_body process_statement
;

id : IDEN
{
	isProcess = 0;
	action_process(1);
        action_sig_assign_lhs((char*)$1);
}
;

concurrent_assignment : id SIGNAL_ASSIGN expression ';'
|id SIGNAL_ASSIGN expression AFTER expression NS ';'
;

logic_exp_val : LOGIC_VAL
{
	create_logic_exp($1);
}
;

expression : IDEN {add_oax_firstop((char*)$1); 
	if(!isProcess)
		{action_sensitivity($1);}
} 
AND {add_oax_op('&'); }
IDEN {add_oax_secondop($5);
	if(!isProcess)
		 {action_sensitivity($5);}
	}
|IDEN {add_oax_firstop($1);
	if(!isProcess)
                 {action_sensitivity($1);}
        }
OR {add_oax_op('|');}
IDEN {add_oax_secondop($5);
	if(!isProcess)
                 {action_sensitivity($5);}
}
| IDEN {add_oax_firstop((char*)$1);
	if(!isProcess)
                 {action_sensitivity($1);}
}
XOR {add_oax_op('^');}
IDEN {add_oax_secondop($5);
	if(!isProcess)
                 {action_sensitivity($5);}
}
| NOT'(' IDEN {create_not_exp($3);
	if(!isProcess)
                 {action_sensitivity($3);}
} ')'
| IDEN {create_iden_exp($1);
	if(!isProcess)
                 {action_sensitivity($1);}
}
| logic_exp_val
| NUM
{
	add_delay($1);
} 
;

process_statement : PROCESS { action_process(0); isProcess = 1;} sensitivity_list BEG process_body END PROCESS ';'
;

sensitivity_list : '(' sens_id_list ')'
| 
;

sens_id_list : IDEN 
{
	action_sensitivity((char*)$1);
}
| sens_id_list ',' IDEN
{
	action_sensitivity((char*)$3);
}
;

process_body : statement
| process_body statement
;

statement : IDEN { action_sig_assign_lhs($1); } SIGNAL_ASSIGN expression delay
;

delay :';' 
| AFTER NUM { add_delay($2); } NS ';'
;


%%


void yyerror (const char* s)
{
    printf("%s\n", s);
}
