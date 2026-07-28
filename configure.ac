AC_INIT([pspvc], [0.4], [ringtailedfox])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])
AC_SUBST([pkgdatadir], ["${datadir}/pspvc"])

# Default install prefix should be /usr, not /usr/local
AC_PREFIX_DEFAULT(/usr)

# Checks for programs
AC_PROG_CC

# GTK3
PKG_CHECK_MODULES([GTK], [gtk+-3.0])

# Gettext
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.19])

# Define LOCALEDIR for main.c
AC_DEFINE_UNQUOTED(LOCALEDIR, "${datadir}/locale", [Locale directory])

# Output files
AC_CONFIG_FILES([
    Makefile
    src/Makefile
    po/Makefile.in
])

AC_OUTPUT
