#include "vsh_printing_module.h"

#include <iostream>

using namespace std;

void vshPrint(const char *cmd_FullPath, const char *cmd,
              const char **parameters) {
  cout << "exec(" << cmd_FullPath << ")" << endl;
  cout << cmd;
  for (const char **pp = parameters; *pp; pp++) {
    cout << " " << *pp;
  }
  cout << endl;
}
