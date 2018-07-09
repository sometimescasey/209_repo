/*
Assumptions: 

1) The filesystem is a directed graph. Edges are one-way and run from 
[parent folder] -> [child folder], either physically or via symlink.

2) Only folders and symlinks can have children. A file cannot have a child.

3) A cycle exists when a symlink points to of its own ancestor folders.

4) Stated otherwise: say a search starts at folder X. If a user can use 
a series of 'cd' commands to reach folder X again, without ever using 'cd.' or 'cd..', 
then a cycle exists.

I.e. given folder structure A > B > C > D, a symlink from D to (C, B, or A) forms a cycle.

A symlink from A > D does not form a cycle.

5) Given structure A > B > C > D, if the search starts at folder B, 
a symlink from D > A does not constitute a cycle within the hierachy rooted at B.
*/

#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>

#define max_path_size 4096

// Global
const char *global_cwd;
int cycle_exists = 0;

int isAncestor(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
	// checks whether the current item points to an ancestor
	// to test we will just print the names for now
	if (typeflag != FTW_F && typeflag != 3) {
		// printf("typeflag: %d\n", typeflag);
		if (typeflag == FTW_SL) {
			char realpath[max_path_size];
			memset(&realpath[0], 0, sizeof(realpath)); 
			readlink(fpath, realpath, 256);
			printf("Found a symlink! Its real path is: %s\n", realpath);
			
			// does the symlink's real path contain roothpath?
			// account for special cases ".." and "."
			if (strcmp(realpath, "..") == 0 
				|| strcmp(realpath, ".") == 0 
				|| (strstr(realpath, global_cwd))){
				cycle_exists = 1;
			printf("Cycle found at %s\n", fpath);
			}
			printf("%s\n", fpath);
		}	
	}
	
	return 0;
}

int isAncestor_ftw(const char *fpath, const struct stat *sb, int typeflag) {
	// checks whether the current item points to an ancestor
	// to test we will just print the names for now
	printf("this!\n");
	printf("%s\n", fpath);
	return 0;
}

void wrapper(const char *rootdir) {
	int fd_limit = 5;
	int flags = FTW_PHYS;
	int flags2 = 0;

	global_cwd = rootdir;

	nftw(rootdir, isAncestor, fd_limit, flags);
	printf("---- CWD: %s ----\n", rootdir);
	nftw(rootdir, isAncestor, fd_limit, flags2);

}

int main(int argc, char **argv) {

	// argument checking
	if (argc > 2) {
		fprintf(stderr, "Usage: cycle [directory] . If no argument, defaults to cwd.\n");
		exit(-1);
	}
	
	char *rootdir;
	if (argc == 1) {
		// use cwd
		printf("No argument provided: using cwd\n");
		// TODO: implement
		char cwd[max_path_size];
		if (getcwd(cwd, sizeof(cwd)) != NULL) {
			rootdir = cwd;
		}
		else {
			fprintf(stderr, "Error: could not obtain cwd\n");
			exit(-1);
		}
	}
	else if (argc == 2) {
		// validate that directory exists
		rootdir = argv[1];
		DIR* dir = opendir(rootdir);
		if (dir == NULL) {
			fprintf(stderr, "Error: Directory '%s' does not exist.\n", rootdir);
			exit(-1);
		}
		else {
			closedir(dir);
		}

	}

	wrapper(rootdir);
	if (!cycle_exists) {
		printf("No cycle found.\n");
	}
	return 0;

}
