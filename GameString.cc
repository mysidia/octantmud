/*
 *  OMud - Game String Structure
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


#define __GameString_cc__
#include "mud.h"
#include "GameString.h"
#include "TextQueue.h"
#include <stdio.h>
#include <errno.h>

void GameString :: load( String_Type type, char* data, char* name )
{
	char buf[MAX_INPUT_LEN];
	int l, m = 0, err = 0;

	str = 0;

	if (type == STR_TEXT) {
		str = str_dup(data);
	}
	else if (type == STR_FILE)	
	{
		FILE* fp = fopen(data, "r");

		if ( !fp ) { 
			log_print("fopen: unable to read %s (%s)",
					data, strerror(errno));
			str = str_dup("");
		}
		else
		{
			if ( fseek( fp, 0, SEEK_END) < 0 ) {
				str = str_dup("");
				err = 1;
				perror("fseek");
			}

			m = ftell(fp);

			if ( fseek( fp, 0, SEEK_SET) < 0) {
				str = str_dup("");
				err = 1;
				perror("fseek");
			}

			if (!err)
			{
				str = new char[m + 1];
				str[0] = '\0';
				l = 0;

				while( (m = fgetc(fp)) != EOF ) {
					str[l++] = m;
				}
				str[l] = '\0';
				if (l > 0 && str[l-1] == '\n')
					str[l-1] = '\0';
				l++;
			}
		}
	}		
}
