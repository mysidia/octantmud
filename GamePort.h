/*
 *  OMud - Game Port Structure
 *      Copyright (C) 2003-2004 Mysidia, All Rights Reserved.
 *
 *      Unless explicitly acquired and licensed from Licensor under the Technical Pursuit
 *      License ("TPL") Version 1.0 or greater, the contents of this file are subject to the
 *      Reciprocal Public License ("RPL") Version 1.1.
 *
 *      You may not copy or use this file in either source code or executable form,
 *      except in compliance with the terms and conditions of the RPL.
 *
 *      All software distributed under the Licenses is provided strictly on an "AS IS" basis,
 *      WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, AND THE AUTHORS AND
 *      CONTRIBUTORS HEREBY DISCLAIM ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION,
 *      ANY WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT,
 *      OR NON-INFRINGEMENT. 
 *
 *      See the Licenses for specific language governing rights and
 *      limitations under the Licenses.
 */


#ifndef __GamePort__h__
#define __GamePort__h__

#include "mud.h"
#include "Descriptor.h"
class Game;

typedef void State_Handler_Func (Descriptor*, Desc_State&, char*, int, int&);
typedef void State_Prompter_Func (Descriptor*, int&);


#define CON_HANDLER(x) \
	void x(Descriptor* d, Desc_State& instate, char* inbuf, int len, int &handler_pass)
#define CON_PROMPTER(x) \
	void x(Descriptor* d, int &prompter_pass)

struct Port_Dispatch_t
{
	Desc_State state;
	void (* func)(Descriptor* d, Desc_State& instate, char* buf, int len, int &handler_pass);
	const char* name;
	int pri;
};

struct Port_Prompt_t
{
	Desc_State state;
	void (* func)(Descriptor*, int& prompter_pass);
};

CON_HANDLER(con_enter_name);
CON_HANDLER(con_confirm_name);
CON_HANDLER(con_get_passwd);
CON_HANDLER(con_choose_email);
CON_HANDLER(con_email_again);
CON_HANDLER(con_prove_humanity);
CON_HANDLER(con_do_nothing);
CON_HANDLER(con_make_account);
CON_HANDLER(con_acct_menu);
CON_HANDLER(con_nc_name);
CON_HANDLER(con_nc_name_confirm);
CON_HANDLER(con_nc_make);
CON_HANDLER(con_char_sheet);
CON_HANDLER(con_choose_race);
CON_HANDLER(con_choose_gender);
CON_HANDLER(con_choose_alignment);
CON_HANDLER(con_motd);
CON_HANDLER(con_playing);

CON_PROMPTER(put_acct_menu);
CON_PROMPTER(put_get_passwd);
CON_PROMPTER(put_choose_email);
CON_PROMPTER(put_email_again);
CON_PROMPTER(put_prove_humanity);
CON_PROMPTER(put_make_account);
CON_PROMPTER(put_nc_name);
CON_PROMPTER(put_nc_name_confirm);
CON_PROMPTER(put_nc_make);
CON_PROMPTER(put_char_sheet);
CON_PROMPTER(put_choose_race);
CON_PROMPTER(put_choose_gender);
CON_PROMPTER(put_choose_alignment);
CON_PROMPTER(put_motd);
CON_PROMPTER(put_playing);

class GamePort
{
	friend class Game;
	friend void info_who_command( class Object * ch, class GamePort * gp );

	private:
		const static Port_Dispatch_t state_handlers[];
		const static Port_Prompt_t state_prompters[];

		bool run_prompter(Descriptor* d);
		bool run_handler(Descriptor* d, int pri, char* buf, int len);

	public:
		void set_poller(class GamePoll* p);
		void set_portnum(int);
		void set_host(const char*);
		bool open(int backlog = 5);
		bool close();
		void print_err();
		char* get_err();
		int  get_fd() { return fd; }
		int  get_port() { return bind_port; }
		const char* get_host() { return bind_host; }
		void new_connection(int fd, const char* addr);
		list<Descriptor*>::iterator dead_socket(class Descriptor*);
		int  connection_count() { return conn_count; }
		int process_in_q(Descriptor*, TextQueue*, int pri);
		int process_out_q(Descriptor*, TextQueue*, int pri);
		void line_in(Descriptor*, char*, int len, int pri);
		void new_connection_message(Descriptor*);
		void write_prompt(Descriptor*);

		Game* get_game_ptr() { return for_game; }

		GamePort ( Game* g) : bind_host(0), bind_port(0), poller(0), fd(-1),
				conn_count(0), for_game(g) { }

