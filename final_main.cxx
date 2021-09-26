#include<stdlib.h>
#include<string.h>
#include<iostream>
#include<stdio.h>
#include<vector>
#include<iterator>
#include<algorithm>
#include "main.h"
#define U 100
#define X 200
#define MAXTIME 9999;

using namespace std;

extern int yydebug;
extern FILE* yyin;
int yyparse();

struct RHS{                                                                 
    char* operand1;
	char* operand2;                                            
    char op;                                                            
    int type;
};  
typedef struct signal_assignment{
	char* lhs;
	RHS* rhs;
	int delay;
}sa;
struct process{
	int state;
	int isCSA;
	vector<sa*> sa_list;
};
struct signal{
	char* sname;
	int val;
	vector<process*> triggered_processes;
};
struct transaction{
	char* sig_name;
	int dval;
	int timestamp;
	process* caller_process;
};

vector<process*> process_list;

vector<signal*> signal_list;

vector<transaction*> tran_list;

signal* s;
process* p;
sa* m;
RHS* exp;
int current_timestamp = 0;

void action_sig_decl(char* a)
{
	s = (signal*)malloc(sizeof(signal));
	signal_list.push_back(s);
	s->sname = (char*)malloc(strlen(a)*sizeof(char));
	strcpy(s->sname,a);
	s->val = U;
}

void action_sig_init(char* a, int count)
{
	char c = a[1];
	int i=signal_list.size()-1;
	while(count != 0)
	{
		signal_list[i]->val = c - '0';
		count--;
		i--;	
	}
}

void action_process(int x)
{
	p = (process*)malloc(sizeof(process));
	p->state = 1;
	p->isCSA = x;
	process_list.push_back(p);		
}
void getFirstWord(char* a,char* b)
{
        int i=0;
        while(a[i] != ' ' && a[i] != ';')
        {
                b[i] = a[i];
                ++i;
        }
        b[i] = '\0';
}
void action_sensitivity(char* a)
{
	char b[10];
	getFirstWord(a,b);
	vector<signal*>::iterator i;
	for(i=signal_list.begin(); i!=signal_list.end(); ++i)
	{
		if(strcmp((*i)->sname,b) == 0)
			((*i)->triggered_processes).push_back(p);
	}	
}
void action_sig_assign_lhs(char* a)
{
        m = (sa*)malloc(sizeof(sa));
        m->lhs = (char*)malloc(strlen(a)*sizeof(char));
        strcpy(m->lhs, a);
        m->delay = 0;
        (p->sa_list).push_back(m);
}
void add_delay(int d)
{
        m->delay = d;
}

void add_oax_firstop(char* b)
{
	char a[10];
	getFirstWord(b,a);
	exp = (RHS*)malloc(sizeof(RHS));
	exp->operand1 = (char*)malloc(strlen(a)*sizeof(char));
	strcpy(exp->operand1, a);
	exp->type = 2;
	m->rhs = exp;
}
void add_oax_secondop(char* a)
{
	exp->operand2 = (char*)malloc(strlen(a)*sizeof(char));
	strcpy(exp->operand2, a);
}
void add_oax_op(char a)
{
	exp->op = a;
}
void create_not_exp(char* a)
{
	exp = (RHS*)malloc(sizeof(RHS));
	exp->operand1 = (char*)malloc(strlen(a)*sizeof(char));
	strcpy(exp->operand1, a);
	exp->type = 3; 
	m->rhs = exp;
}
void create_iden_exp(char* b)
{
	char a[10];
	getFirstWord(b,a);
	RHS* exp = (RHS*)malloc(sizeof(RHS));
	exp->operand1 = (char*)malloc(strlen(a)*sizeof(char));
	strcpy(exp->operand1,a);
	exp->type = 4;
	m->rhs = exp;
	
}
void create_logic_exp(char* a)
{
	RHS* exp = (RHS*)malloc(sizeof(RHS));
	exp->type = a[1] - '0';
	m->rhs = exp;
}


void display_signal(signal* s)
{
	cout <<"signal: "<< s->sname;
	if(s->val == U)
		cout<<" Val: U "<<endl;
	else
		cout<<" val: "<<s->val<<endl;
}
void display_rhs(RHS* r)
{
	cout<<"rhs: ";
	if(r->type == 2)
		cout<<r->operand1<<" "<<r->op<<" "<<r->operand2<<endl;
	else if(r->type == 3 || r->type == 4)
		cout<<r->operand1<<" "<<r->type<<endl;
	else
		cout<<r->type<<endl;
}
void display_sa(sa* a)
{
	cout<<"sa-> lhs:"<<a->lhs<<" ";
	display_rhs(a->rhs);
	cout<<"delay: "<<a->delay<<endl; 	
}
void display_process(process* p)
{
	cout<<"process: "<<p->isCSA<<" "<<p->state<<endl;	
	for(int i=0;i<p->sa_list.size();++i)
		display_sa(p->sa_list[i]);
}

