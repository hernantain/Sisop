#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int cp1( const char* file, const char* file2 ){

	int fd, fd2;
	void* buffer;
	void* pmap;

	fd = open( file, O_RDWR );
	if ( fd < 0 ) return 1;
	fd2 = open( file2, O_RDWR | O_CREAT , 0664 ); //// 0664 = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH
	if ( fd2 < 0 ){
		close(fd);
		return 1;
	}

	size_t bytes = lseek(fd, 0, SEEK_END);

	ftruncate( fd2, bytes );
	buffer = mmap( NULL, bytes, PROT_READ , MAP_PRIVATE, fd, 0);
	if ( buffer == MAP_FAILED ){
		close(fd);
		close(fd2);
		return 1;
	}

	pmap = mmap( NULL, bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd2, 0);
	if ( pmap == MAP_FAILED ){
		close( fd );
		close( fd2 );
		return 1;
	}

	memcpy( pmap, buffer, bytes );

	munmap( buffer, bytes );
    munmap( pmap, bytes );

	close( fd );
	close( fd2 );

	return 0;
}


int main( int argc, char* argv[] ){
	
	assert( argc == 3 );
	return cp1( argv[1], argv[2] );
}