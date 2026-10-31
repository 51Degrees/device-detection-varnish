AM_CPPFLAGS = -Wall -Wno-unused-variable -Wno-missing-braces -Wno-implicit-function-declaration -Wno-strict-aliasing -Werror

vmoddir = @VMOD_DIR@
vmod_LTLIBRARIES = libvmod_fiftyonedegrees.la

libvmod_fiftyonedegrees_la_LDFLAGS = -module -export-dynamic -avoid-version -shared -latomic
libvmod_fiftyonedegrees_la_SOURCES = \
	vmod_fiftyonedegrees.c \
	../../cxx/src/dd/dataset-dd.c \
	../../cxx/src/dd/gethighentropyvalues.c \
	../../cxx/src/dd/results-dd.c \
	../../cxx/src/dd/transformc.c \
	../../cxx/src/dd/hash/hash.c \
	../../cxx/src/dd/hash/graph.c \
	../../cxx/src/common/bool.c \
	../../cxx/src/common/cache.c \
	../../cxx/src/common/collection.c \
	../../cxx/src/common/collectionKeyTypes.c \
	../../cxx/src/common/component.c \
	../../cxx/src/common/data.c \
	../../cxx/src/common/dataset.c \
	../../cxx/src/common/evidence.c \
	../../cxx/src/common/exceptionsc.c \
	../../cxx/src/common/file.c \
	../../cxx/src/common/float.c \
	../../cxx/src/common/headers.c \
	../../cxx/src/common/indices.c \
	../../cxx/src/common/ip.c \
	../../cxx/src/common/json.c \
	../../cxx/src/common/list.c \
	../../cxx/src/common/memory.c \
	../../cxx/src/common/overrides.c \
	../../cxx/src/common/pool.c \
	../../cxx/src/common/process.c \
	../../cxx/src/common/profile.c \
	../../cxx/src/common/properties.c \
	../../cxx/src/common/property.c \
	../../cxx/src/common/propertyValueType.c \
	../../cxx/src/common/resource.c \
	../../cxx/src/common/results.c \
	../../cxx/src/common/status.c \
	../../cxx/src/common/storedBinaryValue.c \
	../../cxx/src/common/string.c \
	../../cxx/src/common/stringBuilder.c \
	../../cxx/src/common/textfile.c \
	../../cxx/src/common/threading.c \
	../../cxx/src/common/tree.c \
	../../cxx/src/common/value.c \
	../../cxx/src/common/weightedItem.c \
	../../cxx/src/common/wkbtot.c \
	../../cxx/src/common/yamlfile.c

datafile = $(or $(51DEGREES_DD_PATH),@FIFTYONEDEGREES_DATAFILE_ABSOLUTE@)

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

vcc_if.h: @VMODTOOL@ $(builddir)/vmod_fiftyonedegrees.vcc
	@VMODTOOL@ $(builddir)/vmod_fiftyonedegrees.vcc

VMOD_TESTS = $(top_srcdir)/src/dd/tests/*.vtc
.PHONY: $(VMOD_TESTS)

$(top_srcdir)/src/dd/tests/*.vtc: libvmod_fiftyonedegrees.la
	@VARNISHTEST@ -Dvarnishd=@VARNISHD@ -Dvmod_fod_datafile=$(datafile) -Dvmod_topbuild=$(abs_top_builddir) $@ $(testargs) \
	|| EC=$$?; if [ "$$EC" != "77" ] ; then (exit $$EC) ; fi;

check: $(VMOD_TESTS)

noinst_HEADERS = \
	vmod_fiftyonedegrees_interface.h \
	../vmod_fiftyonedegrees_varnish.h \
	../../cxx/src/common/array.h \
	../../cxx/src/common/bool.h \
	../../cxx/src/common/cache.h \
	../../cxx/src/common/collection.h \
	../../cxx/src/common/collectionKey.h \
	../../cxx/src/common/collectionKeyTypes.h \
	../../cxx/src/common/common.h \
	../../cxx/src/common/component.h \
	../../cxx/src/common/config.h \
	../../cxx/src/common/constants.h \
	../../cxx/src/common/data.h \
	../../cxx/src/common/dataset.h \
	../../cxx/src/common/date.h \
	../../cxx/src/common/evidence.h \
	../../cxx/src/common/exceptions.h \
	../../cxx/src/common/fiftyone.h \
	../../cxx/src/common/file.h \
	../../cxx/src/common/fileOffset.h \
	../../cxx/src/common/float.h \
	../../cxx/src/common/headers.h \
	../../cxx/src/common/indices.h \
	../../cxx/src/common/ip.h \
	../../cxx/src/common/json.h \
	../../cxx/src/common/list.h \
	../../cxx/src/common/memory.h \
	../../cxx/src/common/overrides.h \
	../../cxx/src/common/pair.h \
	../../cxx/src/common/pool.h \
	../../cxx/src/common/process.h \
	../../cxx/src/common/profile.h \
	../../cxx/src/common/properties.h \
	../../cxx/src/common/property.h \
	../../cxx/src/common/propertyValueType.h \
	../../cxx/src/common/resource.h \
	../../cxx/src/common/results.h \
	../../cxx/src/common/snprintf.h \
	../../cxx/src/common/status.h \
	../../cxx/src/common/storedBinaryValue.h \
	../../cxx/src/common/string.h \
	../../cxx/src/common/stringBuilder.h \
	../../cxx/src/common/textfile.h \
	../../cxx/src/common/threading.h \
	../../cxx/src/common/tree.h \
	../../cxx/src/common/value.h \
	../../cxx/src/common/weightedItem.h \
	../../cxx/src/common/wkbtot.h \
	../../cxx/src/common/yamlfile.h \
	../../cxx/src/dd/config-dd.h \
	../../cxx/src/dd/dataset-dd.h \
	../../cxx/src/dd/fiftyone.h \
	../../cxx/src/dd/gethighentropyvalues.h \
	../../cxx/src/dd/results-dd.h \
	../../cxx/src/dd/transform.h \
	../../cxx/src/dd/hash/fiftyone.h \
	../../cxx/src/dd/hash/graph.h \
	../../cxx/src/dd/hash/hash.h

EXTRA_DIST = \
	vmod_fiftyonedegrees.vcc \
	vmod_fiftyonedegrees.vcc.template \
	vmod_fiftyonedegrees.vcc.template.test \
	vmod_fiftyonedegrees_extra.h.template \
	tests

CLEANFILES = \
	$(builddir)/vcc_if.c \
	$(builddir)/vcc_if.h \
	$(builddir)/vmod_fiftyonedegrees.rst \
	$(builddir)/vmod_fiftyonedegrees.man.rst \
	$(builddir)/vmod_vcs_version.txt

# configure writes these. In a source checkout they are left in place, as the
# .vcc file is tracked.
distclean-local:
	if test "$(srcdir)" != "."; then \
		rm -f vmod_fiftyonedegrees.vcc vmod_fiftyonedegrees_extra.h; \
	fi
