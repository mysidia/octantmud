/*
 *  OMud - Object Header
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



#ifndef __Object_h__
#define __Object_h__

#include "mud.h"
#include "TextQueue.h"
#include "GameString.h"
#include "Account.h"

#define STAT_STR "s_str"
#define STAT_INT "s_int"
#define STAT_WIS "s_wis"
#define STAT_DEX "s_dex"
#define STAT_CON "s_con"
#define STAT_CHA "s_cha"
#define STAT_LUCK "s_luck"
#define STAT_TRAINS "train_points"

#define		CMD_COMM	0x0001

class Location;

class Object
{
	protected:
	friend void unload_object(Object* ptr );
	friend Object* find_object( long id );
	friend Object* load_object( const char*, long id );
	
	vector<class Descriptor*> descriptors;
	long id;
	char name[255];
	class Location* location;
	TextQueue command_q;
	char* tbl_name;
	char* id_col;

	public:
	~Object();

	vector<class Descriptor *>& get_desc_vec() { return descriptors; }
	void link_descriptor( class Descriptor* d );
	void unlink_descriptor( Descriptor* d );
	void enqueue_command( char*, long, Descriptor* );
	void send( const char* );
	void put_line( const char* );
	void send_mml( const char*, ...);
	void sendf( const char*, ...);
	long get_id() { return id; }
	const char* get_name() const { return name; }
	void process_commands( );
	virtual bool is_mobile() const { return false; }

	const char* get_x_f( const char* field ) {
                QueryResult r =
                        gameDb.queryf("select "
                                      "%s from character "
                                      "where id=%d", field, this->get_id());
                const char* s;

                if (r.is_error() || !r.next_row())
                        return "null";
                s = r.get_string(field);
                return s ? s : "null";
	}

	bool set_gender(const char* target) {
		QueryResult r =
			gameDb.queryf("update character "
				      "set gender='%s' where id=%d",
				      target, this->get_id());
		if (r.is_error())
			return false;
		return true;
	}

        const char* get_gender() { return this->get_x_f("gender"); }
	const char* get_race()   { return this->get_x_f("race"); }

	bool set_stat(const char* field, long value) {
                QueryResult r =
                        gameDb.queryf("update character set %s=%ld"
                                      "where id=%d", field, value, this->get_id());
		if (r.is_error())
			return false;
		return true;
	}

	long get_stat(const char* field) {
		QueryResult r =
			gameDb.queryf("select %s from character "
				      "where id=%d", field, this->get_id());
		if (r.is_error()) {
			log_print("Database error: getting stat %s for "
			          "player #%d", field, this->get_id());
			abort();
		}
		if (r.next_row())
			return r.get_long(field);
		return 0;
			
	}

	long get_hit() { return get_stat("hp"); }
	long get_nat_hit() { return get_stat("nat_hp"); }

	long get_mana() { return get_stat("mp"); }
	long get_nat_mana() { return get_stat("nat_mp"); }

	long get_move() { return get_stat("mv"); }
	long get_nat_move() { return get_stat("nat_mv"); }

	long get_level() { return get_stat("level"); }

	class Location* get_location();
	
	Object ( const char in_name[255], long in_id ) {
		id = in_id;
		location = NULL;
		strcpy(name, in_name);
		tbl_name = "object";
		id_col = "id";
	}
};

#endif
