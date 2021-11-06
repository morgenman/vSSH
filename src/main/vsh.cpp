/**
 * See vsh.h for full documentation
 */

#include "vsh.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <cstring>
#include <filesystem>
#include <iostream>
#include <regex>

#include "vsh_exec.h"

/**
 * Main, implements user input loop and quit() catching. Also contains trace
 * logic
 */
int main(int argc, char *argv[]) {
  // User input prompt
  gethostname(hostname, HOST_NAME_MAX);

  // getlogin_r(username, LOGIN_NAME_MAX);
  username = "";
  username.append(getenv("HOME"));

  username = username.erase(0, username.find_last_of("/") + 1);

  prompt = updatePath();
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

    if (processCommand(line) == -1) return 0;

    // Print prompt again
    printf("%s", prompt.c_str());
  }
  return 0;
}

int processCommand(string line) {
  string home = getenv("HOME");
  home.append("/");
  while (line.find("~") != std::string::npos) {
    line = std::regex_replace(line, std::regex("~"), home);
  }
  // in stores the string so we don't loose it on pipe
  stringstream entered(line, ios::in);
  string in = entered.str();
  string cmd;

  // pipe first word to command for quit/trace logic
  entered >> cmd;  // next word

  // quit/trace logic
  if (cmd == "exit") {
    return -1;
  } else if (cmd == "trace") {
    if (trace)
      cout << "Trace OFF" << endl;
    else
      cout << "Trace ON" << endl;
    trace = !trace;
  } else if (cmd == "cd") {
    try {
      string p = home;
      if (entered >> cmd) {
        fs::current_path((fs::path)cmd);
      } else
        fs::current_path((fs::path)home);

      prompt = updatePath();
      // printf("%s", prompt.c_str());
      // continue;
    } catch (...) {
      printf("-cash: %s: No such file or directory\n", in.c_str());
      // continue;
    }
  } else if (cmd == "pwd") {  // NOTE: might not be necessary, /usr/bin/pwd
                              // exists on my device
    cout << fs::current_path().string().c_str() << endl;
  } else if (alias.find(cmd) != alias.end()) {
    in = regex_replace(in, regex(cmd), alias[cmd]);
    executeFile(in);  // Run the command
  } else
    executeFile(in);  // Run the command
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

void executeFile(string in) {
  // arguments is a vector of strings, each string holding a word
  vector<string> arguments = parse(in);
  bool pos = false;
  int redir = 0;  // 1:< 2:> 3:>>
  for (int i = 0; i < (int)arguments.size(); i++) {
    pos = (i == (int)arguments.size() - 2);
    if (arguments[i] == "<") {
      if (pos)
        redir = 1;
      else
        redir = -1;
    } else if (arguments[i] == ">") {
      if (pos)
        redir = 2;
      else
        redir = -1;
    } else if (arguments[i] == ">>") {
      if (pos)
        redir = 3;
      else
        redir = -1;
    }
  }
  // p stores the path of the command once found on the system
  fs::path p = getPath(arguments.front());
  if (p == "$") {
    // $ is the return when no executable matches are found
    printf("Error, no executables found for %s\n", in.c_str());
  } else {
    string fn = "";
    switch (redir) {
      case -1:
        cout << BOLDRED
             << "WARNING: invalid redirect usage detected. May result in "
                "unexpected behavior."
             << RESET << endl;
        break;
      case 1:
      case 2:
      case 3:
        fn = arguments[(int)arguments.size() - 1];
        arguments.resize(arguments.size() - 2);
        break;
    }

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
      vshExec(redir, fn, p.c_str(), cstrings.data());

    else
      vshExec(redir, fn, p.c_str(), args);
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

  prompt.append(username);
  prompt.append("@");
  prompt.append((char *)&hostname);
  prompt.append(RESET);
  prompt.append(":");
  prompt.append(BOLDBLUE);
  string temp = fs::current_path().string();
  string temp2 = getenv("HOME");
  if (temp.find(temp2) == 0) {
    temp.erase(0, temp2.size());
    prompt.append("~");
    prompt.append(temp);
  }
  // if (fs::current_path().string() == getenv("HOME"))
  // prompt.append("~");
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

void restore(int amount, int index, string origin) {
  for (int i = 0; i < amount; i++) {
    if ((int)origin.size() > index + i) putchar(origin.at(index + i));
  }
  for (int i = 0; i < amount; i++) {
    if ((int)origin.size() > index + i) putchar('\b');
  }
}

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
  string keycode;
  while ((c = getchar()) != '\r') {
    keycode.append(" ");
    keycode.append(std::to_string((int)c));
    keycode.append(" ");

    buffer = "";
    if (c == '\n') break;
    /* type a period to break out of the loop, since CTRL-D won't work raw */
    switch (c) {
      case 127:  // backspace
        if (i > 0) {
          if (!out.empty()) out.erase(i - 1, 1);
          backspace(3);
          i--;
          restore(out.size() - i + 2, i, out + " ");
        } else {
          backspace(2);
          restore(2, i, out);
        }
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

        } else if (c == 51) {  // delete
          getchar();
          backspace();
          if (!out.empty()) out.erase(i, 1);
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
          restore(4, i, out);

          if (i > 0) {
            putchar('\b');
            i--;
          }
          break;
        } else if (c == 67) {  // right
          backspace(4);
          if ((int)out.size() > i) {
            putchar(out.at(i));
            restore(3, i + 1, out);
            // putchar('\b');
          }
          if (i < (int)out.size()) {
            i++;
          }
          break;
        }

        backspace(4);
        restore(6, i, out + " ");
        if (!buffer.empty()) {
          backspace(out.size());
          cout << buffer;
          out = buffer;
          i = buffer.size();
        }
        break;
      default:

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
  if (trace) cout << "keys pressed: " << keycode << endl;
  return out;
}
