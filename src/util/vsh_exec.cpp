#include "vsh_exec.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <sstream>  // std::stringstream
#include <string>
#include <vector>

using namespace std;

void vshPrint(const char *cmd_FullPath, const char *cmd,
              const char **parameters) {
  cout << "exec(" << cmd_FullPath << ")" << endl;
  cout << cmd;
  for (const char **pp = parameters; *pp; pp++) {
    cout << " " << *pp;
  }
  // cout << endl;
}

// TODO: Change and explain
// https://www.geeksforgeeks.org/making-linux-shell-c/#
void vshExec(int mode, string filename, const char *cmd_FullPath,
             char **parameters) {
  // Forking a child
  pid_t pid = fork();
  int fd;
  try {
    // cout << "Mode: " << mode << endl;
    switch (mode) {
      case -1:
        break;
      case 1:  // <
        if (pid == 0) {
          fd = open(filename.c_str(), O_RDONLY, 0666);
          dup2(fd, STDIN_FILENO);  // Check `man stdin` for more info
          dup2(fd, STDERR_FILENO);
        }
        break;
      case 2:  // >
        if (pid == 0) {
          fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
          dup2(fd, STDOUT_FILENO);  // Check `man stdin` for more info
          dup2(fd, STDERR_FILENO);
        }
        break;
      case 3:  // >>
        if (pid == 0) {
          fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0666);
          dup2(fd, STDOUT_FILENO);  // Check `man stdin` for more info
          dup2(fd, STDERR_FILENO);
        }
        break;
    }
  } catch (...) {
    cout << BOLDRED << "ERROR: Issue with redirection file" << RESET << endl;
  }

  if (pid == -1) {
    printf("\nFailed forking child..");
    return;
  } else if (pid == 0) {
    if (execv(cmd_FullPath, parameters) < 0) {
      printf("\nCould not execute command..");
    }
    exit(0);
  } else {
    // waiting for child to terminate
    wait(NULL);
    if (mode > 0) close(fd);
    return;
  }
}
