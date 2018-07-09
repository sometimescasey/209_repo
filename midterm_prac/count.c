#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int same(char *str, char *substr) {
	int str_len = strlen(str);
	int sub_len = strlen(substr);
	if (str_len < sub_len) {
		return 0;
	}
	// int same = 1;
	for (int i = 0; i < sub_len; i++) {
		if (str[i] != substr[i]) {
			return 0;
		}
	}

	return 1;
}

void count_occurences(char *str, char *substr, int *result) {
	char *test = str;
	char *sub = substr;

	int counter = 0;
	char *newaddy;

	for (int i = 0; i < strlen(test); i++) {
		newaddy = test + i;
		counter += same(newaddy, sub);
		printf("counter: %d\n", counter);
	}

	*result = counter;

}

int main() {

	int result;
	char *str = "ababa";
	char *substr = "aba";
	count_occurences(str, substr, &result);
	printf("Final count: %d\n", result);

}
