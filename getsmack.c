
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "smack.h"

int getsmack(char *label, size_t n) {
	int fd = 0, rc;

	if(n < SMACK_SIZE) {
		puts("insufficient space");
		goto failspace;
	}

	fd = open(SMACK_PROCSELFATTRCURRENT, O_RDONLY);
	if(fd < 0) {
		perror("open failed");
		goto failopen;
	}

	rc = read(fd, label, n - 1);
	if(rc < 0) {
		perror("read failed");
		goto failread;
	}
	if(rc > 0) {
		label[rc] = 0;
	} else {
		puts("smack disabled");
		goto failnotsup;
	}

	close(fd);

	return 0;

 failread:
 failnotsup:
	close(fd);

 failopen:
 failspace:
	return -1;
}
