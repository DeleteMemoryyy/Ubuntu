/* Use Mmap function to change a ASCII file's first bit to 'J'.
 * The file name is given by command line. */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

int main(int argc, char **argv){
	if (argc != 2)
	{
		fprintf((stderr), "Parameter error!\n" );
		return 1;
	}

	int fd, len;
	char *buf;
	struct stat stat_data;

	if ((fd = open(argv[1], O_RDWR, 0)) < 0)
	{
		fprintf((stderr), "Open file failed!\n" );
		return 1;
	}
	fstat(fd, &stat_data);
	len = stat_data.st_size;
	if ((buf = (char *)mmap(NULL, len, PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED)
	{
		fprintf(stderr, "mmap function error!\n" );
		return 1;
	}
	buf[0] = 'J';
	printf("%s\n",buf);
	
	return 0;
