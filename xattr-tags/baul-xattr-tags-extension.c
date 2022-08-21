/*
 *  Baul xattr tags extension
 *
 *  Copyright (C) 2016 Felipe Barriga Richards
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
 *  Authors: Felipe Barriga Richards <spam@felipebarriga.cl>
 */

#include <config.h>
#include <string.h>
#include <gio/gio.h>
#include <glib/gi18n-lib.h>
#include <libbaul-extension/baul-file-info.h>
#include <libbaul-extension/baul-info-provider.h>
#include <libbaul-extension/baul-column-provider.h>
#include "baul-xattr-tags-extension.h"

#define XATTR_TAGS_NAME "Xattr::Tags"
#define XATTR_TAGS_ATTRIBUTE "xattr_tags"

#define G_FILE_ATTRIBUTE_XATTR_XDG_TAGS "xattr::xdg.tags"

static GObjectClass *parent_class;

typedef struct {
    gboolean cancelled;
    BaulInfoProvider *provider;
    BaulFileInfo *file;
    GClosure *update_complete;
} BaulXattrTagsHandle;

/* Stolen code: why they didn't expose it!?
 * file: glocalfileinfo.c
 * function: hex_unescape_string
 * GIO - GLib Input, Output and Streaming Library
 */
static char *
hex_unescape_string (const char *str,
                     int        *out_len,
                     gboolean   *free_return)
{
    int i;
    char *unescaped_str, *p;
    unsigned char c;
    int len;

    len = strlen (str);

    if (strchr (str, '\\') == NULL)
    {
        if (out_len)
            *out_len = len;
        *free_return = FALSE;
        return (char *)str;
    }

    unescaped_str = g_malloc (len + 1);

    p = unescaped_str;
    for (i = 0; i < len; i++)
    {
        if (str[i] == '\\' &&
            str[i+1] == 'x' &&
            len - i >= 4)
        {
            c =
                (g_ascii_xdigit_value (str[i+2]) << 4) |
                g_ascii_xdigit_value (str[i+3]);
            *p++ = c;
            i += 3;
        }
        else
            *p++ = str[i];
    }
    *p++ = 0;

    if (out_len)
        *out_len = p - unescaped_str;
    *free_return = TRUE;
    return unescaped_str;
}
/* End of stolen code */

static gchar *baul_xattr_tags_get_xdg_tags(BaulFileInfo *file)
{
    gchar *tags = NULL, *uri;
    GFile *location;
    GFileInfo *info;

    uri = baul_file_info_get_activation_uri (file);
    location = g_file_new_for_uri (uri);
    info = g_file_query_info (location,
                              G_FILE_ATTRIBUTE_XATTR_XDG_TAGS,
                              0,
                              NULL,
                              NULL);

    if (info) {
        if (g_file_info_has_attribute(info, G_FILE_ATTRIBUTE_XATTR_XDG_TAGS)) {
            const gchar *escaped_tags =
                g_file_info_get_attribute_string(info, G_FILE_ATTRIBUTE_XATTR_XDG_TAGS);

            gboolean new_created = FALSE;
            gchar *un_escaped_tags = hex_unescape_string (escaped_tags, NULL, &new_created);
            if (new_created) {
                tags = un_escaped_tags;
            } else {
                /* the string didn't contain any escaped character */
                tags = g_strdup(escaped_tags);
            }
        }
        g_object_unref (info);
    }
    g_object_unref (location);
    g_free (uri);

    return tags;
}

static BaulOperationResult
baul_xattr_tags_update_file_info(BaulInfoProvider *provider,
                            BaulFileInfo *file,
                            GClosure *update_complete,
                            BaulOperationHandle **handle)
{
    gchar *value = baul_xattr_tags_get_xdg_tags(file);
    if (value != NULL) {
        baul_file_info_add_string_attribute(file, XATTR_TAGS_ATTRIBUTE, value);
        g_free(value);
    } else {
        baul_file_info_add_string_attribute(file, XATTR_TAGS_ATTRIBUTE, "");
    }
    return BAUL_OPERATION_COMPLETE;
}


static void
baul_xattr_tags_cancel_update(BaulInfoProvider *provider,
                         BaulOperationHandle *handle)
{
    BaulXattrTagsHandle *xattr_handle;

    xattr_handle = (BaulXattrTagsHandle*)handle;
    xattr_handle->cancelled = TRUE;
}

static void
baul_xattr_tags_info_provider_iface_init(BaulInfoProviderIface *iface)
{
    iface->update_file_info = baul_xattr_tags_update_file_info;
    iface->cancel_update = baul_xattr_tags_cancel_update;
}


static GList *
baul_xattr_tags_get_columns(BaulColumnProvider *provider)
{
    GList *ret = NULL;
    BaulColumn *column = NULL;

    column = baul_column_new(XATTR_TAGS_NAME,
                             XATTR_TAGS_ATTRIBUTE,
                             _("Tags"),
                             _("Tags stored in extended attributes"));
    ret = g_list_append(NULL, column);

    return ret;
}

static void
baul_xattr_tags_column_provider_iface_init(BaulColumnProviderIface *iface)
{
    iface->get_columns = baul_xattr_tags_get_columns;
}


static void
baul_xattr_tags_instance_init(BaulXattrTags *baulXattrTags)
{
}


static void
baul_xattr_tags_class_init(BaulXattrTagsClass *class)
{
    parent_class = g_type_class_peek_parent (class);
}


static GType baul_xattr_tags_type = 0;


GType
baul_xattr_tags_get_type(void)
{
    return baul_xattr_tags_type;
}


void
baul_xattr_tags_register_type(GTypeModule *module)
{
    static const GTypeInfo info = {
        sizeof (BaulXattrTagsClass),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc) baul_xattr_tags_class_init,
        NULL,
        NULL,
        sizeof (BaulXattrTags),
        0,
        (GInstanceInitFunc) baul_xattr_tags_instance_init,
    };


    baul_xattr_tags_type = g_type_module_register_type (module,
                                            G_TYPE_OBJECT,
                                            "BaulXattrTags",
                                            &info, 0);

    static const GInterfaceInfo info_provider_iface_info = {
            (GInterfaceInitFunc) baul_xattr_tags_info_provider_iface_init,
            NULL,
            NULL
    };

    g_type_module_add_interface (module,
                                 BAUL_TYPE_XATTR_TAGS,
                                 BAUL_TYPE_INFO_PROVIDER,
                                 &info_provider_iface_info);

    static const GInterfaceInfo column_provider_iface_info = {
            (GInterfaceInitFunc) baul_xattr_tags_column_provider_iface_init,
            NULL,
            NULL
    };


    g_type_module_add_interface (module,
                                 BAUL_TYPE_XATTR_TAGS,
                                 BAUL_TYPE_COLUMN_PROVIDER,
                                 &column_provider_iface_info);

}

void
baul_module_initialize (GTypeModule  *module)
{
    g_print ("Initializing baul-xattr-tags extension\n");

    baul_xattr_tags_register_type (module);
}

void
baul_module_shutdown (void)
{
}

/* List all the extension types.  */
void
baul_module_list_types (const GType **types,
                        int          *num_types)
{
    static GType type_list[1];

    type_list[0] = BAUL_TYPE_XATTR_TAGS;

    *types = type_list;
    *num_types = 1;
}
