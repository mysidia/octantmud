/*
 *  OMud - Command Interpreter Structures
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

#ifndef __interp_h__
#define __interp_h__

#include "Descriptor.h"
#include "Object.h"
#include "Character.h"

#define RETURN(x) \
		

struct Cmd_Return;

#define num_victs() \
		(args->num_victims)

#define vict(n) \
		(args->pvict[(n)])

#define vict_id(n) \
		(args->pvict_id[(n)])

typedef void DO_FUN (	
		class Object*,
		class Descriptor*,
		const struct Interp_Command*,
		const struct Interp_Arguments*,
		int flags,
		Cmd_Return& cmd_ret
		);

typedef bool  ARG_PROC_FUN (
			class Object* ch,
			struct Interp_Arguments* arg_data,
			char* text
		);

#define COMMAND(x) \
void		x ( class Object* ch, \
		    class Descriptor* desc, \
		    const struct Interp_Command* cmd_info, \
	  	    const struct Interp_Arguments* const arg, \
		    int flags, Cmd_Return& cmd_ret )

#define SKILL(x) COMMAND(x)

#define PARSER(x) \
bool		x ( class Object* ch, \
		    struct Interp_Arguments* arg_data, \
		    char* text )
		

struct Cmd_Return
{
	long		status;
	bool		ch_killed;
	bool		vict_char_killed;
	bool		vict_obj_killed;

	public:
	
	Cmd_Return() :
		status(0), ch_killed(0), vict_char_killed(0),
		vict_obj_killed(0) { }

	Cmd_Return(long code) :
		status(code), ch_killed(0), vict_char_killed(0),
		vict_obj_killed(0)
	{
	}


	Cmd_Return operator| (const Cmd_Return& y) const {
		Cmd_Return q = *this;

		if ( y.ch_killed )
			q.ch_killed = true;

		if ( y.vict_char_killed )
			q.vict_char_killed = true;

		if ( y.vict_obj_killed )
			q.vict_obj_killed = true;
		return q;
	}

	Cmd_Return operator|= (const Cmd_Return &y) {
		return (*this) = (*this) | y;
	}
};


struct Interp_Arguments
{
	char* 			text;

	int			num_victims;
	Object**		pvict;
	long long*		pvict_id;

	void clear();
	void add_target(Object*);
	Interp_Arguments();

};

struct Interp_Command
{
	char*	name;
	DO_FUN* func;
	ARG_PROC_FUN* parser;

	int	command_class;
	int	min_level;
	int	min_position;
	int	battle_time;
	int	log;
	int	flags;
	int	sortpos;
	int	lag_count;
	int	mana_cost;
	int	move_cost;
	int	subcmd;

	LIST_ENTRY(Interp_Command) cmd_lst;
};

bool check_command (class Object*, class Descriptor*, char*, int l);
bool check_skill   (class Object*, class Descriptor*, char*, int l);
bool check_social  (class Object*, class Descriptor*, char*, int l);
bool check_special (class Object*, class Descriptor*, char*, int l);

#endif
