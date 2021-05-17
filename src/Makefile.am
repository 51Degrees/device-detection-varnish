AM_CPPFLAGS = -Wall -Wno-unused-variable -Wno-missing-braces -Wno-implicit-function-declaration -Wno-strict-aliasing -Werror

vmoddir = @VMOD_DIR@
vmod_LTLIBRARIES = libvmod_fiftyonedegrees.la

libvmod_fiftyonedegrees_la_LDFLAGS = -module -export-dynamic -avoid-version -shared
libvmod_fiftyonedegrees_la_SOURCES = \
	vmod_fiftyonedegrees.c \
	device-detection-cxx/src/dataset-dd.c \
	device-detection-cxx/src/results-dd.c \
	device-detection-cxx/src/hash/hash.c \
	device-detection-cxx/src/hash/graph.c \
	device-detection-cxx/src/common-cxx/cache.c \
	device-detection-cxx/src/common-cxx/collection.c \
	device-detection-cxx/src/common-cxx/component.c \
	device-detection-cxx/src/common-cxx/coordinate.c \
	device-detection-cxx/src/common-cxx/data.c \
	device-detection-cxx/src/common-cxx/dataset.c \
	device-detection-cxx/src/common-cxx/evidence.c \
	device-detection-cxx/src/common-cxx/exceptionsc.c \
	device-detection-cxx/src/common-cxx/file.c \
	device-detection-cxx/src/common-cxx/float.c \
	device-detection-cxx/src/common-cxx/headers.c \
	device-detection-cxx/src/common-cxx/ip.c \
	device-detection-cxx/src/common-cxx/list.c \
	device-detection-cxx/src/common-cxx/memory.c \
	device-detection-cxx/src/common-cxx/overrides.c \
	device-detection-cxx/src/common-cxx/pool.c \
	device-detection-cxx/src/common-cxx/profile.c \
	device-detection-cxx/src/common-cxx/properties.c \
	device-detection-cxx/src/common-cxx/property.c \
	device-detection-cxx/src/common-cxx/pseudoheader.c \
	device-detection-cxx/src/common-cxx/resource.c \
	device-detection-cxx/src/common-cxx/results.c \
	device-detection-cxx/src/common-cxx/status.c \
	device-detection-cxx/src/common-cxx/string.c \
	device-detection-cxx/src/common-cxx/textfile.c \
	device-detection-cxx/src/common-cxx/threading.c \
	device-detection-cxx/src/common-cxx/tree.c \
	device-detection-cxx/src/common-cxx/value.c

if FIFTYONEDEGREES_DATAFILE_ABS
datafile = @FIFTYONEDEGREES_DATAFILE@
else
datafile = $(abs_top_builddir)/@FIFTYONEDEGREES_DATAFILE@
endif

testargs = 

if FIFTYONEDEGREES_TEST
libvmod_fiftyonedegrees_la_SOURCES += vmod_fiftyonedegrees_test.c
testargs += -DTEST_BUILD=true
else
testargs += -DTEST_BUILD=false
endif


nodist_libvmod_fiftyonedegrees_la_SOURCES = \
	vcc_if.c \
	vcc_if.h

vmod_fiftyonedegrees.lo: vcc_if.c vcc_if.h

vcc_if.c: vcc_if.h

vcc_if.h: @VMODTOOL@ $(top_srcdir)/src/vmod_fiftyonedegrees.vcc
	@VMODTOOL@ $(top_srcdir)/src/vmod_fiftyonedegrees.vcc

VMOD_TESTS = $(top_srcdir)/src/tests/*.vtc
.PHONY: $(VMOD_TESTS)

$(top_srcdir)/src/tests/*.vtc: libvmod_fiftyonedegrees.la
	@VARNISHTEST@ -Dvarnishd=@VARNISHD@ -Dvmod_fod_datafile=$(datafile) -Dvmod_topbuild=$(abs_top_builddir) $@ $(testargs) \
	|| EC=$$?; if [ "$$EC" != "77" ] ; then (exit $$EC) ; fi;

check: $(VMOD_TESTS)

EXTRA_DIST = \
	vmod_fiftyonedegrees.vcc \
	$(VMOD_TESTS)

CLEANFILES = \
	$(builddir)/vcc_if.c \
	$(builddir)/vcc_if.h \
	$(builddir)/vmod_fiftyonedegrees.rst \
	$(builddir)/vmod_fiftyonedegrees.man.rst
