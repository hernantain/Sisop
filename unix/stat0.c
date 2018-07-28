#define _POSIX_C_SOURCE 200809L

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>

int stat0( const char* file ){
	
	struct stat fileStat;

	if (stat( file, &fileStat ) < 0) return 1;
	printf( "Size: %d bytes\n",(int) fileStat.st_size );
	printf( "File: %s\n", file);
	if ( S_ISREG(fileStat.st_mode) ) printf( "Type: regular file\n" );
	else printf("Type: directory\n");
	return 0;
}



int main( int argc, char*argv[] ){
	assert( argc == 2 );
	return stat0( argv[1] );
}