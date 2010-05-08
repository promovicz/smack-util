/* pam_apparmor module */

/*
 * $Id: pam_apparmor.c 437 2007-03-13 17:52:28Z jmichael-at-suse-de $
 *
 * Written by Jesse Michael <jmichael@suse.de> 2006/08/24
 *    and Steve Beattie <sbeattie@suse.de> 2006/10/25
 *
 * Based off of pam_motd by:
 *   Ben Collins <bcollins@debian.org> 2005/10/04
 *   Michael K. Johnson <johnsonm@redhat.com> 1996/10/24
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <syslog.h>
#include <errno.h>
#include <security/pam_ext.h>
#include <security/pam_modutil.h>

/*
 * here, we make a definition for the externally accessible function
 * in this file (this definition is required for static a module
 * but strongly encouraged generally) it is used to instruct the
 * modules include file to define the function prototypes.
 */

#define PAM_SM_SESSION
#include <security/pam_modules.h>

#include "smack.h"

PAM_EXTERN int
pam_sm_close_session (pam_handle_t *pamh, int flags,
              int argc, const char **argv)
{
    return PAM_IGNORE;
}

PAM_EXTERN
int pam_sm_open_session(pam_handle_t *pamh, int flags,
            int argc, const char **argv)
{
	int rc;
	const char *user;
	struct smackuser su;
	struct smackuser *sup;
	char subuf[1024];

	rc = pam_get_user(pamh, &user, NULL);
	if (rc != PAM_SUCCESS || user == NULL || *user == '\0') {
		pam_syslog(pamh, LOG_ERR, "Can't determine user\n");
		return PAM_USER_UNKNOWN;
	}

	rc = getsmackuser_r(user, &su, subuf, sizeof(subuf), &sup);
	if(rc || !sup) {
		pam_syslog(pamh, LOG_ERR, "User does not have a smack label defined.");
		return PAM_USER_UNKNOWN;
	}

	rc = setsmack(su.su_label);
	if(rc) {
		pam_syslog(pamh, LOG_ERR, "Failed to set label.");
		return PAM_SYSTEM_ERR;
	}

	printf("Switched SMACK label to %s for user %s.\n", su.su_label, su.su_name);

	return PAM_SUCCESS;

}

#ifdef PAM_STATIC

/* static module data */

struct pam_module _pam_smack_modstruct = {
	"pam_smack",
	NULL,
	NULL,
	NULL,
	pam_sm_open_session,
	pam_sm_close_session,
	NULL,
};

#endif
