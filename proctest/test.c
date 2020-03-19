#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
	char buf[100];
	char rbuf[100];
	int fd = open("/proc/apertus", O_RDWR);
	printf("\n ---------- WELCOME TO PROC FILE SYSTEM DEMO----------\n");
	printf("\n for exit press 0\n");
	int i=1;
	while(i!=0){
	        scanf("%s",buf);
	        write(fd, buf, sizeof(buf));
	        lseek(fd, 0, SEEK_SET);
	        read(fd, rbuf, 100);
	        puts(rbuf);
	        lseek(fd, 0, SEEK_SET);
		scanf("%d",&i);

	}
	return 0;
}

