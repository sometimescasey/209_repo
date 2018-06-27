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

int main(int argc, char **argv) {
	// argument check
	if (argc != 2) {
		fprintf(stderr, "Usage: uncompress_file compressed-file");
	}

	// parse file
	char *filename = argv[1];
	printf("filename: %s\n", filename);

	FILE *file_in;
	FILE *file_out;

	file_in = fopen(filename, "rb");
	
	if(!isRLE(filename)){
		// check for .rle extension
		fprintf(stderr, "ERROR: File dos not have  .rle extension\n");
		exit(1);
	}
	else if (file_in == NULL) {
		perror("Failed to open file");
		exit(1);
	}
	else {
		// we're in business
		char *fout_name = getOrigFilename(filename);
		printf("Output filename: %s\n", fout_name);
		file_out = fopen(fout_name, "wb"); // TODO: use proper filename

		// fclose(file_in);
		fclose(file_out);

	return 0;
	}
}
