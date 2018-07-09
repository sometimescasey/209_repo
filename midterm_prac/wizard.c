#include <stdio.h>
#include "wizard.h"
#include <stdlib.h>
#include <string.h>

int main() {
	returnOne();
	printf("Hello world!\n");


	char *name = "Me";
	printf("%s\n", name);
	// free(name);

	char *animal = "cat";
	// animal[0] = 'r';
	printf("%s\n", animal);

	printf("%c\n", animal[0]);

	char s[] = "RaCecAr";
	char *p = s;
	int q = 6;
	while (p <= &s[q]) {
		if (*p != s[q]) {
			*p = s[q];
		}
		p++;
		q--;
	}
	printf("%s\n", s);

	char *w = malloc(3 * sizeof(char));
	strcpy(w, "AB");
	char *m[2];
	m[0] = w;
	*(m+1) = w + 1;
	printf("%s, %s\n", *m, *(m+1));

	char s2[10] = "computer";
	s2[6] = '\0';
	printf("%s\n", s2);
}
