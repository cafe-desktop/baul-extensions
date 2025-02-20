/*
 *  Baul Wallpaper extension
 *
 *  Copyright (C) 2005 Adam Israel
 *  Copyright (C) 2014 Stefano Karapetsas
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *  Authors: Adam Israel <adam@battleaxe.net>
 *           Stefano Karapetsas <stefano@karapetsas.com>
 */

#include <config.h>
#include <string.h>
#include <gio/gio.h>
#include <glib/gi18n-lib.h>
#include <libbaul-extension/baul-extension-types.h>
#include <libbaul-extension/baul-file-info.h>
#include <libbaul-extension/baul-menu-provider.h>
#include "baul-wallpaper-extension.h"

#define WP_SCHEMA "org.cafe.background"
#define WP_FILE_KEY "picture-filename"

static GObjectClass *parent_class;

static void
set_wallpaper_callback (BaulMenuItem *item,
			gpointer      user_data G_GNUC_UNUSED)
{
    GList *files;
    GSettings *settings;
    BaulFileInfo *file;
    gchar *uri;
    gchar *filename;

    files = g_object_get_data (G_OBJECT (item), "files");
    file = files->data;

    uri = baul_file_info_get_uri (file);
    filename = g_filename_from_uri(uri, NULL, NULL);

    settings = g_settings_new (WP_SCHEMA);

    g_settings_set_string (settings, WP_FILE_KEY, filename);

    g_object_unref (settings);
    g_free (filename);
    g_free (uri);

}

static gboolean
is_image (BaulFileInfo *file)
{
    gchar   *mimeType;
    gboolean isImage;

    mimeType = baul_file_info_get_mime_type (file);
    isImage = g_str_has_prefix (mimeType, "image/");
    g_free (mimeType);
    return isImage;
}


static GList *
baul_cwe_get_file_items (BaulMenuProvider *provider,
			 CtkWidget        *window G_GNUC_UNUSED,
			 GList            *files)
{
    GList    *items = NULL;
    GList    *scan;
    gboolean  one_item;
    BaulMenuItem *item;

    for (scan = files; scan; scan = scan->next) {
        BaulFileInfo *file = scan->data;
        gchar            *scheme;
        gboolean          local;

        scheme = baul_file_info_get_uri_scheme (file);
        local = strncmp (scheme, "file", 4) == 0;
        g_free (scheme);

        if (!local)
            return NULL;
    }

    one_item = (files != NULL) && (files->next == NULL);
    if (one_item && is_image ((BaulFileInfo *)files->data) &&
        !baul_file_info_is_directory ((BaulFileInfo *)files->data)) {
        item = baul_menu_item_new ("BaulCwe::sendto",
                           _("Set as wallpaper"),
                           _("Set image as the current wallpaper"),
                           NULL);
        g_signal_connect (item,
                  "activate",
                  G_CALLBACK (set_wallpaper_callback),
                provider);
        g_object_set_data_full (G_OBJECT (item),
                    "files",
                    baul_file_info_list_copy (files),
                    (GDestroyNotify) baul_file_info_list_free);
        items = g_list_append (items, item);
    }
    return items;
}


static void
baul_cwe_menu_provider_iface_init (BaulMenuProviderIface *iface)
{
    iface->get_file_items = baul_cwe_get_file_items;
}


static void
baul_cwe_instance_init (BaulCwe *cwe G_GNUC_UNUSED)
{
}


static void
baul_cwe_class_init (BaulCweClass *class)
{
    parent_class = g_type_class_peek_parent (class);
}


static GType cwe_type = 0;


GType
baul_cwe_get_type (void)
{
    return cwe_type;
}


void
baul_cwe_register_type (GTypeModule *module)
{
    static const GTypeInfo info = {
        .class_size = sizeof (BaulCweClass),
        .class_init = (GClassInitFunc) baul_cwe_class_init,
        .instance_size = sizeof (BaulCwe),
        .n_preallocs = 0,
        .instance_init = (GInstanceInitFunc) baul_cwe_instance_init,
    };

    static const GInterfaceInfo menu_provider_iface_info = {
        .interface_init = (GInterfaceInitFunc) baul_cwe_menu_provider_iface_init,
    };

    cwe_type = g_type_module_register_type (module,
                             G_TYPE_OBJECT,
                             "BaulCwe",
                             &info, 0);

    g_type_module_add_interface (module,
                     cwe_type,
                     BAUL_TYPE_MENU_PROVIDER,
                     &menu_provider_iface_info);
}
