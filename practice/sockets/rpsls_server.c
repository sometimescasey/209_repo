#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>     /* inet_ntoa */

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

#define MAX_CONNECTIONS 2

typedef struct sockaddr_in sockaddr_in;

typedef struct userInfo {
    int sock_fd;
    char *username;
    int wins;
} userInfo;

userInfo* initUserArray() {
    userInfo *connections = malloc(sizeof(*connections) * MAX_CONNECTIONS);
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        connections[i].sock_fd = -1;
        connections[i].username = malloc(sizeof(char) * NAMEBUFFER);
        connections[i].wins = 0;
    }
    return connections;
}

void checkArgs(int argc, char **argv, int* port_offset) {
    if (argc > 3) {
        // fprintf(stderr, "Usage: rpsls_server [port_offset]\n");
        exit(1);
    }
    else if (argc == 2) {
        *port_offset = strtol(argv[1], NULL, 10);
    }
}

int initBindListen(int tryPort) {
    // initialize sock, sockaddr_in, bind, listen, return fd
    // make my socket, with which I make outbound connections
    int sock;
    // SOCK_STREAM is TCP (as opposed to UDP)
    // 0 is no specified socket, but we default to TCP here based on the other two settings
    sock = socket(AF_INET, SOCK_STREAM, 0);

    // From example: Make sure we can reuse the port immediately after the
    // server terminates. Avoids the "address in use" error
    int on = 1;
    int status = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
        (const char *) &on, sizeof(on));
    if (status < 0) {
        // perror("setsockopt");
        exit(1);
    }

    // define what outbound connection to do
    sockaddr_in server_address;

    // some properties to define
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(tryPort); // note that you can't put int directly here; use htons() to convert
    server_address.sin_addr.s_addr = INADDR_ANY; // same as 0.0.0.0

    int bind_result;
    bind_result = bind(sock, (struct sockaddr *) &server_address, sizeof(server_address));
    if (bind_result < 0) {
        // fprintf(stderr,"Bind to port %d failed; trying port %d\n", tryPort, tryPort+1);
        // try binding one more time to 60001+n
        tryPort++;
        server_address.sin_port = htons(tryPort);
        bind_result = bind(sock, (struct sockaddr *) &server_address, sizeof(server_address));
        if (bind_result < 0) {
            // perror("bind");
            exit(1);
        }
    }

    int listen_result;

    // allow max 2 connections per port
    listen_result = listen(sock, MAX_CONNECTIONS); 

    if (listen_result < 0) {
        // perror("listen");
        exit(1);
    }

    return sock;
}

int handleNewPlayer(int sock, fd_set *all_fds, userInfo *connections, int *max_fd) {
    // reject if too many players; otherwise update array
    int new_client_fd = -1;
    char nameBuffer[NAMEBUFFER+1];

    // second and third params are pointers to structs that hold address info, and size
    sockaddr_in peer;
    unsigned int peer_len = sizeof(peer);
    peer.sin_family = PF_INET; // do I need this? isn't it autopop?

    new_client_fd = accept(sock, (struct sockaddr *) &peer, &peer_len);
    if (new_client_fd < 0) {
        // no new client yet
        return -1;
    }

    if (connections[MAX_CONNECTIONS-1].sock_fd != -1) { // only accept new connections if we have room 
        const char* close_msg = "SERVER FULL\r\n";
        write(new_client_fd, close_msg, strlen(close_msg));                    
    } else {
        // get new client's username, which is the first thing they write
        memset(nameBuffer, '\0', sizeof(*nameBuffer) * NAMEBUFFER);
        int read_name;
        read_name = read(new_client_fd, nameBuffer, NAMEBUFFER);

        if (read_name < 0) {
            // perror("read");
            exit(1);
        }

        char *welcome_message = "WAITING FOR P2\r\n";
        write(new_client_fd, welcome_message, strlen(welcome_message));

            // update our userInfo array
        int user_index = 0;
        while (user_index < MAX_CONNECTIONS && connections[user_index].sock_fd != -1) {
                user_index++; // find next empty slot in the array
            }

            connections[user_index].sock_fd = new_client_fd;
            strncpy(connections[user_index].username, nameBuffer, strlen(nameBuffer));
            // don't do this: this sets the pointers equal to each other
            // connections[user_index].username = nameBuffer;

            // update max_fd if needed
            if (new_client_fd > *max_fd) *max_fd = new_client_fd;

            // add to our &all_fds
            FD_SET(new_client_fd, all_fds);
        }
}

