#ifndef _MAIN_H_
#define _MAIN_H_
	void action_sig_decl(char*);
	void action_sig_init(char*, int);
	void action_process(int);
	void action_sig_assign_lhs(char*);
	void add_oax_firstop(char*);
	void add_oax_secondop(char*);
	void add_oax_op(char);
	void create_not_exp(char*);
	void create_iden_exp(char*);
	void create_logic_exp(char*);
	void add_delay(int);
	void action_sensitivity(char*);
#endif
