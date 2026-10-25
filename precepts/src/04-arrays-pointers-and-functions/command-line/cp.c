#include <stdio.h>
#include <errno.h>
#include <string.h>

#define TRUE  1
#define FALSE 0

int 
main(const int argc, const char *argv[])
{
   FILE *fi, *fo;
   char buf[4096];
   int end = FALSE;

   /* check the command line parameters */
   if (argc != 3) {
      fprintf(stderr, "Usage: %s src-file dest-file\n", argv[0]);
      return(-1);
   }

   /* open the source file */
   fi = fopen(argv[1], "r");
   if (fi == NULL) {
      fprintf(stderr, "File open %s error: %s\n", argv[1], strerror(errno));
      return (-1);
   }
   
   /* open the destination file */
   fo = fopen(argv[2], "w");
   if (fo == NULL) {
      fprintf(stderr, "File open %s error: %s\n", argv[2], strerror(errno));
      fclose(fi);
      return (-1);
   }

   /* copy content from src to dest file */
   while (!end) {
      int left, res, off = 0;

      left = fread(buf, sizeof(char), sizeof(buf), fi);
      end  = (left != sizeof(buf));
      while (left > 0) {
         res = fwrite(buf + off, sizeof(char), left, fo);
         left -= res;
         off += res;
         if (res == 0 && left > 0) {
            fprintf(stderr, "Can't write to a file, %d bytes left\n", left);
            fclose(fi);
            fclose(fo);
            return (-1);
         }
      }
   }
   
   /* close both files */
   fclose(fi);
   fclose(fo);

   return (0);
}
