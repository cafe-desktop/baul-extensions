/*
 *  baul-image-converter.c
 *
 *  Copyright (C) 2004-2005 Jürg Billeter
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *  Author: Jürg Billeter <j@bitron.ch>
 *
 */

#ifdef HAVE_CONFIG_H
 #include <config.h> /* for GETTEXT_PACKAGE */
#endif

#include "baul-image-converter.h"
#include "baul-image-resizer.h"
#include "baul-image-rotator.h"

#include <libbaul-extension/baul-menu-provider.h>

#include <glib/gi18n-lib.h>
#include <ctk/ctk.h>

#include <string.h> /* for strcmp */

static void baul_image_converter_instance_init (BaulImageConverter      *img);
static void baul_image_converter_class_init    (BaulImageConverterClass *class);
GList *     baul_image_converter_get_file_items (BaulMenuProvider *provider,
						     CtkWidget            *window,
						     GList                *files);

static GType image_converter_type = 0;

static gboolean
image_converter_file_is_image (BaulFileInfo *file_info)
{
	gchar            *uri_scheme;
	gchar            *mime_type;
	gboolean	maybe_image;

	maybe_image = TRUE;
	uri_scheme = baul_file_info_get_uri_scheme (file_info);
	if (strcmp (uri_scheme, "file") != 0)
		maybe_image = FALSE;
	g_free (uri_scheme);

	mime_type = baul_file_info_get_mime_type (file_info);
	if (strncmp (mime_type, "image/", 6) != 0)
		maybe_image = FALSE;
	g_free (mime_type);

	return maybe_image;
}

static GList *
image_converter_filter_images (GList *files)
{
	GList *images;
	GList *file;

	images = NULL;

	for (file = files; file != NULL; file = file->next) {
		if (image_converter_file_is_image (file->data))
			images = g_list_prepend (images, file->data);
	}

	return images;
}

static void
image_resize_callback (BaulMenuItem *item G_GNUC_UNUSED,
		       GList        *files)
{
	BaulImageResizer *resizer = baul_image_resizer_new (image_converter_filter_images (files));
	baul_image_resizer_show_dialog (resizer);
}

static void
image_rotate_callback (BaulMenuItem *item G_GNUC_UNUSED,
		       GList        *files)
{
	BaulImageRotator *rotator = baul_image_rotator_new (image_converter_filter_images (files));
	baul_image_rotator_show_dialog (rotator);
}

static GList *
baul_image_converter_get_background_items (BaulMenuProvider *provider G_GNUC_UNUSED,
					   CtkWidget        *window G_GNUC_UNUSED,
					   BaulFileInfo     *file_info G_GNUC_UNUSED)
{
	return NULL;
}

GList *
baul_image_converter_get_file_items (BaulMenuProvider *provider G_GNUC_UNUSED,
				     CtkWidget        *window G_GNUC_UNUSED,
				     GList            *files)
{
	BaulMenuItem *item;
	GList *file;
	GList *items = NULL;

	for (file = files; file != NULL; file = file->next) {
		if (image_converter_file_is_image (file->data)) {
			item = baul_menu_item_new ("BaulImageConverter::resize",
				        _("_Resize Images..."),
				        _("Resize each selected image"),
				       NULL);
			g_signal_connect (item, "activate",
					  G_CALLBACK (image_resize_callback),
					  baul_file_info_list_copy (files));

			items = g_list_prepend (items, item);

			item = baul_menu_item_new ("BaulImageConverter::rotate",
				        _("Ro_tate Images..."),
				        _("Rotate each selected image"),
				       NULL);
			g_signal_connect (item, "activate",
					  G_CALLBACK (image_rotate_callback),
					  baul_file_info_list_copy (files));

			items = g_list_prepend (items, item);

			items = g_list_reverse (items);

			return items;
		}
	}

	return NULL;
}

static void
baul_image_converter_menu_provider_iface_init (BaulMenuProviderIface *iface)
{
	iface->get_background_items = baul_image_converter_get_background_items;
	iface->get_file_items = baul_image_converter_get_file_items;
}

static void
baul_image_converter_instance_init (BaulImageConverter *img G_GNUC_UNUSED)
{
}

static void
baul_image_converter_class_init (BaulImageConverterClass *class G_GNUC_UNUSED)
{
}

GType
baul_image_converter_get_type (void)
{
	return image_converter_type;
}

void
baul_image_converter_register_type (GTypeModule *module)
{
	static const GTypeInfo info = {
		.class_size = sizeof (BaulImageConverterClass),
		.class_init = (GClassInitFunc) baul_image_converter_class_init,
		.instance_size = sizeof (BaulImageConverter),
		.n_preallocs = 0,
		.instance_init = (GInstanceInitFunc) baul_image_converter_instance_init,
	};

	static const GInterfaceInfo menu_provider_iface_info = {
		.interface_init = (GInterfaceInitFunc) baul_image_converter_menu_provider_iface_init,
	};

	image_converter_type = g_type_module_register_type (module,
						     G_TYPE_OBJECT,
						     "BaulImageConverter",
						     &info, 0);

	g_type_module_add_interface (module,
				     image_converter_type,
				     BAUL_TYPE_MENU_PROVIDER,
				     &menu_provider_iface_info);
}
