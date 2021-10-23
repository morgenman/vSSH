#include <string>
#include <cstring>

#include "vsh_printing_module.h"

using namespace std;
/**
 * main program
 */
int main(int argc, char *argv[]) {
  string cmd = "ls";
  string path = "/usr/bin/ls";

  string p1 = "lepton";
  string p2 = "meson";

  const char ** p = new const char *[3];
  p[0] = p1.c_str();
  p[1] = p2.c_str();
  p[2] = nullptr;
  vshPrint(path.c_str(), cmd.c_str(), p);
  delete[] p;
}
