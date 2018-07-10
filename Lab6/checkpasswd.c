#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXLINE 256
#define MAX_PASSWORD 10

#define SUCCESS "Password verified\n"
#define INVALID "Invalid password\n"
#define NO_USER "No such user\n"

int main(void) {
  char user_id[MAXLINE];
  char password[MAXLINE];

  if(fgets(user_id, MAXLINE, stdin) == NULL) {
      perror("fgets");
      exit(1);
  }
  if(fgets(password, MAXLINE, stdin) == NULL) {
      perror("fgets");
      exit(1);
  }
  
  int fd[2]; // file descriptors for pipe
  int r; // fork

  // make pipe
  if (pipe(fd) == -1) {
    perror("pipe");
    exit(1);
  } 

  // open pipe and put descriptors at fd[0] and fd[1]
  r = fork();
  if (r > 0) {
    // is parent
    // reset STDOUT slot to write to pipe, and then write variables
      // if (dup2(fd[1], fileno(stdout)) == -1) {
      //   perror("dup2");
      //   exit(1);
      // }

    // never reads from pipe; close fd[0]
    // if (close(fd[0]) == -1) {
    //  perror("close");
    // }

    // now close fd[1] after redirecting
    // if (close(fd[1]) == -1) {
    //  perror("close");
    // }

    // write username:pwd to stdout
    // printf("%s:%s",user_id,password);

    // or write directly to pipe
    write(fd[1], user_id, strlen(user_id) + 1);
    write(fd[1], user_id, strlen(password) + 1);
    close(fd[1]);
  } 
  else if (r == 0) {
    // is child, child runs validation
    // reset stdin to read from pipe
    if (dup2(fd[0], fileno(stdin)) == -1) {
        perror("dup2");
        exit(1);
    }

    // never writes to pipe; close fd[1]
    if (close(fd[1]) == -1) {
      perror("close");
    }
    // now close fd[0] after redirecting
    if (close(fd[0]) == -1) {
      perror("close");
    }

    // validate the pw
    int return_val = execl("validate", "validate", NULL);
    // nothing after this should run
    printf("return: %d\n", return_val);
}
  int status;
  wait(&status);
  int result = WEXITSTATUS(status);
  if (3 == result) {
    printf("No such user\n");
  } else if (2 == result) {
    printf("Invalid password\n");
  } else if (1 == result) {
    printf("Password verified\n");
  }
  // printf("WEXITSTATUS(status):%d\n",WEXITSTATUS(status));
  return 0;
}
