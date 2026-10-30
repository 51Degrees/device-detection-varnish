ACLOCAL_AMFLAGS = -I m4 -I ${LIBVARNISHAPI_DATAROOTDIR}/aclocal

SUBDIRS = src/dd src/ipi

DISTCHECK_CONFIGURE_FLAGS = \
	--with-config=test \
	--with-datafile=@FIFTYONEDEGREES_DATAFILE_ABSOLUTE@ \
	--with-datafile-ipi=@FIFTYONEDEGREES_DATAFILE_IPI_ABSOLUTE@ \
	VMOD_DIR='$${libdir}/varnish/vmods'

EXTRA_DIST = README.md LICENSE examples

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

