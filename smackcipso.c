/*
 * smackcipso - properly format smack access cipsos for
 * loading into the kernel by writing to /smack/cipso.
 *
 * Copyright (C) 2007 Casey Schaufler <casey@schaufler-ca.com>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, version 2.
 *
 * Authors:
 *	Casey Schaufler <casey@schaufler-ca.com>
 *	Ahmed S. Darwish <darwish.07@gmail.com>
 *
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "smack.h"

#define LSIZE 23
#define NSIZE 4
#define MAXCATNUM 239
#define MAXCATVAL 63
#define MAXLEVEL 255

int
main(int argc, char *argv[])
{
	int cipsofd;
	char line[512];
	char cipso[LSIZE + NSIZE + NSIZE + (NSIZE * MAXCATNUM)];
	char smack[LSIZE+1];
	char cats[MAXCATNUM+1][NSIZE+1];
	char *cp;
	int level;
	int cat;
	int catcount;
	int i;
	int err;

	cipsofd = open(SMACK_CIPSO, O_RDWR);
	if (cipsofd < 0) {
		perror("opening " SMACK_CIPSO);
		exit(1);
	}

	while (fgets(line, sizeof(line), stdin) != NULL) {
		catcount = 0;
		err = 0;

		if ((cp = strchr(line, '\n')) == NULL) {
			fprintf(stderr, "missing newline \"%s\"\n", line);
			continue;
		}
		*cp = '\0';
		cp = strtok(line, " \t");
		if (cp == NULL) {
			fprintf(stderr, "Empty line: \"%s\"\n", line);
			continue;
		}
		sprintf(cipso, "%-23s ", line);
		if (strlen(cipso) != 24) {
			fprintf(stderr, "Bad label starting: \"%s\"\n", line);
			continue;
		}
		cp = strtok(NULL, " \t");
		if (cp == NULL) {
			fprintf(stderr, "Missing level: \"%s\"\n", line);
			continue;
		}
		if (!isdigit(*cp)) {
			fprintf(stderr, "Bad level: \"%s\"\n", cp);
			continue;
		}
		level = atoi(cp);
		if (level > MAXLEVEL) {
			fprintf(stderr, "Bad level: \"%s\"\n", cp);
			continue;
		}
		sprintf(cipso+LSIZE+1, "%-4d", level);

		cp = strtok(NULL, " \t");
		for (i = 0; cp != NULL; cp = strtok(NULL, " \t"), i++) {
			if (!isdigit(*cp)) {
				fprintf(stderr, "Bad category \"%s\"\n", cp);
				err = 1;
				break;
			}
			cat = atoi(cp);
			if (i >= MAXCATNUM) {
				fprintf(stderr, "Maximum number of categories"
					"exceeded \"%d\"\n", i);
				err = 1;
				break;
			}
			if (cat > MAXCATVAL) {
				fprintf(stderr, "Bad category \"%s\"\n", cp);
				err = 1;
				break;
			}
			sprintf(cats[i], "%-4d", cat);
		}
		if (err)
			continue;

		sprintf(cipso+LSIZE+1+NSIZE, "%-4d", i);
		while (i > 0)
			strcat(cipso, cats[--i]);
		err = write(cipsofd, cipso, strlen(cipso));
		if (err < 0)
			perror("writing /smack/cipso");
	}
	exit(0);
}
