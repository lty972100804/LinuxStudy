#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LEDOFF  0
#define LEDON   1

/*
 * ./ledAPP <filename> <0:1> 0 for close light, 1 for open light
 * 
 * */
int main(int argc, char *argv[])
{
	int fd, retvalue;
	char *filename;
	unsigned char databuf[1];


	if(argc != 3) {
		printf("Error Usage!\n");
		return -1;
	}

	filename = argv[1];

	fd = open(filename, O_RDWR);
	if(fd < 0) {
		printf("file %s open failed!\n", filename);
		return -1;
	}

	databuf[0] = atoi(argv[2]); /* 将字符转换为数字 */
	
	retvalue = write(fd, databuf, sizeof(databuf));
	if(retvalue < 0) {
		printf("LED Control Failed!\n");
		close(fd);
		return -1;
	}

	close(fd);

	return 0;
}
