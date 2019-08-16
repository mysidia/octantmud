/*
 *  [OBSOLETE] Database Handle Implementation
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
#include "Database.h"

#define TL_ESCAPE ((char)160)

DataHandle::DataHandle(Database* x)
 : h_key(0), h_value(0)
{
	fp = 0;
}

bool DataHandle::request_data(char* table_criteria, char* row_criteria)
{
	char buf[MAX_PATH_LEN];

        sprintf(buf, "%s/%s", table_criteria, row_criteria);
        if ((fp = fopen(buf, "r")))
        {
                return true;
        }
        return false;
}

bool DataHandle::iterate()
{
	bool ret;
	
	delete h_key;
	delete h_value;

	ret = nextcol(h_key, h_value);
	return ret;
}

void DataHandle::fill(char*& a, char *& b)
{
	a = h_key;
	b = h_value;
}

char* DataHandle::get_key() {
	return h_key;
}

char* DataHandle::get_value() {
	return h_value;
}
	


char* db_read_text_word( FILE* fp )
{
	static char buf[1024];
	int c, x = 0;

	do {
		if (feof(fp))
			return 0;
		c = fgetc(fp);
	} while(isspace(c));

	ungetc(c, fp);

	for ( ;; ) {
		c = fgetc(fp);
		if (feof(fp) || c == EOF)
			return 0;
		if (isspace(c))
			break;
		buf[x++] = c;
	}
	buf[x++] = '\0';
	return buf;
}


/*bool DataHandle::writecol(char *a, char*b)
{
	bool fail = false;
	long whence = ftell(fp);

	if ( whence == -1 )
		return false;
	
	if ( fprintf(fp, "%-15s ", a) < 0 )
		fail = true;

	while( *b && !fail ) {
		switch(*b)
		{
		default:
			if ( fputc((int)(*b), fp) == EOF )
				fail = 1;
		case '\r':
		case EOF: break;
		case TL_ESCAPE:
		case '~':
			if ( fputc((int)TL_ESCAPE, fp) == EOF )
				fail = 1;
			if ( !fail && fputc((int)(*b), fp) == EOF )
				fail = 1;;
		break;
		}
	}
	if (!fail) {
		fputc('\n', fp);
	}

	if ( fail ) {
		log_print("Database write failed: %s", strerror(errno));
		fseek( fp, SEEK_SET, whence );
		return false;
	}
	return true;
}*/

bool DataHandle::nextcol(char *&a, char*&b)
{
	char buf[8192];
	int c, d, x = 0, sp = 0;
	bool esc = false;

	a = b = 0;

	c = fgetc(fp);
	if (c != 0 && c != '1' && c != '2' && c != '3' && c != '4')
		return false;
	d = c;
	a =  db_read_text_word( fp );
	if (!a)
		return false;
	a = str_dup(a);

	do {
		if (feof(fp))
			return false;
		c = fgetc(fp);
	} while ( isspace (c) );


	if (d == '1') {
		while( c != EOF && c != '\n') {
			if (feof(fp))
				return false;
			
			buf[x++] = c;
			c = fgetc(fp);
		}
		b = str_dup(buf);
		return true;
	}
	else if (d == '2')
	{
		if (c == TL_ESCAPE) {
			esc = true;
		}
		else if (c == '~') {
			b = str_dup("");
			return true;
		}
		else {
			buf[x++] = c;
		}

		for ( ;; )
		{
			if ( x >= 8000 ) {
				buf[x++] = '\0';
				b = str_dup(buf);
				return false;
			}
			buf[x] = fgetc ( fp );
			if (buf[x] == '\n') {
				buf[++x] = '\r';
				esc = false;
			}
			else if (buf[x] == '\r')
				esc = false;
			else if (buf[x] == EOF) {
				buf[x] = '\0';
				b = str_dup(buf);
				return false;
			}
			else if (buf[x] == '~' && !esc) {
				buf[x] = '\0';
				b = str_dup(buf);
				return true;
			}
			else if (buf[x] == TL_ESCAPE && !esc) {
				esc = true;
			}
			else {
				esc = false;
				x++;
			}
		}
		b = str_dup(buf);
		return true;
	}
	return false;
}
