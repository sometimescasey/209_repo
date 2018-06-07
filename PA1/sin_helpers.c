int populate_array(int i, int *arr) {
	printf("populate_array is running\n");
	int current = i;
	int digit;

	for (int i = 8; i >= 0; i--) {
		digit = current % 10;
		arr[i] = digit;
		current /= 10;
		if (current < 1 && i > 8) { // didn't get 9 digits
			return 1;
		}
		printf("%d\n", digit);
	}

	return 0;

}

int check_sin() {
	return 0;
}