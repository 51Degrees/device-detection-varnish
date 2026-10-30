ACLOCAL_AMFLAGS = -I m4 -I ${LIBVARNISHAPI_DATAROOTDIR}/aclocal

SUBDIRS = src/dd src/ipi

DISTCHECK_CONFIGURE_FLAGS = \
	--with-config=test \
	--with-datafile=@FIFTYONEDEGREES_DATAFILE_ABSOLUTE@ \
	--with-datafile-ipi=@FIFTYONEDEGREES_DATAFILE_IPI_ABSOLUTE@ \
	VMOD_DIR='$${libdir}/varnish/vmods'

EXTRA_DIST = README.md LICENSE examples tests

doc_DATA = README.md LICENSE

dist_man_MANS = vmod_fiftyonedegrees.4 vmod_fiftyonedegrees_ipi.4
MAINTAINERCLEANFILES = $(dist_man_MANS)

# A build outside the source folder writes its own copy of the man page.
distclean-local:
	if test "$(srcdir)" != "."; then rm -f $(dist_man_MANS); fi

vmod_fiftyonedegrees.4: src/dd/vmod_fiftyonedegrees.man.rst
vmod_fiftyonedegrees_ipi.4: src/ipi/vmod_fiftyonedegrees_ipi.man.rst

%.1 %.2 %.3 %.4 %.5 %.6 %.7 %.8 %.9:
if HAVE_RST2MAN
	${RST2MAN} $< $@
else
	@echo "========================================"
	@echo "You need rst2man installed to make dist"
	@echo "========================================"
	@false
endif

datafile = @FIFTYONEDEGREES_DATAFILE_ABSOLUTE@
datafile_ipi = @FIFTYONEDEGREES_DATAFILE_IPI_ABSOLUTE@

EXAMPLE_TESTS = $(top_srcdir)/tests/examples/*.vtc
.PHONY: $(EXAMPLE_TESTS)

$(top_srcdir)/tests/examples/*.vtc:
	@VARNISHTEST@ -Dvarnishd=@VARNISHD@ -Dvmod_ipi_datafile=$(datafile_ipi) -Dvmod_fod_datafile=$(datafile) -Dvmod_topbuild=$(abs_top_builddir) -Dvmod_topsrc=$(abs_top_srcdir) $@ \
	|| EC=$$?; if [ "$$EC" != "77" ] ; then (exit $$EC) ; fi;

# Runs each example, so what the examples show stays true.
check-local: $(EXAMPLE_TESTS)

# Builds both modules with the address sanitizer and runs the tests. varnishd
# is not built with it, so its library has to be loaded first. Leaks are not
# checked, as varnishd does not free what it holds when it exits, and the two
# modules define the same names on purpose.
mem-check:
	$(MAKE) clean
	$(MAKE) CFLAGS="$(CFLAGS) -fsanitize=address -fno-omit-frame-pointer -O1" LDFLAGS="$(LDFLAGS) -fsanitize=address"
	LD_PRELOAD=`$(CC) -print-file-name=libasan.so` ASAN_OPTIONS=detect_leaks=0:detect_odr_violation=0 $(MAKE) -k check
