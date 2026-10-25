/* printargv.c */
#include <stdio.h>

/* Write the argc strings of argv to stdout.  Return 0. */
int main(int argc, char *argv[])
{
   int i;

   printf("argc:  %d\n", argc);
   printf("argv:\n");
   for (i = 0; i < argc; i++)
      printf("   %s\n", argv[i]);

   return 0;
}
/* Example Executions:

$ gcc217 printargv.c -o printargv

$ printargv one two three
argc:  4
argv:
   printargv
   one
   two
   three

$ printargv
argc:  1
argv:
   printargv

$ printargv one "two  three" four
argc:  4
argv:
   printargv
   one
   two  three
   four

*/
