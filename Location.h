/*
 *  OMud - Location Header
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

#ifndef __Location_h__
#define __Location_h__

#include "mud.h"
#include "TextQueue.h"
#include "GameString.h"
#include "Account.h"
#include "Character.h"

class Location
{
	private:
		int lx_x, lx_y, lx_z, locid;
		char* descr, *name;

		int refcount, can_free;
	
	public:

		int get_loc_id() const { return locid; }
		
		/* Lowest x */
		int get_x0() { return lx_x; }

		/* Matching y for lowest x */
		int get_y0() { return lx_y; }

		/* Matching z for lowest x */
		int get_z0() { return lx_z; }

		void set_id(int ilocId) {
			locid = ilocId;
		}

		void set_descr(const char* descr) {
			if ( this->descr )
				delete this->descr;

			if ( descr )
				this->descr = str_dup(descr);
			else
				this->descr = NULL;
		}

		void set_name(const char* name) {
			if ( this->name )
				delete this->name;

			if ( name )
				this->name = str_dup(name);
			else
				this->name = NULL;
		}

		const char* get_name() const {
			return name;
		}

		const char* get_descr() const {
			return descr;
		}

		void zero_vars() {
			locid = 0;
		}
		
		void dont_free() {
			can_free = 0;
		}

		void add_ref() {
			refcount++;
		}

		void sub_ref() {
			refcount--;
		}

		Location() : descr(0), name(0) {
			zero_vars();
			refcount = 1;
			can_free = 1;
		}


		Location(Location &x) {
			zero_vars();
			refcount = 1;
			can_free = 1;

			if (x.descr)
				set_descr(x.descr);
			else
				descr = NULL;


			if (x.name)
				set_name(x.name);
			else
				name = NULL;
			locid = x.locid;
		}
		
};

#endif
