/*
 *  Baul-sendto
 *
 *  Copyright (C) 2004 Free Software Foundation, Inc.
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
 *  Author: Roberto Majadas <roberto.majadas@openshine.com>
 *
 */

#include <config.h>
#include <string.h>
#include <glib/gi18n-lib.h>
#include <libbaul-extension/baul-extension-types.h>
#include <libbaul-extension/baul-file-info.h>
#include <libbaul-extension/baul-menu-provider.h>
#include "baul-nste.h"


static GObjectClass *parent_class;

static void
sendto_callback (BaulMenuItem *item,
		 gpointer      user_data G_GNUC_UNUSED)
{
	GList            *files, *scan;
	gchar            *uri;
	GString          *cmd;

	files = g_object_get_data (G_OBJECT (item), "files");

	cmd = g_string_new ("baul-sendto");

	for (scan = files; scan; scan = scan->next) {
		BaulFileInfo *file = scan->data;

		uri = baul_file_info_get_uri (file);
		g_string_append_printf (cmd, " \"%s\"", uri);
		g_free (uri);
	}

	g_spawn_command_line_async (cmd->str, NULL);

	g_string_free (cmd, TRUE);
}

static GList *
baul_nste_get_file_items (BaulMenuProvider *provider,
			  CtkWidget        *window G_GNUC_UNUSED,
			  GList            *files)
{
	GList    *items = NULL;
	gboolean  one_item;
	BaulMenuItem *item;

	if (files == NULL)
		return NULL;

	one_item = (files != NULL) && (files->next == NULL);
	if (one_item &&
	    !baul_file_info_is_directory ((BaulFileInfo *)files->data)) {
		item = baul_menu_item_new ("BaulNste::sendto",
					       _("Send to..."),
					       _("Send file by mail, instant message..."),
					       "document-send");
	} else {
		item = baul_menu_item_new ("BaulNste::sendto",
					       _("Send to..."),
					       _("Send files by mail, instant message..."),
					       "document-send");
	}
  g_signal_connect (item,
      "activate",
      G_CALLBACK (sendto_callback),
      provider);
  g_object_set_data_full (G_OBJECT (item),
      "files",
      baul_file_info_list_copy (files),
      (GDestroyNotify) baul_file_info_list_free);

  items = g_list_append (items, item);

	return items;
}


static void
baul_nste_menu_provider_iface_init (BaulMenuProviderIface *iface)
{
	iface->get_file_items = baul_nste_get_file_items;
}


static void
baul_nste_instance_init (BaulNste *nste G_GNUC_UNUSED)
{
}


static void
baul_nste_class_init (BaulNsteClass *class)
{
	parent_class = g_type_class_peek_parent (class);
}


static GType nste_type = 0;


GType
baul_nste_get_type (void)
{
	return nste_type;
}


void
baul_nste_register_type (GTypeModule *module)
{
	static const GTypeInfo info = {
		.class_size = sizeof (BaulNsteClass),
		.class_init = (GClassInitFunc) baul_nste_class_init,
		.instance_size = sizeof (BaulNste),
		.n_preallocs = 0,
		.instance_init = (GInstanceInitFunc) baul_nste_instance_init,
	};

	static const GInterfaceInfo menu_provider_iface_info = {
		.interface_init = (GInterfaceInitFunc) baul_nste_menu_provider_iface_init,
	};

	nste_type = g_type_module_register_type (module,
					         G_TYPE_OBJECT,
					         "BaulNste",
					         &info, 0);

	g_type_module_add_interface (module,
				     nste_type,
				     BAUL_TYPE_MENU_PROVIDER,
				     &menu_provider_iface_info);
}
