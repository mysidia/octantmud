/*
 *  GamePort Listener Structure
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

/**
 * 
 * */
#define __GamePort_cc__
#include "mud.h"
#include "Game.h"
#include "GamePort.h"
#include "GamePoll.h"
#include "Parser.h"
#include "GameString.h"
#include "Character.h"
#include "Database.h"
#include "Account.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <time.h>
#include <time.h>

#include "md5.h"
#include "md5_iface.h"
#include "telnet.h"

const int TTYPE_IS = 0;
const int TTYPE_SEND = 1;

// const Port_Dispatch_t GamePort::state_handlers[];
// const Port_Prompt_t GamePort::state_prompters[];
void got_terminal(Descriptor *d, char *term_type, bool verbose);

struct _statinfo {
	char* stat_name;
	char* col_name;
	int   trainable;
	
	LIST_ENTRY(_statinfo)	stat_lst;
};

LIST_HEAD(,_statinfo)	plr_stat_list;


void load_player_stats( )
{
	QueryResult r = gameDb.query("select * from stats_info "
                                     "order by stat_name desc");
	if (r.is_error() || !r.next_row()) {
		log_print("Error loading player stats.");
		abort();
	}

	do {
		struct _statinfo *x = new _statinfo;
		memset(x, 0, sizeof(struct _statinfo));
		
		massert(x);
		x->stat_name = r.get_string_copy("stat_name");
		x->col_name  = r.get_string_copy("col_name");
		x->trainable = r.get_long("is_trainable");
		LIST_INSERT_HEAD(&plr_stat_list, x, stat_lst);
	} while (r.next_row ());
}

/**
 * Set the number associated with this port
 */
void GamePort::set_portnum(int pn)
{
	bind_port = pn;
}

/**
 * Set the host name that this port binds to
 */
void GamePort::set_host(const char* x)
{
	copy_alloc_str( bind_host, x );
}

/**
 * Open this port
 * @pre set_port() has been called with a valid port number.
 */
bool GamePort::open(int backlog)
{
	struct sockaddr_in sai;
	int z, sz;

	strcpy(errmsg, "Unknown Error");

	if (bind_port == 0)
		return false;

	if (bind_host && bind_host[0] && bind_host[0] != '*') {
		z = inet_aton(bind_host, &sai.sin_addr);

		if (z == 0) {
			struct hostent* he = gethostbyname(bind_host);

			if (!he) {
				sprintf(errmsg, "Unable to resolve [%s]", bind_host);
				return false;
			}

			if (!he->h_addr_list[0]) {
				sprintf(errmsg, "No addresses for [%s]", bind_host);
				return false;
			}

			memcpy(&sai.sin_addr, he->h_addr_list[0], sizeof(struct in_addr));
			sai.sin_family = he->h_addrtype;
			sz = he->h_length;
		}
		else {
			sai.sin_family = AF_INET;
			sz = 4; // ??? sizeof(struct sockaddr_in);
		}

	}
	else {
		sai.sin_addr.s_addr = INADDR_ANY;
		sai.sin_family = AF_INET;
		sz = 4; // ??? sizeof(struct sockaddr_in);
	}
	sai.sin_port = htons(bind_port);

	fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (fd < 0) {
		sprintf(errmsg, "Unable to create socket: %s", strerror(errno));
		return false;
	}

	z = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &z, sizeof(int)) < 0) {
		sprintf(errmsg, "setsockopt: %s", strerror(errno));
		return false;
	}

	z = 1;
	if (nonblock(fd, errmsg) < 0) 
		return false;

	if (bind(fd, (struct sockaddr *)&sai, sizeof(struct sockaddr_in)) < 0) {
		sprintf(errmsg, "Unable to bind socket: %s", strerror(errno));
		return false;
	}

	if (listen(fd, backlog) < 0) {
		sprintf(errmsg, "Unable to listen on socket: %s", strerror(errno));
		return false;
	}

	if ( poller ) {
		poller->link( this );
	}

	return true;
}

/**
 * Associate a poller with this port object
 */ 
void GamePort :: set_poller(GamePoll* p)
{
	poller = p;
}

/**
 * Close the network socket associated with this port
 */
bool GamePort::close()
{
	::close(fd);

	return false;
}

/**
 * Print the message associated with the last error to the log
 */
void GamePort::print_err()
{
	log_print(GamePort::errmsg);
}

/**
 * @return An error string if a GamePort method indicated an error
 */
char* GamePort::get_err() 
{
	return errmsg;
}

/**
 * Called by the poller manager: notify the port that a new
 * user has connected on a given network descriptor.
 *
 * @post new_connection_message has been called
 * @post a Descriptor object has been created and the network descriptor
 * @post poller->added_child is called with the Descriptor object
 * @post The descriptor is added to the list of connected clients
 */
void GamePort::new_connection (int fd_num, const char* addr)
{
	Descriptor *d = new Descriptor(fd_num, addr, this);

	massert(d);
	conn_count++;

	connections.push_back(d);
	if ( poller )
	{
		poller->added_child( d );
	}

	new_connection_message(d);
}

/**
 * The poller notifies the port that a client socket was closed
 *
 * @post  The connection is removed, and the descriptor deleted.
 */
