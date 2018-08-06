#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>     /* inet_ntoa */

typedef struct sockaddr_in sockaddr_in;

#ifndef PORT
  #define PORT 60000
#endif

#ifndef NAMEBUFFER
	#define NAMEBUFFER 20
#endif


int make_socket(int port, int num_queue) {
	int sock;
	sockaddr_in name;

	// Create socket
	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	// socket name and info
	name.sin_family = AF_INET;
	name.sin_port = htons(port);
	name.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sock, (struct sockaddr *) &name, sizeof(name)) < 0) {
		perror("bind");
		exit(EXIT_FAILURE);
	}

	// Set up a queue in the kernel to hold pending connections.
    if (listen(sock, num_queue) < 0) {
        // listen failed
        perror("listen");
        exit(1);
    }

	return sock;
}

// wait for and accept new connectoin
int accept_connection(int listen_socket) {
	sockaddr_in peer;
	unsigned int peer_len = sizeof(peer);
    peer.sin_family = PF_INET;

    fprintf(stderr, "Waiting for a new connection...\n");
    int client_sock = accept(listen_socket, (struct sockaddr *)&peer, &peer_len);
    if (client_sock < 0) {
    	perror("accept");
    	exit(EXIT_FAILURE);
    } else {
    	fprintf(stderr, "New connection accepted from %s:%d\n",
    		inet_ntoa(peer.sin_addr),
    		ntohs(peer.sin_port));
    return client_sock;
    }

}

int main(int argc, char **argv) {
	int port_offset = 0;

	if (argc > 2) {
        fprintf(stderr, "Usage: rpsls_server [port_offset]\n");
        exit(1);
    }
	else if (argc == 2) {
		port_offset = strtol(argv[1], NULL, 10);
	}

	int listen_socket = make_socket(PORT, 5);

	while(1) {
		// inf loop! keep listening
		int fd = accept_connection(listen_socket);
		if (fd < 0) {
		// while again, keep listening
            continue;
        }

        int read_bytes;
        

        char namebuffer[NAMEBUFFER];
        read_bytes = read(fd, namebuffer, NAMEBUFFER);

        if (read_bytes == NAMEBUFFER) {
        	printf("buffer: %s\n", namebuffer);
        }
	}
	


	
	close(listen_socket);
	return 0;

}