int populate_array(int i, int *arr) {
	int current = i;
	int digit;

	for (int i = 8; i >= 0; i--) {
		digit = current % 10;
		arr[i] = digit;
		current /= 10;
		if (current < 1 && i > 1) { // didn't get 9 digits
			return 1;
		}
	}

	return 0;

}

int check_sin(int *arr) {
	int check[9];
	populate_array(121212121, check);

	int running_sum = 0;
	int checksum[9];
	for (int i = 0; i < 9; i++) {
		int product = check[i] * arr[i];
		if (product >= 10) {
			checksum[i] = (product % 10) + (product / 10);
		}
		else {
			checksum[i] = product;
		}
		running_sum += checksum[i];
		// printf("checksum[i]: %d\n", checksum[i]);
	}

	if (running_sum % 10 == 0) {
			return 0;
		}
		else {
			return 1;
		}
}