list<Descriptor*>::iterator GamePort::dead_socket (Descriptor *d)
{
	list<Descriptor *>::iterator c = connections.begin();

	for ( ; c != connections.end(); c++) {
		if ((*c) == d)
			break;
	}

	if (c == connections.end())
	{
		log_print("Fatal: unknown connection died");
		abort();
	}

	c = connections.erase(c);
	::close(d->get_fd());
	if ( poller )
		poller->removed_child(d);
	delete d;

	return c;
}

/**
 * Process the input queue for a port: this is called by the Poller
 * when input data has been enqueued and now needs processing.
 *
 * This handles actual extraction of protocol messages:
 * in the case of telnet, we extract lines currently.
 *
 * In the future we will need to extract telnet control sequences.
 */
int GamePort::process_in_q(Descriptor* d, TextQueue* q, int pri)
{
	unsigned char buf[MAX_SOCK_LEN];
	char sub_buffer[MAX_INPUT_LEN];
	char line_buffer[MAX_INPUT_LEN];
	int l, i, state, line_pos, skip = 0, sub_pos = 0;
	int escape = 0, ansi = 0, iac = 0, iac_sub = 0;
	int iac_do = 0, iac_x = 0, iac_y = 0, iac_x_set = 0, iac_y_set = 0;

	state = 0;
	line_pos = 0;

	while (!q->empty ())
	{
		l = q->get((char *)buf, MAX_SOCK_LEN);
		if (l == -1) {
			d->disconnect();
			return -1;
		}

		for(i = 0; i < l; i++) {
/*printf("%d,  iac = %d, iac_sub = %d, iac_x = %d, iac_y = %d, ansi = %d\n", buf[i], iac, iac_sub, iac_x, iac_y, ansi);		*/
			if (line_pos + 1 == MAX_INPUT_LEN) {
				skip = 1;
				line_pos = 0;
			}

			if (buf[i] == '\033') {
				escape = 1;
				continue;
			}
                        else if ( buf[i] == IAC ) {
				if (!d->get_telnet())
					d->set_telnet( true );

				if ( iac_sub ) {
					iac = 1;
				}
				else {
	                                ansi = 0;
	                                iac = 1;
	                                iac_x = 0;
					iac_y = 0;
					iac_y_set = 0;
					iac_x_set = 0;
	                                iac_sub = 0;
	                                escape = 0;
				}
				continue;
                        }
			else if ( escape ) {
				escape = 0;
				
				if (buf[i] == '[')
					ansi = 1;
				else
					escape = 0;
				continue;
			}
			else if ( ansi ) {
				if (isdigit(buf[i]) || buf[i] == ';') {
					printf("%c\n", buf[i]);
				}
				else if (buf[i] == ';') {
				}
				else 
					ansi = 0;
				continue;
			}
			else if ( iac && !iac_sub) {
				if (buf[i] == SB) {
					iac = 0;
					iac_sub = 1;
				}
				else {
					iac = 0;
					if (buf[i] == DO ||
					    buf[i] == DONT ||
					    buf[i] == WILL ||
					    buf[i] == WONT) {
						iac_x = buf[i];
						iac_x_set = 1;
					    }
				}
				continue;
			}
			else if ( iac_x_set && !iac_sub ) {
				/* IAC [DO|DONT|WILL|WONT] <option> */
				if (iac_x == DO &&
					buf[i] == TELOPT_MXP) {
					d->set_mxp( true );
				}
				else if (iac_x == DONT &&
						buf[i] == TELOPT_MXP) {
					d->set_mxp( false );
				}
				else if (iac_x == WILL &&
						buf[i] == TELOPT_TTYPE) {

					d->sendf("%c%c%c" "%c%c%c",
					  IAC, SB, TELOPT_TTYPE,
					  TTYPE_SEND, IAC, SE);
				}
				else {
				printf("[ %c ]", buf[i]);
				}
				
				iac_x = 0;
				iac_x_set = 0;
				iac_sub = 0;
				continue;
			}
			else if ( iac_sub ) { /* Inside IAC SB */
				if (!iac_y_set) {
				    sub_pos = 0;
				    iac_y = buf[i];
				    iac_y_set = 1;
				}
				else if (!iac_x_set) {
				    iac_x = buf[i];
				    iac_x_set = 1;
				    if (!iac)
					    continue;
				}

				if (iac_x_set && iac_y_set && !iac) {
					if (sub_pos < MAX_INPUT_LEN) {
					        sub_buffer[sub_pos++] = buf[i];
					}
					/* <opt> [DO|DONT|WILL|WONT] */
				}

				if (iac && buf[i] == SE) {
					sub_buffer[sub_pos++] = '\0';
					if (iac_y == TELOPT_TTYPE &&
					    iac_x == TTYPE_IS) {
					    got_terminal(d, sub_buffer, false);
					}
					iac = 0;
					iac_sub = 0;
				}
				else if (isprint(buf[i])) {
/*					iac_sub = 0;*/
				}
				continue;
			}

			if (buf[i] == '\n') {
				if (line_pos) {
					line_buffer[line_pos++] = '\0';
					line_in(d, line_buffer, line_pos, pri);
					line_buffer[0] = '\0';
					line_pos = 0;
				}
				else {
					line_buffer[0] = '\0';
					line_in(d, line_buffer, 0, pri);
				}
				skip = 0;
			}

			switch (state)
			{
			case 0:
				if (!skip && buf[i] != '\r')
					line_buffer[line_pos++] = buf[i];
				break;
			}
		}
	}

	l = 0;

	if ( ansi ) {
		buf[0] = '\e';
		l = 1;
	}
	else if ( iac ) {
		buf[0] = IAC;
		l = 1;
	}
	else if ( iac_sub ) {
		buf[0] = IAC;
		buf[1] = SB;
		l = 2;
		if ( iac_x ) {
			buf[2] = iac_x;
			l++;
		}
	}
	else if ( iac_x ) {
		buf[0] = IAC;
		buf[1] = iac_x;
		l = 2;
	}

	if ( l > 0 ) {
		q->push_front((char *)buf, &l);
	}

	
	return 0;
}

