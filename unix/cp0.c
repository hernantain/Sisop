#define _POSIX_C_SOURCE 200809L

#include <sys/unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int cp0( const char* file, const char* target ){
	
	int fd = open( file, O_RDONLY );

	if ( fd < 0 ) return 1;
	size_t filesize = lseek(fd, 0, SEEK_END);
	char buffer[ filesize + 1 ];
	close( fd );

	fd = open(file, O_RDONLY);
	
	if (fd < 0) return 1;

	int fd2 = open(target, O_CREAT | O_WRONLY, 0664 );
	if ( fd2 < 0 ){
		close(fd);
		return 1;
	}
	
	int bytes = read( fd, buffer, sizeof(buffer)-1 );
	if ( bytes < 0 ){
		close(fd);
		close(fd2);
		return 1;
	}

	if (write( fd2, buffer, bytes ) < 0 ){
		close(fd);
		close(fd2);
		return 1;
	}
	
	close( fd );
	close( fd2 );	
	return 0;
}



int main( int argc, char*argv[] ){
	assert( argc == 3 );
	return cp0( argv[1], argv[2] );
}
