/*
 *  OMud - Object Structure
 *
 *      For in game objects
 *
 *      --
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
#include "Database.h"
#include "Object.h"
#include "interp.h"

Object::~Object()
{
        if (!command_q.empty())
                command_q.make_empty();
}


QueryResult sql_get_location(int univ_x, int univ_y, int loc_x,
                             int loc_y, int loc_z)
{
	return gameDb.queryf(
	"select * "
	     " from locations where "
	     " 'POINT(%d %d 0)'::geometry && universe "
	     "AND 'POINT(%d %d %d)'::geometry && bounds ",
		univ_x, univ_y,

		loc_x, loc_y, loc_z
	);
}

class Location* Object::get_location()
{

	if ( location )
		return location;

	QueryResult r
		= gameDb.queryf("select z.ux,z.uy,X(z.c) AS cx,Y(z.C) AS cy,Z(z.C) as cz "
                                " from (select "
				"  X(universe) AS ux, Y(universe) AS uy, "
				"   centroid(bounds) AS c "
				"   from %s "
				"   where id=%d ) AS z",
				tbl_name, get_id());
	if (r.is_error() || !r.next_row())
		return NULL;

	QueryResult r2
		= sql_get_location(
			r.get_long("ux"),
			r.get_long("uy"),
			r.get_long("cx"), 
			r.get_long("cy"),
			r.get_long("cz")
		);
	if (r2.is_error() || !r2.next_row())
		return NULL;
	
	if ( !location ) {
		location = new Location;
		massert(location);

		location->set_id(r2.get_long("id"));
	}

	location->set_descr(r2.get_string("descr"));
	location->set_name(r2.get_string("name"));

	return location;
}
