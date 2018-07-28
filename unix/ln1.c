#define _POSIX_C_SOURCE 200809L

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>

int ln1( const char* file, const char* target ){
	
	if ( link( file, target ) < 0 ) return 1;
	return 0;
}



int main( int argc, char*argv[] ){
	assert( argc == 3 );
	return ln1( argv[1], argv[2] );
}