#define _POSIX_C_SOURCE 200809L

#include <sys/unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <utime.h>

int touch1( const char* file ){
	
	int fd;

	if ( utime( file, NULL ) < 0 ){
		fd = open( file, O_CREAT, 0664 );
		if ( fd < 0 ) return 1;
		close( fd );
		return 1;
	}

	fd = open( file, O_CREAT, 0664 ); // 0664 = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH
	if ( fd < 0 ) return 1;
	close( fd );	
	return 0;
}



int main( int argc, char*argv[] ){
	assert( argc == 2 );
	return touch1( argv[1] );
}