/**
 * This file contains the implementations of the builtin functions provided by
 * the shell (or will, once you've finished implementing them all).
 */

#include "shell.h"
#include <iostream>

#define PATH_MAX	1024
using namespace std;

int Shell::com_ls(vector<string>& argv) {


  cout << "ls called" << endl; // delete when implemented
  return 0;
}


int Shell::com_cd(vector<string>& argv) {
 
  chdir(argv[1].c_str()); 
  
return 0;  

}


int Shell::com_pwd(vector<string>& argv) {
  char currDir[PATH_MAX];
  if (getcwd (currDir, PATH_MAX) != 0)
  	printf ("%s\n", currDir);
  else 
    {
      printf ("Error getting pwd: %s\n", currDir);
      return 1;
    }



  return 0;
}


int Shell::com_alias(vector<string>& argv) {
  // TODO: YOUR CODE GOES HERE
  cout << "alias called" << endl; // delete when implemented
  return 0;
}


int Shell::com_unalias(vector<string>& argv) {
  // TODO: YOUR CODE GOES HERE
  cout << "unalias called" << endl; // delete when implemented
  return 0;
}


int Shell::com_echo(vector<string>& argv) {
  // TODO: YOUR CODE GOES HERE
  cout << "echo called" << endl; // delete when implemented
  return 0;
}


int Shell::com_history(vector<string>& argv) {
  // TODO: YOUR CODE GOES HERE
  cout << "history called" << endl; // delete when implemented
  return 0;
}


int Shell::com_exit(vector<string>& argv) {
  exit(0); 
  return 0;
}
