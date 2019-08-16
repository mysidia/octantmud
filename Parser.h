/*
 *  OMud - Buffer Parsing Header
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



#include "mud.h"
#include <stdio.h>
#include <stdarg.h>

/*
 * Maximum number of arguments
 */
const int MAX_ARG = 10;

class Parser
{
	public:
		/* Setup for parse at max args (at most) and buffer buf */
		Parser( int max, char*& buf ) : argc(0) {
			char nb[MAX_INPUT_LEN];
			int count = 0, i, es = 0, fl = 0, sp = 1, x = 0;
			int v = 0, m = 0;
			bool exists = false;

			while(isspace(*buf))
				buf++;


			for(i = 0 ; buf[i]; i++)
			{
				exists = true;

				if (!fl && !es && sp && buf[i] == '\'') {
					fl = 1;
					sp = 0;
					continue;
				} else if (!es && fl && buf[i] == '\'') {
					fl = 0;
					sp = 0;
					continue;
				}
				
				if (!fl && isspace(buf[i])) {
					while(isspace(buf[i+1])) {
						i++;
					}
					sp = 1;
					if ( ++count >= max )
						break;
					nb[x++] = '\0';
					continue;
				}
				else
					sp = 0;

				if (!es && buf[i] == '\\') {
					es = 1;
					continue;
				}

				if (es && buf[i] != '\'')
					nb[x++] = '\'';
				nb[x++] = buf[i];
				
				es = 0;
			}

			nb[x++] = '\0';

			args = new char[count + 1][MAX_INPUT_LEN];
			for(i = 0; i < x; i++) {
				args[v][m++] = nb[i];
				if (nb[i] == '\0') {
					m = 0;
					v++;
				}
			}

			if ( exists )
				argc = ++count;
			else
				argc = count;
//			buf = ::split( &argc, buf, args, PHRASE, END);
		}
		
		Parser( char *&buf, ... ) : argc(0) {
			va_list ap;
			int x = 0;
			Tok_Type p;

			va_start(ap, buf);
			while ( (p = (Tok_Type) va_arg(ap, int)) != END ) {
				if (p == TOK_SKIP)
					continue;
				x++;
			};
			va_end(ap);

			args = new char [x][MAX_INPUT_LEN];

			va_start(ap, buf);
			buf = ::vsplit( &argc, buf, args, TOK_SKIP, ap );
			va_end(ap);
		}
		~Parser(  ) {
			delete args;
		}

		int num() const {
			return argc;
		}

		int int_arg( int n ) {
			char*s = arg(n);

			return atoi(s);
		}

		long long_arg( int n, int base = 10 ) {
			char* s = arg(n);

			return strtol(s, (char **)0, base);
		}

		unsigned long ulong_arg( int n, int base = 10 ) {
			char* s = arg(n);

			return strtoul(s, (char **)0, base);
		}

		char arg_pos( int n, int y ) {
			char *s = arg(n);

			if (y < 0)
				abort();

			return s[y];
		}

		char ch_uc( int n, int y ) {
			char* s = arg(n);

			return UPPER(s[y]);
		}

		char ch_lc( int n, int y ) {
			char* s = arg(n);

			return LOWER(s[y]);
		}

		char *arg( int n ) {
			if ( n < 0 ) {
				log_print("Parser.arg :: n(%d) < 0 requested", n);
				abort();
			}
			if (n >= argc ) {
				log_print("Parser.arg :: n(%d) > argc(%d) requested", n, argc);
				abort();
			}

			return args[n];
		}
		
	private:
		char (*args)[MAX_INPUT_LEN];
		int  argc;
};