/**
 * Process the output queue: this is called by the Poller when
 * it has found that data is waiting to be sent and the network
 * socket can be written to.
 */
int GamePort::process_out_q(Descriptor* d, TextQueue* q, int pri)
{
	char buf[MAX_SOCK_LEN];
	int l, v, z;
	
        while (!q->empty ())
        {
                l = q->get(buf, MAX_SOCK_LEN);
		if (l == -1)
			return 0;

		if (pri == 0) {
			v = send(d->get_fd(), buf, l, 0);
			SYS_ent_4 += buf[0] ^ buf[1];
		}
		else {
			v = send(d->get_fd(), buf, l, MSG_OOB);
			SYS_ent_4 += buf[0] ^ buf[1];
		}
		if (v == -1) {
			d->mark_writable( false );

			return 0;
		}
		if (l > 0 && v > 0 && v < l && (l - v) > 0) {
			z = l - v;
			q->push_front( buf + v, &z );
		}
	}

	return 0;
}

/**
 * This handles a line of incoming text.
 */
void GamePort::line_in(Descriptor*d, char* buf, int len, int pri)
{
	Desc_State n = d->get_state(), no = n;
	int count, ret = 0, found = 0;


	if ( run_handler( d, pri, buf, len ))
		return;

	/* Parser parse(buf, WORD, PHRASE, PHRASE, WORD, END); */
	Parser parse(MAX_ARG, buf);


	for( int i = 0; i < parse.num(); i++)
		printf("W%d:  %s\n", i, parse.arg(i));
	
	d->send("Huh?\n\r");
	d->set_state(n);
//	printf("line: %s\n", buf);
}

/**
 * This function is called when a new connection has been
 * received: after the descriptor has been created, added
 * to the proper lists, and the network Poller object has
 * been notified of the new client.
 */
void GamePort::new_connection_message(Descriptor* d)
{
	d->sendf("%c%c%c",
		IAC, WONT, TELOPT_ECHO);
	d->sendf("%c%c%c",
		IAC, WILL, TELOPT_MXP);
	d->sendf("%c%c%c",
		IAC, DO, TELOPT_TTYPE);
	d->send_string(WELCOME_SCREEN);
}

/***********************************************************************/
/* The routines that follow handle individual messages dependant */
/* on the state of a Descriptor sending the message.             */

CON_HANDLER(con_enter_name)
/* Descriptor* d */
/* Desc_State instate */
/* char* inbuf */
/* int len */
{
	Account* acct = 0;
	long ac_id;

	ltrim_space(inbuf);

	if (strlen(inbuf) < 1) {
		d->disconnect();
		return;
	}

	if (strlen(inbuf) >= ACCT_NAME_LEN) {
		d->put_line("That name is too long.");
		d->send("Please enter a name: ");
		return;
	}

	d->set_acct_name(capitalize(inbuf));
	
	if (/*!load_account(parse.arg(0), &acct)*/
	    !load_account(d->get_acct_name(), &acct)) {
		const char* errmsg;

		if (!legal_account_name ( d, d->get_acct_name(), &errmsg ))
		{
			d->sendf("Sorry, but your name is not valid: %s.\r\n",
					errmsg);
			d->send("What name would you like to use ? ");
			return;
		}
		
		d->send_string(NAME_POLICY_LECTURE);
	        d->sendf("Is '%s' the name you would like to use? (Y/N) ",
	                 d->get_acct_name());

	        instate = CON_CONFIRM_NAME;
	} else {
		if (! acct->is_active()) {
			d->sendf("Sorry, but the account, '%s', has not yet been approved by the administration.\r\n", d->get_acct_name());
			d->disconnect();
			instate = CON_DISCONNECT;
			return;
		}
		if (d->get_terminal() && !str_cmp(d->get_terminal(), "unknown")) {
			if (!d->get_telnet()) {
				d->put_line(
				"Please consider changing clients.\r\n"
				"Yours does not seem to support basic telnet "
				"protocol.\r\n"
				);
			}
			else {
				d->put_line(
				"Your client uses terminal type unknown or does\r\n"
				"not respond to terminal type telnet queries.\r\n"
				"Reduced functionality mode will be assumed for your "
				"unknown terminal.\r\n"
				);
			}
		}
		else if (d->get_terminal()) {
			d->sendf("You are using a %s terminal that "
			         "%s "
			         "support for ANSI codes.\r\n",
				 	d->get_terminal(), 
					d->get_ansi() ? "has" : "does not have");
			d->sendf("Your client %s MXP.\r\n",
				d->get_mxp() ? "does support"
					     : "does not support");
		}
		instate = CON_GET_PASSWD;
	}
	return;
}

