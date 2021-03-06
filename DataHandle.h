/*
 *  OMud - DataHandle Structure
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


#ifndef __DataHandle_h__
#define __DataHandle_h__

#include <stdio.h>
#include "Database.h"

class DataHandle
{
	public:
		DataHandle(Database*);
		~DataHandle() { 
			if (fp)
				fclose(fp); 
			delete h_key;
			delete h_value;
		}
		bool request_data(char* table_criteria, char* row_criteria);
		bool nextcol(char *&a, char*&b);
		void fill(char *&a, char*&b);
		bool iterate();
		char* get_key();
		char* get_value();

	private:
		FILE* fp;

		char* h_key, * h_value;
};
#endif
