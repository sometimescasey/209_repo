#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#ifndef PORT
  #define PORT 30000
#endif
#define MAX_BACKLOG 5
#define MAX_CONNECTIONS 12
#define BUF_SIZE 20


struct sockname {
    int sock_fd;
    char *username;
};


/* Accept a connection. Note that a new file descriptor is created for
 * communication with the client. The initial socket descriptor is used
 * to accept connections, but the new socket is used to communicate.
 * Return the new client's file descriptor or -1 on error.
 */
int accept_connection(int fd, struct sockname *connections) {
    int user_index = 0;
    while (user_index < MAX_CONNECTIONS && connections[user_index].sock_fd != -1) {
        user_index++;
    }

    if (user_index == MAX_CONNECTIONS) {
        fprintf(stderr, "server: max concurrent connections\n");
        return -1;
    }

    int client_fd = accept(fd, NULL, NULL);
    if (client_fd < 0) {
        perror("server: accept");
        close(fd);
        exit(1);
    }

    connections[user_index].sock_fd = client_fd;
    connections[user_index].username = NULL;
    return client_fd;
}


/* Read a message from client_index and echo it back to them.
 * Return the fd if it has been closed or 0 otherwise.
 */
int read_from(int client_index, struct sockname *connections) {
    int fd = connections[client_index].sock_fd;
    char buf[BUF_SIZE + 1];

    int num_read = read(fd, &buf, BUF_SIZE);
    buf[num_read] = '\0';
    if (num_read == 0) {
        connections[client_index].sock_fd = -1;
        return fd;
    }

    for (int i = 0; i < (sizeof connections / sizeof connections[0]); i++) {
        // write to all clients
        fd = connections[i].sock_fd;
        write(fd, buf, strlen(buf));
    }
    
    return 0;
}


int main(void) {
    struct sockname connections[MAX_CONNECTIONS];
    for (int index = 0; index < MAX_CONNECTIONS; index++) {
        connections[index].sock_fd = -1;
        connections[index].username = NULL;
    }

    // Create the socket FD.
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("server: socket");
        exit(1);
    }

    // Set information about the port (and IP) we want to be connected to.
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;

    // This should always be zero. On some systems, it won't error if you
    // forget, but on others, you'll get mysterious errors. So zero it.
    memset(&server.sin_zero, 0, 8);

    // Bind the selected port to the socket.
    if (bind(sock_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("server: bind");
        close(sock_fd);
        exit(1);
    }

    // Announce willingness to accept connections on this socket.
    if (listen(sock_fd, MAX_BACKLOG) < 0) {
        perror("server: listen");
        close(sock_fd);
        exit(1);
    }

    // The client accept - message accept loop. First, we prepare to listen 
    // to multiple file descriptors by initializing a set of file descriptors.
    int max_fd = sock_fd;
    fd_set all_fds;
    FD_ZERO(&all_fds);
    FD_SET(sock_fd, &all_fds);

    while (1) {
        // select updates the fd_set it receives, so we always use a copy and retain the original.
        fd_set listen_fds = all_fds;
        int nready = select(max_fd + 1, &listen_fds, NULL, NULL, NULL);
        if (nready == -1) {
            perror("server: select");
            exit(1);
        }

        // Is it the original socket? Create a new connection ...
        if (FD_ISSET(sock_fd, &listen_fds)) {
            int client_fd = accept_connection(sock_fd, connections);
            if (client_fd > max_fd) {
                max_fd = client_fd;
            }
            FD_SET(client_fd, &all_fds);
            printf("Accepted connection from client %d\n", client_fd);
            const char *welcome = "Hi, welcome to chat.\r\n";
            write(client_fd, welcome, strlen(welcome));
        }

        // Next, check the clients.
        // NOTE: We could do some tricks with nready to terminate this loop early.
        for (int index = 0; index < MAX_CONNECTIONS; index++) {
            if (connections[index].sock_fd > -1 
                   && FD_ISSET(connections[index].sock_fd, &listen_fds)) {
                // Note: never reduces max_fd
                int client_closed = read_from(index, connections);
                if (client_closed > 0) {
                    FD_CLR(client_closed, &all_fds);
                    printf("Client %d disconnected\n", client_closed);
                } else {
                    printf("Echoing message from client %d\n", connections[index].sock_fd);
                }
            }
        }
    }

    // Should never get here.
    return 1;
}
