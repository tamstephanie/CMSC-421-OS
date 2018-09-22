#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXCHAR 10

char *concatenate(char *a, char *b)
{
    /*allocate space for new string*/
    size_t new_size = strlen(a) + strlen(b) + 1;
    char *c = (char *)malloc(new_size);

    /*add each word individually*/
    int i;
    for(i = 0; i < strlen(a); i++) {
      c[i] = a[i];
    }
    for(i = 0; i < strlen(b); i++) {
      c[i + strlen(a)] = b[i];
    }

    return c;
}


int main(int argc, char **argv)
{
    if (argc > 4) {
      printf("Usage: hw1_leaky <count> <firstword> <secondword> \n");
      exit(1);
    }

    char *middle = "cruel";
    char number[MAXCHAR];
    int i = 0;

    for(i = 0; i < atoi(argv[1]); i++)
    {
      sprintf(number,"%d", i);

      /*begin leaky*/
      char *line = concatenate(           /*not leaked*/
        concatenate(argv[2], number),     /*leaked*/
        concatenate(middle,               /*leaked*/
        concatenate(number, argv[3])));   /*leaked*/

      if(i > 0) /*should not free statically allocated "cruel" string*/
        free(middle);

      /*end leaky*/
      printf("%s \n",line);
      middle = line;
    }

    free(middle);
    return 0;
}
