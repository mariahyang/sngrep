AC_PREREQ([2.59])
AC_INIT([sngrep], [0.3.0], [kaian@irontec.com], [sngrep], [http://www.irontec.com/])
AM_INIT_AUTOMAKE([1.9])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AC_CONFIG_HEADERS([src/config.h])

AC_COPYRIGHT("Irontec S.L.")

# Define _GNU_SOURCE etc.
m4_ifdef([AC_USE_SYSTEM_EXTENSIONS], [AC_USE_SYSTEM_EXTENSIONS])

# debug compilation
AC_ARG_ENABLE(debug,
    AC_HELP_STRING(--enable-debug, [Debug compilation (Default = no)]),
    enable_debug=$enableval, enable_debug=no)

if test "$enable_debug" = "yes" ; then
    CFLAGS="$CFLAGS  -g -O0 -Wall -Werror -Wno-unused-but-set-variable -pg"
    CXXFLAGS="$CXXFLAGS $CFLAGS"
fi

# Minimum checks for a C program :)
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_EGREP
AC_LANG(C)

#######################################################################
# Check for other REQUIRED libraries
AC_CHECK_LIB([pthread], [pthread_create], [], [
    AC_MSG_ERROR([ You need to have libpthread installed to compile sngrep.])
])

AC_CHECK_LIB([pcap], [pcap_open_offline], [], [
    AC_MSG_ERROR([ You need to have libpcap installed to compile sngrep.])
])
AC_CHECK_HEADER([pcap.h], [], [
    AC_MSG_ERROR([ You need to have libpcap development files installed to compile sngrep.])
])

####
#### Ncurses Wide character support
####
AC_ARG_ENABLE([unicode],
	AC_HELP_STRING([--enable-unicode], [Enable Ncurses Unicode support]),
	[AC_SUBST(UNICODE, $enableval)],
	[AC_SUBST(UNICODE, no)]
)

AS_IF([test "x$enable_unicode" == "xyes"], [
	# Ncurses with wide-character support
	AC_DEFINE([WITH_UNICODE], [], [Compile With Unicode compatibility])

	AC_CHECK_HEADER([ncursesw/ncurses.h], [], [
	    AC_MSG_ERROR([ You need to have ncurses development files installed to compile sngrep.])
	])

	AC_CHECK_LIB([ncursesw], [initscr], [], [
	    AC_MSG_ERROR([ You need to have libncursesw installed to compile sngrep.])
	])

	AC_CHECK_LIB([panelw], [new_panel], [], [
	    AC_MSG_ERROR([ You need to have ncurses panelw library installed to compile sngrep.])
	])

	AC_CHECK_LIB([formw], [new_form], [], [
	    AC_MSG_ERROR([ You need to have ncurses formsw library installed to compile sngrep.])
	])

	AC_CHECK_LIB([menuw], [new_item], [], [
	    AC_MSG_ERROR([ You need to have ncurses menuw library installed to compile sngrep.])
	])
	], [

	# Ncurses without wide-character support
	AC_CHECK_HEADER([ncurses.h], [], [
	    AC_MSG_ERROR([ You need to have ncurses development files installed to compile sngrep.])
	])

	AC_CHECK_LIB([ncurses], [initscr], [], [
	    AC_MSG_ERROR([ You need to have libncurses installed to compile sngrep.])
	])

	AC_CHECK_LIB([panel], [new_panel], [], [
	    AC_MSG_ERROR([ You need to have ncurses panel library installed to compile sngrep.])
	])

	AC_CHECK_LIB([form], [new_form], [], [
	    AC_MSG_ERROR([ You need to have ncurses forms library installed to compile sngrep.])
	])

	AC_CHECK_LIB([menu], [new_item], [], [
	    AC_MSG_ERROR([ You need to have ncurses menu library installed to compile sngrep.])
	])
])

####
#### OpenSSL Support
####
AC_ARG_WITH([openssl],
    AS_HELP_STRING([--with-openssl], [Enable SSL Support (TLS SIP Transport)]),
    [AC_SUBST(WITH_OPENSSL, $withval)],
    [AC_SUBST(WITH_OPENSSL, no)]
)

# Check for SSL dependencies
AS_IF([test "x$WITH_OPENSSL" == "xno"], [
    AC_ARG_ENABLE([openssl],
        AC_HELP_STRING([--enable-openssl], [Enable SSL Support (TLS SIP Transport)]),
        [AC_SUBST(WITH_OPENSSL, $enableval)],
        [AC_SUBST(WITH_OPENSSL, no)]
    )
], [])

AS_IF([test "x$WITH_OPENSSL" == "xyes"], [
	AC_CHECK_LIB([ssl], [SSL_new], [], [
	    AC_MSG_ERROR([ You need to have libssl installed to compile sngrep])
	])
	
	AC_CHECK_LIB([crypto], [EVP_get_cipherbyname], [], [
	    AC_MSG_ERROR([ You need to have libcrypto installed to compile sngrep])
	])
	AC_DEFINE([WITH_OPENSSL],[],[Compile With Openssl compatibility])
], [])

####
#### PCRE Support
####
AC_ARG_WITH([pcre],
    AS_HELP_STRING([--with-pcre], [Enable Perl compatible regular expressions]),
    [AC_SUBST(WITH_PCRE, $withval)],
    [AC_SUBST(WITH_PCRE, no)]
)

AS_IF([test "x$WITH_PCRE" == "xyes"], [
	AC_CHECK_HEADER([pcre.h], [], [
	    AC_MSG_ERROR([ You need libpcre development files installed to compile with pcre support.])
	])
	AC_CHECK_LIB([pcre], [pcre_exec], [], [
	    AC_MSG_ERROR([ You need libpcre library installed to compile with pcre support.])
	])
	AC_DEFINE([WITH_PCRE],[],[Compile With Perl Compatible regular expressions support])
], [])

# Conditional Source inclusion 
AM_CONDITIONAL([WITH_OPENSSL], [test "x$WITH_OPENSSL" == "xyes"])


######################################################################
# Print Logo
AC_ARG_ENABLE(logo,
    AC_HELP_STRING(--disable-logo, [Disable Irontec Logo from Summary menu]),
    [ enable_logo=$enableval],
    [ enable_logo=yes])

AS_IF([test "x$enable_logo" == "xyes"], [
    echo ''
    echo ' ██╗██████╗  ██████╗ ███╗   ██╗████████╗███████╗ ██████╗'
    echo ' ██║██╔══██╗██╔═══██╗████╗  ██║╚══██╔══╝██╔════╝██╔════╝'
    echo ' ██║██████╔╝██║   ██║██╔██╗ ██║   ██║   █████╗  ██║     '
    echo ' ██║██╔══██╗██║   ██║██║╚██╗██║   ██║   ██╔══╝  ██║     '
    echo ' ██║██║  ██║╚██████╔╝██║ ╚████║   ██║   ███████╗╚██████╗'
    echo ' ╚═╝╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═══╝   ╚═╝   ╚══════╝ ╚═════╝'
    echo ''
])

AC_MSG_NOTICE
AC_MSG_NOTICE(   sngrep configure finished                            	)
AC_MSG_NOTICE( ====================================================== 	)
AC_MSG_NOTICE( OpenSSL Support              : ${WITH_OPENSSL} 			)
AC_MSG_NOTICE( Unicode Support              : ${UNICODE}  				)
AC_MSG_NOTICE( Perl Expressions Support     : ${WITH_PCRE}              )
AC_MSG_NOTICE( ====================================================== 	)
AC_MSG_NOTICE


AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([config/Makefile])
AC_CONFIG_FILES([doc/Makefile])
AC_OUTPUT
