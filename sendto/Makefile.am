NULL=

SUBDIRS = . docs plugins

AM_CPPFLAGS =					\
	-DDATADIR=\"$(datadir)\"		\
	-DPLUGINDIR=\"$(libdir)/baul-sendto/plugins\"	\
	-I$(top_srcdir)				\
	-I$(top_builddir)			\
	-DLOCALEDIR="\"$(datadir)/locale\""	\
	$(SENDTO_CFLAGS)		\
	$(CAJA_CFLAGS)		\
	$(DISABLE_DEPRECATED)			\
	$(WARN_CFLAGS)

baul_sendto_includedir = $(includedir)/baul-sendto/
baul_sendto_include_HEADERS = baul-sendto-plugin.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = baul-sendto.pc

noinst_LTLIBRARIES = libnstplugin.la
libnstplugin_la_SOURCES =			\
	baul-sendto-plugin.h		\
	nst-enum-types.c			\
	nst-enum-types.h
libnstplugin_la_LIBADD = $(SENDTO_LIBS)

bin_PROGRAMS = baul-sendto

resources_built_sources = baul-sendto-resources.h baul-sendto-resources.c
$(resources_built_sources): org.cafe.baul.extensions.sendto.gresource.xml Makefile $(shell $(GLIB_COMPILE_RESOURCES) --generate-dependencies --sourcedir $(srcdir) $(srcdir)/org.cafe.baul.extensions.sendto.gresource.xml)
	$(AM_V_GEN) XMLLINT=$(XMLLINT) $(GLIB_COMPILE_RESOURCES) --target $@ --sourcedir $(srcdir) --generate --c-name sendto $<

baul_sendto_SOURCES = baul-sendto-command.c $(resources_built_sources)
baul_sendto_LDADD = $(SENDTO_LIBS) libnstplugin.la

baul_extensiondir = $(CAJA_EXTENSION_DIR)
baul_extension_LTLIBRARIES = libbaul-sendto.la
libbaul_sendto_la_SOURCES =		\
	baul-nste.c			\
	baul-nste.h			\
	baul-sendto-module.c

libbaul_sendto_la_LDFLAGS = -module -avoid-version -no-undefined
libbaul_sendto_la_LIBADD  = $(CAJA_LIBS)

nst_headers_to_scan_for_enums = baul-sendto-plugin.h
# Generate the enums source code, with glib-mkenums:
# This is based on the same Makefile.am stuff in pango:
nst_built_headers = nst-enum-types.h
nst_built_cfiles = nst-enum-types.c

nst-enum-types.h: $(nst_headers_to_scan_for_enums) Makefile
	$(AM_V_GEN) (cd $(srcdir) && glib-mkenums \
			--fhead "#ifndef __NST_ENUM_TYPES_H__\n#define __NST_ENUM_TYPES_H__\n\n#include <glib-object.h>\n\nG_BEGIN_DECLS\n" \
			--fprod "/* enumerations from \"@filename@\" */\n" \
			--vhead "GType @enum_name@_get_type (void);\n#define NST_TYPE_@ENUMSHORT@ (@enum_name@_get_type())\n" 	\
			--ftail "G_END_DECLS\n\n#endif /* __NST_ENUM_TYPES_H__ */" \
		$(nst_headers_to_scan_for_enums)) > $@

nst-enum-types.c: $(nst_headers_to_scan_for_enums) Makefile nst-enum-types.h
	$(AM_V_GEN) (cd $(srcdir) && glib-mkenums \
			--fhead "#include <baul-sendto-plugin.h>\n" \
			--fhead "#include \"nst-enum-types.h\"\n" \
			--fhead "#include <glib-object.h>" \
			--fprod "\n/* enumerations from \"@filename@\" */" \
			--vhead "GType\n@enum_name@_get_type (void)\n{\n  static GType etype = 0;\n  if (etype == 0) {\n    static const G@Type@Value values[] = {" 	\
			--vprod "      { @VALUENAME@, \"@VALUENAME@\", \"@valuenick@\" }," \
			--vtail "      { 0, NULL, NULL }\n    };\n    etype = g_@type@_register_static (\"@EnumName@\", values);\n  }\n  return etype;\n}\n" \
		$(nst_headers_to_scan_for_enums)) > $@

gsettingsschema_in_files = org.cafe.Baul.Sendto.gschema.xml.in
gsettings_SCHEMAS = $(gsettingsschema_in_files:.xml.in=.xml)
@GSETTINGS_RULES@

man_MANS = baul-sendto.1

extensiondir = $(datadir)/baul/extensions
extension_in_files = libbaul-sendto.baul-extension.desktop.in
extension_DATA = $(extension_in_files:.baul-extension.desktop.in=.baul-extension)
$(extension_DATA): $(extension_in_files)
	$(AM_V_GEN) $(MSGFMT) --desktop --keyword= --keyword=Copyright --keyword=Name --keyword=Description --template $< -d $(top_srcdir)/po -o $@

EXTRA_DIST = \
	$(man_MANS) \
	baul-sendto.pc.in \
	baul-sendto.ui \
	$(gsettingsschema_in_files) \
	org.cafe.baul.extensions.sendto.gresource.xml \
	$(NULL)

CLEANFILES = \
	$(nst_built_headers) \
	$(nst_built_cfiles) \
	$(gsettings_SCHEMAS) \
	$(extension_DATA) \
	*.gschema.valid \
	$(resources_built_sources) \
	$(NULL)
