/*
 *  Text I/O
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


#include "mud.h"
#include "TextQueue.h"
#include <stdio.h>

void TextQueue :: queue_buf(int fb, const char* buffer, int len)
{
	struct Queue_Node* q;

	if ( fb )
		q = new_node_front();
	else
		q = new_node();

        if (len < MAX_SOCK_LEN) {
                memcpy(q->buffer, buffer, len);
                q->len = len;
        }
        else {
                while ( len >= MAX_SOCK_LEN ) {
                        memcpy(q->buffer, buffer, MAX_SOCK_LEN);
			q->len = MAX_SOCK_LEN;
                        buffer += MAX_SOCK_LEN;
                        len -= MAX_SOCK_LEN;
                        if (len > 0) {
				if ( fb )
					q = new_node_front();
				else
					q = new_node();
			}
                }
                if (len > 0) {
                        memcpy(q->buffer, buffer, len);
                        q->len = len;
                }
        }
}


void TextQueue :: add ( const char* buffer, int* len )
{
	int l = 0;

	queue_buf(0, buffer, *len);
}

void TextQueue :: push_front ( const char* buffer, int* len )
{
	struct Queue_Node *q = new_node_front();

	queue_buf(1, buffer, *len);
}


int TextQueue :: get ( char *buf, int len )
{
	struct Queue_Node* q = first_node;
	int l = q ? q->len : 0;

	if (!q || len < l)
		return -1;
	if (len > l)
		len = l;

	memcpy(buf, q->buffer, len);
	del_node(q);
	return l;
}

void TextQueue :: make_empty ()
{ 
	struct Queue_Node* n;

	while ( first_node ) {
		n = first_node->next;

		delete first_node;
		first_node = n;
	}

	last_node = first_node = 0;
}

