AM_CPPFLAGS = -Wall -Wno-unused-variable -Wno-missing-braces -Wno-implicit-function-declaration -Wno-strict-aliasing -Werror

vmoddir = @VMOD_DIR@
vmod_LTLIBRARIES = libvmod_fiftyonedegrees_ipi.la

# The IP Intelligence data file needs differently sized structures, so this
# module compiles its own copy of the common code. A target's own flags replace
# AM_CPPFLAGS and give each of its objects a name of its own.
libvmod_fiftyonedegrees_ipi_la_CPPFLAGS = \
	$(AM_CPPFLAGS) \
	-DFIFTYONE_DEGREES_LARGE_DATA_FILE_SUPPORT \
	-DFIFTYONE_DEGREES_REDUCED_FILE \
	-D_FILE_OFFSET_BITS=64

# The two modules share function names, so each one must call its own.
libvmod_fiftyonedegrees_ipi_la_LDFLAGS = -module -export-dynamic -avoid-version -shared -latomic -Wl,-Bsymbolic
libvmod_fiftyonedegrees_ipi_la_SOURCES = \
	vmod_fiftyonedegrees_ipi.c \
	../../cxx/src/ipi/ipi.c \
	../../cxx/src/ipi/ipi_weighted_results.c \
	../../cxx/src/ipi/graph/graph.c \
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

datafile = @FIFTYONEDEGREES_DATAFILE_ABSOLUTE@
datafile_ipi = @FIFTYONEDEGREES_DATAFILE_IPI_ABSOLUTE@

nodist_libvmod_fiftyonedegrees_ipi_la_SOURCES = \
	vcc_if.c \
	vcc_if.h

$(libvmod_fiftyonedegrees_ipi_la_OBJECTS): vcc_if.h

vcc_if.c: vcc_if.h

vcc_if.h: @VMODTOOL@ $(srcdir)/vmod_fiftyonedegrees_ipi.vcc
	@VMODTOOL@ $(srcdir)/vmod_fiftyonedegrees_ipi.vcc

VMOD_TESTS = $(top_srcdir)/src/ipi/tests/*.vtc
.PHONY: $(VMOD_TESTS)

$(top_srcdir)/src/ipi/tests/*.vtc: libvmod_fiftyonedegrees_ipi.la
	@VARNISHTEST@ -Dvarnishd=@VARNISHD@ -Dvmod_ipi_datafile=$(datafile_ipi) -Dvmod_fod_datafile=$(datafile) -Dvmod_topbuild=$(abs_top_builddir) -Dvmod_topsrc=$(abs_top_srcdir) $@ \
	|| EC=$$?; if [ "$$EC" != "77" ] ; then (exit $$EC) ; fi;

check: $(VMOD_TESTS)

noinst_HEADERS = \
	../../cxx/src/ipi/constantsIpi.h \
	../../cxx/src/ipi/fiftyone.h \
	../../cxx/src/ipi/ipi.h \
	../../cxx/src/ipi/ipi_weighted_results.h \
	../../cxx/src/ipi/graph/graph.h

EXTRA_DIST = \
	vmod_fiftyonedegrees_ipi.vcc \
	tests

CLEANFILES = \
	$(builddir)/vcc_if.c \
	$(builddir)/vcc_if.h \
	$(builddir)/vmod_fiftyonedegrees_ipi.rst \
	$(builddir)/vmod_fiftyonedegrees_ipi.man.rst \
	$(builddir)/vmod_vcs_version.txt
