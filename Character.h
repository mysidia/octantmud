/*
 *  OMud - Character header
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

#ifndef __Character_h__
#define __Character_h__

#include "mud.h"
#include "TextQueue.h"
#include "GameString.h"
#include "Account.h"
#include "Object.h"
#include "Location.h"

#define STAT_STR "s_str"
#define STAT_INT "s_int"
#define STAT_WIS "s_wis"
#define STAT_DEX "s_dex"
#define STAT_CON "s_con"
#define STAT_CHA "s_cha"
#define STAT_LUCK "s_luck"
#define STAT_TRAINS "train_points"

#define		CMD_COMM	0x0001

class Character : public Object
{
	friend void unload_character( Character* ptr );
	friend Character* find_character( long id );
	friend Character* load_character( const char*, long id );
	
	public:
	~Character();

	virtual bool is_mobile() const { return true; }

	Character ( const char in_name[255], long in_id )
		: Object(in_name, in_id) {
		id = in_id;
		tbl_name = "character";
		id_col = "id";
		strcpy(name, in_name);
	}
};

extern Character* find_character( long id );
extern Character* load_character( const char*, long id );
#endif
