/*
 *  OMud - Text Queueing Structure
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


#ifndef __TextQueue_h__
#define __TextQueue_h__
#include "mud.h"


class TextQueue {

	public:
	enum sep_type {
		NONE, DELIM
	};

	void add ( const char* buffer, int* len );

	int get( char*, int );
	bool empty() { return last_node == NULL; }
	int length() { return strlen(last_node->buffer); }

	void push_front( const char*, int* );
	void make_empty();

	TextQueue() : last_node(0), first_node(0) {}

	private:
	struct Queue_Node {
		struct Queue_Node *prev, *next;
		char buffer[MAX_SOCK_LEN];
		int len;
	} *last_node, *first_node;

	struct Queue_Node *new_node() {
		struct Queue_Node* p = new Queue_Node;

		p->next = 0;
		p->prev = 0;
		p->len = 0;
		
		::massert(p);
		if (last_node == NULL)
			last_node = first_node = p;
		else {
			last_node->next = p;
			p->prev = last_node;
			last_node = p;
		}
		return p;
	}

	struct Queue_Node *new_node_front() {
		struct Queue_Node* p = new Queue_Node;

		p->next = 0;
		p->prev = 0;
		p->len = 0;

		::massert(p);
		if (first_node == NULL)
			last_node = first_node = p;
		else {
			p->next = first_node;
			first_node->prev = p;
			first_node = p;
		}
		return p;
	}

	void queue_buf(int fb, const char* buf, int len);

	void del_node(struct Queue_Node* zap) {
		if (last_node == first_node) {
			assert(first_node == zap);
			last_node = first_node = NULL;
		}
		else if (last_node == zap) {
			last_node = last_node->prev;
			last_node->next = 0;
		}
		else if (first_node == zap) {
			first_node = first_node->next;
			first_node->prev = 0;
		}
		else {
			assert(zap->prev);
			assert(zap->next);
			zap->prev->next = zap->next;
			zap->next->prev = zap->prev;
		}
		delete zap;
	}
	
	//                this->input_q.add(read_buffer, &k);
};

//                TextQueue input_q;
//		                TextQueue input_oob_q;


#endif
