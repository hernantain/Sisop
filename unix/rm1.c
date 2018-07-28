#define _POSIX_C_SOURCE 200809L

#include <sys/unistd.h>
#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>


int main(int argc, char*argv[]){
	assert(argc == 2);
	if ( (unlink(argv[1]) < 0) ){
		perror("rm");
		printf("rm: cannot remmove %s : %s\n", argv[1], strerror(errno) );
		return 1;
	}
	return 0;
}