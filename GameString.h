/*
 *  OMud - Game String Structure
 *  Copyright (c) 2003, Mysidia, All Rights Reserved
 *    EXHIBIT A
 *
 *    The Notice below must appear in each file of the Source Code of any copy You distribute of
 *    the Licensed Software or any Extensions thereto, except as may be modified as allowed
 *    under the terms of Section 7.1
 *
 *      Copyright (C) 2003-2004 Mysidia, All Rights Reserved.
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

#ifndef __GameString_h__
#define __GameString_h__

enum String_Type
{
	STR_TEXT,
	STR_FILE
};

class GameString
{
	public:
		GameString( String_Type type, char* data, char* name )
			: str(0)
		{
			load(type, data, name);
		}
		~GameString( ) {
			if ( str )
				delete str;
		}
		void load( String_Type type, char* data, char* name );
		void clear() {
			delete str;
		}

		char* get_str() { return str; }

	private:
		char* str;
};

#ifdef  __GameString_cc__
#define STRING(var, typ, dat, nm) \
	GameString var((typ), (dat), (nm))
#else
#define STRING(var, typ, dat, nm)  extern GameString var
#endif



STRING( WELCOME_SCREEN, STR_FILE, LIB_TEXT WELCOME_FILE, "welcome_screen" );
STRING( NAME_POLICY_LECTURE, STR_FILE, LIB_TEXT NAME_LECTURE_FILE, "name_policy" );

#endif
