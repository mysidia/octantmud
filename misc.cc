#include "mud.h"

/*
** Based on the same principle as the strtoken() in ircd, irc2.8.x,
** same prototype & methods.
**
** Think of it as an improved 'strtok()', the major diference is the 
** state is saved in the first parameter, instead of static data.
**
** So strtoken calls don't destroy each others data if used in recursive
** functions or somesuch
*/

char *strtoken(char **saved /* Saved position from another invokation */,
               char *s /* The string */, 
               char *sep /* The separator */)
{
    char *pos = *saved; /* The saved position */
    char *tmp;

    if (s) {
        pos = s;
    }

    while (pos != 0 && *pos != '\0' && strchr (sep, *pos) != 0) {
        pos += 1;
    }

    if (pos == 0 || *pos == '\0') {
        pos = 0;
        *saved = 0;

        return 0;
    }

    tmp = pos;
    while (*pos != '\0' && strchr (sep, *pos) == 0) {
        pos += 1;
    }

    if (*pos != '\0') { /* Found a separator, stop the returned
                           string and save at next pt */
        *pos = '\0';
        pos += 1;
    }
    else pos = 0;
    *saved = pos;
    return(tmp);
}

