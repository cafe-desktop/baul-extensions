AM_CPPFLAGS = $(CAJA_CFLAGS)

baul_extensiondir = $(CAJA_EXTENSION_DIR)
baul_extension_LTLIBRARIES = libbaul-wallpaper.la

libbaul_wallpaper_la_SOURCES = \
	baul-wallpaper-extension.c\
	baul-wallpaper-extension.h\
	baul-wallpaper-command.c

libbaul_wallpaper_la_LDFLAGS = -module -avoid-version
libbaul_wallpaper_la_LIBADD = $(CAJA_LIBS)

extensiondir = $(datadir)/baul/extensions
extension_in_files = libbaul-wallpaper.baul-extension.desktop.in
extension_DATA = $(extension_in_files:.baul-extension.desktop.in=.baul-extension)
$(extension_DATA): $(extension_in_files)
	$(AM_V_GEN) $(MSGFMT) --desktop --keyword= --keyword=Copyright --keyword=Name --keyword=Description --template $< -d $(top_srcdir)/po -o $@

CLEANFILES = $(extension_DATA)
