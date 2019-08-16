/*
 *  OMud - Account Structure
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

#include "mud.h"
#include "Descriptor.h"
#include "Account.h"
#include "DataHandle.h"
#include <dbi/dbi.h>

map<string,Account*> new_accounts;
map<string,Account*> loaded_accounts;

/**
 * Get a string value from a column named @var f in
 * the Account database table
 *
 * returns as a static.
 */
const char* Account :: get_string_field(const char* f)
  const
{
	/*char buf[512];
	
	sprintf(buf, "select %s from account where acct_id=%ld", f, 
	             this->get_id());*/

	QueryResult r = gameDb.queryf("select acct_id, %s from account where acct_id=%ld",
				f, this->get_id());

	if (r.is_error() || r.row_count() != 1)
		return NULL;

	if (!r.next_row())
		return NULL;
		
	return string(r.get_string(f)).c_str();
}

/**
 * @return True if account named ac_name exists
 * @param ac_id Pointer to be filled with account id
 * @param active Pointer to be filled with activated/approved status
 */
bool account_exists( char * ac_name, long* ac_id, bool* active)
{
	QueryResult r = gameDb.queryf("select * from account where acct_name='%s'",
	                              ac_name);	

	if (r.is_error()) {
		return false;
	}

	if (r.row_count() == 1) {
		if ( ac_id && r.next_row() ) {
			*ac_id = r.get_long("acct_id");

			*active = (r.get_datetime("acct_activated") == 0 ? false : true);
		}
		return true;
	}

	return false;
}

/**
 * Unmaps a mapped account, part of 'unloading' and 'deletion'
 * processes.
 */
void unmap_account( const char* name ) {
	map<string, Account*>::iterator i
			= loaded_accounts.find ( name );

	if ( i != loaded_accounts.end() )
		loaded_accounts.erase(i);
}

/**
 * Find a mapped (loaded) account
 */
Account* find_account( char* name ) {
	map<string, Account*>::iterator i
			= loaded_accounts.find( name );
	
	if (i != loaded_accounts.end() )
		return i->second;
	return NULL;
}

/**
 * Try to load an account
 */
bool load_account( char* name, Account ** ret_acct )
{
	bool active;
	Account* ac = 0;
	map<string, Account*>::iterator i;
	long ac_id;

	i = loaded_accounts.find( name );

	if ( i != loaded_accounts.end() ) {
		ac = (i->second);
	}
	else {
		ac = new Account ( name );
		loaded_accounts[string(name)] = ac;
		i = loaded_accounts.find( name );
	}

        if (!account_exists(name, &ac_id, &active)) {
		unmap_account(name);
		delete ac;

		return false;
	}

	ac->is_active_fl = active;
	ac->acct_id = ac_id;
	*ret_acct = ac;

	return true;
}


bool OLD_load_account( char* name, Account ** acct )
{
        char buf[MAX_PATH_LEN+1], *a, *b;
	bool r;

	DataHandle* h = new DataHandle(0);

	sprintf(buf, "db/acct/%c", toupper(name[0]));
	if (h->request_data(buf, capitalize(name))) {
		while (h->iterate()) {
			h->fill(a, b);
			log_print("Got pair: (%s, %s)", a, b);
		}
	}

	delete h;
	return false;

/*        sprintf(buf, PLAYER_DIR "/%c/%s",
                        toupper(name[0]), capitalize(name));
	if ((fp = fopen(buf, "r")))
	{
		Account* a = new Account();

		r = a->input ( fp );
		fclose( fp );

		return r;
	}
	return false;*/
}

/**
 * Set the file descriptor of the online user connected to the account
 */
void Account :: set_descriptor(Descriptor* d)
{
	acct_desc = d;
}


/**
 * OBSOLETE: Receive input from a Data Handle
 *
 * This was part of the file-based database system model
 */
bool Account :: input (DataHandle *dh)
{
	return false;
}

/* */
