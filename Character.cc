/*
 *  OMud - Character Structure
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
#include "Character.h"
#include "Descriptor.h"
#include "interp.h"

map<long,Character *> loaded_chars;


void process_char_input() {
	map<long, Character *>::iterator i = loaded_chars.begin( );
	map<long, Character *>::iterator n;

	while ( i != loaded_chars.end() ) {
		n = i;
		n++;
		(i->second)->process_commands();
		unload_character((i->second));
		i = n;
	}
}

Character::~Character()
{
	if (!command_q.empty())
		command_q.make_empty();
}

Character* find_character( long ptr )
{
	map<long, Character*>::iterator i
				= loaded_chars.find( ptr );

	if (i != loaded_chars.end())
		return i->second;
	return NULL;
}

Character* load_character( const char* name, long id )
{
  map<long, Character*>::iterator i = loaded_chars.find( id );
  Character* q;

  if ( i != loaded_chars.end() )
  	return i->second;
  q = new Character(name, id);
  return (loaded_chars[id] = q);
}

void unload_character( Character* ptr )
{
	map<long, Character *>::iterator j =
		loaded_chars.find(ptr->get_id());

	if (ptr->descriptors.size() == 0) {
		if (j != loaded_chars.end())
			loaded_chars.erase(j);
		delete ptr;
	}
}

void Object :: sendf( const char* fmt , ...)
{
	vector<Descriptor *>::iterator i;
	va_list ap;
	static char buf[MAX_STRING_LENGTH];

	if (!descriptors.size())
		return;

	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);

	for(i = descriptors.begin(); i != descriptors.end(); i++) {
		(*i)->send(buf);
	}
}

void Object :: send_mml( const char* fmt , ...)
{
        vector<Descriptor *>::iterator i;
        va_list ap;
        static char buf[MAX_STRING_LENGTH];

        if (!descriptors.size())
                return;

        va_start(ap, fmt);
        vsnprintf(buf, sizeof(buf), fmt, ap);
        va_end(ap);

        for(i = descriptors.begin(); i != descriptors.end(); i++) {
                (*i)->send_mml(buf);
        }
}


void Object :: send(const char* messg)
{
	vector<Descriptor *>::iterator i;

	for(i = descriptors.begin(); i != descriptors.end(); i++)
		(*i)->send(messg);
}

void Object :: put_line(const char* messg)
{
        vector<Descriptor *>::iterator i;

        for(i = descriptors.begin(); i != descriptors.end(); i++)
                (*i)->put_line(messg);
}



void Object :: link_descriptor( Descriptor* d ) {
	descriptors.push_back( d );
	d->login_character  = this;
}

void Object :: enqueue_command( char* command_line, long flags, Descriptor* s)
{
	static char buf[MAX_INPUT_LENGTH+8];
	int z;

	pack_int32(buf, flags);
	pack_int32(buf+4, s->get_fd());
	str_ncpy(buf+8, command_line, MAX_INPUT_LENGTH);
	z = 8 + strlen(buf+8);
	command_q.add( buf, &z );
}

void Object :: process_commands () 
{
	vector<Descriptor *>::iterator I;
	Descriptor* d;
	char buf[MAX_INPUT_LENGTH + 10];
	int l;
	long flags, fd;

	while (!command_q.empty())
	{
		l = command_q.get(buf, MAX_INPUT_LEN + 8);
		buf[l] = 0;
		flags = unpack_int32(buf);
		fd = unpack_int32(buf+4);
		for(I = descriptors.begin(); I != descriptors.end(); I++) {
			if ((*I)->get_fd() == fd)	
				break;
		}

		if (I == descriptors.end()) 
			d = NULL;
		else
			d = *I;
			
		if ( check_command ( this, d, buf + 8, l - 8) )
			continue;

		if ( d && *(buf + 8))
			d->put_line("Huh?!");
	}

}

void Object :: unlink_descriptor( Descriptor* d ) {
     vector<class Descriptor*>::iterator i;
     bool found = false;

     i = descriptors.begin();

     while( i != descriptors.end()) {
         if ( (*i) == d ) {
             i = descriptors.erase(i);
             found = true;
             break;
         }
     }

     /*if (descriptors.size() == 0) {
         unload_character ( this );
     }*/

     d->login_character = 0;
}

class Object* find_char_local( class Object* ch, const char* text )
{
	char buf[1024];
	Location* loc = ch->get_location();

	QueryResult r =
		gameDb.queryf("select id from character, location where "
		      "location.id = %d"
	              "lseg(point(loc_x,loc_y),point(loc_x,loc_y)) ?# "
		      "  location.floor"

		      " AND "

		      "lseg(point(loc_x,loc_z),point(loc_x,loc_z)) ?# "
		      "  location.vert"

		      " AND "

		      " name like '%s'",
		        loc->get_loc_id(),
			sqlEscape(text, buf)
		);

	if (r.is_error() || !r.next_row()) {
		return NULL;
	}
	return load_character( text, r.get_long("id") );
}
