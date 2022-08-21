/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/*
 * Copyright (C) 2008 Jader Henrique da Silva
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more av.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301  USA.
 *
 * Author:  Jader Henrique da Silva <vovozito@gmail.com>
 */

#include "config.h"
#include <string.h>
#include <glib/gi18n-lib.h>
#include "nst-common.h"
#include "baul-sendto-plugin.h"

enum {
	COL_PIXBUF,
	COL_LABEL,
	NUM_COLS,
};

#define COMBOBOX_OPTION_NEW_DVD 0
#define COMBOBOX_OPTION_EXISTING_DVD 1

static GFile *burn = NULL;

static
gboolean init (NstPlugin *plugin)
{
	CtkIconTheme *it;
	char *cmd;

	g_print ("Init baul burn plugin\n");

	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");

	it = ctk_icon_theme_get_default ();
	ctk_icon_theme_append_search_path (it, DATADIR "/brasero/icons");

	cmd = g_find_program_in_path ("brasero");
	if (cmd == NULL)
		return FALSE;
	g_free (cmd);

	burn = g_file_new_for_uri ("burn:/");

	return TRUE;
}

static
CtkWidget* get_contacts_widget (NstPlugin *plugin)
{
	CtkWidget *widget;
	CtkCellRenderer *renderer;
	CtkListStore *store;
	CtkTreeModel *model;
	GFileEnumerator *fenum;
	GFileInfo *file_info = NULL;
	int selection = COMBOBOX_OPTION_NEW_DVD;

	fenum = g_file_enumerate_children (burn,
					   G_FILE_ATTRIBUTE_STANDARD_NAME,
					   G_FILE_QUERY_INFO_NONE,
					   NULL,
					   NULL);

	if (fenum != NULL) {
		file_info = g_file_enumerator_next_file (fenum, NULL, NULL);
		g_object_unref (fenum);
	}

	store = ctk_list_store_new (NUM_COLS, G_TYPE_STRING, G_TYPE_STRING);

	ctk_list_store_insert_with_values (store, NULL,
					   INT_MAX,
					   COL_PIXBUF, "media-optical-blank",
					   COL_LABEL, _("New CD/DVD"),
					   -1);

	if (file_info != NULL) {
		ctk_list_store_insert_with_values (store, NULL,
						   INT_MAX,
						   COL_PIXBUF, "media-optical-data-new",
						   COL_LABEL, _("Existing CD/DVD"),
						   -1);
		g_object_unref (file_info);
		selection = COMBOBOX_OPTION_EXISTING_DVD;
	}

	model = CTK_TREE_MODEL (store);
	widget = ctk_combo_box_new_with_model (model);
	renderer = ctk_cell_renderer_pixbuf_new ();
	ctk_cell_layout_pack_start (CTK_CELL_LAYOUT (widget),
				    renderer,
				    FALSE);
	ctk_cell_layout_set_attributes (CTK_CELL_LAYOUT (widget),
					renderer,
					"icon-name", COL_PIXBUF,
					NULL);
	renderer = ctk_cell_renderer_text_new ();
	ctk_cell_layout_pack_start (CTK_CELL_LAYOUT (widget),
				    renderer,
				    TRUE);
	ctk_cell_layout_set_attributes (CTK_CELL_LAYOUT (widget),
					renderer,
					"text", COL_LABEL,
					NULL);

	ctk_combo_box_set_active (CTK_COMBO_BOX (widget), selection);

	return widget;
}

static
gboolean send_files (NstPlugin *plugin,
		     CtkWidget *burntype_widget,
		     GList *file_list)
{
	GFileEnumerator *fenum;
	GFileInfo *file_info;
	GFile *child;

	if (ctk_combo_box_get_active (CTK_COMBO_BOX (burntype_widget)) == COMBOBOX_OPTION_NEW_DVD) {
		fenum = g_file_enumerate_children (burn,
						   G_FILE_ATTRIBUTE_STANDARD_NAME,
						   G_FILE_QUERY_INFO_NONE,
						   NULL,
						   NULL);

		if (fenum != NULL) {
			while ((file_info = g_file_enumerator_next_file (fenum, NULL, NULL)) != NULL) {
				child = g_file_get_child (burn,
							  g_file_info_get_name(file_info));

				g_object_unref (file_info);
				g_file_delete (child, NULL, NULL);
				g_object_unref (child);
			}
			g_object_unref (fenum);
		}
	}

	copy_files_to (file_list, burn);

	ctk_show_uri_on_window (NULL, "burn:///", GDK_CURRENT_TIME, NULL);

	return TRUE;
}

static
gboolean destroy (NstPlugin *plugin){

	g_object_unref (burn);
	burn = NULL;
	return TRUE;

}

static
NstPluginInfo plugin_info = {
	"brasero",
	"baul-burn",
	N_("CD/DVD Creator"),
	NULL,
	BAUL_CAPS_SEND_DIRECTORIES,
	init,
	get_contacts_widget,
	NULL,
	send_files,
	destroy
};

NST_INIT_PLUGIN (plugin_info)

