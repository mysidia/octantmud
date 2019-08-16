/*
 *  OMud - Database Structure Header
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

#ifndef __Database_h__
#define __Database_h__
#include <stdio.h>
#include <stdarg.h>
#include <dbi/dbi.h>
#include "QueryResult.h"

void db_prep_dbi();

/**
 * This is the prototype for the gameDb object whose function is
 * to provide a database interface to the mud system.
 */
class Database
{
	public:
		dbi_conn conn;

		/**
		 * @return true if gameDb object is unconnected or
		 * otherwise in error.
		 */
		bool is_error() const {
			return conn == NULL;
		}

		/**
		 * Submit a query to the database
		 * @pre Database::start() has been called.
		 * @return a query result object
		 */
		QueryResult query(const char* buf) {
                        dbi_result res;
                        const char* p;

                        res = dbi_conn_query(conn, buf);
                        if (res == NULL) {

                                dbi_conn_error(conn, &p);
                                log_print("Error querying database: %s", p);
				return QueryResult(NULL);
                        }

                        return QueryResult(res);
		}

		/**
		 * Same as query() but allows a format string.
		 * printf style.
		 */
		QueryResult queryf(const char* text, ...) {
			static char buf[8192];
			va_list ap;
			dbi_result res;
			const char* p;

			va_start(ap, text);
			vsnprintf(buf, sizeof(buf), text, ap);
			va_end(ap);

			log_print("Executing Query: %s [%p]", buf, conn);

			res = dbi_conn_query(conn, buf);
			if (res == NULL) {

				dbi_conn_error(conn, &p);
				log_print("Error querying database: %s", p);
				return QueryResult(NULL);
			}

			return QueryResult(res);
		}

		~Database() {
			log_print("Db.close");
			dbi_conn_close(conn);
		}

		/**
		 * Initialize the database
		 */
		void start() {
			db_prep_dbi();

			log_print("Database::start");

			conn = dbi_conn_new("pgsql");
			if (conn == NULL)
				abort();

			dbi_conn_set_option(conn, "host", "localhost");
			dbi_conn_set_option(conn, "username", "mud");
			dbi_conn_set_option(conn, "password", "mud");
			dbi_conn_set_option(conn, "dbname", "mud");

			if ( dbi_conn_connect(conn) < 0 ) {
				const char *p;

				dbi_conn_error(conn, &p);

				log_print("Unable to connect to database: %s", p);
				exit(0);
			}
		}

};

extern class Database gameDb;
#include "DataHandle.h"
#endif
