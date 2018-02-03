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
#include <string.h>
#include <algorithm>
#include <numeric>

using namespace std;

int lsh_launch(char**, const char*);
char* convert(const std::string&);



int Shell::execute_external_command(vector<string>& tokens) {
  // TODO: YOUR CODE GOES HERE
  //convert to char** so coding with the system is easier...
  std::vector<char*>  args; //this can be treated as char** if accessed as &args[0]
  std::transform(tokens.begin(), tokens.end(), std::back_inserter(args), convert);   
  std::string s;
  s = accumulate(begin(tokens), end(tokens), s);
  int ret = lsh_launch(&args[0], s.c_str());


  //clean memeory
  for (size_t i = 0; i < args.size(); i++)
    delete [] args[i];

  return ret;
}

char *convert(const std::string &s)
{
   char *pc = new char[s.size()+1];
   strcpy(pc, s.c_str());
   return pc; 
}

int lsh_launch(char **args, const char* tok)
{

  pid_t pid, wpid;
  int status;

  pid = fork();
  if (pid == 0) {
    // Child process
    if (execvp(args[0], args) != 0) {
      perror("lsh");
      return 1;
    }
    perror("exec");
    exit(127);
  } else if (pid < 0) {
    // Error forking
    perror("lsh");
    return 1;
  } else {
    // Parent process
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

 return 0;

}

