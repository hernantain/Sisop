//#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

int mv0( const char* origin, const char* destination ){
	if (rename( origin, destination ) < 0) return 1;
	return 0;
}



int main( int argc, char*argv[] ){
	assert( argc == 3 );
	return mv0( argv[1], argv[2] );
}	