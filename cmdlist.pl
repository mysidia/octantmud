#!/usr/bin/perl
#
#  OMud - Command List File Generation Script
#    EXHIBIT A
#
#    The Notice below must appear in each file of the Source Code of any copy You distribute of
#    the Licensed Software or any Extensions thereto, except as may be modified as allowed
#    under the terms of Section 7.1
#
#      Copyright (C) 2003-2004 ***REMOVED***, All Rights Reserved.
#
#      Unless explicitly acquired and licensed from Licensor under the Technical Pursuit
#      License ("TPL") Version 1.0 or greater, the contents of this file are subject to the
#      Reciprocal Public License ("RPL") Version 1.1.
#
#      You may not copy or use this file in either source code or executable form,
#      except in compliance with the terms and conditions of the RPL.
#
#      All software distributed under the Licenses is provided strictly on an "AS IS" basis,
#      WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, AND THE AUTHORS AND
#      CONTRIBUTORS HEREBY DISCLAIM ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION,
#      ANY WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT,
#      OR NON-INFRINGEMENT.
#
#      See the Licenses for specific language governing rights and
#      limitations under the Licenses.
#
#

use IO::Handle;
my @cmds, @parsers;

opendir DIR, "." || die "Unable to open directory.";
open OUTPUT, ">cmdlist.h" || die "Unable to open output file.";

print OUTPUT "/* Generated automatically *DO NOT EDIT* */\n";
print OUTPUT "\n";


sub pl($)
{
	$_ = shift;

	if (m /COMMAND\(([_a-zA-Z0-9]+)\)/) {
		$cmds[($#cmds)+1] = $1;
	}

	if (m /PARSER\(([_a-zA-Z0-9]+)\)/) {
		$parsers[($#parsers)+1] = $1;
	}
}

while ($fn = readdir(DIR)) {
	if (!grep(/[a-zA-Z0-9_]\.cc?/, $fn)) {
		next;
	}
	open (F, "<" . $fn) || next;
	while (($x = <F>)) {
		pl($x);
	}
	close (F);
}
closedir DIR;

for (@parsers) {
	print OUTPUT "PARSER(" . $_ . ");\n";
}
for (@cmds) {
	print OUTPUT "COMMAND(" . $_ . ");\n";
}
print OUTPUT "\n";

print OUTPUT "static struct plr_command_function_t command_functions[] =\n{\n";
for (@cmds) {
	print OUTPUT "	CMD_FN(" . $_ . "), \n";
}
for (@parsers) {
	print OUTPUT "	PARSE_FN(" . $_ . "), \n";
}
print OUTPUT "};\n";
