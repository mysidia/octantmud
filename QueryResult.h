/*
 *  OMud - Database Query Class
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



#ifndef __QueryResult_h__
#define __QueryResult_h__
#include "mud.h"
#include <stdio.h>
#include <dbi/dbi.h>

class QueryResult {
	public:
		
	QueryResult(dbi_result res) {
		r_r = res;
		row_active = false;
	}

	~QueryResult() {
		if ( r_r ) {
			log_print( "dbi_result_free" );
			dbi_result_free(r_r);
		}
	}

	bool is_error() const {
		return r_r == NULL;
	}

	int row_count() {
		return dbi_result_get_numrows(r_r);
	}

	int affected_row_count() {
		return dbi_result_get_numrows_affected(r_r);
	}
	
	int next_row() {
		int i = dbi_result_next_row(r_r);

		row_active = (i > 0);
		return i;
	}

	int prev_row() {
		int i = dbi_result_prev_row(r_r);

		row_active = (i > 0);
		return i;
	}

	int last_row() {
		int i = dbi_result_last_row(r_r);

		row_active = (i > 0);
		return i;
	}

	int first_row() {
		int i = dbi_result_first_row(r_r);

		row_active = (i > 0);
		return i;
	}

	int seek_row(unsigned int row) {
		int i = dbi_result_seek_row(r_r, row);

		row_active = (i > 0);
		return i;
	}

	int get_index(const char* nm) {
		return dbi_result_get_field_idx(r_r, nm);
	}

	const char* get_name(int i) {
		return dbi_result_get_field_name(r_r, i);
	}

	int col_count() {
		return dbi_result_get_numfields(r_r);
	}

	dbi_result& result_handle() { return r_r; }

	char get_char(const char* field) { 
		return dbi_result_get_char(r_r, field);
	}

	unsigned char get_uchar(const char* field) {
		return dbi_result_get_uchar(r_r, field);
	}

	short get_short(const char* f) {
		return dbi_result_get_short(r_r, f);
	}

	unsigned short get_ushort(const char* f) {
		return dbi_result_get_ushort(r_r, f);
	}

	long get_long(const char* f) {
		return dbi_result_get_long(r_r, f);
	}

	unsigned long get_ulong(const char* f) {
		return dbi_result_get_ulong(r_r, f);
	}

	long long get_longlong(const char* f) {
		return dbi_result_get_longlong(r_r, f);
	}

	unsigned long long get_ulonglong(const char* f) {
		return dbi_result_get_ulonglong(r_r, f);
	}

	float get_float(const char* f) {
		return dbi_result_get_float(r_r, f);
	}

	double get_double(const char* f) {
		return dbi_result_get_double(r_r, f);
	}

	const char* get_string(const char*f) {
		return dbi_result_get_string(r_r, f);
	}

	char* get_string_copy(const char* f){ 
		return dbi_result_get_string_copy(r_r, f);
	}

	const unsigned char* get_binary(const char* f)
	{
		return dbi_result_get_binary(r_r, f);
	}

	unsigned char* get_binary_copy(const char* f)
	{
		return dbi_result_get_binary_copy(r_r, f);
	}

/*	const char *get_enum(const char* f) {
		return dbi_result_get_enum(r_r, f);
	}

	const char *get_set(const char* f) {
		return dbi_result_get_set(r_r, f);
	}*/

	time_t get_datetime(const char* f) {
		return dbi_result_get_datetime(r_r, f);
	}

	int bind_char(const char* f, char* var) {
		int res = dbi_result_bind_char(r_r, f, var);
		if ( res < 0 ) 
			log_print("Error binding char at %p to field %s", var, f);

		return res;
	}


	int bind_uchar(const char* f, unsigned char* var) {
		int res = dbi_result_bind_uchar(r_r, f, var);
		if ( res < 0 ) 
			log_print("Error binding uchar at %p to field %s", var, f);

		return res;
	}

	int bind_short(const char* f, short* var) {
		int res = dbi_result_bind_short(r_r, f, var);
		if ( res < 0 ) 
			log_print("Error binding char at %p to field %s", var, f);

		return res;
	}


	int bind_ushort(const char* f, unsigned short* var) {
		int res = dbi_result_bind_ushort(r_r, f, var);
		if ( res < 0 ) 
			log_print("Error binding ushort at %p to field %s", var, f);

		return res;
	}

	int bind_long(const char* f, long* var) {
		int res = dbi_result_bind_long(r_r, f, var);
		if ( res < 0 ) 
			log_print("Error binding long at %p to field %s", var, f);

		return res;
	}


	int bind_ulong(const char* f, ulong* var) {
		int res = dbi_result_bind_ulong(r_r, f, var);
		if ( res < 0 ) 
			log_print("Error binding unsigned long at %p to field %s", var, f);

		return res;
	}

	int bind_longlong(const char* f, long long* var) {
		int res = dbi_result_bind_longlong(r_r, f, var);
		if ( res < 0 ) 
			log_print("Error binding long long at %p to field %s", var, f);

		return res;
	}

	int bind_ulonglong(const char* f, unsigned long long* var) {
		int res = dbi_result_bind_ulonglong(r_r, f, var);
		if ( res < 0 ) 
			log_print("Error binding unsigned long long at %p to field %s", var, f);

		return res;
	}

	int bind_float(const char* f, float* var) {
		int res = dbi_result_bind_float(r_r, f, var);
		if ( res < 0 ) 
			log_print("Error binding float at %p to field %s", var, f);

		return res;
	}

	int bind_double(const char* f, double* var) {
		int res = dbi_result_bind_double(r_r, f, var);
		if ( res < 0 ) 
			log_print("Error binding double at %p to field %s", var, f);

		return res;
	}


	int bind_string(const char* f, const char** var) {
		int res = dbi_result_bind_string(r_r, f, var);
		if ( res < 0 ) 
			log_print("Error binding string at %p to field %s", var, f);

		return res;
	}


	int bind_binary(const char* f, const unsigned char** var) {
		int res = dbi_result_bind_binary(r_r, f, var);
		if ( res < 0 ) 
			log_print("Error binding binary at %p to field %s", var, f);

		return res;
	}


	int bind_binary_copy(const char* f, unsigned char** var) {
		int res = dbi_result_bind_binary_copy(r_r, f, var);
		if ( res < 0 ) 
			log_print("Error binding binary copy at %p to field %s", var, f);

		return res;
	}

	int bind_string_copy(const char* f, char** var) {
		int res = dbi_result_bind_string_copy(r_r, f, var);
		if ( res < 0 ) 
			log_print("Error binding string copy at %p to field %s", var, f);

		return res;
	}