		~GamePort( ) {
			delete bind_host;
		}
		GamePort (GamePort &b) : poller(0) {
			copy_alloc_str(bind_host, b.bind_host);
			bind_port = b.bind_port;
			fd = b.fd;
		}

	private:
		friend class GamePoll;
		friend void send_needed_prompts(Game* g);
			

                list <Descriptor *> connections;
		class GamePoll *poller;
		char errmsg[512];
		char *bind_host;
		int  bind_port;
		int conn_count;
		int fd;
		Game* for_game;
};

#ifdef __GamePort_cc__
const Port_Dispatch_t GamePort::state_handlers[] = {
	{ CON_ENTER_NAME, con_enter_name, "Enter name" },
	{ CON_CONFIRM_NAME, con_confirm_name, "Confirm name" },
	{ CON_GET_PASSWD, con_get_passwd, "Enter password" },
	{ CON_CHOOSE_EMAIL, con_choose_email, "Choose email" },
	{ CON_EMAIL_AGAIN, con_email_again, "Re-Enter email" },
	{ CON_PROVE_HUMANITY, con_prove_humanity, "Prove Humanity" },
	{ CON_MAKE_ACCOUNT, con_make_account, "Making Account"},
	{ CON_ACCT_MENU, con_acct_menu, "Main Menu"},
	{ CON_NC_NAME, con_nc_name, "New character: name" },
	{ CON_NC_NAME_CONFIRM, con_nc_name_confirm, "New character: confirm" },
	{ CON_NC_MAKE, con_nc_make, "New character: making" },
	{ CON_CHAR_SHEET, con_char_sheet, "New character: building score sheet" },
	{ CON_CHOOSE_RACE, con_choose_race, "New character: choose race" },
	{ CON_CHOOSE_GENDER, con_choose_gender, "New character: choose gender" },
	{ CON_CHOOSE_ALIGNMENT, con_choose_alignment,
		"New character: choose alignment" },
	{ CON_MOTD, con_motd, "Showing motd" },
	{ CON_PLAYING, con_playing, "Playing" },
	{ (Desc_State)0, 0 }
};

const Port_Prompt_t GamePort::state_prompters[] = {
	{ CON_ACCT_MENU, put_acct_menu },
	{ CON_GET_PASSWD, put_get_passwd },
	{ CON_CHOOSE_EMAIL, put_choose_email },
	{ CON_EMAIL_AGAIN, put_email_again },
	{ CON_PROVE_HUMANITY, put_prove_humanity },
	{ CON_MAKE_ACCOUNT, put_make_account },
	{ CON_NC_NAME, put_nc_name },
	{ CON_NC_NAME_CONFIRM, put_nc_name_confirm },
	{ CON_NC_MAKE, put_nc_make },
	{ CON_CHAR_SHEET, put_char_sheet },
	{ CON_CHOOSE_RACE, put_choose_race },
	{ CON_CHOOSE_GENDER, put_choose_gender },
	{ CON_CHOOSE_ALIGNMENT, put_choose_alignment },
	{ CON_MOTD, put_motd },
	{ CON_PLAYING, put_playing },
	{ (Desc_State)0, NULL }
};

bool GamePort::run_prompter(Descriptor* d) 
{
	int ret = 0, found = 0, prompter_pass, count;
	Desc_State n = d->get_state();
	Desc_State no = n;

	for(count = 0; state_prompters[count].func; count++)
	{
		prompter_pass = 0;

		if (state_prompters[count].state == n)
		{
			(* state_prompters[count].func)(d, prompter_pass);
			if ( ! prompter_pass )
				ret = 1;

				if (n != no) {
				d->set_state(no = n);
			}
			if ( ret ) {
				return ret;
			}
		}
	}
	return ret || found;
}

bool GamePort::run_handler(Descriptor* d, int pri, char* buf, int len)
{
	Desc_State n = d->get_state(), no = n;
	int ret = 0, count, found = 0, handler_pass;
		
	 for(count = 0; this->state_handlers[count].func; count++) 
	 {
		handler_pass = 0;
		 
		if (this->state_handlers[count].state == n &&
		    pri == state_handlers[count].pri)
	        {
			(* GamePort::state_handlers[count].func)(d,n,buf,len,handler_pass);

			if ( !handler_pass )
                                ret = 1;

			found = 1;

			if (n != no) {
				d->prev_state = no;
				d->set_state(no = n);
			}

			run_prompter(d);	

			if ( ret ) {
				return ret;
			}
		}
	 }
		
	return (ret || found);
}
#endif
#endif
