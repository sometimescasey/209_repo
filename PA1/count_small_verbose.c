#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <regex.h>


// assumed usage: ls -l | ./count_small size [permissions]

int check_permissions(char perm[10], char req[10]) {
	int has_perms = 0;

	for (int i= 0; i < 9; i++) {
		if (req[i] != '-' && perm[i] != req[i]) {
				has_perms = 1; // false
		}
	}
	printf("perm: %s | req: %s | has_perms: %d \n", perm, req, has_perms);
	return has_perms;
}

int is_regular(char c) {
	return (c == '-');
}

int get_size(char *line) {
	const int size_pos = 4; // hardcode for column 4 since we were told to stick to class functions; otherwise would use regex
	const char *delim = " ";
	char *token;
	token = strtok(line, delim);
	for (int i = 0; i < size_pos; i++) {
		token = strtok(NULL, delim);
	}

	char *ptr;
	int size = strtol(token, &ptr, 10);
	return size;
}

int main(int argc, char **argv) {
	// check for correct # of parameters
	if (argc < 2 || argc > 3) {
        fprintf(stderr, "USAGE: count_small size [permissions]\n");
        return 1;
    }
    
    int perm_mode = 0;
    if (argc == 3) {
    	perm_mode = 1;
    	// printf("perm mode on\n");
    }

    
    char *ptr;
    int cutoff_size = strtol(argv[1], &ptr, 10);
    char req_perms[10];
    if (perm_mode) {
    	strncpy(req_perms, argv[2], 9);
    	req_perms[10] = '\0';
	}

    int files_count = 0;

	// parse 'ls-l' input and remove header line
	char char1;
	char str1[100]; // big enough for the whole line
	const char * header = "total ";
	while (scanf("%[^\n]%*c",str1)==1) {
		char *is_header;
		is_header = strstr(str1, header);
		char1 = str1[0];
		if (is_header == NULL && is_regular(char1)){
				// printf("size: %d\n", get_size(str1));
				if (get_size(str1) < cutoff_size) {
					if (perm_mode) {
						char perms[10];
						strncpy(perms, str1+sizeof(char), 9);
						perms[10] = '\0';

						if (check_permissions(perms, req_perms) == 0) {
							files_count++;
						}
					}
					else {
						files_count++;
					}	
				}
				
			}
	}

	printf("%d\n", files_count);
	
	return 0;
}