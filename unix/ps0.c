#define _POSIX_C_SOURCE 200809L
#define COMM_NAME_SIZE 16  //Strings longer than TASK_COMM_LEN (16) characters are silently truncated.
#define PATCH_SIZE 25

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

void ps0(){

	void * dir;
	struct dirent * file;
	int fd;
	char cmd_name[COMM_NAME_SIZE];

	dir = opendir("/proc");
	if ( dir == NULL ) exit(1);
	printf("PID\tCMD\n");
	

	while ( (file = readdir(dir)) != NULL) {

		if(!isdigit( file->d_name[0]) )	continue; //con que el primer elemento del string sea un num alcanza

		char path[25];
		strcpy( path, "/proc/" );
		strcat( path, file->d_name );
		strcat( path, "/comm" );

		fd = open( path, O_RDONLY );
		if ( fd < 0 ) exit(1);

		int bytes = read( fd, cmd_name, sizeof(cmd_name) );
		if (bytes < 0 ) exit(1);

		cmd_name[bytes] = '\0';
		printf( "%s\t%s", file->d_name, cmd_name );
		close(fd) ;
	}
	closedir(dir);
}


int main(){
	ps0();
}