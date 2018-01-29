/**
 * This file contains the implementations of the builtin functions provided by
 * the shell (or will, once you've finished implementing them all).
 */

#include "shell.h"
#include <iostream>
#include <dirent.h>
#include <readline/history.h>

#define BUFF_SIZE 1024
using namespace std;

int Shell::com_ls(vector<string>& argv) {
    
  if (argv.size() > 1)
  {
     argv[0] = "cd";
     int ret = com_cd(argv);
     
     if (ret != 0) 
     {
        fprintf(stderr, "%s: command not found\n", argv[1].c_str());
        return 1;
     }
  }


  FILE* file = popen("ls", "r");
  if (file == 0)
	{
		printf("error listing files");
		return 1;
	}

	char buffer[BUFF_SIZE];
	while (fgets(buffer, BUFF_SIZE, file))
		fprintf(stdout, "%s", buffer);

  if (argv.size() > 1)
	chdir("..");
  pclose(file);

  return 0;

}


int Shell::com_cd(vector<string>& argv) {

  int ret;
  if (argv.size() == 1)
	ret = chdir(getenv("HOME"));
  else 
  {
    if (argv.size() > 2)
    {
      fprintf(stderr, "too many arguments\n");
      return 1;
    } 
     ret = chdir(argv[1].c_str()); 
  }
  
return ret;  

}


int Shell::com_pwd(vector<string>& argv) {
  if (argv.size() > 1)
  {
	fprintf(stderr, ": too many arguments\n");
	return 1;
  }
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

  if (argv.size() > 2)
  {
    fprintf(stderr, "too many arguments\n");
    return 1;
  }

 if (argv.size() == 1) {
    for (auto const &elem : aliases)
    {
	printf("%s='%s'\n", elem.first.c_str(), elem.second.c_str());
    }
    return 0;
  }


  if (argv[1].find('=') == std::string::npos)
  {
  for (auto elem : aliases)
  {
    if (elem.second == argv[1])
    {
        printf("%s='%s'\n", elem.first.c_str(), elem.second.c_str());
        return 0;
    } 
  }

   fprintf(stderr, "alias is not assigned yet\n");
   return 1;

  } 
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

  if (argv.size() > 2)
  {
    fprintf(stderr, "too many arguments\n");
    return 1;
  }
  //give user info how to use command if used wrong (just like linux bash does).
  if (argv.size() == 1)
  {
    std::cout << "unalias: usage: unalias [-a] name [name ...]" << std::endl;
    return 1;
  }


  if (argv[1] == "-a")
    aliases.clear(); //removes all aliases
  else
  {
  //find if value is aliases
  for (auto elem : aliases)
  {
    if (elem.second == argv[1])
      aliases.erase(aliases.find(elem.first));
  }
  }

  return 0;
}


int Shell::com_echo(vector<string>& argv) {
  for (auto itr = argv.begin() + 1; itr != argv.end(); itr++)
		std::cout << *itr << " ";
	printf("\n");

	return 0;
}


int Shell::com_history(vector<string>& argv) {
	HISTORY_STATE* hist_state = history_get_history_state(); 
	HIST_ENTRY** histlst = history_list();
	printf("history\n");
	for (int i = 1; *histlst; i++, histlst++) 
            printf("%4d %s\n", i, (*histlst)->line);

	return 0;
}


int Shell::com_exit(vector<string>& argv) {
  exit(0); 
  return 0;
}
