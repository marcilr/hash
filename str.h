/*
 * str.h
 * Created Fri Aug  9 14:08:36 AKDT 2002
 * by Raymond E. Marcil <marcilr@rockhounding.net>
 *
 * This small library contains some useful libraries
 * for munipulating strings.
 *
 * FUNCTIONS:        strTrimTail        Trim trailing whitespace from string.
 *                   strStrip           Strip non-alphanumeric from string.
 *
 */

#ifndef STR_H
#define STR_H

void strTrimTail(char *str);
void strStrip(char *str);
int lineRead(FILE *fp, char *line, int maxlength);

#endif
