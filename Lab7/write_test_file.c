#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#define write_to 100

/* Write random integers (in binary) to a file with the name given by the command line
 * argument.  This program creates a data file for use by the time_reads program.
 */

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: write_test_file filename\n");
        exit(1);
    }

    FILE *fp;
    if ((fp = fopen(argv[1], "w")) == NULL) { // shouldn't this be "wb"
        perror("fopen");
        exit(1);
    }

    // TODO: complete this program according its description above.
    /* Intialize random number generator */
   srand(time(NULL));

    int r_i;
    for (int i = 0; i < write_to; i++) {
        r_i = rand() % write_to;
        fwrite(&r_i, 1, 1, fp); // writing binary so each write is 1 byte
    }

    fclose(fp);
    return 0;
}
