#include <stdio.h>

int main() {
	// TODO
	char phone[11];
	int integer;

	scanf("%10s", phone);

	int errors = 0;

	while (scanf("%d", &integer) != EOF) {
		if (integer == 0) {
			for (int i = 0; i < 11; i++) {
				printf("%c", phone[i]);
			}
			printf("\n");
		}
		else if (integer >= 1 && integer <= 9) {
		printf("%c\n", phone[integer - 1]); // array is zero-indexed. 
											// instructions say to print corresponding digit
											// i.e. assuming "1" means first digit
		}
		else {
		printf("ERROR\n");
		errors = 1;
		}
	}
	
	if (errors == 1) {
		return 1;
	}
	else {
		return 0;
	}
}