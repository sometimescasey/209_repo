#include <stdio.h>

int main() {
	int x,y;

	int a = 2;
	int b = 8;

	x = a|b; // bitwise: 2 = 0010, 8 = 0100, 10 = 0110
	y = a||b;

	printf("x, y: %d, %d \n", x, y);
}