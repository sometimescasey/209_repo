#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
  Write the main() function of a program that takes exactly two arguments,
  and prints one of the following:
    - "Same\n" if the arguments are the same.
    - "Different\n" if the arguments are different.
    - "Invalid\n" if the program is called with an incorrect number of
      arguments.

  Your main function should return 0, regardless of what is printed.
*/

int main(int argc, char **argv) {
	char *str1;
	char *str2;

	if (argv[1] != NULL && argv[2] != NULL && argv[3] == NULL) {
		str1 = malloc(strlen(argv[1]) + 1); // plus one for ending null
		str2 = malloc(strlen(argv[2]) + 1); // plus one for ending null
		strcpy(str1, argv[1]);
		strcpy(str2, argv[2]);

		int result = strcmp(str1, str2);
		
		if (result == 0) {
			printf("Same\n");
		}
		else {
			printf("Different\n");
		}
		free(str1);
		free(str2);
	}
	else {
		printf("Invalid\n");
	}
	

	return 0;
}