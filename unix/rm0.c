#define _POSIX_C_SOURCE 200809L

#include <sys/unistd.h>
#include <assert.h>

int rm0(const char* file){
	if ((unlink( file ) < 0)) return 1;
	return 0;
}

int main(int argc, char*argv[]){
	assert(argc == 2);
	return rm0(argv[1]);
}