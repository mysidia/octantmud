/*
 *  OMud - Account Header
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

#ifndef __Account_H__
#define __Account_H__
#include "mud.h"

class Descriptor;

class Account
{
	public:
		friend bool load_account( char* name, Account** acct );
			
		Account ( const char* name ) : 
			acct_name(str_dup(name)), inuse(0)
		{
			is_active_fl = 0;
			acct_desc = 0;
		}

		Account( const Account& b )
		{
			copy_alloc_str(acct_name, b.acct_name);
		}

		~Account ( )
		{
			delete acct_name;
		}

		void set_descriptor( Descriptor* );
		long  get_id ( ) const { return acct_id; }
		bool  is_active ( ) { return is_active_fl; }
		void  set_active( bool fl ) { is_active_fl = fl; }
		char* get_name( ) { return acct_name; }
		const char* get_string_field(const char*) const;
		const char* get_passwd() const { return get_string_field("acct_passwd"); }
		const char* get_email() const { return get_string_field("acct_email"); }
		Descriptor* get_descriptor() { return acct_desc; }

	private:
		bool input(DataHandle *);

		Descriptor* acct_desc;
		char* acct_name;
		long  acct_id;
		bool  is_active_fl;
		
	public:
		int inuse;

};


extern map<string,Account*> loaded_accounts;

Account* find_account( char* name );
bool load_account( char* name, Account ** acct );
bool account_exists( char* name, long* id, bool*);
#endif
