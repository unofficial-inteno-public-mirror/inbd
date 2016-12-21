#PRG_VERSION := $(shell cd @top_srcdir@; if ! git describe --tags ;then echo version-$(PACKAGE_VERSION) ;fi )

AM_CFLAGS = $(OUR_CFLAGS)

bin_PROGRAMS = inbd

inbd_SOURCES = \
	src/inbd.c

inbd_LDADD = $(UCI_LIB) $(UBOX_LIB) $(UBUS_LIB) $(LIBNL_LIBS) -lm

inbd_CFLAGS =  $(AM_CFLAGS) $(LIBNL_CFLAGS) -DPRG_VERSION=\"$(PRG_VERSION)\" 