CON_HANDLER(con_confirm_name)
{
	Parser parse(inbuf, WORD, END);

	if (parse.num() < 1) {
		d->put_line("Please answer yes or no.");
		return;
	}


        if (parse.ch_uc(0,0) == 'Y') {
		instate = CON_CHOOSE_EMAIL;
		return;
        }
	else if (parse.ch_uc(0,0) == 'N') {
		d->put_line("Ok");
		d->send("What name would you like to use then? ");
		instate = CON_ENTER_NAME;
	}
	else {
	    d->put_line("Please answer Yes or No.\r\n");
//	    d->sendf("Is %s the name you would like to use?\r\n", );
	}
	return;
}

CON_HANDLER(con_get_passwd)
{
	Account* acct;
	char* nm;

	if ((nm = d->get_acct_name()) == NULL || !nm[0]
			|| !(acct = find_account(nm))) {
		d->put_line("Invalid account.");
		d->put_line("What name would you like to use ?");
		instate = CON_ENTER_NAME;

		return;
	}

/*	d->sendf("P = [%s]\r\nE = [%s]\r\n", inbuf, 
			acct->get_passwd());*/

	if (strcmp(acct->get_passwd(), inbuf)) {
		d->put_line("Invalid password.");
		return;
	}

	if ( acct->get_descriptor() ) {
		d->put_line("x");
	}
	
	if ( !d->link_account( acct ) ) {
		d->put_line( "Account in use." );
		return;
	}

	instate = CON_ACCT_MENU;
}

CON_HANDLER(con_choose_email) 
{
	ltrim_space(inbuf);

	if (!should_accept_email(inbuf, d)) {
		return;
	}
	
	if (!d->new_account_buffer)
	     d->new_account_buffer = new Creation_Buffer;
	strcpy(d->new_account_buffer->email, inbuf);
	instate = CON_EMAIL_AGAIN;

}

CON_HANDLER(con_email_again)
{
	if (!d->new_account_buffer) {
		instate = CON_CHOOSE_EMAIL;
		return;
	}

	if (str_cmp(inbuf, d->new_account_buffer->email)) {
		d->put_line("E-mail does not match.  Try again.");
		instate = CON_CHOOSE_EMAIL;
		return;
	}

#ifdef USE_ANTIBOT
	instate = CON_PROVE_HUMANITY;
#else
	assert (!d->get_account());
	instate = CON_MAKE_ACCOUNT;
#endif
}

CON_PROMPTER(put_get_passwd)
{
	d->send("Password: ");
}

CON_PROMPTER(put_choose_email)
{
	d->put_line("Please enter your e-mail address");
	d->put_line("Your password will be sent there.");
	d->send("Email: ");
}

CON_PROMPTER(put_email_again) {
	d->put_line("Please re-enter to confirm.");
	d->send("Email: ");
}

CON_PROMPTER(put_acct_menu)
{
	QueryResult r =
		gameDb.queryf("select * from character where "
		   "acct_id=%d"
		   " order by name", d->get_account()->get_id());
	int m = 0;

	d->sendf("Currently logged in as %s.\r\n", d->get_account()->get_name());
	d->put_line("--- Account Menu ---");
	d->put_line("N. Create new Character");
	d->put_line("Q. Quit");
	if (r.row_count() > 0)
	{
		d->put_line("--- Character Menu ---");
		while (r.next_row())
		{
			d->sendf("%2d. %-25s (level %3d)\r\n", ++m, r.get_string("name"),
				r.get_long("level"));
		}
	}
	
	
}

CON_HANDLER(con_prove_humanity)
{
	ltrim_space(inbuf);
	
	if (str_cmp(d->human_code, inbuf)) {
		d->put_line("Sorry, that's not the code.");
		d->put_line("Try again.");
		return;
	}
	else {
		if (d->prev_state == CON_NC_NAME_CONFIRM) {
			instate = CON_NC_MAKE;
		} else {
			assert (!d->get_account());
			instate = CON_MAKE_ACCOUNT;
		}
	}
}

