/*
 *  OMud - Game Poll Structure
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

#ifndef __GamePoll_h__
#define __GamePoll_h__

#include "GamePort.h"
#include "Descriptor.h"

class GamePoll 
{
	public:
		void link(GamePort *p) {
			ports.push_back(p);
			linked_sock_count++;
		}

		void unlink(GamePort *p) {
			list<GamePort*>::iterator q;

			for (q = ports.begin(); q != ports.end(); q++)
				if (*q == p) {
					linked_sock_count --;
					ports.erase(q);
				}
		}

		void added_child ( Descriptor* d ) {
			linked_sock_count++;
		}

		void removed_child ( Descriptor* d ) {
			linked_sock_count--;
		}

		void poll(struct timeval *tv = 0);
		void process_io();

		GamePoll() : linked_sock_count(0) { }

	private:
		list<GamePort *> ports;
		int linked_sock_count;
};

#endif