int evaluate_oax(char* fop, char op, char* sop)
{
	int arr[2],j=0;
	for(int i=0;i<signal_list.size();++i)
	{
		if(strcmp(fop,signal_list[i]->sname) == 0 || strcmp(sop, signal_list[i]->sname) == 0)
			arr[j++] = signal_list[i]->val;
	}
	if(arr[0] == U && arr[1] == U)
		return U;
	if (op == '|')
	{
		if(arr[0] == U)
			if(arr[1] == 1)
				return 1;
			else
				return U;
		if(arr[1] == U)
			if(arr[0] == 1)
				return 1;
			else
				return U;
		return arr[0] | arr[1];
	}
	if (op == '&')
	{
		if(arr[0] == U)
                        if(arr[1] == 0)
                                return 0;
                        else
                                return U;
                if(arr[1] == U)
                        if(arr[0] == 0)
                                return 1;
                        else
                                return U;
		return arr[0] & arr[1];
	}
	if (op == '^')
	{
		if(arr[0] == U || arr[1] == U)
			return U;
		return arr[0] ^ arr[1];
	}
}

int evaluate_not_iden(char* fop, int type)
{
	for(int i=0; i<signal_list.size();++i)
		if(strcmp(fop, signal_list[i]->sname) == 0)
		{
			if(type == 3)
			{
				if(signal_list[i]->val == U)
					return U;
				else if(signal_list[i]->val == 0)
					return 1;
				else
					return 0;
			}
			else
				return signal_list[i]->val;
		}
}
int evaluate_rhs(RHS* temp)
{
	if(temp->type == 2)
		return evaluate_oax(temp->operand1,temp->op,temp->operand2);
	else if(temp->type == 3 || temp->type == 4)
		return evaluate_not_iden(temp->operand1, temp->type);
	else
		return temp->type;
}
void schedule_transaction(char* sig, int dval, int timestamp, process* cp)
{
	transaction* t = (transaction*)malloc(sizeof(transaction));
	t->sig_name = (char*)malloc(strlen(sig)*sizeof(char));
	strcpy(t->sig_name,sig);
	t->dval = dval;
	t->timestamp = timestamp;
	t->caller_process = cp;

	vector<transaction*>::iterator it;
	
	if(tran_list.size() == 0)
	{
		tran_list.push_back(t);

	}
	else
	{
		for(it=tran_list.begin() ;it!=tran_list.end(); ++it)
		{
			if(strcmp((*it)->sig_name, t->sig_name) == 0 && (*it)->caller_process == t->caller_process)
			{
				//tran_list.erase(it);
				(*it)->timestamp = MAXTIME;
			}
		}
		tran_list.push_back(t);
	}	
}

int execute_transaction(transaction* temp)
{
	int n,f = 0;
	for(int i=0; i<signal_list.size();++i)
	{
		if(strcmp(signal_list[i]->sname, temp->sig_name)==0)
		{
			if(signal_list[i]->val != temp->dval)
			{
				f = 1;
				n = (signal_list[i]->triggered_processes).size();
				if (n != 0)
				{
					for(int j=0; j<n;++j)
						(signal_list[i]->triggered_processes)[j]->state = 1;
				}	
			}
			signal_list[i]->val = temp->dval;
		}
	}
	return f;	
}
int getNextTimestamp()
{
	int min = MAXTIME;
	for(int i=0; i<tran_list.size(); ++i)
		if(min > tran_list[i]->timestamp)
			min = tran_list[i]->timestamp;
	return min;
}

void simulate(int simulation_time)
{
	int iter = 0,flag = 0;
	vector<transaction*>::iterator it;
	
	cout<<"Timestamp: "<<current_timestamp<<"  Iteration: "<<iter<<endl;
        for(int i=0;i<signal_list.size();++i)
        	display_signal(signal_list[i]);

	while(current_timestamp <= simulation_time)
	{
		for(int i=0; i<process_list.size();++i)
		{
			if(process_list[i]->state == 1)
			{
				for(int j=0;j<(process_list[i])->sa_list.size();++j)
				{
					
					sa* temp = (process_list[i]->sa_list)[j];
					int dval = evaluate_rhs(temp->rhs);
					int timestamp = current_timestamp + temp->delay;
					schedule_transaction(temp->lhs,dval,timestamp,process_list[i]);
				}
				process_list[i]->state = 0;
			}
		}

		for(it=tran_list.begin(); it!=tran_list.end();++it)
		{
			if((*it)->timestamp == current_timestamp)
			{
				flag = flag + execute_transaction(*it);
				(*it)->timestamp = MAXTIME;
			
			}			
		}
		

		if(flag != 0)
		{
			++iter;
			cout<<"Timestamp: "<<current_timestamp<<"  Iteration: "<<iter<<endl;
                        for(int i=0;i<signal_list.size();++i)
                                display_signal(signal_list[i]);
                        flag = 0;

		}
		else
		{
			current_timestamp = getNextTimestamp();
			iter = -1;
		}	
	}		
}
int main(int argc, char* argv[])
{
	yyin = fopen(argv[1], "r");

//	yydebug = 1;
	yyparse();

	fclose(yyin);
	
	/*for(int i=0;i<signal_list.size();++i)
		display_signal(signal_list[i]);
	for(int i=0;i<process_list.size();++i)
                display_process(process_list[i]);
	*/

	int simul_time = atoi(argv[2]);
	cout<<"Simulation Time: "<<simul_time<<" ns"<<endl;
	simulate(simul_time);

	return 0;
}

