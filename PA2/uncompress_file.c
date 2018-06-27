#include <stdio.h>
#include <stdlib.h>
#include  <string.h>
#include  <sys/types.h>
#include  <unistd.h>

#define EXT ".rle"

char* getOrigFilename(char *filename) {
	int ext_len = strlen(EXT);
	int fin_len = strlen(filename);
	int copyto = (fin_len - ext_len);
	char *fout_name = malloc((copyto + 1) * sizeof(char)); 
	//copy all but ".rle", add null terminator
	strncpy(fout_name, filename, copyto);
	fout_name[(copyto + 1) * sizeof(char)] = '\0';

	return fout_name;
}

int isRLE(char *str) {
	char *dot = strrchr(str, '.');
	if (dot && !strcmp(dot, EXT)) {
		return 1;
	}
	else return 0;
}

void writeN(FILE *fout, char *byte, int n) {
	// writes byte to fout, n times
	for (int i = 0; i < n; i++) {
		fwrite(byte, 1, 1, fout);
	}
}

int main(int argc, char **argv) {
	// argument check
	if (argc != 2) {
		fprintf(stderr, "Usage: uncompress_file compressed-file.rle");
	}

	// parse file
	char *filename = argv[1];

	FILE *file_in;
	FILE *file_out;

	
	// check for .rle extension
	if(isRLE(filename)){
		file_in = fopen(filename, "rb");
	} else {
		fprintf(stderr, "ERROR: File dos not have .rle extension\n");
		exit(1);	
	}
	
	if (file_in == NULL) {
		perror("Failed to open file");
		exit(1);
	}
	else {
		// we're in business
		char *fout_name = getOrigFilename(filename);
		file_out = fopen(fout_name, "wb");

		// start reading file byte by byte
		char *count_buffer = malloc(sizeof(char));
		char *byte_buffer = malloc(sizeof(char));

		int notEmpty;

		// check for and handle empty file case
		notEmpty = fread(count_buffer, sizeof(char), 1, file_in);
		if (!notEmpty) {
			fprintf(stderr, "ERROR: file is empty. Exiting.\n");
			exit(1);
		}
		fread(byte_buffer, sizeof(char), 1, file_in);
		writeN(file_out, byte_buffer, *count_buffer);
		
		// keep reading
		while(fread(count_buffer, sizeof(char), 1, file_in)) {
			fread(byte_buffer, sizeof(char), 1, file_in);
			writeN(file_out, byte_buffer, *count_buffer);
		}

		fclose(file_in);
		fclose(file_out);

	return 0;
	}
}
