AM_CPPFLAGS =						\
	-DG_LOG_DOMAIN=\"Caja-Open-Terminal\"	\
	-DDATADIR=\"$(datadir)\"			\
	-DCAFELOCALEDIR=\""$(datadir)/locale"\" 	\
	-I$(top_srcdir)					\
	-I$(top_builddir)				\
	$(WARN_CFLAGS)                                  \
	$(DISABLE_DEPRECATED_CFLAGS)			\
	$(CAJA_CFLAGS)				\
	$(OPENTERMINAL_CFLAGS)

baul_extensiondir=$(CAJA_EXTENSION_DIR)

baul_extension_LTLIBRARIES=libbaul-open-terminal.la

libbaul_open_terminal_la_SOURCES = \
	baul-open-terminal.c       \
	baul-open-terminal.h       \
	open-terminal.c

libbaul_open_terminal_la_LDFLAGS = -module -avoid-version
libbaul_open_terminal_la_LIBADD  = $(CAJA_LIBS) $(OPENTERMINAL_LIBS)

gsettings_schemas_in_files = org.cafe.baul-open-terminal.gschema.xml.in
gsettings_SCHEMAS = $(gsettings_schemas_in_files:.xml.in=.xml)
@GSETTINGS_RULES@

extensiondir = $(datadir)/baul/extensions
extension_in_files = libbaul-open-terminal.baul-extension.desktop.in
extension_DATA = $(extension_in_files:.baul-extension.desktop.in=.baul-extension)
$(extension_DATA): $(extension_in_files)
	$(AM_V_GEN) $(MSGFMT) --desktop --keyword= --keyword=Copyright --keyword=Name --keyword=Description --template $< -d $(top_srcdir)/po -o $@

DISTCLEANFILES = \
	org.cafe.baul-open-terminal.gschema.xml

CLEANFILES = \
	$(extension_DATA) \
	$(gsettings_SCHEMAS) \
	*.gschema.valid

EXTRA_DIST = \
	$(gsettings_schemas_in_files)
