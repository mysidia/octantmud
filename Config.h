/*
 *  OMud - Mud Configuration Structure
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

#ifndef __config_h__
#define __config_h__

#include <stdlib.h>
#include "mud.h"

/**
 * This is a temporary module to take the place of the system
 * for reading certain configuration items from a file
 * and certain configuration items from a DBM for testing
 * purposes until it can be completed.
 */

struct Config_str_pair { const char* key, *value; };
extern struct Config_str_pair Config_pairs[];

class Config {
	public:


	Config( const char* file_name );

	/*! Does a given key exist in the configuration? */
	bool exists(const char* key) {
                int i = 0;

                for( i = 0; Config_pairs[i].key ; i++ )
                        if (!str_cmp(Config_pairs[i].key, key))
                                return 1;
                return 0;
	}
	
	/*! Get the string value assigned to a key */
	const char* get_str(const char* key) {
		int i = 0;

		for( i = 0; Config_pairs[i].key ; i++ )
			if (!str_cmp(Config_pairs[i].key, key))
				return Config_pairs[i].value;
		return "";
	}

	/*! Get the value assigned to a key, converted to an int */
	const int   get_int(const char* key) {
		const char *s = key;

		assert ( key );
		s = get_str(s);

		return s ? atoi(s) : 0;
	}

	/*! Convert the value, parsed as a long */
	const long   get_long(const char* key, int base = 10) {
		const char *s = key;

		assert ( key );
		s = get_str ( key );

		return s ? strtol(s, (char **)0, base) : 0;
	}
};

#endif
