
#include <stdio.h>

#include "smack.h"

int
main(int argc, char **argv) {
	struct smackuser su;
	struct smackuser *sup;
	char buf[1024];

	if(argc != 2) {
		printf("Usage: %s <username>\n", argv[0]);
		return 1;
	}

	int rc = getsmackuser_r(argv[1], &su, buf, sizeof(buf), &sup);
	if(!sup) {
		puts("ERR!");
		return 1;
	}

	printf("user %s label %s\n", su.su_name, su.su_label);

	return 0;
}
