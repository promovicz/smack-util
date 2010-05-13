
#include <stdio.h>
#include <string.h>

int smackenabled(void) {
	FILE *fp;
	size_t num;
	size_t len;
	char *buf = NULL, *p;
	int enabled = 0;

	fp = fopen("/proc/mounts", "r");
	if (!fp)
		goto out;

	while ((num = getline(&buf, &len, fp)) != -1) {
		char *tmp;
		p = strchr(buf, ' ');
		if (!p)
			goto out;
		p++;
		tmp = strchr(p, ' ');
		if (!tmp)
			goto out;
		if (!strncmp(tmp + 1, "smackfs ", strlen("smackfs"))) {
			enabled = 1;
			goto out;
		}
	}

 out:
	if(fp) {
		fclose(fp);
		fp = NULL;
	}

	return enabled;
}
