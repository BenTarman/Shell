/**
 * This file contains the implementations of the builtin functions provided by
 * the shell (or will, once you've finished implementing them all).
 */

#include "shell.h"
#include <iostream>

#define BUFF_SIZE 1024
using namespace std;

int Shell::com_ls(vector<string>& argv) {
  FILE* file = popen("ls", "r");
  if (file == 0)
	{
		printf("error listing files");
		return 1;
	}

	char buffer[BUFF_SIZE];
	while (fgets(buffer, BUFF_SIZE, file))
		fprintf(stdout, "%s", buffer);

	pclose(file);

  return 0;
}


int Shell::com_cd(vector<string>& argv) {
 
  chdir(argv[1].c_str()); 
  
return 0;  

}


int Shell::com_pwd(vector<string>& argv) {
  char currDir[BUFF_SIZE];
  if (getcwd (currDir, BUFF_SIZE) != 0)
  	printf ("%s\n", currDir);
  else 
    {
      printf ("Error getting pwd: %s\n", currDir);
      return 1;
    }



  return 0;
}


int Shell::com_alias(vector<string>& argv) {
 
  std::string key, value; 
  size_t pos = 0;
  std::string temp;

  //loop till we find delimiter, and we get the key
  while ((pos = argv[1].find("=")) != std::string::npos)
  {
  key = argv[1].substr(0, pos);
  argv[1].erase(0, pos + 1);
  }
  //rest of the string is the value for the alias
  value = argv[1];

  //place in alias map
  aliases.emplace(key, value);
  return 0;
}


int Shell::com_unalias(vector<string>& argv) {
  // TODO: YOUR CODE GOES HERE
  cout << "unalias called" << endl; // delete when implemented
  return 0;
}


int Shell::com_echo(vector<string>& argv) {
  for (auto itr = argv.begin() + 1; itr != argv.end(); itr++)
		std::cout << *itr << " ";
	printf("\n\r");

	return 0;
}


int Shell::com_history(vector<string>& argv) {
  FILE *historyFile;
	char c;

	historyFile = fopen("/home/bentarman/cplusplus/project-1-BenTarman/history", "r");

	unsigned int counter = 0;
	while(1)
	{
		c = fgetc(historyFile);

		if(c==EOF) break;
		else
		{
		  printf("%c", c);
		if (c == '\n')	
		  printf("%i ", counter++);
		}
	}

	fclose(historyFile);



	return 0;
}


int Shell::com_exit(vector<string>& argv) {
  exit(0); 
  return 0;
}