CON_PROMPTER(put_prove_humanity)
{
	struct MD5Context md5_ctx;
	unsigned char digest[16];
	char buf[80] = {}, code[32];
	time_t val;
	Config conf("RandData");

	pack_int32(buf + 0, time(0));
	pack_int16(buf + 4, getpid());
	pack_int16(buf + 6, getppid());
	pack_int32(buf + 8, lrand48());
	pack_int32(buf + 12, conf.get_long("seed3", 16));
	pack_int32(buf + 15, conf.get_long("seed4", 16));
	if (d->new_account_buffer) {
		pack_int16(buf + 18, strlen(d->new_account_buffer->email));
		pack_int16(buf + 20,  d->new_account_buffer->email[0] ^ d->new_account_buffer->email[2]);
		pack_int16(buf + 22, d->new_account_buffer->email[3]);
	} else if (d->get_account()) {
		pack_int16(buf + 18, d->get_account()->get_id());
		pack_int16(buf + 20, (int)(d->get_account()));
	}
	pack_int16(buf + 24, (int)d);
	pack_int32(buf + 26, SYS_ent_1);
	pack_int32(buf + 30, SYS_ent_2);
	pack_int32(buf + 34, SYS_ent_3);
	pack_int32(buf + 38, SYS_ent_4);
	pack_int32(buf + 42, boot_time);
	pack_int32(buf + 46, (int)d->get_account());

	MD5Init(&md5_ctx);
	MD5Update(&md5_ctx, (const unsigned char *) buf, 50);
	MD5Final(digest, &md5_ctx);

	str_ncpy(d->human_code, md5_printable(digest), sizeof(d->human_code));

	d->put_line("You will need to type the code below and press enter to "
	            "convince me that your ip address is not spoofed.\r\n");
	d->put_line("Your verification code is:");
	d->human_code[6] = 0;
	if (d->new_account_buffer)
	    str_ncpy(d->new_account_buffer->passwd, md5_printable(digest), sizeof(d->new_account_buffer->passwd));

	type_char_write( d->human_code, 6, d );
	d->send("\r\n\r\nPlease type the code shown above and press enter: ");
	
}

CON_HANDLER(con_do_nothing)
{
}

CON_HANDLER(con_make_account)
{
	d->disconnect();
	instate = CON_DISCONNECT;
}

CON_PROMPTER(put_make_account)
{
	d->put_line("Ok, i'm attempting to submit your account request now.");
	QueryResult r0 = gameDb.queryf(
			"select acct_id from account "
			"where acct_email='%s'",
			d->new_account_buffer->email
	);
	if (r0.is_error()) {
		d->put_line("Database error.   Please try again later.");
		d->disconnect();
		return;
		
	}

	if (r0.row_count() > 0) {
		d->put_line("Sorry, there is already an account registered for "
		         "that e-mail address.");
		d->disconnect();
		return;
	}

	
	QueryResult r = gameDb.queryf(
		"insert into account (acct_name, acct_passwd, acct_email)"
		" VALUES ('%s', '%s', '%s')",
		  	d->get_acct_name(),
		  	d->new_account_buffer->passwd,
			d->new_account_buffer->email

	);

	if (r.is_error() ) {
		d->put_line("Sorry, I wasn't able to create your account.");
		if ( find_account(d->get_acct_name()) )
			d->put_line("It seems someone else registered it first.");
		else
			d->put_line("Please try again later.");
		d->disconnect();
		return;
	}

	d->put_line("Your account request has been successfully submitted.\r\n");
	d->put_line("After the administrators approves you account, you will receive an e-mail");
	d->put_line("message with your password.");
	d->put_line("Press [ENTER] to quit");
}


CON_HANDLER(con_acct_menu)
{
	int limit = d->get_port_ptr()->get_game_ptr()->conf()->get_int("char_limit");
	int v;

	ltrim_space(inbuf);

	switch(UPPER(*inbuf)) {
		default:
			if (isdigit(*inbuf) && (v = atoi(inbuf)) != 0) {
				QueryResult r = gameDb.queryf(
					"select name, id, "
					"level is null as rolled "
					"from character "
					"where acct_id=%d "
					"order by name",
					d->get_account()->get_id());
				
				if (r.is_error() || r.row_count() < v)
					return;
				while ( (v > 0) && r.next_row())
					v--;
				if ( v == 0 ) {
					Character *login_ch;
					const char *s;
					
					d->send("Logging in...");
					if (!(login_ch = find_character(r.get_long("id")))) 
					{
					login_ch = load_character(r.get_string("name"), r.get_long("id"));
					}
					massert(login_ch);
					login_ch->link_descriptor( d );
					d->put_line(" ok");
					s = r.get_string("rolled");

					if (s && *s == 't')
						instate = CON_CHAR_SHEET;
					else if (!s) {
						d->put_line("SYSERR: Unable to access character");
						d->disconnect();
					}
					else { 
						if (!str_cmp("null", login_ch->get_race()))
						{
							instate = CON_CHOOSE_RACE;
							return;
						}
						instate = CON_MOTD;
					}
					return;
				}
				return;

			}
		case 'Q':
			d->put_line("\r\nGoodbye!");
			d->disconnect();
			instate = CON_DISCONNECT;
		break;
		case 'N':
		{
		      QueryResult r  = gameDb.queryf("select count(*) from character where "
                                        "acct_id=%d",
                                                d->get_account()->get_id());
		      if (r.is_error() || !r.next_row()) {
			  d->put_line("Error accessing database.");
			  d->put_line("Please try again later.");
			  return;
		      }

		      if (r.get_longlong("count") >= limit)
		      {
				d->sendf("Sorry, you have already registered %d characters.\r\n",
				r.get_longlong("count"));
				if (limit != 1)
					d->sendf("There is a limit of %d characters per person.\r\n", limit);
				else
					d->sendf("There is a limit of 1 character per person.\r\n");
				return;
		      }

			instate = CON_NC_NAME;
		}
		break;
	}
}

