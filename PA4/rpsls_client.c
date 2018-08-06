#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>     /* inet_ntoa */
#include <netdb.h>         /* gethostname */

#ifndef PORT
  #define PORT 60000
#endif

#ifndef NAMEBUFFER
	#define NAMEBUFFER 20
#endif

typedef struct sockaddr_in sockaddr_in;

int connect_to_server(int port, const char* hostname) {
	int soc = socket(PF_INET, SOCK_STREAM, 0);
    if (soc < 0) {
        perror("socket");
        exit(1);
    }
    sockaddr_in addr;

    addr.sin_family = PF_INET;
    addr.sin_port = htons(port);
    memset(&(addr.sin_zero), 0, 8);

    struct hostent *host_ip = gethostbyname(hostname);
    if (host_ip == NULL) {
        fprintf(stderr, "unknown host %s\n", hostname);
        exit(1);
    }

    addr.sin_addr = *((struct in_addr *) host_ip->h_addr);

    // Request connection to server.
    if (connect(soc, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("connect");
        exit(1);
    }

    return soc;

}

char* getName() {
	char *buf = malloc(NAMEBUFFER * sizeof buf);
	printf("What is your name? Max 20 char, (A-Za-z0-9-_ )\n");
	fgets(buf, NAMEBUFFER, stdin);
	// TODO: name error check
	printf("Player's name is %s, strlen is %zu\n", buf, strlen(buf));

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

	// Connect to server and write name
	int port = PORT + port_offset;
	int server_socket = connect_to_server(port, hostname);
	write(server_socket, name, NAMEBUFFER);

	free(name);


}