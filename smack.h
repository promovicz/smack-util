
#ifndef SMACK_H_
#define SMACK_H_

#include <sys/types.h>

/* Predefined labels */
#define SMACK_STAR	"*"
#define SMACK_FLOOR	"_"
#define SMACK_HAT	"^"

/* The default label */
#define SMACK_DEFAULT SMACK_FLOOR

/* Control file locations */
#define SMACK_LOAD	"/smack/load"
#define SMACK_CIPSO "/smack/cipso"

#define SMACK_PROCSELFATTRCURRENT "/proc/self/attr/current"

#define SMACK_USERS "/etc/smack/users"

/* Maximum size of label, including null terminator. */
#define SMACK_SIZE	24

struct smackuser {
	char *su_name;
	char *su_label;
};

int getsmackuser_r(const char *name, struct smackuser *su,
				   char *buf, size_t buflen, struct smackuser **result);

/**
 * Retrieve the smack label of the current process.
 *
 * Label will be deposited in buffer behind LABEL,
 * limited in length by N.
 *
 * On error, eturns -1 with the following codes in errno:
 *
 *  ENOSYS - not operating under smack
 *  ENOMEM - insufficient memory at LABEL
 */
int getsmack(char *label, size_t n);

/**
 * Set the SMACK label of the current process.
 *
 * Fails by returning -1, error code in errno.
 */
int setsmack(const char *label);

/**
 * Check if SMACK would allow an access.
 *
 * This requires permission to read /smack/load.
 */
int smackaccess(char *subject, char *object, char *access);

/**
 * Check if SMACK is enabled.
 *
 */
int smackenabled(void);

#endif /* !SMACK_H_ */
