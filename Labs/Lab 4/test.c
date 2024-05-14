/* lab03-cp3.c */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{

  // Open up the text file
  FILE* file = fopen("book-1984.txt", "r");
  if (file == NULL) {
    perror("fopen() failed");
    return EXIT_FAILURE;
  }
  
  char line[128];
  char *i;
  int alnum_chars;
  while (1) {
   alnum_chars = 0;
   fgets(line, 100, file);
   if (strcmp(line, "Chapter 2\n") == 0) { //dont know if \n needs to be here
      break;
   }
   /*else if (line == NULL) {
      continue;
   }*/
   for (i = line; *i != '\0'; i++) {
      if (isspace(i[0]) != 0) {
        i++;
      }
      if (isalnum(i[0]) != 0) {
         printf("%c", i[0]);
         alnum_chars++;
      }
   }
   printf(" [%d alnum chars]\n", alnum_chars);
  }
  
  fclose(file);

  return EXIT_SUCCESS;
}