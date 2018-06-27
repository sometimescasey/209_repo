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
		file_out = fopen(fout_name, "wb");
		free(fout_name);

		// start reading file byte by byte
		char *buffer = malloc(sizeof(char));
		char current;
		char next;
		int seq_count;

		int notEmpty;

		// handle empty file case
		notEmpty = fread(buffer, sizeof(char), 1, file_in);
		if (!notEmpty) {
			fprintf(stderr, "ERROR: file is empty. Exiting.\n");
			exit(1);
		}
		
		current = *buffer;
		seq_count = 1;

		while(fread(buffer, sizeof(char), 1, file_in)) {
			next = *buffer;
			if (next == current) {
				seq_count++;
				current = next;
				continue;
			}
			else {
				// write the value
				fwrite(&seq_count, 1, 1, file_out);
				fwrite(&current, 1, 1, file_out);
				current = next;
				seq_count = 1; // reset seq_count
			}
		}
		// take care of last item
		fwrite(&seq_count, 1, 1, file_out);
		fwrite(&current, 1, 1, file_out);
		
		free(buffer);
	}

	fclose(file_in);
	fclose(file_out);

	return 0;
}