#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include<sys/ioctl.h>

#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*)

void read_proc()
{
	long value;
	char buf[100],buf2[100];
	int prc = open("/proc/apertus", O_RDWR);
	printf("Reading From the procfile  :)\n");
	lseek(prc, 0 , SEEK_SET);
	read(prc, buf2, 100);
	puts(buf2);
	printf("\n");
	close(prc);
}
int read_ioctl()
{
	int fd3;
	long value;
	fd3 = open("/dev/ioctl_device", O_RDWR);
	if(fd3 < 0) {
		printf("Cannot open device file...\n");
		return 0;
	}
	printf("\nReading Checksum Value from device file using ioctl\n");
	ioctl(fd3, RD_VALUE, &value);
	printf("\nValue is %ld\n", value);
	close(fd3);

}
void write_chdv(int argc, char *argv[])
{
	int fd;
	char buf[100];
	int n,c;
	int ret;
	if (argc < 2) {
		fprintf(stderr, "usage: %s <dev>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	ret = open(argv[1], O_RDWR);
	if (ret < 0) {
		perror("open");
		exit(EXIT_FAILURE);
	}
	printf("file %s opened\n", argv[1]);
	fd = ret;
	printf("Enter the data to be sent :\n");
	scanf("%s",buf);
	for (c = 0; c < strlen(buf); c += n) {
	ret = write(fd, buf + c, strlen(buf) - c);
	if (ret < 0) {
	perror("write");
	exit(EXIT_FAILURE);
	}
	n = ret;
	printf("wrote %d bytes into file %s\n", n, argv[1]);
	}
	close(fd);	

}
int main(int argc, char *argv[])
{
	write_chdv(argc,argv);
	read_proc();
	read_ioctl();
	return 0;
}
