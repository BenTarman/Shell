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
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;


int lsh_launch(int n, struct command *cmd);
char* convert(const std::string&);
int spawnProc(int in, int out, struct command *cmd);

struct command
{
  char **argv;
};

int makesSense(vector<string>& tokens)
{

   std::vector<string> order;

   for (auto it = tokens.begin(); it != tokens.end(); it++)
   {
    	if (*it == "|")
	  order.push_back(*it);
    	if (*it == ">")
	  order.push_back(*it);
    	if (*it == "<")
	  order.push_back(*it);
   }


   std::string temp = "";
   for (auto x : order)
   {
	temp += x;
	if (temp == "|<")
	   return 1;
	if (temp == ">|")
	   return 1;
	if (temp == ">>")
	   return 1;
	if (temp == "<<")
	   return 1;

   }
	


  return 0;
  




}


int Shell::execute_external_command(vector<string>& tokens) {
  
  if (makesSense(tokens) != 0)
	return 1;


  //convert to char** so coding with the system is easier...
  std::vector<char*>  args; //this can be treated as char** if accessed as &args[0]
  std::transform(tokens.begin(), tokens.end(), std::back_inserter(args), convert);   
  struct command cmd[40];
  char* temp[40];

  int i = 0, j = 0, numPipes = 1;

  //do some sketchy shit now to put command in datastructure
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
  
  temp[i] = NULL;
  cmd[j].argv = (char**) malloc(sizeof(temp));
  memcpy(cmd[j].argv, temp, sizeof(temp));



  //error if any pipe commands are just empty
  for (int i = 0; i < numPipes; i++)
  	if (cmd[i].argv[0] == NULL) 
    		return 1;
  


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

   struct stat res;

   if (outFile)
    {
    if (stat(output, &res) < 0) return 1;
    mode_t bits = res.st_mode;

    //check if we have write permissions
    if ((bits & S_IWUSR) == 0)
	return 1; 	

	FILE* catStream = freopen(output, "w", stdout);

     }
   else if (inFile)
	FILE* catStream = freopen(input, "r", stdin);
   else if (appFile)
	FILE* catStream = freopen(output, "a", stdout);

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

 

  
  int status;
  pid_t wpid;
 
  pid_t pid;
  int in, fd[2];
  in = 0;


  pid = fork();
  
  if (pid == 0)
{
  for (i = 0; i < n - 1; i++)
  {
    pipe (fd); 
    if (spawnProc(in, fd[1], cmd + i) == 1)
	return 1;
    close (fd[1]);
    in = fd[0];
  }





  char** args = &cmd[i].argv[0];
  
  if (in != 0)
    dup2 (in, 0);
 
    char output[40];
    char input[40];
    int outFile = 0, inFile = 0, appFile = 0;
   
    int numOuts = 0; 
    for (int i = 0; args[i] != NULL; i++)
    {
	if (strcmp(args[i], ">") == 0)
	{
	  numOuts++;
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

   if (numOuts > 1) 
   { 
	exit(127);
   }

   struct stat res;
   if (outFile)
   {

    //if (stat(output, &res) < 0) return 1;
    //mode_t bits = res.st_mode;

    //check if we have write permissions
    //if ((bits & S_IWUSR) == 0)
//	return 1; 	
    FILE* catStream = freopen(output, "w", stdout);
    }
   else if (inFile)
    {

	if (access(input, F_OK) == -1)
	  {
		perror("file no exist");
		exit(EXIT_FAILURE);

	  }

    FILE* catStream = freopen(input, "r", stdin);

   }
   else if (appFile)
	FILE* catStream = freopen(output, "a", stdout);

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
