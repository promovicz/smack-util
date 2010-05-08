
#include <fcntl.h>
#include <string.h>

#define PROCSELFATTRCURRENT	"/proc/self/attr/current"

int setsmack(const char *label) {
	int fd, rc;

	fd = open(PROCSELFATTRCURRENT, O_RDWR);
	if(fd <= 0) {
		return -1;
	}

	rc = write(fd, label, strlen(label) + 1);
	if (rc < 0) {
		return -1;
	}

	// XXX copied from original openssh patch. needed? why?
	fsync(fd);

	close(fd);

	return 0;
}
