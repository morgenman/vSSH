#include <unistd.h>

#include <cstring>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "vsh_printing_module.h"

using namespace std;
namespace fs = std::filesystem;

vector<string> _path = {".", "/usr/local/bin", "/usr/bin", "/bin"};

string getPath(string in) {
  string path = "";
  for (auto dir : _path) {
    fs::path p = dir.append("/").append(in);

    printf("checking %s\n", p.c_str());

    if (exists(p)) {
      printf("found %s in %s\n", in.c_str(), dir.c_str());

      // TODO: Explain this
      // https://stackoverflow.com/questions/5719694/how-to-check-if-file-is-executable-in-c
      if (!access(p.c_str(), X_OK)) {
        printf("file is marked as executable\n");
      } else {
        printf("file is marked as not executable\n");
      }

    } else {
    }
  }
  return p.string();
}

/**
 * main program
 */
int main(int argc, char *argv[]) {
  string cmd = "ls";
  string path = getPath("ls");

  string p1 = "lepton";
  string p2 = "meson";

  const char **p = new const char *[3];
  p[0] = p1.c_str();
  p[1] = p2.c_str();
  p[2] = nullptr;
  vshPrint(path.c_str(), cmd.c_str(), p);
  delete[] p;
}
