/*
 *  OMud - Game Structure
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


#include "Config.h"
#include "GamePort.h"
#include "GamePoll.h"

#define MUD_CF "mud.cf"

class Game {
	public:
		friend void send_needed_prompts(Game* g);
			
		enum Game_State {
			MUD_RUNNING,
			MUD_SHUTDOWN,
			MUD_REBOOT
		};

		Game () : config(MUD_CF), state(MUD_RUNNING) { }

		bool init ();
		void run ();
		void io  ();
		void end ();
		list<GamePort *>& get_ports() { return ports; }
		Config *conf() { return &config; }
		Game_State get_state();

		bool disconnect_account ( Account* );

	private:
		Game_State state;
		GamePoll mud_poller;
		Config config;
		list<GamePort *> ports;
};

extern vector<Game *> running_games;
