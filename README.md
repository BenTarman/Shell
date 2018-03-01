# Project 1

#Name
Benjamin Tarman


#List of files and what each does

	makeFile			makefile
	main.cpp			starts shell instance
	shell_builtins.cpp		implements builtins
	shell_cmd_execution.cpp		implements external commands and has piping and file redirection logic

	shell_core.cpp			parses command user enters
	shell_tab_completion		has code for tab completion logic
	shell.h				header file that contains all functions used. Singleton pattern used.
	
	



#Extra Features
History is stored in a file in the users home directory. I just made it
easily visible calling the file "history." This history file stores
previous session commands used.




# Time spent on project

I spent 13 hours getting this to work.



# Graded
 * Score tag final: 160/160
 * Score total: 200/200
