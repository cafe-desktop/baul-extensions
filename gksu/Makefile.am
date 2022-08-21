AM_CFLAGS = -g -O2 -Wall
AM_CPPFLAGS = -DLOCALEDIR=\"$(datadir)/locale\" -DDATA_DIR=\"$(datadir)\" -DPREFIX=\"$(prefix)\" $(BAUL_CFLAGS)

baul_extensiondir=$(libdir)/baul/extensions-2.0
baul_extension_LTLIBRARIES = libbaul-gksu.la
libbaul_gksu_la_SOURCES = libbaul-gksu.c
libbaul_gksu_la_LDFLAGS = -module -avoid-version -no-undefined
libbaul_gksu_la_LIBADD  = $(BAUL_LIBS)

extensiondir = $(datadir)/baul/extensions
extension_in_files = libbaul-gksu.baul-extension.desktop.in
extension_DATA = $(extension_in_files:.baul-extension.desktop.in=.baul-extension)
$(extension_DATA): $(extension_in_files)
	$(AM_V_GEN) $(MSGFMT) --desktop --keyword= --keyword=Copyright --keyword=Name --keyword=Description --template $< -d $(top_srcdir)/po -o $@

CLEANFILES = $(extension_DATA)
