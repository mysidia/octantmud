/*
 *  Polling Structure
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
#include "Game.h"
#include "GamePort.h"
#include "GamePoll.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/**
 * Proces input and output at the port level
 */
void GamePoll :: process_io ()
{
	list<GamePort *>::iterator m;
	list<Descriptor *>::iterator d;
	GamePort* p;
	Descriptor *dd;

	for(m = ports.begin(); m != ports.end(); m++)
	{
		p = (*m);

		for(d = p->connections.begin(); 
			d != p->connections.end(); d++)
		{
			dd = (*d);

			if (dd->has_outdata())
				dd->process_output();

			if (dd->has_indata() && !dd->has_disconnect())
				dd->process_input();

/*			if (!dd->has_disconnect() && !dd->is_dead()) {
				if (dd->has_outdata())
					dd->process_output();
			}*/

			if ((!dd->has_outdata() && dd->has_disconnect()) || dd->is_dead()) {
				d = dd->dead_socket();
				if (d == p->connections.end())
					break;
			}
		}
	}
}

/**
 * Poll for incoming data, writability of waiting data, etc.
 */
void GamePoll :: poll(struct timeval *tv)
{
	list<GamePort *>::iterator m;
	list<Descriptor *>::iterator d;
	struct pollfd fds[linked_sock_count];
	struct sockaddr_in sai;
	GamePort *prt[linked_sock_count];
	Descriptor *dsc[linked_sock_count];
	char errmsg[512];
	socklen_t len;
	int i, k, mm, con_fd;

	for(i = 0, m = ports.begin(); m != ports.end(); m++ )
	{
		if ((k = (*m)->get_fd()) != -1) {
			fds[i].fd = (*m)->get_fd();
			fds[i].events = POLLIN;
			fds[i].revents = 0;
			prt[i] = (*m);
			dsc[i] = 0;
			i++;

			for(d = (*m)->connections.begin(); 
			         d != (*m)->connections.end(); d++)
			{
				con_fd = (*d)->get_fd();
				if ( con_fd == -1 )
					continue;
				fds[i].fd = con_fd;
				fds[i].events = POLLIN | POLLPRI | POLLHUP;
				if ((*d)->has_outdata()
					&& !((*d)->can_write()))
					fds[i].events |= POLLOUT;
				fds[i].revents = 0;
				prt[i] = 0;
				dsc[i] = (*d);
				i++;
			}
		}
	}

	i = ::poll(fds, mm = i, 200);

	if (i < 1)
		return;

        for(i = 0; i < mm; i++ )
        {
		if (dsc[i] && (fds[i].revents & POLLOUT)) {
			dsc[i]->mark_writable( true );	
		}
		if (dsc[i] && (fds[i].revents & POLLHUP)) {
			dsc[i]->mark_dead();
		}
                if (dsc[i] && (fds[i].revents & POLLPRI)) {
                        dsc[i]->input_oob();
		}
		if (dsc[i] && (fds[i].revents & POLLIN)) {
			dsc[i]->input();
		}
		if (prt[i] && (fds[i].revents & POLLIN)) {
			len = sizeof(struct sockaddr_in);

			k = ::accept(fds[i].fd, (struct sockaddr*) &sai,
				&len);
			if (k < 0) {
				log_print("Accept error: %s, Port: %d", strerror(errno),
					prt[i] ? prt[i]->get_port() : -1);
				continue;
			}

			if (nonblock(k, errmsg) < 0) {
				log_print("Error making the socket non-blocking\n");
				::close(k);
				continue;
			}
			
			if (prt[i] == 0)
				::close(k);
			else {
				static char buf[1024];

				str_ncpy(buf, inet_ntoa(sai.sin_addr), sizeof(buf));
				prt[i]->new_connection(k, buf);
			}
		}
        }
}
