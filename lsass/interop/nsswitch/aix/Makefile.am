netsvclibdir=/usr/lib/netsvc/dynload

if ENABLE_COMPAT
nsslib_LTLIBRARIES = LSASS_64.la
netsvclib_LTLIBRARIES = lsass_64.la
else
nsslib_LTLIBRARIES = LSASS.la
netsvclib_LTLIBRARIES = lsass.la
endif

LSASS_la_SOURCES = \
	lam-main.c \
	lam-group.c \
	lam-user.c \
	lam-auth.c \
	globals.c

LSASS_64_la_SOURCES = \
	lam-main.c \
	lam-group.c \
	lam-user.c \
	lam-auth.c \
	globals.c

LSASS_la_CFLAGS = -shared $(AM_CFLAGS)

LSASS_64_la_CFLAGS = -shared $(AM_CFLAGS)

LSASS_la_CPPFLAGS = \
	$(AM_CPPFLAGS) \
	-I@srcdir@/../include \
	-I@top_srcdir@/interop/auth/include

LSASS_64_la_CPPFLAGS = \
	$(AM_CPPFLAGS) \
	-I@srcdir@/../include \
	-I@top_srcdir@/interop/auth/include

LSASS_la_LIBADD = \
	@builddir@/../common/libnsscommon.la \
	@top_builddir@/interop/auth/common/liblsaauth.la \
	@top_builddir@/client/liblsaclient.la

LSASS_64_la_LIBADD = \
	@builddir@/../common/libnsscommon.la \
	@top_builddir@/interop/auth/common/liblsaauth.la \
	@top_builddir@/client/liblsaclient.la

LSASS_la_LDFLAGS = -shared -module -avoid-version -export-dynamic -Wl,-eLsaNssInitialize -shrext "$(NSS_SHR_EXT)"

LSASS_64_la_LDFLAGS = -shared -module -avoid-version -export-dynamic -Wl,-eLsaNssInitialize -shrext "$(NSS_SHR_EXT)"

lsass_la_SOURCES = \
	netsvc-netgrp.c

lsass_64_la_SOURCES = \
	netsvc-netgrp.c

lsass_la_CPPFLAGS = \
	$(AM_CPPFLAGS) \
	-I@srcdir@/../include \
	-I@top_srcdir@/interop/auth/include

lsass_64_la_CPPFLAGS = \
	$(AM_CPPFLAGS) \
	-I@srcdir@/../include \
	-I@top_srcdir@/interop/auth/include

lsass_la_CFLAGS = -shared $(AM_CFLAGS)

lsass_64_la_CFLAGS = -shared $(AM_CFLAGS)

lsass_la_LIBADD = \
	@builddir@/../common/libnsscommon.la \
	@top_builddir@/interop/auth/common/liblsaauth.la \
	@top_builddir@/client/liblsaclient.la

lsass_64_la_LIBADD = \
	@builddir@/../common/libnsscommon.la \
	@top_builddir@/interop/auth/common/liblsaauth.la \
	@top_builddir@/client/liblsaclient.la

lsass_la_LDFLAGS = -shared -module -avoid-version -Wl,-brtl

lsass_64_la_LDFLAGS = -shared -module -avoid-version -Wl,-brtl
