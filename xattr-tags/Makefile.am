AM_CPPFLAGS = $(BAUL_CFLAGS)

baul_extensiondir = $(BAUL_EXTENSION_DIR)
baul_extension_LTLIBRARIES = libbaul-xattr-tags.la

libbaul_xattr_tags_la_SOURCES = \
	baul-xattr-tags-extension.c \
	baul-xattr-tags-extension.h

libbaul_xattr_tags_la_LDFLAGS = -module -avoid-version
libbaul_xattr_tags_la_LIBADD = $(BAUL_LIBS)

extensiondir = $(datadir)/baul/extensions
extension_in_files = libbaul-xattr-tags.baul-extension.desktop.in
extension_DATA = $(extension_in_files:.baul-extension.desktop.in=.baul-extension)
$(extension_DATA): $(extension_in_files)
	$(AM_V_GEN) $(MSGFMT) --desktop --keyword= --keyword=Copyright --keyword=Name --keyword=Description --template $< -d $(top_srcdir)/po -o $@

CLEANFILES = $(extension_DATA)
