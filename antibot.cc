#include "mud.h"
#include "Descriptor.h"

/**
 * These describe the printable character objects
 */
static char letters[4][30][10][30] = 
 {

/*
Derived (subset) from figlet standard type. - jmh

You can get figlet at  http://www.figlet.org/

Standard by Glenn Chappell & Ian Chai 3/93 -- based on Frank's .sig
Includes ISO Latin-1
figlet release 2.1 -- 12 Aug 1994
Modified for figlet 2.2 by John Cowan <cowan@ccil.org>
  to add Latin-{2,3,4,5} support (Unicode U+0100-017F).
  Permission is hereby given to modify this font, as long as the
  modifier's name is placed on a comment line.

  Modified by Paul Burton <solution@earthlink.net> 12/96 to include new parameter
  supported by FIGlet and FIGWin.  May also be slightly modified for better use
  of new full-width/kern/smush alternatives, but default output is NOT changed.
 */
  {
   { "  __ _ ",
   " / _` |",
   "| (_| |",
   " \\__,_|", 0 },
   { " _     ",    "| |__  ",    "| '_ \\ ",    "| |_) |",    "|_.__/ ", 0},
   { "  ___ ",    " / __|",    "| (__ ",    " \\___|", 0},
   { "     _ ",    "  __| |",    " / _` |",    "| (_| |",    " \\__,_|", 0 },
   { "  ___ ",    " / _ \\",    "|  __/",    " \\___|", 0 },
   { "  __ ",     " / _|",     "| |_ ",     "|  _|",     "|_|  ", 0 },
   { "  ___  ",     " / _ \\ ",     "| | | |",     "| |_| |",     " \\___/ ", 0 },
   { " _ ",     "/ |",     "| |",     "| |",     "|_|", 0 },
   { " ____  ",    "|___ \\ ",    "  __) |",    " / __/ ",    "|_____|", 0 },
   { " _____ ",     "|___ / ",     "  |_ \\ ",     " ___) |",     "|____/ ", 0 },
   { " _  _   ",     "| || |  ",     "| || |_ ",     "|__   _|",     "   |_|  ", 0 },
   { " ____  ",     "| ___| ",     "|___ \\ ",     " ___) |",     "|____/ ", 0 },
   // 5

   { "  __   ",     " / /_  ",     "| '_ \\ ",     "| (_) |",     " \\___/ ", 0 },
   { " _____ ",     "|___  |",     "   / / ",     "  / /  ",     " /_/   ", 0 },
   { "  ___  ",     " ( _ ) ",     " / _ \\ ",     "| (_) |",     " \\___/ ", 0 },
   { "  ___  ",     " / _ \\ ",     "| (_) |",     " \\__, |",     "   /_/ ", 0 },
   { " " }
 },

/*
Derived (subset) from figlet small type. - jmh

Small by Glenn Chappell 4/93 -- based on Standard
Includes ISO Latin-1
figlet release 2.1 -- 12 Aug 1994
Permission is hereby given to modify this font, as long as the
modifier's name is placed on a comment line.
*/
   {
     {"   _   ",
     "  /_\\  ",
     " / _ \\ ",
     "/_/ \\_\\", 0 },
     {" ___ ", "| _ )", "| _ \\", "|___/", "     ", 0},
     {"  ___ ", " / __|", "| (__ ", " \\___|", "      ", 0},
     {" ___  ", "|   \\ ", "| |) |", "|___/ ", "      ", 0},
     {" ___ ", "| __|", "| _| ", "|___|", "     ", 0},
     {" ___ ", "| __|", "| _| ", "|_|  ", "     ", 0},
     {"  __  ", " /  \\ ", "| () |", " \\__/ ", "      ", 0},
     {" _ ", "/ |", "| |", "|_|", "   "},
     {" ___ ", "|_  )", " / / ", "/___|","     ", 0},
     {" ____", "|__ /", " |_ \\", "|___/", "     ", 0},
     {" _ _  ", "| | | ", "|_  _|", "  |_| ", "      ", 0},
     {" ___ ", "| __|", "|__ \\", "|___/", "     ", 0},
     {"  __ ", " / / ", "/ _ \\", "\\___/", "     ", 0},
     {" ____ ", "|__  |", "  / / ", " /_/  ", "      ", 0},
     {" ___ ", "( _ )", "/ _ \\", "\\___/", "     ", 0},
     {" ___ ", "/ _ \\", "\\_, /", " /_/ ", "     ", 0}
   },
   {
     {" "," ","  A  "}, {" "," ","  B  "}, {" "," ","  C  "}, 
     {" "," ","  D  "}, {" "," ","  E  "}, {" "," ","  F  "},
     {" "," ","  0  "}, {" "," ","  1  "}, {" "," ","  2  "},
     {" "," ","  3  "}, {" "," ","  4  "}, {" "," ","  5  "},
     {" "," ","  6  "}, {" "," ","  7  "}, {" "," ","  8  "}, 
     {" "," ","  9  "}
   }
   ,
   0
};

/**
 * @return Number of maps available for a character
 */
int type_char_nfonts( ) {
	int i;

	for(i = 0; *letters[i][0][0] != '\0'; i++)
		;
	return i;
}

/**
 * Convert a character to the array index
 * @return An array index @var c into letters[a][b][c]
 */
int type_char_image_deref( int letter ) 
{ 
	switch ( letter ) {
		case 'a': letter = 0; break;
		case 'b': letter = 1; break;
		case 'c': letter = 2; break;
		case 'd': letter = 3; break;
		case 'e': letter = 4; break;
		case 'f': letter = 5; break;
		case '0': letter = 6; break;
		case '1': letter = 7; break;
		case '2': letter = 8; break;
		case '3': letter = 9; break;
		case '4': letter = 10; break;
		case '5': letter = 11; break;
		case '6': letter = 12; break;
		case '7': letter = 13; break;
		case '8': letter = 14; break;
		case '9': letter = 15; break;
		case ' ': letter = 16; break;
		default: letter = 0; break;
	}
        return letter;
}

/**
 * @return The number of rows in a particular letter and map
 */
int type_char_image_nrows( int letter, int tp ) {
	int i = 0;

	letter = type_char_image_deref(letter);	

	for ( i = 0; *letters[tp][letter][i] != '\0'; i++)
		;
	return i;
}

/**
 * @return The content of a single row of the character map
 */
char* type_char_image( int letter, int tp, int row )
{
	letter = type_char_image_deref(letter);

	return letters[tp][letter][row];
}

/** 
 * Writes a sequence of characters to a descriptor
 * @param a The sequence
 * @param len Length of the sequence (should be < 8)
 * @param d   Descriptor to send to
 */
void type_char_write( char *a, int len, Descriptor *d ) 
{
	int i, r;
	int t = 0, b = 0, sp;
	int type_sel[len];
	int space_add[len];

	for( r = 0; r < 6; r++) {
		for(i = 0; i < len; i++) {
			if (r == 0) {
				type_sel[i] = rand() % type_char_nfonts();
				space_add[i] = rand() % 2;
			}
			t = type_sel[i];
			sp = space_add[i];
			/*if (type_char_image_nrows(a[i], t) > r) {*/
			    d->sendf("%-10s", type_char_image(a[i], t, r));
			    d->sendf("%.*s", sp, "        ");
			/*}*/
		}
		d->send("\r\n");
	}

}

