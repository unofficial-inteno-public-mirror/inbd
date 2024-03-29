dnl init stuff needs to be first in file
AC_INIT([inbdf], [0.1], [Kenneth Johansson <kenneth@southpole.se>])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AM_SILENT_RULES([yes])

AC_PROG_CC
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile])

#        "-Wformat=2 -Wformat-security -Wformat-nonliteral" \
CC_CHECK_FLAGS_APPEND([with_cflags], [CFLAGS], [\
        -std=gnu11 \
	-pipe \
        -Wall \
        -Wextra \
        -Wno-inline \
        -Wundef \
        -Wlogical-op \
        -Wsign-compare \
        -Wmissing-include-dirs \
        -Wold-style-definition \
        -Wpointer-arith \
        -Winit-self \
        -Wdeclaration-after-statement \
        -Wfloat-equal \
        -Wsuggest-attribute=noreturn \
        -Wmissing-prototypes \
        -Wstrict-prototypes \
        -Wredundant-decls \
        -Wmissing-declarations \
        -Wmissing-noreturn \
        -Wshadow \
        -Wendif-labels \
        -Wstrict-aliasing=2 \
        -Wwrite-strings \
        -Wno-long-long \
        -Wno-overlength-strings \
        -Wno-unused-parameter \
        -Wno-missing-field-initializers \
        -Wno-unused-result \
        -Werror=overflow \
        -Wdate-time \
        -Wnested-externs \
        -ffast-math \
        -fno-common \
        -fdiagnostics-show-option \
        -fno-strict-aliasing \
        -fvisibility=hidden \
        -ffunction-sections \
        -fdata-sections \
        -fstack-protector \
        -fstack-protector-strong \
        -fPIE \
        --param=ssp-buffer-size=4])

AC_SUBST([OUR_CFLAGS], "$with_cflags")

dnl check for uci , its manadatory
AC_SEARCH_LIBS([uci_load], [uci], [], [AC_MSG_ERROR([*** UCI library not found])])

dnl check ubox , its manadatory
AC_SEARCH_LIBS([uloop_init], [ubox], [], [AC_MSG_ERROR([*** UBOX library not found])])

dnl chek ubus  , its manadatory
AC_SEARCH_LIBS([ubus_connect], [ubus], [], [AC_MSG_ERROR([*** UBUS library not found])])

# Check for libnl3
PKG_CHECK_MODULES([LIBNL], [libnl-3.0 libnl-genl-3.0])

AC_OUTPUT

AC_MSG_RESULT([
        $PACKAGE_NAME $VERSION
        CFLAGS:                  ${OUR_CFLAGS}  ${CFLAGS} 
        CPPFLAGS:                ${OUR_CPPFLAGS} ${CPPFLAGS}
        LDFLAGS:                 ${OUR_LDFLAGS} ${LDFLAGS}
])
