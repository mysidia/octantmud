/* Generated automatically *DO NOT EDIT* */

PARSER(proc_onechar_local);
COMMAND(do_quit);
COMMAND(do_not_quit);
COMMAND(do_save);
COMMAND(do_look);
COMMAND(do_who);
COMMAND(do_commands);

static struct plr_command_function_t command_functions[] =
{
	CMD_FN(do_quit), 
	CMD_FN(do_not_quit), 
	CMD_FN(do_save), 
	CMD_FN(do_look), 
	CMD_FN(do_who), 
	CMD_FN(do_commands), 
	PARSE_FN(proc_onechar_local), 
};
