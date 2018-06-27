#include <stdio.h>
#include <stdlib.h>
#include  <string.h>
#include  <sys/types.h>
#include  <unistd.h>

#define EXT ".rle"

char* getNewFilename(char *filename) {
	int ext_len = strlen(EXT);
	int fin_len = strlen(filename);
	char *fout_name = malloc((fin_len + ext_len + 1) * sizeof(char)); 
	//add ".rle" plus null terminator
	strncpy(fout_name, filename, fin_len);
	strncpy(fout_name + fin_len, EXT, ext_len);
	fout_name[(fin_len + ext_len) * sizeof(char)] = '\0';

	return fout_name;
}

int main(int argc, char **argv) {
	// argument check
	if (argc != 2) {
		fprintf(stderr, "Usage: compress_file original-file");
	}

	// parse file
	char *filename = argv[1];
	printf("filename: %s\n", filename);

	FILE *file_in;
	FILE *file_out;

	file_in = fopen(filename, "rb");
	
	if (file_in == NULL) {
		perror("Failed to open file");
		exit(1);
	}
	else {
		// we're in business
		char *fout_name = getNewFilename(filename);
		file_out = fopen(fout_name, "wb"); // TODO: use proper filename

		// start reading file byte by byte
		char *buffer = malloc(sizeof(char));
		char current;
		char next;
		int seq_count;

		// TODO: empty file case
		fread(buffer, sizeof(char), 1, file_in);
		current = *buffer;
		seq_count = 1;

		while(fread(buffer, sizeof(char), 1, file_in)) {
			printf("seq_count: %d | buffer: %d\n", seq_count, current);
			printf("buffer: %d\n", *buffer);
			next = *buffer;
			if (next == current) {
				seq_count++;
				current = next;
				continue;
			}
			else {
				// write the value
				printf("Writing seq_count: %d\n", seq_count);
				fwrite(&seq_count, 1, 1, file_out);
				printf("Writing current byte: %d\n", current);
				fwrite(&current, 1, 1, file_out);
				current = next;
				seq_count = 1; // reset seq_count
			}
		}
		// TODO: take care of last item
	}

	fclose(file_in);
	fclose(file_out);

	//TODO: fix memory leaks from malloc

	return 0;
}