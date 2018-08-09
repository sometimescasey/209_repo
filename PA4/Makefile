CC=gcc -std=gnu99 

all: rpsls_client rpsls_server guess

rpsls_client : rpsls_client.c rpsls_utils.c
	$(CC) -o rpsls_client rpsls_client.c rpsls_utils.c

rpsls_server : rpsls_server.c rpsls_utils.c
	$(CC) -o rpsls_server rpsls_server.c rpsls_utils.c

guess : guess.c
	$(CC) -o guess guess.c

.PHONY: clean

clean:
	find . -type f -executable -exec rm '{}' \;