/*
 *  Command Interpreter
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
#include "Game.h"
#include "interp.h"
#include "Location.h"
#include "Parser.h"

#define CMD_HASHSIZE 127

// LIST_HEAD(, Interp_Command) ;

// static struct Interp_Command * command_hash[CMD_HASHSIZE];

static LIST_HEAD(, Interp_Command) command_hash[CMD_HASHSIZE];

#define CMD_FN(x) \
		{ #x, x, NULL }
#define PARSE_FN(x) \
		{ #x, NULL, x }

struct plr_command_function_t
{
	char* func_name;

	DO_FUN* cmd_func;
	ARG_PROC_FUN* parse_func;
};

#include "cmdlist.h"

static int interp_hash_nn(const char* cmd_name)
{
	int y;

	y = (unsigned char)(cmd_name[0]);
	if ( y < 0 )
		y = -y;

	y %= CMD_HASHSIZE;
	return y;
}

struct Interp_Command * lookup_command( const char* cmd_name, int cclass )
{
	struct Interp_Command* p;
	int y;

	if ( !cmd_name || cmd_name[0] == '\0' )
		return NULL;
	y = interp_hash_nn( cmd_name );
	
	for( p = LIST_FIRST(&command_hash[y]); p; p = LIST_NEXT(p, cmd_lst) ) {
		if (cclass != p->command_class)
			continue;
		if (!str_cmp(p->name, cmd_name))
			return p;
	}
	return NULL;
}


struct Interp_Command * find_command_abbrev( const char* cmd_name, int cclass )
{
	struct Interp_Command* p;
	int y;

	if ( !cmd_name || cmd_name[0] == '\0' )
		return NULL;
	y = interp_hash_nn( cmd_name );
	
	for( p = LIST_FIRST(&command_hash[y]); p; p = LIST_NEXT(p, cmd_lst) ) {
		if (cclass != p->command_class)
			continue;
		if (is_abbrev(cmd_name, p->name))
			return p;
	}
	return NULL;
}

struct Interp_Command * find_command_use( class Object* ch,
					  class Descriptor* desc,
					  const char* cmd_name,
					  int cclass )
{
	struct Interp_Command* p;
	int y;

	if ( !cmd_name || cmd_name[0] == '\0' )
		return NULL;
	y = interp_hash_nn( cmd_name );
	
	for( p = LIST_FIRST(&command_hash[y]); p; p = LIST_NEXT(p, cmd_lst) ) {
		if (cclass != p->command_class)
			continue;
		if (is_abbrev(cmd_name, p->name))
			return p;
	}
	return NULL;
}
	

DO_FUN* player_commands_findfunc(const char* nm)
{
	int i;

	for(i = 0; i < (sizeof command_functions)/(sizeof command_functions[0])
	    ; i++) {
	    if ( command_functions[i].cmd_func == NULL)
	    	continue;
	    if ( !str_cmp(command_functions[i].func_name, nm) )
	    	return command_functions[i].cmd_func;
	}
	
	return NULL;
}


const char* player_commands_funcname(DO_FUN* fn)
{
	int i;

	if ( !fn )
		return "NULL";

	for(i = 0; i < (sizeof command_functions)/(sizeof command_functions[0])
	    ; i++) {
	    if ( command_functions[i].cmd_func == fn)
	    	return command_functions[i].func_name;
	}
	
	return NULL;
}

ARG_PROC_FUN* player_parser_findfunc(const char* nm)
{
	int i;

	for(i = 0; i < (sizeof command_functions)/(sizeof command_functions[0])
	    ; i++) {
	    if ( command_functions[i].parse_func == NULL)
	    	continue;
	    if ( !str_cmp(command_functions[i].func_name, nm) )
	    	return command_functions[i].parse_func;
	}

	return NULL;
}

const char* player_parser_funcname(ARG_PROC_FUN* fn)
{
	int i;

	if ( !fn )
		return "NULL";

	for(i = 0; i < (sizeof command_functions)/(sizeof command_functions[0])
	    ; i++) {
	    if ( command_functions[i].parse_func == fn)
	    	return command_functions[i].func_name;
	}
	
	return NULL;
}

static struct Interp_Command* row_to_plr_command( QueryResult* r )
{
	struct Interp_Command* n;
	const char *s;

	massert(n = new struct Interp_Command);	
	memset(n, 0, sizeof(struct Interp_Command));

	n->sortpos = r->get_long("sortpos");
	n->name = r->get_string_copy("name");
	if ( !n->name ) {
		printf("Invalid command (name null)\n");
		abort();
	}
	s = r->get_string("func");

	if ( s )
		n->func = player_commands_findfunc(s);
	else
		n->func = 0;

	n->min_position = r->get_long("min_position");
	n->min_level = r->get_long("min_level");
	n->log = r->get_long("log");
	n->lag_count = r->get_long("lag_count");
	n->mana_cost = r->get_long("mana_cost");
	n->move_cost = r->get_long("move_cost");
	n->command_class = r->get_long("command_class");
	n->battle_time = r->get_long("battle_time");
	n->flags = r->get_long("flags");
	n->subcmd = r->get_long("subcmd");

	s = r->get_string("parser");
	if ( s )
		n->parser = player_parser_findfunc(s);
	else
		n->parser = 0;

	return n;
}

void load_player_commands( )
{
	int y;

	QueryResult r = gameDb.queryf(
		"select p.*, t.str, t.parser from "
		   "player_commands p, mud_target_templates t "
		"where p.target = t.name order by p.sortpos desc"
	);

	struct Interp_Command *o;
	const char *s;
	

	if (r.is_error()) {
		log_print("load_commands: database error");
		abort();
	}

	while (r.next_row ())
	{
		o = row_to_plr_command(&r);
		if ( !o )
			continue;

		y = interp_hash_nn(o->name);
		LIST_INSERT_HEAD(&command_hash[y], o, cmd_lst);
		
/*		printf("load_cmd: %s\n",
				r.get_string("name"));*/
	}
}

