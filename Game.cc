/*
 *  Game Structure
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



#include "mud.h"
#include "Game.h"
#include "Config.h"
#include "GamePort.h"
#include "GamePoll.h"
#include "Sys_Uid.h"
#include "Database.h"
#include "Account.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void process_char_input();

/**
 * Initialize game
 * @return true on success, false on failure
 */
bool Game::init() {
	int port_num = config.get_int("port");
	const char *host = config.get_str("host");
	GamePort *port = new GamePort(this);

	massert(port);

        srand(time(0) + config.get_long("seed1", 16));
        srand48(time(0) + (rand()%10) + config.get_long("seed2", 16));


	if (config.exists("uid")) {
		int uid_x = config.get_int("uid");

		if ( Sys_Uid :: setreuid(uid_x, uid_x) < 0 )
			return false;
	}

	if ( Sys_Uid :: is_root() ) {
		printf("*** ERROR: Mud server is running with administrative privileges\n");
		printf("*** Either move the server to a regular user account, OR\n");
		printf("*** set the UID field in Config.cc\n");
	}

	port->set_portnum(port_num);
	port->set_poller(&mud_poller);

	if (!port->open()) {
		port->print_err();
		return false;
	}

	ports.push_back(port);
	state = MUD_RUNNING;

	gameDb.start();

	return true;
}

/**
 * Periodic maintenance of the game
 */
void Game::run() {
	/* Game Updates */
}

/**
 * Periodic i/o polling
 */
void Game::io() {

	mud_poller.poll();
	mud_poller.process_io();

	process_char_input();
	send_needed_prompts(this);

// list<GamePort *>::iterator m;

//	for(m = ports.begin(); m != ports.end(); m++) {
		
//	}
}

/**
 * Terminate the game
 */
void Game::end() {
	printf("Game over\n");
}

/**
 * Get the state of the game
 */ 
Game::Game_State Game::get_state() {
	return state;
}

/**
 * Disconnect the descriptors logged in as an account
 */
bool Game :: disconnect_account ( Account* p )
{
	list<GamePort *>::iterator x = ports.begin();
	list<Descriptor *>::iterator y;

	while (x != ports.end ()) {
		y = (*x)->connections.begin();

		while (y != (*x)->connections.end()) {
			if ( (*y)->get_account()->inuse )
				(*y)->get_account()->inuse--;
			(*y)->link_account(NULL);
			(*y)->mark_dead ();
			if ( !(*y)->get_account()->inuse )
				return true;
			y++;
		}
		x++;
	}
	return false;
}

