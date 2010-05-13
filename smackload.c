/*
 * smackload - properly format smack access rules for
 * loading into the kernel by writing to /smack/load.
 *
 * Copyright (C) 2007 Casey Schaufler <casey@schaufler-ca.com>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, version 2.
 *
 * Author:
 *	Casey Schaufler <casey@schaufler-ca.com>
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define LSIZE 23
#define ASIZE 4

int
main(int argc, char *argv[])
{
	int loadfd;
	char line[80];
	char rule[LSIZE + LSIZE + ASIZE + 3];
	char subject[LSIZE+1];
	char object[LSIZE+1];
	char accesses[ASIZE+1];
	char real[ASIZE+1];
	char *cp;
	int i;
	int err;

	loadfd = open("/smack/load", O_RDWR);
	if (loadfd < 0) {
		perror("opening /smack/load");
		exit(1);
	}

	while (fgets(line, 80, stdin) != NULL) {
		err = 0;
		if ((cp = strchr(line, '\n')) != NULL)
			*cp = '\0';
		if (sscanf(line,"%23s %23s %4s",subject,object,accesses) != 3) 
			err = 1;
		else {
			strcpy(real, "----");
			for (i = 0;
			     i < ASIZE && accesses[i] != '\0' && err == 0;
			     i++) {
				switch (accesses[i]) {
				case 'r':
				case 'R':
					real[0] = 'r';
					break;
				case 'w':
				case 'W':
					real[1] = 'w';
					break;
				case 'x':
				case 'X':
					real[2] = 'x';
					break;
				case 'a':
				case 'A':
					real[3] = 'a';
					break;
				case '\0':
				case '-':
					break;
				default:
					err = 1;
					break;
				}
			}
		}
		if (err == 0) {
			sprintf(rule, "%-23s %-23s %4s", subject,object,real);
			err = write(loadfd, rule, LSIZE + LSIZE + ASIZE + 2);
			if (err < 0)
				perror("writing /smack/load");
		}
		else
			fprintf(stderr, "Bad input line \"%s\"\n", line);
	}
	exit(0);
}
