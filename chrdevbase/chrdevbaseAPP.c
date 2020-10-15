#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

/*
 *
 * ./chrdevbaseAPP <filename> <1:2> 1 for read, 2 for write
 *
 *
 * */
int main(int argc, char *argv[])
{
	int ret = 0;
	int fd = 0;
	char *filename = NULL;
	char readbuf[100], writebuf[100] = "hello world!";

	if (argc != 3) {
		printf("Error usage!\n");
		return -1;
	}

	filename = argv[1];

	/* open */
	fd = open(filename, O_RDWR);
	if (fd < 0) {
		printf("Can't open file %s\n", filename);
		return -1;
	}

	/* read */
	ret = read(fd, readbuf, 50);
	if (ret < 0) {
		printf("read file %s failed\n", filename);
	}
	else {

	}

	/* write */
	ret = write(fd, writebuf, 50);
	if (ret < 0) {
		printf("write file %s failed\n", filename);
	}
	else {

	}

	/* close */
	ret = close(fd);
	if (ret < 0 ) {
		printf("close file %s failed\n", filename);
	}

	return 0;
}
