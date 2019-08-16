/*
 *  Descriptor Structure
 *
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


#include "mud.h"
#include "Descriptor.h"
#include "Character.h"
#include "Game.h"
#include "GamePort.h"
#include "TextQueue.h"
#include "GameString.h"
#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/socket.h>

int SYS_ent_1, SYS_ent_2, SYS_ent_3, SYS_ent_4, SYS_ent_5;

/**
 * @return Pointer to the GamePort this descriptor is connected with
 */
int Descriptor :: get_port() 
{
	return port_ptr->get_port(); 
}

void Descriptor :: init_mxp() {
}


void Descriptor :: logoff_char() 
{
        if ( login_character )
	{
		login_character->unlink_descriptor( this );
		login_character = 0;
	}		
}

Descriptor :: ~Descriptor() {
	this->logoff_char();
	input_q.make_empty();
	input_oob_q.make_empty();
	delete new_char_buffer;
	delete new_account_buffer;
}


/**
 * Read input from the system socket layer
 */
void Descriptor :: input ()
{
	int k;

	SYS_ent_1 += k;

	if ( Descriptor::sock_is_dead ) {
		this->dead_socket();
		return;
	}

#ifdef USE_NONB
	while ( 1 ) {
		k = recv(fd_num, read_buffer, sizeof(read_buffer), 0);
		SYS_ent_2 += k;
		SYS_ent_3 += read_buffer[0] ^ read_buffer[1];

		if (k == -1)
			return;
		if (k == 0)
			break;

		this->input_q.add(read_buffer, &k);
	}
#else
        k = recv(fd_num, read_buffer,  sizeof(read_buffer), 0);
	SYS_ent_2 += k;
	SYS_ent_3 += read_buffer[0] ^ read_buffer[1];

	if (k > 0)
		this->input_q.add(read_buffer, &k);
#endif


	if (k == 0)
		this->dead_socket();
}

/**
 * Read Out of Bounds data from the socket layer
 */
void Descriptor :: input_oob ()
{
	int k;
#ifdef USE_NONB	
	while ( 1 ) 
	{
		k = recv(fd_num, read_buffer_oob, sizeof(read_buffer_oob), MSG_OOB);
		if (k == -1)
			return;
		if (k == 0)
			break;
		this->input_oob_q.add(read_buffer, &k);
	}
#else
 	k = recv(fd_num, read_buffer_oob, sizeof(read_buffer_oob), MSG_OOB);
	if (k > 0)
	    this->input_oob_q.add(read_buffer, &k);
#endif
	if (k == 0)
		this->dead_socket();
}

/**
 * Pass up dead socket messages to the port manager (GamePort) object
 */
list<Descriptor*>::iterator Descriptor::dead_socket()
{
	return port_ptr->dead_socket(this);
}

/**
 * Flag disconnect mode for a socket
 */
void Descriptor::disconnect() {
	sock_disconnect = true;
}

/**
 * Relay queued input from the I/O poller (GamePoll)
 * to the port manager (GamePort) for actual parsing
 */
void Descriptor :: process_input()
{
	port_ptr->process_in_q(this, &input_oob_q, 1) ;

	port_ptr->process_in_q(this, &input_q, 0);
}

/**
 * Relay pending output for further processing once the
 * socket is writable.
 */
void Descriptor :: process_output()
{
	if (can_write()) {
		port_ptr->process_out_q(this, &output_oob_q, 1);

		port_ptr->process_out_q(this, &output_q, 0);
	}
}

/**
 * Queue a message to be sent to a descriptor.
 * @pre messg != 0
 */
void Descriptor :: send(const char* messg)
{
	int z = strlen(messg);
	
	output_q.add( messg, &z );

}

/**
 * Queue a line to be sent to a descriptor with \r\n
 * @pre messg != 0
 */
void Descriptor :: put_line(const char* messg) {
	char buf[MAX_SOCK_LEN+1024];
	int z = strlen(messg); 

	if (z >= MAX_SOCK_LEN) {
		log_print("put_line: line too long:: %*s", MAX_SOCK_LEN, messg);
		return;
	}
	memcpy(buf, messg, z);
	buf[z] = '\r';
	buf[z+1] = '\n';
	z += 2;

	output_q.add( buf, &z);
}

/**
 * Queue a game string to be sent to a descriptor
 */
void Descriptor :: send_string(GameString& s)
{
	const char *str = s.get_str();
	int z = strlen(str);
	
	output_q.add( str, &z );
}

/**
 * Enqueue formatted output to a descriptor
 * @pre messg != 0, (valid args given)
 */
void Descriptor :: sendf(const char* messg, ...)
{
	static char buf[MAX_OUTPUT_LEN];
	int z;
	va_list ap;

	va_start(ap, messg);
	vsnprintf(buf, sizeof(buf), messg, ap);
	va_end(ap);

	z = strlen(buf);
	output_q.add( buf, &z );
}


void Descriptor :: send_mml(const char* messg, ...)
{
        static char buf[MAX_OUTPUT_LEN];
        int z;
        va_list ap;

        va_start(ap, messg);
        vsnprintf(buf, sizeof(buf), messg, ap);
        va_end(ap);

	parse_mml( this, buf );

        z = strlen(buf);
        output_q.add( buf, &z );
}

Character* Descriptor :: get_ch() {
    return dynamic_cast<class Character *>(login_character);
}


void send_needed_prompts(Game* g)
{
	GamePort* p;
	Descriptor* d;

	list<GamePort *>::iterator gp = g->ports.begin();
	list<Descriptor *>::iterator cp;

	while( gp != g->ports.end() ) {
		for( cp = (*gp)->connections.begin();
		     cp != (*gp)->connections.end();
		     cp++ )
	       {
	       		if (!(*cp)->get_need_prompt())
				continue;
	       		(*gp)->write_prompt((* cp));
	       }
	       gp++;
	}

}

