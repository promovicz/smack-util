
#include <stdio.h>
#include <syslog.h>
#include <sys/types.h>
#include <security/pam_ext.h>
#include <security/pam_modutil.h>

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
		pam_syslog(pamh, LOG_ERR,
		           "User %s does not have a smack label defined",
		           user);
		return PAM_USER_UNKNOWN;
	}

	rc = setsmack(su.su_label);
	if(rc) {
		pam_syslog(pamh, LOG_ERR,
		           "Failed to set label %s for user %s",
		           su.su_label, user);
		return PAM_SYSTEM_ERR;
	}

	printf("Switched SMACK label to %s for user %s\n", su.su_label, su.su_name);

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
