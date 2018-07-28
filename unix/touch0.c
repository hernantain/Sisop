#define _POSIX_C_SOURCE 200809L

#include <sys/unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int touch0( const char* file ){
	
	int fd = open( file, O_CREAT, 0664 ); //// 0664 = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH
	if ( fd < 0 ) return 1;
	close( fd );	
	return 0;
}



int main( int argc, char*argv[] ){
	assert( argc == 2 );
	return touch0( argv[1] );
}