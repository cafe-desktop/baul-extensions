interfacesdir = $(datadir)/baul-extensions
interfaces_DATA = share-dialog.ui

libbaul_share_la_CFLAGS =				\
	-Wall						\
	-DG_LOG_DOMAIN=\"Baul-Share\"		\
	-DINTERFACES_DIR=\"$(interfacesdir)\"		\
	-DDATADIR=\"$(datadir)\"			\
	-DCAFELOCALEDIR=\""$(datadir)/locale"\" 	\
	-I$(top_srcdir)					\
	-I$(top_builddir)				\
	$(WARN_CFLAGS)                                  \
	$(DISABLE_DEPRECATED_CFLAGS)			\
	$(BAUL_CFLAGS)				\
	$(SHARE_CFLAGS)

baul_extensiondir=$(BAUL_EXTENSION_DIR)
baul_extension_LTLIBRARIES=libbaul-share.la

libbaul_share_la_SOURCES =  \
	baul-share.c        \
	baul-share.h	\
	shares.c		\
	shares.h

libbaul_share_la_LDFLAGS = -module -avoid-version
libbaul_share_la_LIBADD  = $(SHARE_LIBS) $(BAUL_LIBS)

extensiondir = $(datadir)/baul/extensions
extension_in_files = libbaul-share.baul-extension.desktop.in
extension_DATA = $(extension_in_files:.baul-extension.desktop.in=.baul-extension)
$(extension_DATA): $(extension_in_files)
	$(AM_V_GEN) $(MSGFMT) --desktop --keyword= --keyword=Copyright --keyword=Name --keyword=Description --template $< -d $(top_srcdir)/po -o $@

EXTRA_DIST = share-dialog.ui

CLEANFILES = $(extension_DATA)
