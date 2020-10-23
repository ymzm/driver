/*
 * test.c
 *
 * Simple test code for the toggle case char-dev driver
 *
 * Copyright (C) 2020 USTC
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

int main (void) 
{
	int fd;
	char buff[256]="QWERTYUIOPasdfghjklZXCVBNM";
	
	fd = open ("/dev/togglecase",O_RDWR);
	if (fd < 0) 
	{
	  perror ("fd open failed");
	  exit(-1);
	}

	printf ("\n/dev/hello opened, fd=%d\n",fd);
	printf ("buf to write is:\n%s\n", buff);
	
	if (write (fd, buff, strlen(buff)) < 0)
	{
		perror("fail to write");
		close(fd);
		exit(-1);
	}
   
        // I'm lazy, buff will be modified, but not now.
	if (read (fd, buff, strlen(buff)) < 0)
	{
		perror("fail to write");
		close(fd);
		exit(-1);
	}

	printf ("buf read is:\n%s\n", buff);
	close (fd);
	printf ("/dev/hello closed :)\n");

	return 0;
}