void monitorForNew(int sock, fd_set *listen_fds, fd_set *all_fds, int *max_fd, userInfo* connections) {
    // update listen_fds
    // lets sock handle new connections from clients.
    
    // last parameter is a timeout block
    int nready = select(*max_fd+1, listen_fds, NULL, NULL, NULL);
    if (nready == -1) {
        // perror("server: select");
        exit(1);
    }
    // listen_fds has now been modified to ready ones

    if (FD_ISSET(sock, listen_fds)) {
        // original socket has something to say; must be a new connection
        handleNewPlayer(sock, all_fds, connections, max_fd);
    }
}

void writeToAll(char *msg, userInfo *connections) {
    // send the message to all the other active clients
    // int to_fd;
    for (int j = 0; j < MAX_CONNECTIONS; j++) { 
        int to_fd = connections[j].sock_fd;
        if (to_fd > 0) {
            // printf("writing to %s\n", connections[j].username);
            // const char* msg_template = "%s: %s\n";
            // char *msg = malloc(sizeof(*msg) * BUFSIZE);
            // sprintf(msg, msg_template, connections[i].username, readBuffer);
            int written;
            written = write(to_fd, msg, strlen(msg));
            // printf("wrote %d bytes to %d \n", written, to_fd);
            // const char *term = "\r\n";
            // write(to_fd, term, 2); // write network newline
        } 
    }
}

int judgeRound(char *moves) {
    // returns 0 if connections[0] won, 
    // 1 if connections[1] won,
    // -1 if tie
    // printf("judging...\n");

    if (moves[0] == moves[1]) {
        return -1;
    }

    switch(moves[0]) {
        case 'r': // rock crush scissor, rock crush lizard
            if (moves[1] == 's' || moves[1] == 'l') { return 0; }
            return 1;
        case 'p': // paper covers rock, paper disproves Spock
            if (moves[1] == 'r' || moves[1] == 'S') { return 0; }
            return 1;
        case 's': // scissors cut paper, scissors decapitate lizard
            if (moves[1] == 'p' || moves[1] == 'l') { return 0; }
            return 1;
        case 'l': // lizard poisons spock, lizard eats paper
            if (moves[1] == 'S' || moves[1] == 'p') { return 0; }
            return 1;
        case 'S': // Spock vaporizes rock, Spock smashes scissors
            if (moves[1] == 'r' || moves[1] == 's') { return 0; }
            return 1;
    }
}

int getWinner(userInfo *connections) {
    if (connections[0].wins == connections[1].wins) {
        return -1;
    }

    if (max(connections[0].wins, connections[1].wins) == connections[0].wins) {
        return 0;
    } else {
        return 1;
    }
}

