AM_CPPFLAGS = -Wall -Wno-unused-variable -Wno-missing-braces -Wno-implicit-function-declaration -Wno-strict-aliasing -Werror

vmoddir = @VMOD_DIR@
vmod_LTLIBRARIES = libvmod_fiftyonedegrees.la

libvmod_fiftyonedegrees_la_LDFLAGS = -module -export-dynamic -avoid-version -shared -latomic
libvmod_fiftyonedegrees_la_SOURCES = \
	vmod_fiftyonedegrees.c \
	../cxx/src/dd/dataset-dd.c \
	../cxx/src/dd/gethighentropyvalues.c \
	../cxx/src/dd/results-dd.c \
	../cxx/src/dd/transformc.c \
	../cxx/src/dd/hash/hash.c \
	../cxx/src/dd/hash/graph.c \
	../cxx/src/common/bool.c \
	../cxx/src/common/cache.c \
	../cxx/src/common/collection.c \
	../cxx/src/common/collectionKeyTypes.c \
	../cxx/src/common/component.c \
	../cxx/src/common/data.c \
	../cxx/src/common/dataset.c \
	../cxx/src/common/evidence.c \
	../cxx/src/common/exceptionsc.c \
	../cxx/src/common/file.c \
	../cxx/src/common/float.c \
	../cxx/src/common/headers.c \
	../cxx/src/common/indices.c \
	../cxx/src/common/ip.c \
	../cxx/src/common/json.c \
	../cxx/src/common/list.c \
	../cxx/src/common/memory.c \
	../cxx/src/common/overrides.c \
	../cxx/src/common/pool.c \
	../cxx/src/common/process.c \
	../cxx/src/common/profile.c \
	../cxx/src/common/properties.c \
	../cxx/src/common/property.c \
	../cxx/src/common/propertyValueType.c \
	../cxx/src/common/resource.c \
	../cxx/src/common/results.c \
	../cxx/src/common/status.c \
	../cxx/src/common/storedBinaryValue.c \
	../cxx/src/common/string.c \
	../cxx/src/common/stringBuilder.c \
	../cxx/src/common/textfile.c \
	../cxx/src/common/threading.c \
	../cxx/src/common/tree.c \
	../cxx/src/common/value.c \
	../cxx/src/common/weightedItem.c \
	../cxx/src/common/wkbtot.c \
	../cxx/src/common/yamlfile.c

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
