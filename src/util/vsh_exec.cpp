#include "vsh_exec.h"

#include <sys/wait.h>
#include <unistd.h>

#include <iostream>
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
void vshExec(const char *cmd_FullPath, const char *cmd, char **parameters) {
  // Forking a child
  pid_t pid = fork();

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
    return;
  }
}

int fork(int argc, char *argv[]) { return 0; }
