/*
 * test.c
 *
 * Simple test code for the toggle case char-dev driver
 *
 * Copyright (C) 2020 USTC
 *
 * by howif,dzw
 */

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

int main (void) 
{
	int fd;
	char buff[256]="QWERTYUIOP-asdfghjkl-ZXCVBNM";
	
	fd = open ("/dev/toggle-case",O_RDWR);
	if (fd < 0) 
	{
	  perror ("fd open failed");
	  exit(-1);
	}

	printf ("\n/dev/toggle-case opened, fd=%d\n",fd);
	printf ("buf to write is:\n%s\n", buff);
	
	if (write (fd, buff, strlen((char *)buff) + 1) < 0)
	{
		perror("fail to write");
		close(fd);
		exit(-1);
	}
   
	if (read (fd, buff, strlen((char *)buff) + 1) < 0)
	{
		perror("fail to write");
		close(fd);
		exit(-1);
	}

	printf ("buf read is:\n%s\n", buff);
	close (fd);
	printf ("/dev/toggle-case closed :)\n");

	return 0;
}
