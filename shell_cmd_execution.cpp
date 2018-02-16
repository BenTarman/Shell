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

char* Shell::convert(const std::string &s)
{
	char *pc = new char[s.size()+1];
	strcpy(pc, s.c_str());
	return pc; 
}

int Shell::spawnProc(int in, int out, struct command *cmd)
{
	pid_t pid, wpid;
  int status;

  char** args = &cmd->argv[0];

  pid = fork();
  if (pid == 0)
  {
    if (in != 0) 
		{
			dup2(in, 0);
			close(in);
    }
    if (out != 1)
    { 
			dup2(out, 1);
			close(out);
    }

		//variables for file permissions and stdout/stdin
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
		FILE* catStream;
		if (outFile)
    {
			if (stat(output, &res) < 0) exit(EXIT_FAILURE);
			mode_t bits = res.st_mode;

			//check if we have write permissions
			if ((bits & S_IWUSR) == 0)
				exit(EXIT_FAILURE);

			catStream = freopen(output, "w", stdout);

     }
		else if (inFile)
		{
			if (access(input, F_OK) == -1)
			{
				perror("file no exist");
				exit(EXIT_FAILURE);
			}

			if (stat(input, &res) < 0) exit(EXIT_FAILURE);
			mode_t bits = res.st_mode;

			//check if we have write permissions
			if ((bits & S_IRUSR) == 0)
				exit(EXIT_FAILURE);
			 catStream = freopen(input, "r", stdin);
		}
		else if (appFile) 
		{
			if (stat(output, &res) < 0) exit(EXIT_FAILURE);
			mode_t bits = res.st_mode;

			//check if we have write permissions
			if ((bits & S_IWUSR) == 0)
				exit(EXIT_FAILURE);
			 catStream = freopen(output, "a", stdout);
		}

		if (execvp(cmd->argv[0], (char* const*) cmd->argv) != 0) 
		{
			perror("lsh");
			exit(EXIT_FAILURE);
     }

     perror("exec");
     exit(127);
  }
  else if (pid < 0)
  {
		perror("lsh");
		exit(EXIT_FAILURE);
   }
  else 
  {
	do {
	  wpid = waitpid(pid, &status, WUNTRACED);
	} while (!WIFEXITED(status) && !WIFSIGNALED(status));
   }

	return status;

}

int Shell::lsh_launch(int n, struct command *cmd)
{
  int i;
  int status;
  pid_t wpid;
 
  pid_t pid;
  int in, fd[2];
  in = 0;

  //start 2 processes
  pid = fork();
  
  if (pid == 0)
  {
  //child
		for (i = 0; i < n - 1; i++)
		{
			//pipe each command in spawnProc function 
			pipe (fd); 
			if (spawnProc(in, fd[1], cmd + i) != 0)
			{
				status = spawnProc(in, fd[1], cmd + i); 
			}
			close (fd[1]);

			//input state of pipe is output state of last
			in = fd[0];
		}

		char** args = &cmd[i].argv[0];
  
    if (in != 0)
      dup2 (in, 0);
 
    char output[40];
    char input[40];
    int outFile = 0, inFile = 0, appFile = 0;
   
    for (int k = 0; args[k] != NULL; k++)
    {
			if (strcmp(args[k], ">") == 0)
			{
			args[k] = NULL;
			strcpy(output, args[k+1]);
			outFile = 1;
			}
			else if (strcmp(args[k], "<") == 0)
			{
				args[k] = NULL;
				strcpy(input, args[k+1]);
				inFile = 1;
			}
			else if (strcmp(args[k], ">>") == 0)
			{
				args[k] = NULL;
				strcpy(output, args[k+1]);
				appFile = 1;
			}
		}


   struct stat res;
   FILE* catStream;
		if (outFile)
		{
			if (stat(output, &res) < 0) return 1;
			mode_t bits = res.st_mode;

			//check if we have write permissions
			if ((bits & S_IWUSR) == 0) return 1; 	
			 catStream = freopen(output, "w", stdout);
		}
		else if (inFile)
		{
			if (access(input, F_OK) == -1)
			{
				perror("file no exist");
				exit(EXIT_FAILURE);
			}

			if (stat(input, &res) < 0) return 1;
			mode_t bits = res.st_mode;

			//check if we have write permissions
			if ((bits & S_IRUSR) == 0) return 1; 	

		 catStream = freopen(input, "r", stdin);
		}
		else if (appFile)
		{
			if (stat(output, &res) < 0) return 1;
			mode_t bits = res.st_mode;

			//check if we have write permissions
			if ((bits & S_IWUSR) == 0) return 1; 
	
			catStream = freopen(output, "a", stdout);
		}

    if (execvp(args[0], args) != 0) 
    {
			perror("lsh");
			exit(EXIT_FAILURE);
    }
    perror("exec");
    exit(127);
  }

  else if (pid < 0)
  {
    perror("lsh");
    exit(EXIT_FAILURE);
  }
  else
  {
		do {
			wpid = waitpid(pid, &status, WUNTRACED);
		} while(!WIFEXITED(status) && !WIFSIGNALED(status));
  }

	return WEXITSTATUS(status);
}

int Shell::makesSense(vector<string>& tokens)
{
	if (tokens[0] == ">" || tokens[0] == "<" || tokens[0] == ">>")
		return 1;

	std::vector<string> order;

	for (auto it = tokens.begin(); it != tokens.end(); it++)
	{
		if (*it == "|")
			order.push_back(*it);
		if (*it == ">")
		{
			if ((it + 1) == tokens.end())
				return 1;
			order.push_back(*it);
		}
		if (*it == "<")
		{
			if ((it + 1) == tokens.end())
				return 1;
			order.push_back(*it);
		}
		if (*it == ">>")
		{
			if ((it + 1) == tokens.end())
				return 1;
		}
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

