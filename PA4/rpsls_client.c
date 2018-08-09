#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>     /* inet_ntoa */
#include <netdb.h>         /* gethostname */

#include "rpsls_utils.h"

#ifndef PORT
  #define PORT 60000
#endif

#ifndef NAMEBUFFER
	#define NAMEBUFFER 20
#endif

#ifndef BUFSIZE
    #define BUFSIZE 256
#endif

typedef struct sockaddr_in sockaddr_in;

int find_network_newline(const char *buf, int n); // if this works consolidate into one file

char* getName() {
	char *buf = malloc(NAMEBUFFER * sizeof buf);
	printf("What is your name? Max 20 char, (A-Za-z0-9-_ )\n");
	fgets(buf, NAMEBUFFER, stdin);
    // get rid of trailing newline
    buf[strcspn(buf, "\r\n")] = 0;
	// TODO: name error check
	// printf("Player's name is %s, strlen is %zu\n", buf, strlen(buf));
	return buf;
}

int main(int argc, char **argv) {

	int port_offset = 0;
	const char *hostname;

	if (argc < 2 || argc > 3) {
        fprintf(stderr, "Usage: rpsls_client ip_address [port_offset]\n");
        exit(1);
    }
	else if (argc >= 2) {
		hostname = argv[1];
		if (argc == 3) {
			port_offset = strtol(argv[2], NULL, 10);
		}
	}

	// Get player name
	char *name = getName();

    // make my socket, with which I make outbound connections
    int sock;
    // SOCK_STREAM is TCP (as opposed to UDP)
    // 0 is no specified socket, but we default to TCP here based on the other two settings
    sock = socket(AF_INET, SOCK_STREAM, 0);

    // define what outbound connection to do
    sockaddr_in server_address;

    // some properties to define
    int tryPort = PORT+port_offset;

    // from example: Clear this field; sin_zero is used for padding for the struct.
    memset(&(server_address.sin_zero), 0, 8);

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(tryPort); // note that you can't put int directly here; use htons() to convert
    server_address.sin_addr.s_addr = INADDR_ANY; // same as 0.0.0.0

    // connect() needs socket, casted pointer to server_address, and its size
    int connect_result = connect(sock, (struct sockaddr *) &server_address, sizeof(server_address)); 

    // returned int can be used for error handling
    if (connect_result < 0) {
        fprintf(stderr,"Connect to port %d failed; trying port %d\n", tryPort, tryPort+1);
        // try connecting one more time to 60001+n
        tryPort += 1;
        server_address.sin_port = htons(tryPort);
        connect_result = connect(sock, (struct sockaddr *) &server_address, sizeof(server_address));
        if (connect_result < 0) {
            perror("connect");
            exit(1);
        }
    } else {
        printf("Successfully connected to server on port %d\n", tryPort);
        // write username
        int write_name;
        write_name = write(sock, name, strlen(name));
        printf("write_name returned %d\n", write_name);
    }

    char buffer[BUFSIZE+2];

    while(1) {
        const char* close_msg = "Sorry the server is full. Closing\r\n";
        // check for new messages from the server
        // clear buffer
        memset(buffer, '\0', sizeof(*buffer) * BUFSIZE);
        int read_result;
        read_result = bufferedRead(sock, buffer);
        if (read_result < 0) {
            perror("read");
            exit(1);
        } else if (strncmp(close_msg, buffer, strlen(buffer)) == 0){
            fprintf(stderr, "Server is full. Try again later. Closing socket.\n");
            close(sock);
            return 0;
        }

        // set line end just in case
        buffer[BUFSIZE] = '\0';

        printf("%s\n", buffer);

        // write a message to the server
        // clear buffer
        memset(buffer, '\0', sizeof(*buffer) * BUFSIZE);
        printf("Type a message:\n");
        fgets(buffer, BUFSIZE, stdin);
        int end = strlen(buffer);
        buffer[end] = '\r';
        buffer[end+1] = '\n'; // add network term for safety

        int write_result;
        write_result = write(sock, buffer, BUFSIZE);
        if (write_result < 0) {
            perror("write");
            exit(1);
        }
    }
    
    return 0;

    close(sock);


	free(name);


}