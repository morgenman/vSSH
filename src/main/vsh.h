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

#include <limits.h>

#include <filesystem>
#include <map>
#include <string>
#include <vector>

using namespace std;
namespace fs = std::filesystem;

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

vector<string> history;
// Trace offers an easy way to see diagnostic messages
bool trace = false;
// Storing the path as a vector allows it to have directories added and removed
vector<string> _path = {".", "/usr/local/bin", "/usr/bin", "/bin"};
map<string, string> alias = {{"egrep", "egrep --color=auto"},
                             {"l", "ls -CF"},
                             {"ls", "ls --color=auto"},
                             {"fgrep", "fgrep --color=auto"},
                             {"grep", "grep --color=auto"},
                             {"la", "ls -A"},
                             {"ll", "ls -alF"}};

string prompt;

char hostname[HOST_NAME_MAX];
string username;

int processCommand(string line);

/**
 * getPath takes in a command (like ls) and searches for it in the path
 * directories. It returns the first executable match for that file in the
 * order defined in {_path}
 *
 * @param  {string} in : takes a string object storing the command to be
 * run, sans arguments
 * @return {fs::path}  : fs::path is an type in std::filesystem
 * It is functionally basically the same as a string, but has better
 * compatibility with built in filesystem functions
 */
fs::path getPath(string in);

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
fs::path getPath(string in, int index);

/**
 * Returns boolean for if a file is executable or not
 *
 * @param  {fs::path} path : path (including file)
 * @return {bool}          : is file executable by current user?
 */
bool getExec(fs::path path);

/**
 * This function calls all of the other helper functions. The big cheese so to
 * speak
 *
 * @param  {string} in : input from user, untouched
 * @return {bool}      : returns if
 */
void executeFile(string in);

/**
 * Super simple splitter using streamstring
 *
 * @param  {string} in       : string to be split
 * @return {vector<string>}  : vector holding strings
 */
vector<string> parse(string in);

/**
 *
 * @return {string}  :
 */
string updatePath();

/**
 *
 */
void backspace();

/**
 *
 * @param  {int} i :
 */
void backspace(int i);

/**
 *
 * @return {string}  :
 */
string chomper();