Interp_Arguments::Interp_Arguments() {
	text = 0;
	pvict = 0;
	pvict_id = 0;
	num_victims = 0;
}

void Interp_Arguments::clear()
{
	delete text;
	if ( pvict )
		free( pvict );
	if ( pvict_id )
		free( pvict_id );
	text = 0;
	pvict = 0;
	pvict_id = 0;
	num_victims = 0;
}

void Interp_Arguments::add_target(Object* vict)
{
	if ( !pvict ) {
		pvict = (Object **) malloc(sizeof(Object *) * 1);
		pvict_id = (long long*) malloc(sizeof(long long) * 1);
	}
	else {
		pvict = (Object **)realloc(pvict, sizeof(Object *) * (num_victims+1));
		pvict_id = (long long*)realloc(pvict_id, sizeof(long long) * (num_victims+1));
	}

	massert(pvict);
	massert(pvict_id);
	assert(num_victims >= 0);
	
	pvict[num_victims] = vict;
	pvict_id[num_victims] = vict->get_id();
	num_victims++;
}
	

PARSER(proc_onechar_local)
{
	Parser p( text, WORD, END );
	Object* victim;

	if ( p.num() < 1 ) {
		return false;
	}

	ch->sendf("%s\n", p.arg(0));

	victim = find_char_local( ch, text );

	// arg_data->add_target

	return false;
}

bool check_command (class Object* ch, class Descriptor* desc, char* text,
                    int l)
{
	struct Interp_Command* ic;
	char* args, buf[MAX_INPUT_LENGTH];
	struct Interp_Arguments arg_data;
	Cmd_Return cmd_ret;
	int count = 2;
	
/*	x->sendf("check_command (%s, %d, '%s', %d)\n", x->get_name(), 
					y ? y->get_fd() : -2,
					z, l);*/
	args = strchr(text, ' ');

	if ( args )
		*args++ ='\0';
	else
		args = strcpy(buf, "");
	

	if (!(ic = find_command_use(ch, desc, text, 0) )) {
		return false;
	}

	memset(&arg_data, 0, sizeof(struct Interp_Arguments));

	arg_data.text = str_dup( args );

	if (ic->parser)
		if (!ic->parser(ch, &arg_data, args))
			return true;

	if (ic->func) {
		ic->func(ch, desc, ic, &arg_data, 0, cmd_ret);
		
	}

	arg_data.clear();
	return true;
}

COMMAND(do_quit)
{
	ch->put_line("Goodbye!!\r\n");

	desc->disconnect();
	return;
}

COMMAND(do_not_quit)
{
	ch->put_line("If you want to quit, you need to type the whole word 'quit'.");
	return;
}

COMMAND(do_save) {
	ch->put_line("No need for saving.\r\n");
	return;
}

COMMAND(do_look) {
	Location* l = ch->get_location();

	if ( l )
		look_around(ch, l);
}

COMMAND(do_who) {
	vector<Game *>::iterator gi = running_games.begin();
	list<GamePort *>::iterator pi;
	vector<GamePort *>::iterator pvi;
	vector<GamePort*> ps;
	GamePort *gp;

	while(gi != running_games.end()) {
		for(pi = ((*gi)->get_ports()).begin();
		    pi != ((*gi)->get_ports()).end(); pi++)
		{
			gp = (*pi);
			if (find(ps.begin(), ps.end(), gp) == ps.end())
				ps.push_back(gp);
		}
		gi++;
	}

	for(pvi = ps.begin(); pvi != ps.end(); pvi++) {
		info_who_command( ch, (*pvi) );
	}
}

COMMAND(do_commands)
{
	int i = 0, j = 0;
	struct Interp_Command* p = 0;

	
	while (i < CMD_HASHSIZE) {
		for(p = LIST_FIRST(&command_hash[i]); p; p = LIST_NEXT(p, cmd_lst)) {
			ch->sendf("%-25s", p->name);
			if (++j % 2 == 0)
				ch->put_line("");
		}
		i++;
	}
}
