#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXCHAR 10

void* mymalloc(int size) {
    void* ptr = malloc(size);
    fprintf(stderr,"malloc: %p\n",ptr);
    return ptr;
}

void myfree(void* ptr) {
    fprintf(stderr,"free: %p\n",ptr);
    free(ptr);
}

char *concatenate(char *a, char *b)
{
    /*allocate space for new string*/
    size_t new_size = strlen(a) + strlen(b) + 1;
    char *c = (char *)mymalloc(new_size);

    /*add each word individually, character by character*/
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
      char *part1 = concatenate(number, argv[3]);
      char *part2 = concatenate(middle, part1);
      char *part3 = concatenate(argv[2], number);
      char *line = concatenate(part3, part2);
      myfree(part2);
      myfree(part3);

      if(i > 0) /*should not free statically allocated "cruel" string*/
        myfree(middle);
      /*end leaky*/

      printf("%s \n",line);
      middle = line;
    }

    myfree(middle);
    return 0;
}
