#ifndef _RPSLS_UTILS_H_
#define _RPSLS_UTILS_H_

#include <netinet/in.h>    /* Internet domain header, for struct sockaddr_in */

int max(int one, int two);
int bufferedRead(int current_fd, char *output);
int find_network_newline(const char *buf, int n);

#endif