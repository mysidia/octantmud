/*
 *  OMud - Preliminary Config implementation (STUB CODE)
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

struct Config_str_pair Config_pairs[] = {
	{"host",	"*"},
	{"port",	"1234"},
#if 0
	{"uid",		"505"},
#endif
	{"seed1",	"0x36822"},
	{"seed2",	"0x33586"},
	{"seed3",	"0x552268"},
	{"seed4",	"0x123456"},
	{"char_limit",	"10" },

	{NULL, NULL}
};


Config :: Config (const char* file_name)
{
	  /* Load a configuration from a file */

}

