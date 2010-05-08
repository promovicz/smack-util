
#include <stdio.h>
#include <string.h>

#include "smack.h"

int getsmackuser_r(const char *name, struct smackuser *su,
				   char *buf, size_t buflen, struct smackuser **result) {
	int rc;

	FILE *fp;

	char *line;
	size_t linelen;

	char *line_user, *line_label;

	fp = fopen(SMACK_USERS, "r");
	if(!fp) {
		perror("opening " SMACK_USERS);
		return -1;
	}

	*result = NULL;

	line = buf;
	linelen = buflen;
	while(getline(&line, &linelen, fp) != -1) {
		rc = strspn(line," \t\r\n");
		line += rc;

		rc = strcspn(line, " \t\r\n");
		if(!rc) {
			goto next;
		}
		line[rc] = 0;
		if(strcmp(line, name)) {
			goto next;
		}
		su->su_name = line;
		line += rc + 1;

		rc = strspn(line," \t\r\n");
		if(!rc) {
			//			puts("no space after user?");
			//goto next;
		}
		line += rc;

		rc = strcspn(line, " \t\r\n");
		if(!rc) {
			puts("no label after user?");
			goto next;
		}
		line[rc] = 0;
		su->su_label = line;
		line += rc;

		*result = su;

		break;

	next:
		line = buf;
		linelen = buflen;
	}
	if(ferror(fp)) {
		printf("error reading line: %s\n", strerror(ferror(fp)));
		fclose(fp);
		return -1;
	}
	fclose(fp);

	return 0;
}

