/*
Notes:

do not use nftw
use readdir() and opendir()
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAXPATH 4096
#define MAXNAME 31

char* getFullPath(const char *path, const char *cwd) {
	// given a current entry and the cwd, join the two to return a full path
	int cwd_l = strlen(cwd);
	int path_l = strlen(path);
	
	char *fullpath = malloc(sizeof(char) * (cwd_l + path_l + 2));
	strcpy(fullpath, cwd);
	strcpy(fullpath+cwd_l, "/");
	strcpy(fullpath+cwd_l+1, path);
	strcpy(fullpath+cwd_l+1+path_l, "\0");

	return fullpath;
}

int isDir(const char *path, const char *cwd) {
	// special cases
	if (strcmp(path, "..") == 0 || strcmp(path, ".") == 0) return 0;

	char *fullpath = getFullPath(path, cwd);

	struct stat statbuffer;
	if (stat(fullpath, &statbuffer) != 0) {
		// stat() failed; definitely not a dir
		free(fullpath);
		return 0;
	}
	else {
		// printf("S_ISDIR(statbuffer.st_mode): %d\n", S_ISDIR(statbuffer.st_mode));
		free(fullpath);
		return S_ISDIR(statbuffer.st_mode);
	}
}

void searchdir(char *dirname, const char *findme) {
	printf("cwd is: %s\n", dirname);  
	DIR *dirp;
	struct dirent *dp;

	dirp = opendir(dirname);
	dp = readdir(dirp);

	while (dp != NULL) {
		char *current = dp->d_name;

		// printf("%s\n", current);
		// printf(" | (isDir?) : %d \n", isDir(current));
		if (isDir(current, dirname)) {
			char *newpath = getFullPath(current, dirname);
			searchdir(newpath, findme);
			free(newpath);
		}
		if (strcmp(current, findme) == 0) {
			// found match!
			char* fullResult = getFullPath(current, dirname);
			printf("Match found for %s! | %s\n", findme, fullResult);
			free(fullResult);
		}
		dp = readdir(dirp);
	}
	closedir(dirp);
}

int main(int argc, char **argv) {
	// argument checking
	if (argc != 3) {
		fprintf(stderr, "Usage: ./myfind root_dir filename\n");
		exit(-1);
	}

	// validate that directory exists
	char *rootdir;
	rootdir = argv[1];
	DIR* dir = opendir(rootdir);
	if (dir == NULL) {
		fprintf(stderr, "Error: Directory '%s' does not exist.\n", rootdir);
		exit(-1);
	}
	else {
		closedir(dir);
	}

	// get filename
	char *filename = malloc(MAXNAME * sizeof(char));
	int filename_l = strlen(argv[2]);
	strncpy(filename, argv[2], filename_l);
	filename[filename_l] = '\0';

	searchdir(rootdir, filename);
	free(filename);
	return 0;
	
}