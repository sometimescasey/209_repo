#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DELIMITER ":"

struct flight {
	char *code;
	int seats_available;
};

/* Return a pointer to a new struct flight with its code and seats_available
* initialized to the data given in arg. arg will be a flight code (e.g., AC1123)
* followed by a colon (:) and the number of seats on that flight (e.g., 100).
* Once a flight is created, its code must refer to a dynamically-allocated string. */
struct flight *create_flight(char *arg) {
	struct flight *newFlight = malloc(sizeof(struct flight));

	// strstr returns pointer to first occurence of substring
	char *delimiter = strstr(arg, ":");
	char *after = (delimiter + 1);

	int before_len = strlen(arg) - strlen(after);
	char *before = malloc(sizeof(char) * before_len); // LOSE A POINT HERE
	strncpy(before, arg, before_len);
	before[before_len-1] = '\0';

	newFlight->code = before;
	newFlight->seats_available = strtol(after, NULL, 10);

	return newFlight;
}

/* If there are enough seats available on f, book num_seats; Otherwise do nothing. */
void book_seat(struct flight* f, int num_seats) {
	if (f->seats_available >= num_seats) {
		f->seats_available -= num_seats;
	}
	printf("Seats available: %d\n", f->seats_available);
}

int main(int argc, char **argv) {
	// argv[1] represents a flight in the format code:seats (e.g., AC1123:100, WSG324:24)
	struct flight *my_flight = create_flight(argv[1]);
	book_seat(my_flight, 2);
	// Free memory
	free(my_flight->code); // LOSE A POINT HERE
	free(my_flight);
	
	return 0;
}