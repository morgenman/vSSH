/**
 * Very Simple SHell Part A
 *
 * This is part one of our very simple shell assignment. In this implementation,
 * we have to presenting a user input loop which runs until the user issues the
 * quit command. We need to parse the user input, check the command against
 * path, and if the command is found, return it, the path found at, and the
 * arguments in a string** to Ladd's function. This function will be implemented
 * in the following assignment
 *
 *
 * @author Dylan C. Morgen
 * @email morgendc203@potsdam.edu
 * @course CIS 310 Course Name
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

/** I am not sure if prototyping is required if functions are declared in the
same file as main. I think it is required for overloading however, so I am
doing it for all of them for consistency*/

fs::path getPath(string in);
fs::path getPath(string in, int index);
bool getExec(fs::path path);
void run(string in);
vector<string> parse(string in);

// Trace offers an easy way to see diagnostic messages
bool trace = false;
// Storing the path as a vector allows it to have directories added and removed
vector<string> _path = {".", "/usr/local/bin", "/usr/bin", "/bin"};

/**
 * getPath takes in a command (like ls) and searches for it in the path
 * directories. It returns the first executable match for that file in the order
 * defined in {_path}
 *
 * @param  {string} in : takes a string object storing the command to be run,
 * sans arguments
 * @return {fs::path}  : fs::path is an type in std::filesystem
 * It is functionally basically the same as a string, but has better
 * compatibility with built in filesystem functions
 */
fs::path getPath(string in) {
  fs::path p;
  // Loop through, prioritizing each of the paths incrementally until you find
  // an executable that matches (or run out of things to check)
  for (size_t i = 0; i < _path.size(); i++) {
    p = getPath(in, (int)i);
    // If it matched a path, see if executable
    if (p.string() != "$") {
      if (getExec(p)) return p;
      // If not executable, try again
      else
        p = "$";
    } else if (i == 0)
      break;
  }
  return p;
}

/**
 * Overloading so we can ignore non-executable entries found without calling
 * getExec() in this method (and having to pass it through somehow)
 *
 * @param  {string} in :
 * @param  {int} index : index of path out of * existing paths * .
 * That is, if ls is found in both /usr/local/bin and /bin:
 * getPath("ls", 0) -> /usr/local/bin/ls
 * getpath("ls", 1) -> /bin/ls
 *
 * @return {fs::path}  :
 */
fs::path getPath(string in, int index) {
  fs::path p = "$";
  fs::path temp = "$";
  int i = 0;
  // Iterable objects go brrr
  for (auto dir : _path) {
    if (trace) printf("checking %s/%s\n", dir.c_str(), in.c_str());
    // If this is the first string, and we are at the index, set p
    temp = dir.append("/").append(in);
    // exists() takes in a path, sees if the file exists
    if (exists(temp)) {
      if (p.string() == "$" && i == index) p = temp;
      if (trace) printf("%s exists\n", dir.c_str());
      if (trace) printf("p:[%s]\n", p.c_str());
      // I could probably return here, but .. well I don't want to break
      // anything
      i++;
    }
  }
  return p;
}

/**
 * Returns boolean for if a file is executable or not
 *
 * @param  {fs::path} path : path (including file)
 * @return {bool}          : is file executable by current user?
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
 * This function calls all of the other helper functions. The big cheese so to
 * speak
 *
 * @param  {string} in : input from user, untouched
 * @return {bool}      : returns if
 */

void run(string in) {
  // arguments is a vector of strings, each string holding a word
  vector<string> arguments = parse(in);
  // p stores the path of the command once found on the system
  fs::path p = getPath(arguments.front());
  if (p == "$") {
    // $ is the return when no executable matches are found
    printf("Error, no executables found for %s\n", in.c_str());
  } else {
    // This is a dummy char** in case no arguments are passed
    const char **args = new const char *[1];

    // I found this implementation here:
    // https://stackoverflow.com/questions/26032039/convert-vectorstring-into-char-c
    // Essentially it's iterating over into a vector holding <char*>
    // TODO: investigate if switching from vector<string> globally makes sense

    vector<const char *> cstrings;
    // Preallocating memory
    cstrings.reserve(arguments.size());
    for (size_t i = 1; i < arguments.size(); ++i)
      cstrings.push_back(const_cast<char *>(arguments[i].c_str()));

    cstrings.push_back(nullptr);

    // if not empty, pass through to ladd's function.
    // For cstrings, give the address of the first element, casted to const
    // char**
    // This is super weird but works because we reserved memory a few lines up
    // TODO: see if we need to deallocate that memory
    if (!cstrings.empty())
      vshPrint(p.c_str(), arguments.front().c_str(), cstrings.data());

    else
      vshPrint(p.c_str(), arguments.front().c_str(), args);
  }
}

/**
 * Super simple splitter using streamstring
 *
 * @param  {string} in       : string to be split
 * @return {vector<string>}  : vector holding strings
 */
vector<string> parse(string in) {
  vector<string> out;
  istringstream ss(in);
  string temp;
  // Auto splits by whitespace
  while (ss >> temp) {
    out.push_back(temp);
  }
  return out;
}

/**
 * Main, implements user input loop and quit() catching. Also contains trace
 * logic
 */
int main(int argc, char *argv[]) {
  // User input prompt
  // TODO replace this with path and user info
  string prompt = "\n> ";
  string line;
  printf("%s", prompt.c_str());

  while (getline(cin, line)) {
    // Just keep swimming
    if (line.empty()) continue;

    // in stores the string so we don't loose it on pipe
    stringstream entered(line, ios::in);
    string in = entered.str();
    string cmd;

    // pipe first word to command for quit/trace logic
    entered >> cmd;  // next word

    // quit/trace logic
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
    // Run the command
    run(in);
    // Print prompt again
    printf("%s", prompt.c_str());
  }
  return 0;
}
