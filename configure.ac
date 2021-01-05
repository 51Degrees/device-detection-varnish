AC_PREREQ(2.59)
AC_INIT([libvmod-fiftyonedegrees], [trunk])
AC_CONFIG_MACRO_DIR([m4])
AM_CONFIG_HEADER(config.h)

AC_CANONICAL_SYSTEM
AC_LANG(C)

AM_INIT_AUTOMAKE([foreign subdir-objects])

AC_GNU_SOURCE
AC_PROG_CC
AC_PROG_CC_STDC
if test "x$ac_cv_prog_cc_c99" = xno; then
	AC_MSG_ERROR([Could not find a C99 compatible compiler])
fi
AC_PROG_CPP

LT_INIT([disable-static])
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_MAKE_SET

AC_PROG_EGREP

# Check for rst utilities
AC_CHECK_PROGS(RST2MAN, [rst2man rst2man.py], "no")
if test "x$RST2MAN" = "xno"; then
	AC_MSG_WARN([rst2man not found - not building man pages])
fi
AM_CONDITIONAL(HAVE_RST2MAN, [test "x$RST2MAN" != "xno"])

# Check for python
AC_CHECK_PROGS(PYTHON,
	[python3 python3.1 python3.2 python2.7 python2.6 python2.5 python2 python],
	[AC_MSG_ERROR([Python is needed to build this vmod, please install python.])])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([sys/stdlib.h])

# backwards compat with older pkg-config
# - pull in AC_DEFUN from pkg.m4
m4_ifndef([PKG_CHECK_VAR], [
# PKG_CHECK_VAR(VARIABLE, MODULE, CONFIG-VARIABLE,
# [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# -------------------------------------------
# Retrieves the value of the pkg-config variable for the given module.
AC_DEFUN([PKG_CHECK_VAR],
[AC_REQUIRE([PKG_PROG_PKG_CONFIG])dnl
AC_ARG_VAR([$1], [value of $3 for $2, overriding pkg-config])dnl

_PKG_CONFIG([$1], [variable="][$3]["], [$2])
AS_VAR_COPY([$1], [pkg_cv_][$1])

AS_VAR_IF([$1], [""], [$5], [$4])dnl
])# PKG_CHECK_VAR
])

# Check for Varnish directories
PKG_CHECK_VAR([LIBVARNISHAPI_DATAROOTDIR], [varnishapi], [datarootdir])
PKG_CHECK_VAR([LIBVARNISHAPI_BINDIR], [varnishapi], [bindir])
PKG_CHECK_VAR([LIBVARNISHAPI_SBINDIR], [varnishapi], [sbindir])
AC_SUBST([LIBVARNISHAPI_DATAROOTDIR])

AC_ARG_WITH([config],
			AS_HELP_STRING([--with-config=(release|test)], [The version of fiftyonedegrees vmod. This can be either release or test. Setting to test will add extra functions to get values stored internally. By default, release version will be built]),
    [config=$withval],
    [config=release])

if test "x$config" = "xtest"; then
	AC_MSG_NOTICE([Build test version])
else
	AC_MSG_NOTICE([Build release version])
fi

AM_CONDITIONAL([FIFTYONEDEGREES_TEST], [test x$config = xtest])

# Define the version and add the option to redefine in configure.
AC_ARG_WITH([version],
    AS_HELP_STRING([--with-version=version], [The version of Varnish to build the module for.]),
    [varnishversion=$withval],
	[])

if test "x$varnishversion" = x; then
	# Set the version and the source directory using the installed
	# Varnish
	PKG_CHECK_MODULES([libvarnishapi], [varnishapi]);
	AC_PATH_PROG([VARNISHD], [varnishd], [],
    	[$LIBVARNISHAPI_SBINDIR:$LIBVARNISHAPI_BINDIR:$PATH])
	varnishversion=`$VARNISHD -V 2>&1 | tr ' ' '\n' | $EGREP -o '@<:@0-9@:>@+\.@<:@0-9@:>@+\.@<:@0-9@:>@+'`
	if test  ! -d "${VARNISHSRC}"; then
		VARNISHSRC=/usr/local/include/varnish
	fi
