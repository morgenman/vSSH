#include <unistd.h>

#include <cstring>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "vsh_printing_module.h"

using namespace std;
namespace fs = std::filesystem;

fs::path getPath(string in);
fs::path getPath(string in, int index);
bool getExec(fs::path path);
bool run(string in);

vector<string> _path = {".", "/usr/local/bin", "/usr/bin", "/bin"};

fs::path getPath(string in) {
  fs::path p;
  for (size_t i = 0; i < _path.size(); i++) {
    p = getPath(in, (int)i);
    if (p.string() != "$")
      if (getExec(p)) return p;
  }
  return p;
}

/**
 *
 * @param  {string} in :
 * @param  {int} index : index of path out of * existing paths * .
 * That is, if ls is found in both /usr/local/bin and /bin:
 * getPath("ls", 0) -> /usr/local/bin/ls
 * getpath("ls", 1) -> /bin/ls
 * @return {fs::path}  :
 */
fs::path getPath(string in, int index) {
  fs::path p = "$";
  int i = 0;
  for (auto dir : _path) {
    if (p.string() == "$" && i == index) p = dir.append("/").append(in);

    printf("checking %s\n", p.c_str());

    if (exists(p)) {
      printf("found %s in %s\n", in.c_str(), dir.c_str());
      i++;
    } else
      p = "$";
  }
  return p;
}

bool getExec(fs::path path) {
  // TODO: Explain this
  // https://stackoverflow.com/questions/5719694/how-to-check-if-file-is-executable-in-c
  if (!access(path.c_str(), X_OK)) {
    printf("file is marked as executable\n");
    return true;
  } else {
    printf("file is marked as not executable\n");
    return false;
  }
}

bool run(string in) {
  fs::path path = getPath(in);
  if (path == "$") {
    printf("Error, no executables found for %s\n", in.c_str());
  } else if (getExec(path)) {
  } else {
    printf("Error, file is not marked as executable\n");
  }
  return false;
}

/**
 * main program
 */
int main(int argc, char *argv[]) {
  string cmd = "ls";
  fs::path path = getPath("ls");

  string p1 = "lepton";
  string p2 = "meson";

  const char **p = new const char *[3];
  p[0] = p1.c_str();
  p[1] = p2.c_str();
  p[2] = nullptr;
  vshPrint(path.c_str(), cmd.c_str(), p);
  delete[] p;
}
