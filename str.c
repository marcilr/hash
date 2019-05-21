/*
 * str.c
 * Created  Fri Aug  9 14:05:56 AKDT 2002
 * by Raymond E. Marcil <marcilr@rockhounding.net>
 *
 * This small library contains some string utilities
 * used in the project. Just put them here to keep
 * things tidy.
 */

#include <stdio.h>
#include <stdlib.h>

/* ================== Public Functions =================== */
/* ================== Public Functions =================== */

/*
 * strTrimTail()
 * This function removes trailing spaces from a string.
 *
 * INPUT:      str      Pointer to string to trim.
 * OUTPUT:              Trims tail of string.
 */
void strTrimTail(char *str){
  int i;

  for (i = strlen(str)-1; i > 0 && isspace(str[i]); i--);
  str[i + 1] = '\0';

}

/*
 * strStrip()
 * This function strips all non-alphanumeric characters
 * from a string. I rewrote this using pointer math
 * and it is slightly more efficient.
 *
 * INPUT:     str       String to remove non-alphanumerics
 *                      from.
 * OUTPUT:              Removes non-alphanumeric characters
 *                      from string
 */

void strStrip(char *str){
  char *i,*j;

  /* Loop, only copy alpa-numeric */
  for (i = str, j = str; *i != '\0'; i++)
    if (isalnum(*i) != 0)
      *j++ = *i;

  *j = '\0';           /* Terminate string */

} /* end strStrip() */


/*
 * lineRead()
 * This file reads a single line of data from a file.
 *
 * INPUT:     fp          File handle to read from
 *            line        Pointer to string to write to.
 *            maxlength   Max bytes to read
 */

int lineRead(FILE *fp, char *line, int maxlength){
  char  c;
  char  *i;

  i = line;

  c = getc(fp);
  while (c != '\n' && c != EOF){
    *i++ = c;
    c = getc(fp);
  }

  *i = '\0';

  return c;
} /* end lineRead() */
