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

#ifndef GESTUREBUF
    #define GESTUREBUF 2
#endif

typedef struct sockaddr_in sockaddr_in;

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

void checkArgs(int argc, char **argv, int* port_offset, char *hostname) {
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Usage: rpsls_client ip_address [port_offset]\n");
        exit(1);
    }
    else if (argc >= 2) {
        strncpy(hostname, argv[1], strlen(argv[1]));
        if (argc == 3) {
            *port_offset = strtol(argv[2], NULL, 10);
        }
    }
}

int initConnectWriteName(int tryPort, struct hostent *hp, char *name) { 
    // init sockaddr_in struct, connect, return sock's fd

    // SOCK_STREAM is TCP (as opposed to UDP)
    // 0 is no specified socket, but we default to TCP here based on the other two settings
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    // define what outbound connection to do
    sockaddr_in server_address;

    // from example: Clear this field; sin_zero is used for padding for the struct.
    memset(&(server_address.sin_zero), 0, 8);

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(tryPort); // note that you can't put int directly here; use htons() to convert
    server_address.sin_addr = *((struct in_addr *) hp->h_addr);
    // server_address.sin_addr.s_addr = INADDR_ANY; // same as 0.0.0.0

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
        if (write_name < 0) {
            perror("write");
            exit(1);
        }
        printf("write_name returned %d\n", write_name);
    }

    return sock;

}

char* getGesture() {
    char *buf = malloc(BUFSIZE * sizeof buf);
    printf("Your gesture:\n");
    fgets(buf, BUFSIZE, stdin);

    char onechar[1];
    strncpy(onechar, buf, 1);
    // printf("onechar: %s | strlen: %zu | (strncmp(onechar, r, 1): %d\n", onechar, strlen(onechar), strncmp(onechar, "r", 1));

    // check that it's valid
    if ((strlen(buf) > 2) || 
        ((strncmp(onechar, "r", 1) &&
        strncmp(onechar, "p", 1) &&
        strncmp(onechar, "s", 1) &&
        strncmp(onechar, "l", 1) &&
        strncmp(onechar, "S", 1) &&
        strncmp(onechar, "e", 1)
        )) == 1) {

        printf("Invalid gesture, please enter [r, p, s, l, S, e].\n");
        getGesture();
    }

    // add network line-end
    char *lineend = "\r\n";
    char *ges = malloc(sizeof *onechar * 3);
    sprintf(ges, "%s%s", onechar, lineend);
    free(buf);
    return ges;
}


int main(int argc, char **argv) {

	int port_offset = 0;
	char *hostname = malloc(sizeof(*hostname) * INET_ADDRSTRLEN);

    checkArgs(argc, argv, &port_offset, hostname);

    // Parse IP address.
    struct hostent *hp = gethostbyname(hostname);
    if (hp == NULL) {
        fprintf(stderr, "unknown host %s\n", hostname);
        exit(1);
    }

	// Get player name
	char *name = getName();

    int tryPort = PORT+port_offset;
    // make my socket, with which I make outbound connections
    int sock = initConnectWriteName(tryPort, hp, name);

    char buffer[BUFSIZE+2];

    // Wait for player 2
    while(1) {
        const char* close_msg = "SERVER FULL\r\n";
        const char* wait_msg = "WAITING FOR P2\r\n";
        const char* ready_msg = "P2 READY\r\n";
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
        } else if (strncmp(wait_msg, buffer, strlen(buffer)) == 0) {
            continue;
        } else if (strncmp(ready_msg, buffer, strlen(buffer)) == 0) {
            break;
        }
    }

    printf("Found p2!\n");

    // Get gesture
    
    while(1) {
        char *ges;
        ges = getGesture();
    }
    
    return 0;

    close(sock);
	free(name);


}