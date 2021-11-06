/**
 * Very Simple SHell
 * 
 * I have taken part A and completed a decently feature complete shell.
 * Base functionality has been completed, as well as: 
 * 
 * Terminal colors & prompt exactly the same as bashes with username and
 * computer name
 * 
 * Command history, available on up arrow.
 * 
 * Aliasing, with a few hardcoded aliases baked in. Note, I haven't added the
 * add alias function, but it would be trivial to do so
 * 
 * Tab completion is set up, but not implemented. 
 * 
 * Trace functionality, with lots of information.
 * 
 * Manual user input controls
 * 
 * ~ detection and expansion (this happens automatically in bash)
 * 
 * 
 * 
 *
 * @author Dylan C. Morgen
 * @email morgendc203@potsdam.edu
 * @course CIS 310 Course Name
 * @assignment vsh.cpp
 * @due 11/05/2021
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

/**
 * This function handles built in functions and calls executeFile
 * Alias functionality has been implemented here.
 *
 * @param  {string} line : 
 * @return {int}         :
 */
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
 * This function calls all of the other helper functions when running a command
 * from disk. The big cheese so to speak
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
 * updates the prompt shown to the user. Hopefully this won't error out on other
 * systems with different username and computer name variables in different
 * places
 *
 * @return {string}  : returns said prompt, with color codes baked in
 */
string updatePrompt();

/**
 * Overloaded backspace which defaults to 1 character
 *
 */
void backspace();

/**
 * This function deletes characters from the screen.
 *
 * @param  {int} i : how many characters to delete
 */
void backspace(int i);

/**
 * chomper.. is going to be very hard to read. Essentially it handles all sorts
 * of logic relating to standard text input. It captures all input, and uses
 * ANSI escape codes to manipulate the console and cursor. Unfortunately tab
 * completion was not implemented
 *
 * @return {string}  : on EOF, return final copy of user input (by line)
 */
string chomper();

/**
 * Restores user input to the console, provided in a string
 *
 * @param  {int} amount    : How many characters to restore
 * @param  {int} index     : What index in string to restore from
 * @param  {string} origin : String containing user input
 */
void restore(int amount, int index, string origin);
