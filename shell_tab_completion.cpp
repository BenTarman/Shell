/**

 * This file contains implementations of the functions that provide
 * tab-completion for the shell.
 *
 * You will need to finish the implementations of the completion functions,
 * though you're spared from implementing the high-level readline callbacks with
 * their weird static variables...
 */

#include "shell.h"
#include <cstdlib>
#include <iostream>
#include <readline/readline.h>
#include <readline/history.h>
#include <sstream>
#include <dirent.h>
#include <algorithm>
#include <numeric>

using namespace std;

char *myconvert(const std::string &s)
{
    char *pc = new char[s.size() + 1];
    strcpy(pc, s.c_str());
    return pc;
}


char* Shell::cmdGenerator(const char *text, int state)
{
    std::vector<char*> cmds;
    std::vector<std::string> temp;
    std::vector<char*> builtin_alias_cmds;
    std::vector<char*> aliases;


	DIR *dp;
        struct dirent *dirp;
        char *path = strdup(getenv("PATH"));
        char *dir = strtok(path, ":");

        while (dir) {
            if ((dp = opendir(dir)) == NULL) {
                dir = strtok(NULL, ":");
                continue;
            }
            while ((dirp = readdir(dp)) != NULL) {
		cmds.push_back(strdup(dirp->d_name));
            }
            closedir(dp);
            dir = strtok(NULL, ":");
        }

	free(path);
	

    for (auto const &x : Shell::getInstance().builtins) 
	temp.push_back(x.first);
	
    for (auto const &x : Shell::getInstance().aliases)
	temp.push_back(x.first); 
	
 std::transform(temp.begin(), temp.end(), std::back_inserter(builtin_alias_cmds), myconvert);



cmds.insert( cmds.end(), builtin_alias_cmds.begin(), builtin_alias_cmds.end() );

    static int list_index, len;
    char *name;

    if (!state) {
        list_index = 0;
        len = strlen(text);
    }

    while ((name = cmds[list_index++])) {
        if (strncmp(name, text, len) == 0) {
            return strdup(name);
        }
    }
    return NULL; 
}





char* Shell::envGenerator(const char *text, int state)
{

    std::vector<char*> envs;
    std::vector<std::string> temp;
    std::vector<char*> localenvs;

    char* p = *environ;
    int i = 1; 
    for (; p; i++)
    {
	char* t = "$";
	char* both = (char*)malloc(strlen(p) + strlen(t) + 2);
	strcpy(both, t);
	strcat(both, p);
	//snprintf(t, sizeof(t), "%s%s", "$", p);
	
	for (int i = 0; i < strlen(both); i++)
	{
	   if (both[i] == '=') both[i] = '\0';
	}
	envs.push_back(both);
	p = *(environ + i);
    }
    

    for (auto x : Shell::getInstance().localvars)
	temp.push_back((char*)("$" + x.first).c_str());

 std::transform(temp.begin(), temp.end(), std::back_inserter(localenvs), myconvert);

  envs.insert( envs.end(), localenvs.begin(), localenvs.end() );


    static int list_index, len;
    char *name;

    if (!state) {
        list_index = 0;
        len = strlen(text);
    }

    while ((name = envs[list_index++])) {
        if (strncmp(name, text, len) == 0) {
            return strdup(name);
        }
    }
    return NULL; 
}



char** Shell::word_completion(const char* text, int start, int end) {
    char **matches = NULL;
    if (*text == '$')
    	matches = rl_completion_matches(text, envGenerator);

    else if (start == 0)
        matches = rl_completion_matches(text, cmdGenerator);
    return matches;
}












