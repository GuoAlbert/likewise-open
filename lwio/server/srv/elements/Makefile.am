noinst_LTLIBRARIES = libsrvelements.la

libsrvelements_la_SOURCES = \
    config.c        \
    context.c       \
    creditor.c      \
    globals.c       \
    libmain.c       \
    mt19937ar.c     \
    resources.c     \
    srvasyncclose.c \
    srvasyncstate.c \
    srvconnection.c \
    srvenum.c       \
    srvfile.c       \
    srvfile2.c      \
    srvfinder.c     \
    srvgss.c        \
    srvioapi.c      \
    srvmpxtracker.c \
    srvsession.c    \
    srvsession2.c   \
    srvtimer.c      \
    srvtree.c       \
    srvtree2.c

libsrvelements_la_CPPFLAGS = \
    $(AM_CPPFLAGS) \
    -DLW_ENABLE_THREADS=1  \
    -I@top_srcdir@         \
    -I@top_srcdir@/include \
    -I@top_srcdir@/server/include \
    -I@top_srcdir@/server/srv/include

libsrvelements_la_LDFLAGS =

