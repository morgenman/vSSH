/**
 * See vsh.h for full documentation
 */

#include "vsh.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <cstring>
#include <filesystem>
#include <iostream>
#include <regex>

#include "vsh_exec.h"

#define RESET "\033[0m"
#define BLACK "\033[30m"              /* Black */
#define RED "\033[31m"                /* Red */
#define GREEN "\033[32m"              /* Green */
#define YELLOW "\033[33m"             /* Yellow */
#define BLUE "\033[34m"               /* Blue */
#define MAGENTA "\033[35m"            /* Magenta */
#define CYAN "\033[36m"               /* Cyan */
#define WHITE "\033[37m"              /* White */
#define BOLDBLACK "\033[1m\033[30m"   /* Bold Black */
#define BOLDRED "\033[1m\033[31m"     /* Bold Red */
#define BOLDGREEN "\033[1m\033[32m"   /* Bold Green */
#define BOLDYELLOW "\033[1m\033[33m"  /* Bold Yellow */
#define BOLDBLUE "\033[1m\033[34m"    /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m" /* Bold Magenta */
#define BOLDCYAN "\033[1m\033[36m"    /* Bold Cyan */
#define BOLDWHITE "\033[1m\033[37m"   /* Bold White */

/**
 * Main, implements user input loop and quit() catching. Also contains trace
 * logic
 */
