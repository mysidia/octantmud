/*
 *  OMud - Descriptor Structure
 *    EXHIBIT A
 *
 *    The Notice below must appear in each file of the Source Code of any copy You distribute of
 *    the Licensed Software or any Extensions thereto, except as may be modified as allowed
 *    under the terms of Section 7.1
 *
 *      Copyright (C) 2003-2004 ***REMOVED***, All Rights Reserved.
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

#ifndef __Descriptor_h__
#define __Descriptor_h__
#include "mud.h"
#include "TextQueue.h"
#include "GameString.h"
#include "Account.h"

#define MAX_OUTPUT_LEN 8192
#define ACCT_NAME_LEN 25
#define NAME_LEN 25

class GamePort;

struct Creation_Buffer {
	char name[NAME_LEN];
	char email[EMAIL_LEN];
	char passwd[256];
	
};

enum Desc_State
{
	CON_ENTER_NAME,
	CON_CONFIRM_NAME,
	CON_GET_PASSWD,
	CON_ACCT_MENU,
	CON_CHOOSE_EMAIL,
	CON_EMAIL_AGAIN,
	CON_PROVE_HUMANITY,
	CON_MAKE_ACCOUNT,
	CON_NC_NAME,
	CON_NC_NAME_CONFIRM,
	CON_NC_MAKE,
	CON_CHAR_SHEET,
	CON_CHOOSE_RACE,
	CON_CHOOSE_GENDER,
	CON_CHOOSE_ALIGNMENT,

	CON_MOTD,
	CON_PLAYING,

	CON_DISCONNECT
};

const Desc_State CON_START_PT = CON_ENTER_NAME;

class Descriptor
{
	public:
		friend class Character;
		friend class Object;
			
		Creation_Buffer *new_account_buffer;
		Creation_Buffer *new_char_buffer;
		Desc_State prev_state;
		char human_code[10];
		char term_name[25];
		bool need_prompt;
		
		Descriptor(int in_fd, const char *addr, GamePort* pt)
			: fd_num(in_fd), port_ptr(pt), sock_is_dead(0),
			sock_is_writable(0), dsc_state(CON_START_PT),
			login_account(0)
		{
			term_name[0] = '\0';
			sock_is_mxp = false;
			sock_is_ansi = false;
			sock_is_telnet = false;
			new_account_buffer = 0;
			new_char_buffer = 0;
			sock_disconnect = false;
			prev_state = (Desc_State)0;
			login_character = 0;
			need_prompt = 0;
		}

		~Descriptor();


		void set_terminal(const char* x) {
			str_ncpy(term_name, x, sizeof(term_name));
		}
		const char* get_terminal() { return term_name; }

		void close_fd( ) { close_socket(fd_num); fd_num = -1; }
		int get_fd() { return fd_num; }
		int get_port();
		bool is_dead() { return sock_is_dead; }
		bool set_need_prompt(bool v) { return need_prompt = v; }
		bool get_need_prompt() { return need_prompt; }
		bool has_disconnect() { return sock_disconnect; }
		bool has_indata() { 
			return !input_q.empty() || !input_oob_q.empty(); 
		}
		bool has_outdata() { 
			return !output_q.empty() || !output_oob_q.empty(); 
		}
		bool can_write() { return sock_is_writable; }
		void mark_dead() { sock_is_dead = 1; }
		void mark_writable(bool w = true) { sock_is_writable = w; }
		GamePort* get_port_ptr() { return port_ptr; }
		class Character* get_ch();
		class Object* get_obj() { return login_character; }

		void put_line(const char* messg);
		void send_mml(const char* messg, ...);
		void send(const char* messg);
		void sendf(const char* messg, ...);
		void send_string(GameString&);
		void init_mxp();
		bool set_mxp( bool val ) { return sock_is_mxp = val; }
		bool set_ansi( bool val ) { return sock_is_ansi = val; }
		bool set_telnet( bool val) { return sock_is_telnet = val; }
		bool get_ansi( ) { return sock_is_ansi; }
		bool get_mxp( ) { return sock_is_mxp; }
		bool get_telnet( ) { return sock_is_telnet; }

		void input();
		void input_oob();
		list<class Descriptor*>::iterator dead_socket();
		void disconnect();
		void process_input();
		void process_output();
		bool link_account(Account* x) {
			if ( login_account )
				login_account->set_descriptor ( NULL );
			login_account = x;
			login_account->set_descriptor( this );
			return true;
		}
		Desc_State get_state() { return dsc_state; };
		void set_state(Desc_State x) { dsc_state = x; }
		char* get_acct_name() { return acct_name; }
		void set_acct_name(char* b) { str_ncpy(acct_name, b, sizeof(acct_name)); }
		Account* get_account() { return login_account; }
		void logoff_char();

		class TextQueue input_q;
		class TextQueue input_oob_q;
		class TextQueue output_q;
		class TextQueue output_oob_q;

	public:
		Account* login_account;

	private:
		class Object* login_character;
		GamePort* port_ptr;
		int fd_num;
		Desc_State dsc_state;
		bool sock_is_mxp;
		bool sock_is_ansi;
		bool sock_is_telnet;
		bool sock_is_dead;
		bool sock_disconnect;
		bool sock_is_writable;
		char read_buffer[8192];
		char read_buffer_oob[512];
		char acct_name[ACCT_NAME_LEN];

		Descriptor(Descriptor &x) {memset(&x, 0, sizeof(Descriptor));}
};

void send_needed_prompts(class Game*);


#endif