CON_HANDLER(con_nc_name)
{
	const char* errmsg;

	ltrim_space(inbuf);
	if (! d->new_char_buffer)
		d->new_char_buffer = new Creation_Buffer;
	if (! legal_character_name(d, inbuf, &errmsg)) {
		d->sendf("Sorry, but that character name is not valid: %s.\r\n",
			errmsg);
		return;
	}

	strcpy(d->new_char_buffer->name, capitalize(inbuf));
        QueryResult r0 = gameDb.queryf("select acct_id from character "
                                "where name='%s'", d->new_char_buffer->name);
        if (r0.is_error()) {
                d->put_line("Error accessing database.");
                d->put_line("Please try again later.");
                return;
        }

        if (r0.row_count() > 0) {
                d->put_line("Sorry, but that name has already been taken.");
                d->put_line("Press [ENTER] to continue");
                return;
        }

	instate = CON_NC_NAME_CONFIRM;
}

CON_PROMPTER(put_nc_name)
{
	d->send("What do you wish to call your new character? ");
}

CON_PROMPTER(put_nc_name_confirm)
{
	d->sendf("Are you sure that you want to make a character called '%s' ?",
		d->new_char_buffer->name);
}

CON_HANDLER(con_nc_name_confirm)
{
	ltrim_space(inbuf);

	if (str_cmp(inbuf, "yes") == 0) {
/*		d->prev_state = CON_NC_NAME_CONFIRM;*/
#ifdef USE_ANTIBOT
		instate = CON_PROVE_HUMANITY;
#else
		instate = CON_NC_MAKE;
#endif
	}
	else if (str_cmp(inbuf, "no") == 0) {
		delete d->new_char_buffer;
		d->new_char_buffer = 0;
		instate = CON_ACCT_MENU;
	}
	else {
		d->put_line("Please answer YES or NO.");
	}
}

CON_HANDLER(con_nc_make) {
	instate = CON_ACCT_MENU;	
}

CON_PROMPTER(put_nc_make) {
	d->put_line("Ok. Attempting to create character.");
	QueryResult r0 = gameDb.queryf("select acct_id from character "
				"where name='%s'", d->new_char_buffer->name);
	if (r0.is_error()) {
		d->put_line("Error accessing database.");
		d->put_line("Please try again later.");
		return;
	}

	if (r0.row_count() > 0) {
		d->put_line("Sorry, but that name has already been taken.");
		d->put_line("Press [ENTER] to continue");
		return;
	}
	
	QueryResult r = gameDb.queryf("insert into character "
	               "(acct_id, name) "
		       "VALUES (%d, '%s')",
		       d->get_account()->get_id(),
		       d->new_char_buffer->name);
	if (r.is_error()) {
		d->put_line("Sorry, character creation failed.");
		d->put_line("Unable to proceed, try again later.");
		delete d->new_char_buffer;
		d->new_char_buffer = 0;
	}
	else {
		d->put_line("Character created.");
	}
	d->put_line("Press [ENTER] to continue");
}


CON_PROMPTER(put_char_sheet)
{
	Character* ch = d->get_ch();
	struct _statinfo *st;
	int c = 0;

	d->send("\r\n\r\n");
	
	d->sendf("%-12s: %-15s ",
			"Name",	ch->get_name());
	d->sendf("%-12s: %-15s ",
			"Gender", ch->get_gender());
        d->send("\r\n");

	st = LIST_FIRST(&plr_stat_list);

	while ( st ) {
		if (!st->trainable)
			continue;
		d->sendf("%-12s: %-15d ",
				st->stat_name, ch->get_stat(st->col_name));
	
		if ((++c) % 2 == 0)
			d->send("\r\n");
	
		st = LIST_NEXT(st, stat_lst);
	}
	d->send("\r\n");
	
/*	d->sendf("%-12s: %-15d ",
	                "Strength", ch->get_stat(STAT_STR));
	d->sendf("%-12s: %-15d ",
			"Intelligence", ch->get_stat(STAT_INT));
	d->send("\r\n");
	d->sendf("%-12s: %-15d ", 
			"Wisdom", ch->get_stat(STAT_WIS));
	d->sendf("%-12s: %-15d ",
			"Dexterity", ch->get_stat(STAT_DEX));
	d->send("\r\n");
	d->sendf("%-12s: %-15d ",
			"Constitution", ch->get_stat(STAT_CON));
	d->sendf("%-12s: %-15d ",
			"Charisma", ch->get_stat(STAT_CHA));*/
	d->send("\r\n\r\n");

	if ( c ) {
		d->sendf("You have %d stat points to assign.\r\n"
		         "    To increase a stat, type '+<stat>'\r\n"
			 "    To decrease a stat, type '-<stat>'\r\n"
			 "For example: +strength +strength -intelligence\r\n"
			 "When you are done, type 'done' to confirm your "
			 "stats.\r\n",
		  ch->get_stat(STAT_TRAINS));
	}
	else
		d->send("press [ENTER] to continue\r\n");
}

