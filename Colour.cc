/*
 *  OMud - Preliminary Colour String Parsing
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



#include "mud.h"
#include "Descriptor.h"

static void enter_color(
  Descriptor	*d,
  string	col,
  string &	target,
  long		color_state
);

struct {
char* name;
long code, ansi;
} color_chart[] =
{
{	"black"	,	0x000000	, 30 },
{	"red"	,	0xff0000	, 31 },
{	"green"	,	0x00ff00	, 32 },
{	"orange",	0xff9900	, 33 },
{	"blue"	,	0x0000ff	, 34 },
{	"magenta",	0xcc00ff	, 35 },
{	"cyan"	,	0x00ffff	, 36 },
{	"white"	,	0xffffff	, 37 },
};

/*
 *  Get the number of a color by name
 */
static int get_color(const char *nm) {
	int i;

	for(i = 0; i < (sizeof color_chart)/sizeof(color_chart[0]); i++)
	{
		if (!str_cmp(nm, color_chart[i].name))
			return i;
	}
	return -1;
}


/*
 * Entry into a <font>  tag
 */
static void enter_font(
	Descriptor* d,
	vector<string>& attribs,
	string&	target,
	long color_state
)
{
	int i, j;

	for(i = 0; i < attribs.size(); i++) {
		if (attribs[i].length() < 1)
			continue;
		j = attribs[i].find("=");

		if ( j == -1 || j < 2 ||
			j >= attribs[i].length() - 1)
			continue;

		if (attribs[i].substr(0,j)
			== "color") {
			enter_color(d,	attribs[i].substr(j+1),
					target,
					color_state
			);
		}
	}
}

/*
 * Entry into a tag
 */
static void enter_tag(
	Descriptor* d,
	char* str_tag_name,
	vector<string>& attribs,
	string& target,
	long color_state
)
{
	if (!str_cmp(str_tag_name, "font")) {
		enter_font(d, attribs, target, color_state);
	}
}

/*
 * </font>
 */
static void exit_font(
	Descriptor * d,
	string &	target,
	long		color_state
)
{
  if (! d)
  	return;

  if (d->get_ansi() && !d->get_mxp())
  	target += "\033[0m";
}


/* 
 * Entry into a color attribute
 */
static void enter_color(
  Descriptor * d,
  string	col,
  string &	target,
  long		color_state
)
{
	char buf[255];
	long newcol = 0;
	int v;

	if ( ! d )
		return;
	
	if ( col.length() > 3 && col[0] == '#') {
		newcol = strtoul(col.c_str()+1, (char **)0, 16); 
		return;
	}
	else {
		v = get_color(col.c_str());

		if (v >= 0)
			newcol = color_chart[v].code;
		else
			return;

		if (d->get_mxp() == false &&
		    d->get_ansi())
		{
			sprintf(buf, "\033[0m\033[%dm", color_chart[v].ansi);
			target += buf;
		}
	}
}


/**
 * Parse markup
 */
void parse_mml(class Descriptor * d, char * buf)
{
	vector<string> attribs;
	string target = "";
	char str_tag_name[MAX_INPUT_LEN];
	char tag_buf[MAX_INPUT_LEN], entity_buf[MAX_INPUT_LEN];
	char* p = buf;
	int state = 0, tag_pos = 0, entity_pos = 0, i, j;
	long color_state = 0;

	bool tag = false;
	bool tag_name = false;
	bool dquote = false;
	bool squote = false;
	bool entity = false;

	if ( d && d->get_mxp ( ) ) {
		return;
	}

	for ( p = buf; *p; p++ )
	{
		if (*p == '<' && !dquote && !squote) {
			attribs.clear();
			tag_pos = 0;
			tag = true;
			tag_name = true;
		}
		else if (*p == '>' && !dquote && !squote) {
			if ( tag_name ) {
				/*tag_name_done*/
				tag_buf[tag_pos++] = '\0';
				strcpy (str_tag_name, tag_buf);
				tag_name = false;
				tag_pos = 0;

				if (!str_cmp(str_tag_name, "/font"))
					exit_font(d, target, color_state);
			}
			else {
				tag_buf[tag_pos++] = '\0';
				tag_pos = 0;
				attribs.push_back(string(tag_buf));

				enter_tag(d, str_tag_name, attribs, target, color_state);
			}
			tag = false;
		}
		else if (isspace(*p) && tag && !dquote && !squote) {
			if ( tag_name ) {
				/*tag_name_done*/
				tag_buf[tag_pos++] = '\0';
				strcpy (str_tag_name, tag_buf);
				tag_name = false;
				tag_pos = 0;
			}
			else {
				tag_buf[tag_pos++] = '\0';
				attribs.push_back(string(tag_buf));
				tag_pos = 0;
			}
		}
		else if (*p == '"' && tag && !dquote && !squote) {
			dquote = true;
		}
		else if (*p == '\'' && tag && !dquote) {
			if ( !squote ) 
				squote = true;
			else
				squote = false;
		}
		else if (*p == '"' && dquote && !squote) {
			dquote = false;
		}
		else if (*p == '&') {
			entity_pos = 0;
			entity = true;
		}
		else if ((*p == ';' || isspace(*p)) && entity) {
			const char* s;
			/* entity_name_done */
			entity_buf[entity_pos++] = '\0';
			entity = false;

			if (!str_cmp(entity_buf, "lt"))
				target += '<';
			else if (!str_cmp(entity_buf, "gt"))
				target += '>';
			else if (!str_cmp(entity_buf, "amp"))
				target += '&';
			else if (!str_cmp(entity_buf, "copy"))
				target += "(C)";
			else if (!str_cmp(entity_buf, "trade")) {
				target += "(tm)";
			}
			else if (!str_cmp(entity_buf, "times")) {
				target += " x ";
			}
		}
		else if ( tag_name ) {
			tag_buf[tag_pos++] = LOWER(*p);
		}
		else if ( entity ) {
			entity_buf[entity_pos++] = LOWER(*p);
		}
		else if ( tag ) {
			tag_buf[tag_pos++] = *p;
		}
		else {
			target += *p;
		}
	}
	strcpy(buf, target.c_str());
}
