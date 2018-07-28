#include "builtin.h"

// returns true if the 'exit' call
// should be performed
int exit_shell(char* cmd) {

	if (strcmp(cmd,"exit") == 0)
		return true;

	return 0;
}

// returns true if "chdir" was performed
// this means that if 'cmd' contains:
// 	$ cd directory (change to 'directory')
// 	$ cd (change to HOME)
// it has to be executed and then return true
int cd(char* cmd) {
	
	if(strlen(cmd) == 2){
		if(strcmp(cmd,"cd") == 0){
			int ret = chdir(getenv("HOME"));
			if (ret == 0) return true;
			else return 0;
		}
		else return 0;
	}


	if((cmd[0] == 'c') && (cmd[1] == 'd')){
		char* splited = split_line(cmd,' ');
		if (chdir(splited) == 0) return true;
		return 0;
	}

	return 0;
}

// returns true if 'pwd' was invoked
// in the command line
int pwd(char* cmd) {

	if (strcmp(cmd,"pwd") == 0){
		printf("%s\n",get_current_dir_name());
		return true;
	}
	return 0;
}