CON_HANDLER(con_char_sheet)
{
	Character* ch = d->get_ch();
	struct _statinfo *st;
	Parser parse(5, inbuf);
	const char* s;
	const char* stat_name;
	const char* stat_chosen;
	int i = 0, mode = 0, c = 0, v, t;

	i = -1;

	for(st = LIST_FIRST(&plr_stat_list); st; st = LIST_NEXT(st, stat_lst))
		if (st->trainable)
			break;

	if (!st) {
		instate = CON_CHOOSE_RACE;
		return;
	}
	

	while( i + 1 < parse.num() )
	{
		i++;
		c = 0;
		st = LIST_FIRST(&plr_stat_list);
		s = parse.arg(i);
		stat_chosen = 0;
		stat_name = 0;

		if (*s == '+') {
			mode = 1;
			s++;
		}
		else if (*s == '-') {
			mode = -1;
			s++;
		}

		while ( st ) {
			if (!st->trainable)
				continue;
			if (is_abbrev(s, st->stat_name))
				break;
			c++;
			st = LIST_NEXT(st, stat_lst);
		}

		if ( str_cmp(s, "done") && st ) {
			stat_chosen = st->col_name;
			stat_name = st->stat_name;
		}
/*		else if (is_abbrev(s, "intelligence")) {
			stat_chosen = STAT_INT;
			stat_name = "intelligence";
		}
		else if (is_abbrev(s, "wisdom")) {
			stat_chosen = STAT_WIS;
			stat_name = "wisdom";
		}
		else if (is_abbrev(s, "dexterity")) {
			stat_chosen = STAT_DEX;
			stat_name = "dexterity";
		}
		else if (is_abbrev(s, "constitution")) {
			stat_chosen = STAT_CON;
			stat_name = "constitution";
		}
		else if (is_abbrev(s, "charisma")) {
			stat_chosen = STAT_CHA;
			stat_name = "charisma";
		}
		else if (is_abbrev(s, "luck")) {
			stat_chosen = STAT_LUCK;
			stat_name = "luck";
		}*/
		else if (!str_cmp(s, "done")) {
			if (c && (ch->get_stat(STAT_TRAINS) > 0)) {
				d->sendf(
				"You need to use your stat points first!\r\n"
				);
				return;
			}
			instate = CON_CHOOSE_RACE;
		}
		else {
			if (c)
				d->sendf("Unknown stat, '%s'.\r\n", s);
			else 
				instate = CON_CHOOSE_RACE;
		}

		if ( stat_chosen ) {
			if ( mode == 0 ) {
				d->send("Increase or decrease?\r\n");
				return;
			}
			if (((t = ch->get_stat(STAT_TRAINS)) < 1) && mode == 1) {
				d->send("You are out of stat points.\r\n");
				continue;
			}

			if ((v = ch->get_stat(stat_chosen)) <= 3 && mode == -1) {
				d->sendf("You cannot reduce your %s below 3.\r\n",
					stat_name);
				return;
			}

			if (v >= 18 && mode == 1) {
				d->sendf("You cannot increase your %s above "
					 "18.\r\n", stat_name);
				return;
			}

			if ( ch->set_stat(STAT_TRAINS, t - 1 * mode ) )
			{
				if ( ch->set_stat(stat_chosen, v + 1 * mode) )
					;
				else { 
					ch->set_stat(STAT_TRAINS, t);
				}
			}
		}
	}
}


void got_terminal(Descriptor *d, char *term_type, bool verbose)
{
   char buf[MAX_INPUT_LEN];
   bool has_color = false;
   char sbuf[MAX_INPUT_LEN], *s, *p, *field;
   int i = 0;

   struct { char *info, *name; bool is_color; }
   TermTab[] = {
        { "generic", "vg|vt-generic",                           true },
	{ "zmud",  "zmud",					true },
        { "color", "color",                                      true },
        { "linux", "lx|linux|console|con80x25",                 true },
        { "linux", "lx|con80x28|con80x43|con80x50|con100x37|con100x40|con132x43"
, true },
        { "vt102", "v2|vt102",                                  true },
        { "vt100", "vt|vt100",                                  true },
        { "vt320", "v3|vt320",                                  true },
        { "vt320-8", "v8|vt320-8",                              true },
        { "minicom", "mc|minicom|ansi-mc",                      true },
        { "xterm", "vs|xterm|xterm-color|vs100",                true },
        { "xterm", "v2|xterms|vs100s",                          true },
        { "xterm-bold", "vb|xterm-bold",                        true },
        { "xterm-ins", "vi|xterm-ins",                          true },
        { "ansi", "an|ansi|ansi-bbs",                           true },
        { "screen", "screen",                                   true },
        { "at", "at|at386-m|386AT-M|386at-m|at/386 console",    true },
        { "minix", "ma|minix|minix-am|minix-vcam",              true },
        { "sa", "sa|network|ethernet|arpanet",                  true },
        { "su", "su|dumb|un|unknown",                           true },
        { "sd", "sd|du|dialup",                                 true },
        { "t7", "t7|37|tty37|model",                            true },
        { "sun", "mu|sun",                                      true },
        { NULL },
   };

   if (!term_type || !d)
       return;

   for(i = 0; TermTab[i].name; i++) {
       str_ncpy(sbuf, TermTab[i].name, sizeof(sbuf));

       for (p = field = NULL, s = sbuf; (field = strtoken(&p, s, "|")); s = NULL)
       {
            if (!strcasecmp(field, term_type))
                break;
       }
       if (field)
           break;
   }

   if (TermTab[i].name) {
       if ( verbose ) {
           sprintf(buf, "`%s' terminal detected.\r\n", TermTab[i].info);
           d->send(buf);
       }
       has_color = TermTab[i].is_color;
   }
   else if ( verbose ) {
           d->put_line("'Unknown' terminal detected.");
   }

   if (has_color) {
       d->set_ansi( true );
   }
   else d->set_ansi( false );

   if (TermTab[i].name)
       d->set_terminal(TermTab[i].info);
    else d->set_terminal("unknown");
}

