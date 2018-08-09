#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>     /* inet_ntoa */
#include <netdb.h>         /* gethostname */
#include <sys/socket.h>

#include "rpsls_utils.h"

#ifndef BUFSIZE
    #define BUFSIZE 256
#endif

int max(int one, int two) {
    if (one > two) {
        return one;
    }
    return two;
}

int bufferedRead(int current_fd, char *output) {
        int fullLineFound = 0; 

        while (fullLineFound != 1) {
        // Receive messages
        char buf[BUFSIZE] = {'\0'};
        int inbuf = 0;           // How many bytes currently in buffer?
        int room = sizeof(buf);  // How many bytes remaining in buffer?
        char *after = buf;       // Pointer to position after the data in buf

        int nbytes;
        while ((nbytes = read(current_fd, after, room)) > 0) {
            // printf("buf: %s\n", buf);
            
            // Step 1: update inbuf (how many bytes were just added?)
            inbuf += nbytes;
            room -= nbytes;

            int where;

            // Step 2: the loop condition below calls find_network_newline
            // to determine if a full line has been read from the client.
            // Your next task should be to implement find_network_newline
            // (found at the bottom of this file).
            //
            // Note: we use a loop here because a single read might result in
            // more than one full line.
            while ((where = find_network_newline(buf, inbuf)) > 0) {
                fullLineFound = 1;
                // keep \r\n
                buf[where+1] = '\0';

                // printf("Got complete message: %s\n", buf);
                strncpy(output, buf, strlen(buf));
                // printf("returning from bufferedRead!\n");
                return 0;
                // Note that we could have also used write to avoid having to
                // put the '\0' in the buffer. Try using write later!

                // Step 4: update inbuf and remove the full line from the buffer
                // There might be stuff after the line, so don't just do inbuf = 0.

                memmove(buf, buf+where+1, inbuf);

                room += inbuf;
                after -= inbuf;
                
                // You want to move the stuff after the full line to the beginning
                // of the buffer.  A loop can do it, or you can use memmove.
                // memmove(destination, source, number_of_bytes)

            }
            // Step 5: update after and room, in preparation for the next read.
            after += nbytes;
            room -= nbytes;
        }
    }
    // if successful return 0
    return -1;
}

int find_network_newline(const char *buf, int n) {
    int i = 0;
    while ((buf[i]) != '\0') {
        if (buf[i] == '\r') {
            // printf("found r at position %d, ", i);
            if ((buf[i+1]) == '\n') {
                // printf("found n at position %d\n", i+1);
                return i+1;
            }
        } 
        i++;
    }
    return -1;
}