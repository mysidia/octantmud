/*
 *  Packing Integers into printable bytes
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
#include <endian.h>
#include <netinet/in.h>

void pack_int16(char* arr, long val)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
	val = (short)htons((short)val);
#endif

	arr[0] = val & 0x00ff;
	arr[1] = (val & 0xff00) >> 8;
	
}

void pack_int32(char* arr, long val)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
        val = htonl(val);
#endif
	arr[0] = val & 0x000000ff;
	arr[1] = (val & 0x0000ff00) >> 8;
	arr[2] = (val & 0x00ff0000) >> 16;
	arr[3] = (val & 0xff000000) >> 24;

}

long unpack_int16(char* arr)
{
	short val = 0;

	val |= (arr[0]);
	val |= (arr[1] << 8);

#if __BYTE_ORDER == __LITTLE_ENDIAN
        val = (short)ntohs((short)val);
#endif

	return (long)val;
}

long unpack_int32(char *arr)
{
	long val = 0;

	val |= (arr[0]);
	val |= (arr[1] << 8);
	val |= (arr[2] << 16);
	val |= (arr[3] << 24);

#if __BYTE_ORDER == __LITTLE_ENDIAN
        val = ntohl(val);
#endif

	
	return val;
}

