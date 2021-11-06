#include "vsh_exec.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <iostream>
#include <string>

using namespace std;

void vshExec(int mode, string filename, const char *cmd_FullPath,
             char **parameters) {
  // Forking a child
  pid_t pid = fork();
  int fd;
  try {
    switch (mode) {
      case -1:
        break;
      case 1:  // <
        if (pid == 0) {
          fd = open(filename.c_str(), O_RDONLY,
                    0666);         // 0666 is the permissions
          dup2(fd, STDIN_FILENO);  // replace stdin terminal fd with our file
                                   // descriptor
          dup2(fd, STDERR_FILENO);
        }
        break;
      case 2:  // >
        if (pid == 0) {
          fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
          dup2(fd, STDOUT_FILENO);
          dup2(fd, STDERR_FILENO);
        }
        break;
      case 3:  // >>
        if (pid == 0) {
          fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0666);
          dup2(fd, STDOUT_FILENO);
          dup2(fd, STDERR_FILENO);
        }
        break;
    }
  } catch (...) {
    cout << BOLDRED << "ERROR: Issue with redirection file" << RESET << endl;
  }

  if (pid == -1) {  // Failure
    printf("\nFailed forking child..");
    return;
  } else if (pid == 0) {  // Child
    if (execv(cmd_FullPath, parameters) < 0) {
      printf("\nCould not execute command..");
    }
    exit(0);
  } else {       // Parent
    wait(NULL);  // waiting for child to terminate, no ide how this works
    if (mode > 0) close(fd);  // gotta close that file
    return;
  }
}
