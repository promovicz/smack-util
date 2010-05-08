
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define ETCSMACKUSER		"/etc/smack/user"
#define PROCSELFATTRCURRENT	"/proc/self/attr/current"

int
main(int argc, char **argv)
{
	int rc;
	char *label;
	char *program;
	char **arguments;

	if(argc < 3) {
		printf("Usage: %s <label> <program> [<argument>...]\n", argv[0]);
		goto fail;
	}

	label = argv[1];
	program = argv[2];
	arguments = argv + 2;

	if(geteuid() != 0) {
		// XXX: check system-wide table of allowed label transitions
		printf("You are not root.\n");
		goto fail;
	}

	if(!smackenabled()) {
		printf("SMACK does not seem to be enabled.\n");
		goto fail;
	}

	rc = setsmack(label);
	if(rc < 0) {
		perror("Could not set SMACK label");
		goto fail;
	}

	execvp(program, arguments);

	printf("Could not exec %s: %s\n", program, strerror(errno));

	// exec return always a failure

 fail:
	return 1;
}
