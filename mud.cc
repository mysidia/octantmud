/*
 *  Mud Server Entry Point and supplemental Routines
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
#include "Game.h"
#include "Parser.h"
#include "Database.h"
#include "Descriptor.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>

class Database gameDb;
vector<Game *> running_games;
time_t boot_time;

int str_cmp( const char* A, const char * B )
{
	assert(A && B);

	for( ; *A && toupper(*A) == toupper(*B) ; A++, B++) 
		; /* Search string for a difference */

	/*
	 * Branch if what we have is unequal
	 */
	if (*A != *B)
		return (*A > *B) ? 1 : -1;

	/* Return 0 if the two strnigs are equal */
	return 0;
}

int strn_cmp( const char* A, const char * B, int C )
{
	int i = 0;
        assert(A && B);

        for( ; A[i] && toupper(A[i]) == toupper(B[i]) && i < C ; i++)
                ; /* Search string for a difference */

        /*
         * Branch if what we have is unequal
         */
        if (A[i] != B[i])
                return (A[i] > B[i]) ? 1 : -1;

        /* Return 0 if the two strnigs are equal */
        return 0;
}


bool is_abbrev( const char* in_string, const char* abbrev_for )
{
	for( ; *in_string && *abbrev_for ; ) {
		if (UPPER(*in_string) != UPPER(*abbrev_for))
			return false;
		in_string++;
		abbrev_for++;
	}
	if (*in_string)
		return false;
	return true;
}

void log_print( const char* fmt, ...)
{
	char buf[1024 + 512];
	int l;
	va_list ap;

	buf[0] = '\0';
	l = sprintf(buf, " ::");

	va_start(ap, fmt);
	vsnprintf(buf + l, 1024, fmt, ap);
	va_end(ap);

	fputs(buf, stderr);
	fputs("\n", stderr);
}

void massert(void *x) {
        if (x == 0) {
                log_print("Out of memory");
                abort();
        }
}

void copy_alloc_str(char *&a, const char *b)
{
	a = new char [strlen(b) + 1];
	strcpy(a, b);
}

char* str_ncpy(char *d, const char* s, int n)
{
	int i;

	if (n < 1) {
		return NULL;
	}
	
	for(i = 0; i < (n - 1) && *s; i++) {
		*d++ = *s++;
	}
	*d++ = '\0';
}

int nonblock(int fd, char *errmsg) {
 int z = 1, k;

 if (k = ioctl(fd, FIONBIO, &z) < 0) {
     sprintf(errmsg, "ioctl(Non block): %s", strerror(errno));
     return k;
 }
 return k;
}

void ltrim_space(char *&x)
{
        while(isspace(*x))
              x++;
}

char* split(int* count, char* buf, char target[][MAX_INPUT_LEN] ...)
{
	va_list ap;
	char* b;

	va_start(ap, target);
	b = vsplit(count, buf, target, (Tok_Type)va_arg(ap, int), ap);
	va_end(ap);
	return b;
}

char* vsplit(int* count, char* buf, char target[][MAX_INPUT_LEN], Tok_Type in, va_list ap)
{
	Tok_Type p = in;
	int i, l, x, fl, es;

	if (!buf || !*buf) {
		*target[0] = '\0';
		return buf;
	}

	if (p == TOK_SKIP) {
		p = (Tok_Type)va_arg(ap, int);
		if (p == END)
			return buf;
	}

	while (isspace(*buf))
		buf++;

	x = 0;

	do
	{
		fl = es = 0;
		l = 0;

		if (!buf[0]) {
			target[x++][0] = 0;
			continue;
		}

		for(i = 0; buf[i]; i++) {
			if (!es && p == PHRASE && buf[i] == '\''
			     && (i == 0 || fl)) 
			{
				fl = fl ? 0 : 1;
				continue;
			}

			if (!fl && (isspace(buf[i]))) {
				while(isspace(buf[i+1]))
					i++;
				break;
			}

                        if (!es && p == PHRASE && buf[i] == '\\') {
				es = 1;
				continue;
			}
			target[x][l++] = buf[i];
			
			es = 0;
		}

		{
			/*l = i;
			for(i = 0; i < l; i++) {
				/*printf("target[%d][%d]='%c'\n", 
					x,i,buf[i]);//
				target[x][i] = buf[i];
			}*/
			target[x][l] = 0;
			x++;

			if (i > 0) {
				if (buf[i]) {
					buf++;
					(*count)++;
				}
				buf += i;
			}
		}
	} while ( (p = (Tok_Type) va_arg(ap, int)) != END );

	(*count)++;
	return buf;
}

