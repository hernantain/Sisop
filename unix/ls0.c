#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>


void ls0(){
	
	void * dir;
	struct dirent * file;

	dir = opendir(".");

	while ( (file = readdir(dir)) ) {

		printf("%s\n", file->d_name);
	}

	closedir(dir);
}



int main( int argc, char*argv[] ){
	assert( argc == 1 );
	ls0();
}