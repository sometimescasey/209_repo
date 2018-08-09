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
	#define NAMEBUFFER 22
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
	printf("What is your name?\n");
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
        // printf("Successfully connected to server on port %d\n", tryPort);
        // write username
        int write_name;
        write_name = write(sock, name, strlen(name));
        if (write_name < 0) {
            perror("write");
            exit(1);
        }
    }

    return sock;

}

char* getGesture(int sock) {
    // get gesture from user and send through socket
    char *buf = malloc(BUFSIZE * sizeof buf);
    int valid = 0;

    char onechar[1];

    while (valid == 0) {
        printf("Your gesture:\n");
        fgets(buf, BUFSIZE, stdin);

        onechar[0] = '\0';
        strncpy(onechar, buf, 1);
        
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
        } else {
             valid = 1;
        }
    }

    // add network line-end
    char *lineend = "\r\n";
    char *ges = malloc(sizeof *onechar * 3);
    sprintf(ges, "%s%s", onechar, lineend);
    free(buf);

    // write gesture to server
    int write_ges;
    write_ges = write(sock, ges, strlen(ges));
    if (write_ges < 0) {
        perror("write");
        exit(1);
    }
    // printf("wrote gesture %s to server, %d bytes written\n", ges, write_ges);

    return ges;
}


int main(int argc, char **argv) {

    int my_wins = 0;
    char opponent[NAMEBUFFER];

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

    // Loop 1: Wait for player 2
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
            // fprintf(stderr, "Server is full. Try again later. Closing socket.\n");
            close(sock);
            return 0;
        } else if (strncmp(wait_msg, buffer, strlen(buffer)) == 0) {
            continue;
        } else if (strncmp(ready_msg, buffer, strlen(buffer)) == 0) {
            break;
        }
    }

    // get name
    char oppbuffer[NAMEBUFFER];
    memset(oppbuffer, '\0', sizeof(*oppbuffer) * (NAMEBUFFER));
        int read_result;
        read_result = bufferedRead(sock, oppbuffer);
        if (read_result < 0) {
            perror("read");
            exit(1);
    }
    strncpy(opponent, oppbuffer, strlen(oppbuffer));
    printf("Playing against %s\n", opponent);

    char *ges;
    ges = getGesture(sock);

    // Loop 2: Wait for win/lose message, or game over
    const char* game_over_msg = "GAME OVER\r\n";
    
    while(1) {
        // check for new messages from the server
        // clear buffer
        memset(buffer, '\0', sizeof(*buffer) * BUFSIZE);
        int read_result;
        read_result = bufferedRead(sock, buffer);
        if (strncmp(buffer, game_over_msg, strlen(game_over_msg-2)) == 0) {
            break;
        }
        if (read_result < 0) {
            perror("read");
            exit(1);
        }
        char *winmessage = malloc(sizeof(char) * BUFSIZE);
        sprintf(winmessage, "%s WINS!", name);
        my_wins += 1;
        if (strncmp(buffer, winmessage, strlen(winmessage)) == 0) {
            printf("You won!\n");
            fprintf(stderr, "win\n");
        }  else if (strncmp(buffer, "TIE", strlen("TIE")) == 0) {
            printf("Tie\n");
            fprintf(stderr, "tie\n");
        } else {
            printf("You lost!\n");
            fprintf(stderr, "lose\n");
        }
        getGesture(sock);
    }

    // parse game end data from buffer

    int games = 0;
    int wins = 0;
    int losses = 0;
    int ties = 0;



    char *pt;

    pt = strtok (buffer, ",");
    pt = strtok (NULL, ",");

    char copybuf[BUFSIZE];

    strncpy(copybuf, pt+2, strlen(pt-2));
    games = strtol(copybuf, NULL, 10);

    pt = strtok (NULL, ","); // NEXT: wins
    strncpy(copybuf, pt+2, strlen(pt-2));
    wins = strtol(copybuf, NULL, 10);

    pt = strtok (NULL, ","); // NEXT: losses
    strncpy(copybuf, pt+2, strlen(pt-2));
    losses = strtol(copybuf, NULL, 10);

    pt = strtok (NULL, ","); // NEXT: ties. note account for \r\n
    strncpy(copybuf, pt+2, strlen(pt-4));
    ties = strtol(copybuf, NULL, 10);

    if (wins == losses) {
        // tie
        printf("%d total games. %s and %s tied %d-%d, %d ties\n", games, name, opponent, wins, losses, ties);
    }
    else if (wins > losses) {
        // I'm the winner
        printf("%d total games. %s wins %d-%d, %d ties\n", games, name, wins, losses, ties);
    } else {
        // I lost
        printf("%d total games. %s loses %d-%d, %d ties\n", games, name, wins, losses, ties);
    }

    fprintf(stderr, "%d %d-%d\n", sock, wins, losses);
    
    close(sock);
    free(name);
    return 0;

    


}