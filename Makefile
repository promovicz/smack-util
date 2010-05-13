#
# Makefile for Smack utilities.
# Generated by hand, as the Intelligent Designer intended.
#
# Copyright (c) 2007 Casey Schaufler <casey@schaufler-ca.com>
#

VERSION_MAJOR = 0
VERSION_MINOR = 2

CFLAGS = -Wall -Werror -g -O2
PREFIX ?= /

UTILS = smackload smackcipso smackexec

PAMSMACK = pam_smack.so
PAMSMACKSRC = pam_smack.c

LIBSMACK_STATIC = libsmack.a
LIBSMACK_SONAME = libsmack.so.${VERSION_MAJOR}
LIBSMACK_SHARED = libsmack.so.${VERSION_MAJOR}.${VERSION_MINOR}
LIBSMACK_HEADER = smack.h
LIBSMACKSRC = smackaccess.c setsmack.c getsmack.c smackenabled.c getsmackuser.c
LIBSMACKOBJ = $(patsubst %.c,%.o,${LIBSMACKSRC})
LIBSMACKOBJ_STATIC = $(patsubst %.o,static/%.o,${LIBSMACKOBJ})
LIBSMACKOBJ_SHARED = $(patsubst %.o,shared/%.o,${LIBSMACKOBJ})

# GENERAL RULES

default: ${LIBSMACK_STATIC} ${LIBSMACK_SHARED} ${PAMSMACK} ${UTILS}
.PHONY: default

install: default
	install -d -m 755 -o root -g root                    ${PREFIX}/lib/security
	install    -m 644 -o root -g root ${PAMSMACK}        ${PREFIX}/lib/security/
	
	install -d -m 755 -o root -g root                    ${PREFIX}/lib
	install    -m 644 -o root -g root ${LIBSMACK_SHARED} ${PREFIX}/lib
	ln -s ${LIBSMACK_SHARED} ${PREFIX}/lib/${LIBSMACK_SONAME}
	ln -s ${LIBSMACK_SONAME} ${PREFIX}/lib/libsmack.so
	
	install -d -m 755 -o root -g root                    ${PREFIX}/usr/lib
	install    -m 644 -o root -g root ${LIBSMACK_STATIC} ${PREFIX}/usr/lib
	
	install -d -m 755 -o root -g root                    ${PREFIX}/usr/include
	install    -m 644 -o root -g root ${LIBSMACK_HEADER} ${PREFIX}/usr/include
	
	install -d -m 755 -o root -g root                    ${PREFIX}/sbin
	install    -m 755 -o root -g root smackload          ${PREFIX}/sbin
	install    -m 755 -o root -g root smackexec          ${PREFIX}/sbin
	install    -m 755 -o root -g root smackcipso         ${PREFIX}/sbin

clean:
	rm -rf static shared
	rm -f ${UTILS} ${LIBSMACK_STATIC} ${LIBSMACK_SHARED} ${PAMSMACK}

# UTILS

utils: ${UTILS}

%: %.c ${LIBSMACK_STATIC}
	$(CC) $(CFLAGS) -o $@ $^

# PAM MODULE

${PAMSMACK}: ${PAMSMACKSRC} ${LIBSMACK_SHARED}
	$(CC) $(CFLAGS) -fPIC -shared -lpam -o $@ $^

# STATIC LIBRARY

static:
	mkdir static

static/%.o: %.c | static
	$(CC) $(CFLAGS) -c -o $@ $^

${LIBSMACK_STATIC}: ${LIBSMACKOBJ_STATIC}
	ar cr ${LIBSMACK_STATIC} ${^}

# SHARED LIBRARY

shared:
	mkdir shared

shared/%.o: %.c | shared
	$(CC) $(CFLAGS) -c -fPIC -o $@ $^

${LIBSMACK_SHARED}: ${LIBSMACKOBJ_SHARED}
	$(CC) $(CFLAGS) -fPIC -shared -Xlinker -soname -Xlinker ${LIBSMACK_SONAME} -o ${LIBSMACK_SHARED} ${LIBSMACKOBJ_SHARED}


