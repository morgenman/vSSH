#ifndef VSH_PRINTING_MODULE_H
#define VSH_PRINTING_MODULE_H
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#define RESET "\033[0m"
#define BOLDRED "\033[1m\033[31m" /* Bold Red */

/**
 * vshPrint - Pretty-print the parameters that would be used to exec a command
 * in a shell.
 *
 * @param cmd_FullPath - the full path name of the command that will be executed
 * @param cmd - original first word in the command line entered
 * @param parameters - a null-terminated array of character pointers
 * pointing at the rest of the words of the commandline
 */
void vshPrint(const char *cmd_FullPath, const char *cmd,
              const char **parameters);

void vshExec(int mode, std::string filename, const char *cmd_FullPath,
             char **parameters);

#endif /* VSH_PRINTING_MODULE_H */
