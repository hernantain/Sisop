#define _POSIX_C_SOURCE 200809L

#include <sys/unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int tee0( const char* file ){
	
	char buffer[80];
	int fd = open(file, O_CREAT | O_RDWR, 0664 ); // 0664 = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH
	if ( fd < 0 ) return 1;
	int bytes = read( 0, buffer, sizeof(buffer)-1 );
	
	if (bytes < 0){
		close(fd);
		return 1;
	}

	if (write( fd, buffer, bytes ) < 0 ){
		close(fd);
		return 1;
	}

	if (write( 1, buffer, bytes ) < 0 ){
		close(fd);
		return 1;
	}

	close( fd );	
	return 0;
}



int main( int argc, char*argv[] ){
	assert( argc == 2 );
	return tee0( argv[1] );
}