else
	# Check for wget
	AC_CHECK_PROGS(WGET,
		[wget],
		[AC_MSG_ERROR([Wget is needed to download Varnish source, please install wget.])])

	# Download the source for the requested version and set the path.
	AC_CHECK_FILES(varnish-cache-varnish-$varnishversion,
		[],
		[`$WGET https://github.com/varnishcache/varnish-cache/archive/varnish-$varnishversion.tar.gz && tar -xzf varnish-$varnishversion.tar.gz`])
	`cd varnish-cache-varnish-$varnishversion && ./autogen.sh && ./configure && make`
	VARNISHSRC=`cd varnish-cache-varnish-$varnishversion && pwd`

	# Check that varnishtest is built in the varnish source directory
	AC_CHECK_FILE([$VARNISHSRC/bin/varnishtest/varnishtest],
		[],
		[AC_MSG_WARN([Can't find "$VARNISHSRC/bin/varnishtest/varnishtest". Currently installed varnishtest will be used by 'make check' (this may fail if the version does not match).])]
	)

	AC_PATH_PROG([VARNISHD], [varnishd], [],
	    [$VARNISHSRC/bin/varnishd:$LIBVARNISHAPI_SBINDIR:$LIBVARNISHAPI_BINDIR:$PATH])
fi

# Varnish include files tree
VARNISH_VMOD_DIR
VARNISH_VMODTOOL

# Get the Varnish executable path
AC_PATH_PROG([VARNISHTEST], [varnishtest], [],
    [$VARNISHSRC/bin/varnishtest:$LIBVARNISHAPI_BINDIR:$LIBVARNISHAPI_SBINDIR:$PATH])

# Get the version in integer form, and set the compile arguments.
parsedvarnishversion=`chmod +x version.py && ./version.py $varnishversion`
CFLAGS+=" -DFIFTYONEDEGREES_VARNISH_VERSION=$parsedvarnishversion -Wno-unused-but-set-variable -Wno-unused-function -Wno-format-security -Wno-uninitialized -I$VARNISHSRC/include -I$VARNISHSRC -I$VARNISHSRC/bin/varnishd"

`cat src/vmod_fiftyonedegrees.vcc.template > src/vmod_fiftyonedegrees.vcc`
if test "x$config" = "xtest"; then
	`cat src/vmod_fiftyonedegrees_extra.h.template | sed 's/OBTAIN_RESULTS_FUNC_NAME/obtainResultsFromAvailableHeadersDecor/g;s/GET_HAS_VALUES_FUNC_NAME/ResultsHashGetHasValuesDecor/g' > src/vmod_fiftyonedegrees_extra.h`
	`cat src/vmod_fiftyonedegrees.vcc.template.test >> src/vmod_fiftyonedegrees.vcc`
else
	`cat src/vmod_fiftyonedegrees_extra.h.template | sed 's/OBTAIN_RESULTS_FUNC_NAME/obtainResultsFromAvailableHeaders/g;s/GET_HAS_VALUES_FUNC_NAME/ResultsHashGetHasValues/g' > src/vmod_fiftyonedegrees_extra.h`
fi
AC_CONFIG_SRCDIR(src/vmod_fiftyonedegrees.vcc)

AC_CONFIG_FILES(
    Makefile
    src/Makefile)

missingFileMsg=["source file not found, this will be copied automatically if src/src is removed."]
missingDataMsg=["data file not found, this will be copied automatically if data is removed."]
AC_CHECK_FILES(src/device-detection-cxx/src/common-cxx/fiftyone.h,
    [],
    [AC_MSG_ERROR($missingFileMsg)])
AC_CHECK_FILES(src/device-detection-cxx/src/dataset-dd.h,
    [],
    [AC_MSG_ERROR($missingFileMsg)])
AC_CHECK_FILES(src/device-detection-cxx/src/config-dd.h,
    [],
    [AC_MSG_ERROR($missingFileMsg)])
AC_CHECK_FILES(src/device-detection-cxx/src/results-dd.h,
    [],
    [AC_MSG_ERROR($missingFileMsg)])
AC_CHECK_FILES(src/device-detection-cxx/device-detection-data/51Degrees-LiteV4.1.hash,
    [],
    [AC_MSG_WARN($missingDataMsg)])


AC_OUTPUT

