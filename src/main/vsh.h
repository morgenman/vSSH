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

#include <filesystem>
#include <string>
#include <vector>

using namespace std;
namespace fs = std::filesystem;

vector<string> history;
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
void run(string in);

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