int main(int argc, char **argv) {

    int ties = 0;
    int games = 0;

    // TODO; remove all prints at the end

    // initialize array to hold user info
    userInfo *connections = initUserArray();

    // get/check user args
    int port_offset = 0;
    checkArgs(argc, argv, &port_offset);
    int tryPort = PORT+port_offset;

    // set up socket
    int sock = initBindListen(tryPort);

    fd_set all_fds;
    FD_ZERO(&all_fds);
    // add the currently open socket
    FD_SET(sock, &all_fds);
    int max_fd = sock; // tracker for highest valued fd; so far it's just sock

    // printf("Waiting for connections...\n");

    // select updates the fd_set it receives, so we always use a copy and retain the original.
    fd_set listen_fds = all_fds;

    // first while loop: wait for two players
    while(connections[MAX_CONNECTIONS-1].sock_fd < 0) { // loop until a second player connects
        monitorForNew(sock, &listen_fds, &all_fds, &max_fd, connections);
    }

    char* ready_msg = "P2 READY\r\n";
    writeToAll(ready_msg, connections);

    write(connections[0].sock_fd, connections[1].username, strlen(connections[1].username));
    write(connections[0].sock_fd, "\r\n", 2);

    write(connections[1].sock_fd, connections[0].username, strlen(connections[0].username));
    write(connections[1].sock_fd, "\r\n", 2);

int stopPlaying = 0;
while(stopPlaying != 1) {

    char moves[2]; // array to store this round's moves; clear out
    moves[0] = '\0';
    moves[1] = '\0';

    // wait for both players to make their moves
    while((moves[0] == '\0' || moves[1] == '\0') && stopPlaying != 1) {
        fd_set listen_fds2 = all_fds;
        
        // struct timeval timeout;
        // timeout.tv_sec = 5;
        // timeout.tv_usec = 0;

        int nready = select(max_fd+1, &listen_fds2, NULL, NULL, NULL);
        // printf("max_fd+1: %d, nready: %d\n", max_fd+1, nready);
        if (nready == -1) {
            // perror("server: select");
            exit(1);
        }
        

        // check clients for updates; iterate thru all (since more than one can be ready)
        int current_fd;
        char readBuffer[BUFSIZE+1];
        for (int i = 0; i < MAX_CONNECTIONS; i++) {
            current_fd = connections[i].sock_fd;
            if (current_fd > 0 && FD_ISSET(current_fd, &listen_fds2)) {     

                // clear buffer
                memset(readBuffer, '\0', sizeof(*readBuffer) * BUFSIZE);
                
                int read_result;
                // bufferedRead returns 0 if successful. TODO: handle
                read_result = bufferedRead(current_fd, readBuffer);

                // check if someone wants to end the game
                if (strncmp(readBuffer, "e", 1) == 0) {
                    stopPlaying = 1;
                    break;
                }

                moves[i] = (char) readBuffer[0]; 
                int readBuffer_len = strlen(readBuffer); 
            }
        }   
        
    }

    if (stopPlaying != 1) {
        // process moves, find winner
        int winner = judgeRound(moves);
        games += 1;
        // printf("winner = %d\n", winner);
        char *msg = malloc(sizeof *msg * BUFSIZE);
        if (winner == -1) {
            strncpy(msg, "TIE\r\n", strlen("TIE\r\n"));
            ties += 1;
        } else {
            sprintf(msg, "%s WINS!\r\n", connections[winner].username);
            connections[winner].wins += 1;
        }
        // write to all
        writeToAll(msg, connections);
    }

}

    char* game_over_template = "GAME OVER,G:%d,w:%d,l:%d,t:%d\r\n";

    char *game_over_msg = malloc(sizeof(char) * BUFSIZE);

    int winner = getWinner(connections);

    if (winner == -1) {
        // tie
        sprintf(game_over_msg, game_over_template, games, connections[0].wins, connections[1].wins, ties);
        write(connections[0].sock_fd, game_over_msg, strlen(game_over_msg));
        write(connections[1].sock_fd, game_over_msg, strlen(game_over_msg));
    } else {
        sprintf(game_over_msg, game_over_template, games, connections[0].wins, connections[1].wins, ties);
        write(connections[0].sock_fd, game_over_msg, strlen(game_over_msg));

        memset(game_over_msg, '\0', sizeof(*game_over_msg) * BUFSIZE);
        sprintf(game_over_msg, game_over_template, games, connections[1].wins, connections[0].wins, ties);
        write(connections[1].sock_fd, game_over_msg, strlen(game_over_msg));
    }

    // cleanup
    free(connections);

	return 0;

}