/*	int bind_enum(const char* f, char** var) {
		int res = dbi_result_bind_enum(r_r, f, var);
		if ( res < 0 ) 
			log_print("Error binding enum at %p to field %s", var, f);

		return res;
	}

	int bind_set(const char* f, char** var) {
		int res = dbi_result_bind_set(r_r, f, var);
		if ( res < 0 ) 
			log_print("Error binding set at %p to field %s", var, f);

		return res;
	}*/

	int bind_datetime(const char* f, time_t* var) {
		int res = dbi_result_bind_datetime(r_r, f, var);
		if ( res < 0 ) 
			log_print("Error binding datetime at %p to field %s", var, f);

		return res;
	}


	char get_char_idx(int idx) { 
		return dbi_result_get_char_idx(r_r, idx);
	}

	unsigned char get_uchar_idx(int idx) {
		return dbi_result_get_uchar_idx(r_r, idx);
	}

	short get_short_idx(int idx) {
		return dbi_result_get_short_idx(r_r, idx);
	}

	unsigned short get_ushort_idx(int idx) {
		return dbi_result_get_ushort_idx(r_r, idx);
	}

	long get_long_idx(int idx) {
		return dbi_result_get_long_idx(r_r, idx);
	}

	unsigned long get_ulong_idx(int idx) {
		return dbi_result_get_ulong_idx(r_r, idx);
	}

	long long get_longlong_idx(int idx) {
		return dbi_result_get_longlong_idx(r_r, idx);
	}

	unsigned long long get_ulonglong_idx(int idx) {
		return dbi_result_get_ulonglong_idx(r_r, idx);
	}

	float get_float_idx(int idx) {
		return dbi_result_get_float_idx(r_r, idx);
	}

	double get_double_idx(int idx) {
		return dbi_result_get_double_idx(r_r, idx);
	}

	const char* get_string_idx(int idx) {
		return dbi_result_get_string_idx(r_r, idx);
	}

	char* get_string_copy_idx(int idx){ 
		return dbi_result_get_string_copy_idx(r_r, idx);
	}

	const unsigned char* get_binary_idx(int idx) {
		return dbi_result_get_binary_idx(r_r, idx);
	}

	const unsigned char* get_binary_copy_idx(int idx) {
		return dbi_result_get_binary_copy_idx(r_r, idx);
	}

/*	const char* get_enum_idx(int idx) {
		return dbi_result_get_enum_idx(r_r, idx);
	}

	const char* get_set_idx(int idx) {
		return dbi_result_get_set_idx(r_r, idx);
	}*/

	time_t get_datetime_idx(int idx) {
		return dbi_result_get_datetime_idx(r_r, idx);
	}




	private:
	dbi_result r_r;
	bool row_active;
};
#endif
