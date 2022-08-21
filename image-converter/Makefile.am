AM_CPPFLAGS =						\
	-DG_LOG_DOMAIN=\"Baul-Image-Converter\"	\
	-DDATADIR=\"$(datadir)\"			\
	-DCAFELOCALEDIR=\""$(datadir)/locale"\" 	\
	-I$(top_srcdir)					\
	-I$(top_builddir)				\
	$(WARN_CFLAGS)                                  \
	$(DISABLE_DEPRECATED_CFLAGS)			\
	$(CAJA_CFLAGS) \
	$(IMAGE_CONVERTER_CFLAGS)

baul_extensiondir=$(CAJA_EXTENSION_DIR)

baul_extension_LTLIBRARIES=libbaul-image-converter.la

libbaul_image_converter_la_SOURCES =				\
	image-converter.c						\
	baul-image-converter.c	baul-image-converter.h	\
	baul-image-resizer.c	baul-image-resizer.h	\
	baul-image-rotator.c	baul-image-rotator.h

libbaul_image_converter_la_LDFLAGS = -module -avoid-version
libbaul_image_converter_la_LIBADD  = $(CAJA_LIBS) $(IMAGE_CONVERTER_LIBS)

builderdir = $(datadir)/baul-extensions
builder_DATA = 				\
	baul-image-resize.ui	\
	baul-image-rotate.ui

extensiondir = $(datadir)/baul/extensions
extension_in_files = libbaul-image-converter.baul-extension.desktop.in
extension_DATA = $(extension_in_files:.baul-extension.desktop.in=.baul-extension)
$(extension_DATA): $(extension_in_files)
	$(AM_V_GEN) $(MSGFMT) --desktop --keyword= --keyword=Copyright --keyword=Name --keyword=Description --template $< -d $(top_srcdir)/po -o $@

EXTRA_DIST = $(builder_DATA)

CLEANFILES = $(extension_DATA)
