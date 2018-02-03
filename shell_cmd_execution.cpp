/**
 * This file contains implementations of the functions that are responsible for
 * executing commands that are not builtins.
 *
 * Much of the code you write will probably be in this file. Add helper methods
 * as needed to keep your code clean. You'll lose points if you write a single
 * monolithic function!
 */

#include "shell.h"
#include <iostream>
#include <sys/wait.h>

using namespace std;


int Shell::execute_external_command(vector<string>& tokens) {
  // TODO: YOUR CODE GOES HERE

  std::vector<char *> argv(tokens.size() + 1);    // one extra for the null

for (std::size_t i = 0; i != tokens.size(); ++i)
{
   std::cout << tokens[i];
    argv[i] = &tokens[i][0];
}

execvp(argv[0], argv.data());
  return 1;
}



int lsh_launch(char **args)
{
  pid_t pid, wpid;
  int status;

  pid = fork();
  if (pid == 0) {
    // Child process
    if (execvp(args[0], args) == -1) {
      perror("lsh");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("lsh");
  } else {
    // Parent process
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

