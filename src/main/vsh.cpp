/**
 * One line summary
 *
 * Mulitline description describing intent of file
 *
 * @author Dylan C. Morgen
 * @email morgendc203@potsdam.edu
 * @course CIS XXX Course Name
 * @assignment vsh.cpp
 * @due 10/25/2021
 */

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
vector<string> parse(string in);

bool trace = true;

vector<string> _path = {".", "/usr/local/bin", "/usr/bin", "/bin"};

/**
 *
 * @param  {string} in :
 * @return {fs::path}  :
 */
fs::path getPath(string in) {
  fs::path p;
  for (size_t i = 0; i < _path.size(); i++) {
    p = getPath(in, (int)i);
    if (p.string() != "$") {
      if (getExec(p))
        return p;
      else
        p = "$";
    }
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
    if (trace) printf("checking %s/%s\n", dir.c_str(), in.c_str());
    if (p.string() == "$" && i == index) p = dir.append("/").append(in);
    if (exists(p)) {
      if (trace) printf("%s exists\n", dir.c_str());
      i++;
    } else
      p = "$";
  }
  return p;
}

/**
 *
 * @param  {fs::path} path :
 * @return {bool}          :
 */
bool getExec(fs::path path) {
  // TODO: Explain this
  // https://stackoverflow.com/questions/5719694/how-to-check-if-file-is-executable-in-c
  if (!access(path.c_str(), X_OK)) {
    if (trace) printf("file is marked as executable\n");
    return true;
  } else {
    if (trace) printf("file is marked as not executable\n");
    return false;
  }
}
/**
 *
 * @param  {string} in :
 * @return {bool}      :
 */
bool run(string in) {
  vector<string> arguments = parse(in);
  fs::path p = getPath(arguments.front());
  if (p == "$") {
    printf("Error, no executables found for %s\n", in.c_str());
  } else {
    const char **args = new const char *[1];

    // TODO: Explain
    // https://stackoverflow.com/questions/26032039/convert-vectorstring-into-char-c

    vector<char *> cstrings;
    cstrings.reserve(arguments.size());
    for (size_t i = 1; i < arguments.size(); ++i)
      cstrings.push_back(const_cast<char *>(arguments[i].c_str()));

    if (!cstrings.empty())
      vshPrint(p.c_str(), arguments.front().c_str(),
               (const char **)&cstrings[0]);

    else
      vshPrint(p.c_str(), arguments.front().c_str(), args);
  }
  return false;
}

/**
 * TODO: explain:
 * https://www.geeksforgeeks.org/split-a-sentence-into-words-in-cpp/
 *
 * @param  {string} in       :
 * @return {vector<string>}  :
 */
vector<string> parse(string in) {
  vector<string> out;
  istringstream ss(in);
  string temp;
  while (ss >> temp) {
    out.push_back(temp);
  }
  return out;
}
using namespace std;

/**
 * main program
 */
int main(int argc, char *argv[]) {
  string prompt = "\n> ";
  string line;
  printf("%s", prompt.c_str());
  while (getline(cin, line)) {
    if (line.empty()) continue;

    stringstream entered(line, ios::in);
    string in = entered.str();
    string cmd;

    entered >> cmd;  // next word

    if (cmd == "quit()") {
      break;
    }
    if (cmd == "trace") {
      if (trace) {
        cout << "Trace OFF" << endl << "> ";
        trace = false;
        continue;
      } else {
        cout << "Trace ON" << endl << "> ";
        trace = true;
        continue;
      }
    }
    run(in);
    printf("%s", prompt.c_str());
  }
  return 0;
}
