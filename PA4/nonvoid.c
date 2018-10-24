#include <stdio.h>

int badFunction(int number) {
	if (number > 0) {
		printf("This is a disjoint set");
		return 0;
	}
	else if (number <= 0) {
		printf("But the compiler does not know that");
		return 0;
	}
}		

int main() {
	return 0;
}