AC_INIT( [griddaamfs], [0.1], [okoeroo@nikhef.nl])
AM_INIT_AUTOMAKE( [-Wall -Werror foreign] )


AC_PROG_INSTALL
AC_PROG_CC

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
				 Makefile
				 src/Makefile
				 ])

PKG_PROG_PKG_CONFIG(0.20)


# LIBCURL_CHECK_CONFIG( [yes], [7.19.0], [], AC_MSG_ERROR([libcurl not found]) )
#LIBCURL_CHECK_CONFIG([yes], [7.15.1], [], AC_MSG_ERROR(["libcurl not found"]))

AC_CHECK_LIB( [curl], [curl_easy_init],,[AC_MSG_ERROR([Missing libcurl (libcurl)])])

# AC_CHECK_LIB( [magic], [magic_open],,[AC_MSG_ERROR([Missing libmagic (libmagic-dev)])])

AC_OUTPUT
