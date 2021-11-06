#ifndef VSH_PRINTING_MODULE_H
#define VSH_PRINTING_MODULE_H
#include <string>
#define RESET "\033[0m"
#define BOLDRED "\033[1m\033[31m" /* Bold Red */

/**
 * vshExec - Fork and run, with redirection
 * https://www.geeksforgeeks.org/making-linux-shell-c/#
 *
 * @param mode - redirection mode -- 0: normal 1:< 2:> 3:>>
 * @param cmd_FullPath - the full path name of the command that will be executed
 * @param parameters - a null-terminated array of character pointers
 * pointing at the rest of the words of the commandline
 */
void vshExec(int mode, std::string filename, const char *cmd_FullPath,
             char **parameters);

#endif /* VSH_PRINTING_MODULE_H */
