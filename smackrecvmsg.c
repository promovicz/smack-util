/*
 * Copyright (C) 2007 Casey Schaufler <casey@schaufler-ca.com>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, version 2.
 *
 * Authors:
 *	Casey Schaufler <casey@schaufler-ca.com>
 *	Ahmed S. Darwish <darwish.07@gmail.com>
 */

#include <linux/types.h>
#include <linux/socket.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

int smackrecvmsg(int sock, struct msghdr *msgp, int flags,
		 char *smack, int smacklen)
{
	struct cmsghdr *chp;
	char *cp;
	int rc;

	rc = recvmsg(sock, msgp, flags);
	if (rc < 0) {
		perror("recvmsg error");
		return -1;
	}
	if (msgp->msg_controllen <= sizeof(struct cmsghdr)) {
		fprintf(stderr, "%s out of control %d <= %d\n", __func__,
			msgp->msg_controllen, sizeof(struct cmsghdr));
		return -1;
	}
	chp = CMSG_FIRSTHDR(msgp);
	if (chp->cmsg_level != SOL_IP) {
		fprintf(stderr, "%s level not SOL_IP\n", __func__);
		return -1;
	}
	if (chp->cmsg_type != SCM_SECURITY) {
		fprintf(stderr, "%s type not SCM_SECURITY\n", __func__);
		return -1;
	}
	cp = CMSG_DATA(chp);
	if (strlen(cp) >= smacklen) {
		fprintf(stderr, "%s len %d too small, smack is %d\n", __func__,
			smacklen, strlen(cp));
		return -1;
	}
	if (chp->cmsg_len >= smacklen) {
		fprintf(stderr, "%s len %d too small, needed %d\n", __func__,
			smacklen, chp->cmsg_len);
		return -1;
	}
	strcpy(smack, cp);
	return 0;
}
