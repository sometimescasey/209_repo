#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Write the copy function to perform exactly as strncpy does, with one
   exception: your copy function will guarantee that dest is always
   null-terminated.
   You shoud read the man page to learn how strncpy works.

  NOTE: You must write this function without using any string functions.
  The only function that should depend on string.h is memset.
 */

char *copy(char *dest, const char *src, int capacity) {
  int i;

  for (i = 0; i < capacity && src[i] != '\0'; i++) {
    dest[i] = src[i];
  }
  
  for (; i < capacity; i++) {
    dest[i] = '\0';
  }

  // check if dest[capacity-1] is null terminated
  if (dest[capacity-1] != '\0') {
    // we need to make a new array of length capacity+1, 
    // copy dest, add a null terminator, and return that
    char *newdest = malloc((capacity+1) * sizeof(char));

    for (int j = 0; j < capacity; j++) {
      newdest[j] = dest[j];
    }
    newdest[capacity] = '\0';
    dest = newdest;
    free(newdest);
  }   

  return dest;
}


int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: copy size src\n");
        exit(1);
    }
    int size = strtol(argv[1], NULL, 10);
    if (size < 1) { // error checking for bad values of size
      fprintf(stderr, "size cannot be 0 or negative\n");
        exit(1);
    }
    char *src = argv[2];

    char dest[size];
    memset(dest, 'x', size);

    copy(dest, src, size);

    printf("%s\n", dest);
    return 0;
}