CON_PROMPTER(put_choose_gender)
{
	d->put_line("The genders available are:");
	d->put_line(" Male ");
	d->put_line(" Neither ");
	d->put_line(" Female ");
	d->send("Please enter a gender for your character:\r\n");
}


CON_HANDLER(con_choose_gender)
{
	Character* ch = d->get_ch();
	bool ret = false;

	ltrim_space(inbuf);
	
	if ( complete_abbr(inbuf, "Male", inbuf) )           ;
	else if ( complete_abbr(inbuf, "Female", inbuf) )    ;
	else if ( complete_abbr(inbuf, "Neither", inbuf) )   ;
	else {
		d->put_line("No such gender.  Try again.");
		return;
	}

	if ( !ch->set_gender(inbuf) ) {
		d->put_line("Unable to set gender.");
		return;
	}
	d->put_line("Gender set.");
	instate = CON_CHOOSE_ALIGNMENT;
}


CON_PROMPTER(put_choose_race)
{
	QueryResult x = gameDb.query("select name from races");
	int c = 0;
	if (x.is_error())
		abort();
	d->send("Available races:\r\n");
	while (x.next_row()) {
		c++;
		d->sendf("  %-15s\r\n", x.get_string("name"));
	}
	if (c == 0) {
		d->put_line("SYSERR: No player races defined");
		d->disconnect();
		return;
	}
	d->send("Please choose a race for your character: ");
}

CON_HANDLER(con_choose_race)
{
	char buf[MAX_STRING_LEN];
	ltrim_space(inbuf);

	if (!strn_cmp(inbuf, "info ", 5)) {
		return;
	}

	sqlEscape(capitalize(inbuf), buf);

	QueryResult x = gameDb.queryf("select name from races where name='%s'",
					buf);
	
	if ( x.is_error() ) {
		d->disconnect();
		return;
	}

	if ( x.row_count() < 1 ) {
		d->put_line("Invalid race.  Try again.\r\n");
		return;
	}

	QueryResult x2 = gameDb.queryf("update character set race='%s' "
                                       "where id=%ld",
				       buf, d->get_ch()->get_id());

	if (x2.is_error()) {
		d->put_line("Error setting race.");
		return;
	}
	d->put_line("Ok");
	instate = CON_CHOOSE_GENDER;
}

CON_PROMPTER(put_choose_alignment)
{
	d->put_line("How is your character to be aligned?");
	d->put_line(" [1] Evil");
	d->put_line(" [2] Neutral");
	d->put_line(" [3] Good");
	d->put_line("Choose one:");
}

CON_HANDLER(con_choose_alignment)
{
	Character* ch = d->get_ch();
	int v = atoi(inbuf);
	char* a = 0;

	ltrim_space(inbuf);
	if (is_abbrev(inbuf, "evil"))
		v = 1;
	else if (is_abbrev(inbuf, "neutral"))
		v = 2;
	else if (is_abbrev(inbuf, "good"))
		v = 3;

	switch( v ) {
	default:
		d->put_line("Invalid alignment");
		return;
	case 1:		a = "evil";     v = -700; break;
	case 2:		a = "neutral";  v = 0   ; break;
	case 3:		a = "good";     v = 700 ; break;
	}

	QueryResult x = gameDb.queryf("update character set alignment=%d "
	                              "where id=%ld", v, ch->get_id());
	if (x.is_error()) {
		d->put_line("Unable to create your alignment.");
		return;
	}

	d->sendf("Ok, your alignment is now %s.\r\n", a);

	QueryResult x2 = 
	    gameDb.queryf("update character set level=1, class='citizen' "
	                  "where id=%ld", ch->get_id());
	if (x2.is_error()) {
		d->put_line("Error preparing character.");
		return;
	}
	d->put_line("Ok, get ready to enter the game.");
	instate = CON_MOTD;
}


CON_PROMPTER(put_motd) {
	d->put_line("Motd here");
	d->put_line("Press [ENTER] to continue");
}

CON_HANDLER(con_motd) {
	instate = CON_PLAYING;
	if ( d->get_obj() )
		look_around( d->get_obj(), d->get_obj()->get_location() );
}

CON_PROMPTER(put_playing) {
	d->set_need_prompt( true );
/*	d->send("> "); */
}

void GamePort :: write_prompt(Descriptor* d)
{
	d->send("> ");
	d->set_need_prompt( false );
}
		

CON_HANDLER(con_playing) {
	Character* ch = d->get_ch();

	ch->enqueue_command(inbuf, CMD_COMM, d);
}
