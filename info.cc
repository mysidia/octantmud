/*
 *  OMud - Information Gathering Routines
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
#include "GamePort.h"
#include "Character.h"
#include "Location.h"
#include "Database.h"

void look_around( Object * ch, Location * area )
{
	if ( !ch )
		return;

	if ( area && area->get_descr() ) {
		ch->send_mml( area->get_name() );
		ch->put_line("");

		ch->send_mml( area->get_descr() );
		ch->put_line("");
	}
	else {
		ch->put_line("You see nothing of interest.\r\n");
	}
}



void info_who_command( class Object * ch, class GamePort * gp )
{
	class Object* tch;
	int count = 0;
	list<Descriptor*>::iterator di;
	vector<Object*> who_list;
	vector<Object *>::iterator wi;

	for( di = gp->connections.begin(); di != gp->connections.end(); di++)
	{
		tch = (*di)->get_obj();

		if (!tch || !tch->is_mobile())
			continue;
		who_list.push_back(tch);
	}


	// Perform any sorting here

	write_who_start( ch );
	for(wi = who_list.begin(); wi != who_list.end(); wi++)
		write_who_item( ch, (*wi) );
	write_who_end( ch, who_list.size() );


}

void write_who_start( class Object * ch ) {
	ch->put_line("");
}

void write_who_item( class Object * ch , class Object * tch ) {
	ch->sendf("[%2d]  %-15s\r\n",
		  tch->get_level(),
		  tch->get_name());
}

void write_who_end( class Object * ch, int count ) {
	ch->sendf("%d players visible.\r\n", count);
}