int main(int argc, char *argv[]) {
  // User input prompt
  // TODO replace this with path and user info
  string prompt = updatePath();
  string line;

  printf("%s", prompt.c_str());

  while (true) {
    // Maaaagic
    line = chomper();
    // Just keep swimming
    if (line.empty()) {
      printf("%s", prompt.c_str());
      continue;
    }
    history.push_back(line);
    if (trace) {
      cout << endl << "History :" << endl;
      for (auto i : history) cout << i << endl;
      cout << endl;
    }

    // in stores the string so we don't loose it on pipe
    stringstream entered(line, ios::in);
    string in = entered.str();
    string cmd;

    // pipe first word to command for quit/trace logic
    entered >> cmd;  // next word

    // quit/trace logic
    if (cmd == "exit") {
      break;
    }
    if (cmd == "trace") {
      if (trace) {
        cout << "Trace OFF" << endl;
        printf("%s", prompt.c_str());
        trace = false;
        continue;
      } else {
        cout << "Trace ON" << endl;
        printf("%s", prompt.c_str());
        trace = true;
        continue;
      }
    }
    if (cmd == "cd") {
      try {
        string home = getenv("HOME");
        home.append("/");
        string p = home;
        if (entered >> cmd) {
          if (cmd.find("~") != std::string::npos) {
            cmd = std::regex_replace(cmd, std::regex("~"), home);
          }
          fs::current_path((fs::path)cmd);
        } else
          fs::current_path((fs::path)home);

        prompt = updatePath();
        printf("%s", prompt.c_str());
        continue;
      } catch (...) {
        printf("\nError: Invalid Directory\n%s", prompt.c_str());
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
    char **args = (char **)nullptr;

    // I found this implementation here:
    // https://stackoverflow.com/questions/26032039/convert-vectorstring-into-char-c
    // Essentially it's iterating over into a vector holding <char*>
    // TODO: investigate if switching from vector<string> globally makes sense

    vector<char *> cstrings;
    // Preallocating memory
    cstrings.reserve(arguments.size());
    for (size_t i = 0; i < arguments.size(); ++i)
      cstrings.push_back(const_cast<char *>(arguments[i].c_str()));

    cstrings.push_back(nullptr);

    // if not empty, pass through to ladd's function.
    // For cstrings, give the address of the first element, casted to const
    // char**
    // This is super weird but works because we reserved memory a few lines up
    // TODO: see if we need to deallocate that memory
    if (!cstrings.empty())
      vshExec(p.c_str(), arguments.front().c_str(), cstrings.data());

    else
      vshExec(p.c_str(), arguments.front().c_str(), args);
  }
}

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

string updatePath() {
  string prompt = BOLDGREEN;
  prompt.append("username@computername");
  prompt.append(RESET);
  prompt.append(":");
  prompt.append(BOLDBLUE);
  if (fs::current_path().string() == getenv("HOME"))
    prompt.append("~");
  else
    prompt.append(fs::current_path().string());
  prompt.append(RESET);
  prompt.append("$ ");
  return prompt;
}

void backspace(int i) {
  for (int j = 0; j < i; j++) putchar('\b');
  for (int j = 0; j < i; j++) putchar(' ');
  for (int j = 0; j < i; j++) putchar('\b');
}
void backspace() { backspace(1); }

string chomper() {
  string out = "";
  // tab completion:
  // https://stackoverflow.com/questions/1798511/how-to-avoid-pressing-enter-with-getchar-for-reading-a-single-character-only
  // working on it!
  /* use system call to make terminal send all keystrokes directly to stdin */
  system("/bin/stty -icrnl raw");
  int i = 0;  // Cursor position at start = 0;
  char c;
  int iter = (int)history.size();
  int max = iter;
  string buffer, temp;
  while ((c = getchar()) != '\r') {
    buffer = "";
    if (c == '\n') break;
    /* type a period to break out of the loop, since CTRL-D won't work raw */
    switch (c) {
      case 127:  // backspace

        if (i > 0) {
          if (!out.empty()) out.pop_back();
          backspace(3);
          i--;
        } else
          backspace(2);
        break;

      case '\t':
        cout << " insert autocomplete logic";
        break;
      case 27:
        getchar();
        c = getchar();
        if (c == 53 || c == 54) {  // pg up/down
          getchar();
          backspace();
        } else if (c == 65) {  // up
          if (iter > 0) {
            if (iter == max) {
              temp = out;
            }
            iter--;
            buffer = history.at(iter);
          }
        } else if (c == 66) {  // down
          if (iter < max - 1) {
            iter++;
            buffer = history.at(iter);
          } else if (iter == max - 1) {
            iter++;
            backspace(out.size());
            buffer = temp;
            out = "";
            temp = "";
            i = 0;
          }
        } else if (c == 68) {  // left
          backspace(4);
          if ((int)out.size() > i) {
            putchar(out.at(i));
            if ((int)out.size() > i + 1) {
              putchar(out.at(i + 1));
              if ((int)out.size() > i + 2) {
                putchar(out.at(i + 2));
                if ((int)out.size() > i + 3) {
                  putchar(out.at(i + 3));
                  putchar('\b');
                }
                putchar('\b');
              }
              putchar('\b');
            }
            putchar('\b');
          }
          if (i > 0) {
            putchar('\b');
            i--;
          }
          break;

        } else if (c == 67) {  // right
          backspace(4);
          if ((int)out.size() > i) {
            putchar(out.at(i));
            if ((int)out.size() > i + 1) {
              putchar(out.at(i + 1));
              if ((int)out.size() > i + 2) {
                putchar(out.at(i + 2));
                if ((int)out.size() > i + 3) {
                  putchar(out.at(i + 3));
                  putchar('\b');
                }
                putchar('\b');
              }
              putchar('\b');
            }
            // putchar('\b');
          }
          if (i < (int)out.size()) {
            i++;
          }
          break;
        }
        backspace(4);
        if (!buffer.empty()) {
          backspace(out.size());
          cout << buffer;
          out = buffer;
          i = buffer.size();
        }
        break;
      default:

        // cout << " " << (int)c << " ";
        out.insert(i, 1, c);
        i++;
        if (i < (int)out.size() - 1) {
          int tempi = i;
          while (i < (int)out.size()) {
            putchar(out.at(i));
            i++;
          }
          while (i > tempi) {
            putchar('\b');
            i--;
          }
        }
    }
  }
  backspace(2);
  while (i < (int)out.size()) {
    putchar(out.at(i));
    i++;
  }

  /* use system call to set terminal behaviour to more normal behaviour */
  system("/bin/stty cooked");
  putchar('\n');
  return out;
}
