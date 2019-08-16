/*
 *  OMud - Mud Routines
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



#ifndef __mud_h__
#define __mud_h__
#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>

#include <vector>
#include <string>
#include <list>
#include <map>
#include <algorithm>
#include <functional>
#include "enum.h"
#include "misc.h"
#include "queue.h"
#include "compat.h"
#define USE_NONB

using std::list;
using std::string;
using std::map;
using std::vector;

const bool FALSE = false;
const bool TRUE = true;

#define MAX_SOCK_LEN 8192

#define MAX_INPUT_LENGTH 1024
#define MAX_INPUT_LEN MAX_INPUT_LENGTH

#define MAX_PATH_LEN 1024

#define MAX_STRING_LEN	  MAX_STRING_LENGTH
#define MAX_STRING_LENGTH 8192
#define EMAIL_LEN 255

#define LIB_TEXT "text/"
#define LIB_ACCOUNT "db/acct/"
#define WELCOME_FILE "welcome.txt"
#define NAME_LECTURE_FILE "name.txt"

#define UPPER(x) toupper((x))
#define LOWER(x) tolower((x))

extern int SYS_ent_1, SYS_ent_2, SYS_ent_3, SYS_ent_4, SYS_ent_5;
extern time_t boot_time;


void ltrim_space(char *&x);
void close_socket(int fd);

int str_cmp(const char* a, const char * b);
int strn_cmp(const char* a, const char * b, int);
bool is_abbrev( const char* in_string, const char* abbrev_for );
void log_print(const char* fmt, ...);
int nonblock(int fd, char* messg);
char* str_dup(const char*);
bool complete_abbr(const char*, const char*, char *);

char* sqlEscape(const char *input, char *output, char wc_esc = '\0');


void massert(void*);
void copy_alloc_str(char *&, const char *);
bool legal_account_name(class Descriptor*, const char*, const char**);
bool legal_character_name(class Descriptor*, const char*, const char**);
bool should_accept_email(const char* email, Descriptor*);

char* str_ncpy(char *, const char*, int);
char* vsplit(int*, char*, char (*)[MAX_INPUT_LEN], Tok_Type, va_list);
char* split(int*, char*, char (*)[MAX_INPUT_LEN], ...);
char* capitalize(char* string);
void pack_int16(char*, long);
void pack_int32(char*, long);

long unpack_int16(char*);
long unpack_int32(char *);

void type_char_write( char *a, int len, class Descriptor * d );
void parse_mml(class Descriptor*, char *);
void load_player_commands( );
void load_player_stats( );
void look_around( class Object * ch, class Location * area );
void info_who_command( class Object * ch, class GamePort * gp );
void write_who_start( class Object * );
void write_who_item( class Object *, class Object * );
void write_who_end( class Object *, int );

class Object* find_char_local( class Object*, const char* );



/*char* split(char* buf, char** target, ...);*/

#include "Database.h"

#endif