char* str_dup(const char *x)
{
	char* v = new char[ strlen(x)+1 ];

	massert(v);
	strcpy(v , x);
	return v;
}

char* capitalize(char* string)
{
	static char buf[MAX_STRING_LENGTH];
	char *p = buf;

	if (!string)
		return string;
	*(p++) = UPPER(*(string++));

	while(*string) {
		*(p++) = LOWER(*(string++));
	}
	*(p++) = '\0';

	return buf;
}

void close_socket(int fd)
{
	close(fd);
}


bool legal_account_name(Descriptor* d, const char* name, const char** errmsg)
{
	int l = strlen(name);
	const char *s;

	if (l >= ACCT_NAME_LEN) {
		*errmsg = "name too long";
		return false;
	}

	if (l < 3) {
		*errmsg = "the name too short";
		return false;
	}

	if (!isalpha(name[0])) {
		*errmsg = "the name does not begin with a letter";
		return false;
	}

	s = name;
	while ( *s ) {
#ifdef ALLOW_UTF8		
		if ((!isalnum(*s) && (*s & 0x80) == 0 ) )
#else
		if (!isalnum(*s))
#endif
		{
			*errmsg = "the name has special characters";
			return false;
		}
		s++;
	}
	return true;
}

bool legal_character_name(Descriptor* d, const char* name, const char** errmsg)
{
	int l = strlen(name), spc = 0;
	const char *s;

	if (l >= NAME_LEN) {
		*errmsg = "name too long";
		return false;
	}

	if (l < 3) {
		*errmsg = "the name too short";
		return false;
	}

	if (!isalpha(name[0])) {
		*errmsg = "the name does not begin with a letter";
		return false;
	}

	s = name;
	while ( *s ) {
		if (*s == ' ' && *(s + 1) == ' ') {
			*errmsg = "the name has two adjacent spaces";
			return false;
		}
		else if (*s == ' ')
			spc++;
#ifdef ALLOW_UTF8		
		else if ((!isalnum(*s) && (*s & 0x80) == 0 ) )
#else
		else if (!isalnum(*s))
#endif
		{
			*errmsg = "the name has special characters";
			return false;
		}
		s++;
	}
	if ( s[-1] == ' ' ) {
		*errmsg = "the name has trailing space";
		return false;
	}

	if (spc > 3) {
		*errmsg = "the name has more than 3 spaces";
		return false;
	}
	return true;
}

bool valid_email(const char* email)
{
	const char* s = email;
	const char* uh = 0;
	int dot = 0;

	if (strlen(email) >= EMAIL_LEN || strlen(email) < 3)
		return false;

	while( *s ) {
		if (*s == '@') {
			if (! uh)
				uh = s;
			else
				return false;
		}
		
		if (!isalnum(*s) && (*s != '@') &&
				(*s != '.') &&
				(*s != '_') &&
				(*s != '-') &&
				(*s != '#'))
			return false;
		s++;
	}

	if ( uh ) {
		while( *uh ) {
			if (*uh == '.')
				dot = 1;
			uh++;
		}
		if (!dot) {
			return false;
		}
	} else return false;

	return true;
}

bool should_accept_email(const char* email, Descriptor* d)
{
	if (!valid_email(email)) {
		if ( d )
			d->put_line("That e-mail address is not valid.");
		return false;
	}

	return true;
}

bool complete_abbr(const char* in, const char* txt, char * out)
{
	if (!is_abbrev(in, txt))
		return false;
	strcpy(out, txt);
	return true;
}


char* sqlEscape(const char* input, char* result, char wc_esc) {
        const char *p;
	char *result_i = result;

        if (!input) return 0;

        for(p = input; *p != '\0'; p++) {
                if (*p == '\'')
                        *result++ = '\'';
		else if (*p == '\\')
			*result++ = '\\';
                else if (wc_esc) {
                        if (*p == wc_esc || *p == '%' || *p == '_')
                                *result++ = wc_esc;
                }
                *result++ = *p;
        }
        *result++ = 0;

        return result_i;
}


int main ( ) {
	Game x;
	char asdf[512] = "<font color='red'>Mysidia the insane&trade;</font>\n";

	parse_mml(0, asdf);
	printf("_%s_\n", asdf);

	boot_time = time(0);

	if (x.init() == false) {
		log_print("Unable to initialize game");
	}

	load_player_commands( );
	load_player_stats( );
	running_games.push_back(&x);

	while (x.get_state() == Game::MUD_RUNNING) {
		x.run();
		x.io();
	}

	x.end();
}
