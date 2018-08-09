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
} userInfo;

int find_network_newline(const char *buf, int n);

int max(int one, int two) {
	if (one > two) {
		return one;
	}
	return two;
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
        perror("setsockopt");
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
        fprintf(stderr,"Bind to port %d failed; trying port %d\n", tryPort, tryPort+1);
        // try binding one more time to 60001+n
        tryPort++;
        server_address.sin_port = htons(tryPort);
        bind_result = bind(sock, (struct sockaddr *) &server_address, sizeof(server_address));
        if (bind_result < 0) {
            perror("bind");
            exit(1);
        }
    }

    int listen_result;

    // allow max 2 connections per port
    listen_result = listen(sock, MAX_CONNECTIONS); 

    if (listen_result < 0) {
        perror("listen");
        exit(1);
    }

    return sock;
}

int main(int argc, char **argv) {

    // TODO; remove all prints at the end

    // initialize array to hold user info
    userInfo *connections = malloc(sizeof(*connections) * MAX_CONNECTIONS);

    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        connections[i].sock_fd = -1;
        connections[i].username = malloc(sizeof(char) * NAMEBUFFER);
    }

    int port_offset = 0;
    const char *hostname;

    if (argc > 3) {
        fprintf(stderr, "Usage: rpsls_server [port_offset]\n");
        exit(1);
    }
    else if (argc == 2) {
        port_offset = strtol(argv[1], NULL, 10);
    }

    int tryPort = PORT+port_offset;

    int sock = initBindListen(tryPort);

    fd_set all_fds;
    FD_ZERO(&all_fds);
    // add the currently open socket
    FD_SET(sock, &all_fds);
    int max_fd = sock; // tracker for highest valued fd; so far it's just sock

    // second and third params are pointers to structs that hold address info, and size
    sockaddr_in peer;
    unsigned int peer_len = sizeof(peer);
    peer.sin_family = PF_INET; // do I need this? isn't it autopop?

    printf("Waiting for connections...\n");
    int new_client_fd = -1;

    char nameBuffer[NAMEBUFFER+1];

    while(1) { // inf loop

        // select updates the fd_set it receives, so we always use a copy and retain the original.
        fd_set listen_fds = all_fds;
        // last parameter is a timeout block
        int nready = select(max_fd+1, &listen_fds, NULL, NULL, NULL);
        if (nready == -1) {
            perror("server: select");
            exit(1);
        }
        // listen_fds has been modified to a ready one

        

        if (FD_ISSET(sock, &listen_fds)) {
            printf("FD_ISSET\n");
            // if sock is the ready one, it must be a new connection; accept it
            new_client_fd = accept(sock, (struct sockaddr *) &peer, &peer_len);

            if (connections[MAX_CONNECTIONS-1].sock_fd != -1) { // only accept new connections if we have room 
                const char* close_msg = "Sorry the server is full. Closing\r\n";
                write(new_client_fd, close_msg, strlen(close_msg));                    
            } else {
            // get new client's username, which is the first thing they write
                memset(nameBuffer, '\0', sizeof(*nameBuffer) * NAMEBUFFER);
                int read_name;
                read_name = read(new_client_fd, nameBuffer, NAMEBUFFER);

                if (read_name < 0) {
                    perror("read");
                    exit(1);
                }

                printf("Accepted connection from peer %d, username %s\n", new_client_fd, nameBuffer);
                const char* welcome_template = "Hi %s! Waiting for another player...\r\n";
                char *welcome_message = malloc(sizeof(*welcome_message) * BUFSIZE);
                sprintf(welcome_message, welcome_template, nameBuffer);
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
            printf("set user_index = %d to username %s\n", user_index, nameBuffer);
            for (int q = 0; q < MAX_CONNECTIONS; q++) {
                printf("connections[%d].username =%s, sock_fd=%d\n", 
                    q, 
                    connections[q].username, 
                    connections[q].sock_fd);
            }

            // update max_fd if needed
            if (new_client_fd > max_fd) max_fd = new_client_fd;

            // add to our &all_fds
            FD_SET(new_client_fd, &all_fds);
        }
    }




        // check other clients for updates; iterate thru all (since more than one can be ready)
        int current_fd;
        char readBuffer[BUFSIZE+1];
        for (int i = 0; i < MAX_CONNECTIONS; i++) {
            current_fd = connections[i].sock_fd;
            if (current_fd > 0 && FD_ISSET(current_fd, &listen_fds)) {     

                // clear buffer
                memset(readBuffer, '\0', sizeof(*readBuffer) * BUFSIZE);
                
                int read_result;
                // bufferedRead returns 0 if successful. TODO: handle
                read_result = bufferedRead(current_fd, readBuffer);
                
                int readBuffer_len = strlen(readBuffer); 
                printf("readBuffer_len is %d\n", readBuffer_len);

                printf("Getting connections[%d].username = %s\n", i, connections[i].username);
                printf("Read message from client %d, username %s: %s\n", current_fd, connections[i].username, readBuffer);

                // send the message to all the other active clients
                int to_fd;
                for (int j = 0; j < MAX_CONNECTIONS; j++) { 
                    to_fd = connections[j].sock_fd;
                    if (to_fd > 0 && to_fd != current_fd) {
                        printf("writing to %s\n", connections[j].username);
                        const char* msg_template = "%s: %s\n";
                        char *msg = malloc(sizeof(*msg) * BUFSIZE);
                        sprintf(msg, msg_template, connections[i].username, readBuffer);
                        int written;
                        written = write(to_fd, msg, strlen(msg));
                        printf("wrote %d bytes | strlen(readBuffer) is %zu\n", written, strlen(readBuffer));
                        const char *term = "\r\n";
                        write(to_fd, term, 2); // force it
                    } 
                }

            }
        }



        
        
    }

    

	return 0;

}