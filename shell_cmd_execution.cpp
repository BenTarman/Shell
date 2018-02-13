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
#include <fcntl.h>

using namespace std;


int lsh_launch(int n, struct command *cmd);
char* convert(const std::string&);
int spawnProc(int in, int out, struct command *cmd);

struct command
{
  char **argv;
};


int Shell::execute_external_command(vector<string>& tokens) {
  // TODO: YOUR CODE GOES HERE
  //convert to char** so coding with the system is easier...
  std::vector<char*>  args; //this can be treated as char** if accessed as &args[0]
  std::transform(tokens.begin(), tokens.end(), std::back_inserter(args), convert);   
  struct command cmd[40];
  char* temp[40];

  int i = 0, j = 0, numPipes = 1;

  //do some sketchy shit now
  for (auto x : args)
  {
   if (strcmp(x, "|") == 0)
   {
	temp[i] = NULL;
	cmd[j].argv = (char**) malloc(sizeof(temp));
	memcpy(cmd[j].argv, temp, sizeof(temp));
	j++;
	i = 0;
	numPipes++;
   }
   else
	temp[i++] = x;
  }
  
  cmd[j].argv = (char**) malloc(sizeof(temp));
  memcpy(cmd[j].argv, temp, sizeof(temp));

  int ret = lsh_launch(numPipes, cmd);

  return ret;
}

char *convert(const std::string &s)
{
   char *pc = new char[s.size()+1];
   strcpy(pc, s.c_str());
   return pc; 
}

int spawnProc(int in, int out, struct command *cmd)
{
  pid_t pid, wpid;
  int status;

char** args = &cmd->argv[0];
  pid = fork();
  if (pid == 0)
  {
    if (in != 0) {
      dup2(in, 0);
	close(in);
     }
    if (out != 1)
    { 

    dup2(out, 1);
    close(out);
    }
   /* 
    char output[40];
    char input[40];
    int outFile = 0, inFile = 0, appFile = 0;
    
    for (int i = 0; args[i] != NULL; i++)
    {
	if (strcmp(args[i], ">") == 0)
	{
	  args[i] = NULL;
	  strcpy(output, args[i+1]);
	  outFile = 1;
	}
	else if (strcmp(args[i], "<") == 0)
	{
	  args[i] = NULL;
	  strcpy(input, args[i+1]);
	  inFile = 1;
	}
	else if (strcmp(args[i], ">>") == 0)
	{
	  args[i] = NULL;
	  strcpy(output, args[i+1]);
	  appFile = 1;
	}
    }

   if (outFile)
	FILE* catStream = freopen(output, "w", stdout);
   else if (inFile)
	FILE* catStream = freopen(input, "r", stdin);
   else if (appFile)
	FILE* catStream = freopen(output, "a", stdout);

*/
    if (execvp(cmd->argv[0], (char* const*) cmd->argv) != 0) {
       perror("lsh");
	return 1;
     }

     perror("exec");
     exit(127);
  }
  else if (pid < 0)
  {
     perror("lsh");
     return 1;
   }
  else 
  {
	do {
	  wpid = waitpid(pid, &status, WUNTRACED);
	} while (!WIFEXITED(status) && !WIFSIGNALED(status));
   }

  return status;

}

int lsh_launch(int n, struct command *cmd)
{
  int i;

  pid_t pid;
  int in, fd[2];
  in = 0;
  for (i = 0; i < n - 1; i++)
  {
    pipe (fd); 
    spawnProc(in, fd[1], cmd + i);
    close (fd[1]);
    in = fd[0];
  }

  if (in != 0)
    dup2 (in, 0);
  
  int status;
  pid_t wpid;

  //this wasn't null terminated before
  char** args = &cmd[i].argv[0];
  args[n] = NULL;

  pid = fork();
  
  if (pid == 0)
  {
    if (execvp(args[0], args) != 0) 
    {
	perror("lsh");
	return 1;
    }
    perror("exec");
    exit(127);
  }
  else if (pid < 0)
  {
    perror("lsh");
    return 1;
  }
  else
  {
	do {
	wpid = waitpid(pid, &status, WUNTRACED);
	} while(!WIFEXITED(status) && !WIFSIGNALED(status));
  }	
  return status;

  
}
