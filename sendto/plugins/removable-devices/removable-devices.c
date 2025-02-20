/*
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
 * Authors:  Maxim Ermilov <ermilov.maxim@gmail.com>
 *           Bastien Nocera <hadess@hadess.net>
 */

#include "config.h"
#include <string.h>
#include <glib/gi18n-lib.h>
#include <gio/gio.h>
#include "nst-common.h"
#include "baul-sendto-plugin.h"

enum {
	NAME_COL,
	ICON_COL,
	MOUNT_COL,
	NUM_COLS,
};

GVolumeMonitor* vol_monitor = NULL;
CtkWidget *cb;

static void
cb_mount_removed (GVolumeMonitor *volume_monitor G_GNUC_UNUSED,
		  GMount         *mount,
		  NstPlugin      *plugin G_GNUC_UNUSED)
{
	CtkTreeIter iter;
	CtkListStore *store;
	gboolean b, found;

	store = CTK_LIST_STORE (ctk_combo_box_get_model (CTK_COMBO_BOX (cb)));
	b = ctk_tree_model_get_iter_first (CTK_TREE_MODEL (store), &iter);
	found = FALSE;

	while (b) {
		GMount *m;
		ctk_tree_model_get (CTK_TREE_MODEL (store), &iter, MOUNT_COL, &m, -1);
		if (m == mount) {
			ctk_list_store_remove (store, &iter);
			g_object_unref (m);
			found = TRUE;
			break;
		}
		g_object_unref (m);
		b = ctk_tree_model_iter_next (CTK_TREE_MODEL (store), &iter);
	}

	/* If a mount was removed */
	if (found != FALSE) {
		/* And it was the selected one */
		if (ctk_combo_box_get_active (CTK_COMBO_BOX (cb)) == -1) {
			/* Select the first item in the list */
			ctk_combo_box_set_active (CTK_COMBO_BOX (cb), 0);
		}
	}
}

static void
cb_mount_changed (GVolumeMonitor *volume_monitor,
		  GMount         *mount,
		  NstPlugin      *plugin)
{
	CtkTreeIter iter;
	gboolean b;
	CtkListStore *store;

	if (g_mount_is_shadowed (mount) != FALSE) {
		cb_mount_removed (volume_monitor, mount, plugin);
		return;
	}

	store = CTK_LIST_STORE (ctk_combo_box_get_model (CTK_COMBO_BOX (cb)));
	b = ctk_tree_model_get_iter_first (CTK_TREE_MODEL (store), &iter);

	while (b) {
		GMount *m;
		ctk_tree_model_get (CTK_TREE_MODEL (store), &iter, MOUNT_COL, &m, -1);

		if (m == mount) {
			char *name;

			name = g_mount_get_name (mount);
			ctk_list_store_set (store, &iter,
					    NAME_COL, name,
					    ICON_COL, g_mount_get_icon (mount),
					    -1);
			g_free (name);
			g_object_unref (m);
			break;
		}
		g_object_unref (m);
		b = ctk_tree_model_iter_next (CTK_TREE_MODEL (store), &iter);
	}
}

static void
cb_mount_added (GVolumeMonitor *volume_monitor G_GNUC_UNUSED,
		GMount         *mount,
		NstPlugin      *plugin G_GNUC_UNUSED)
{
	char *name;
	CtkTreeIter iter;
	CtkTreeModel *model;
	gboolean select_added;

	if (g_mount_is_shadowed (mount) != FALSE)
		return;

	name = g_mount_get_name (mount);
	model = ctk_combo_box_get_model (CTK_COMBO_BOX (cb));

	select_added = ctk_tree_model_iter_n_children (model, NULL) == 0;

	ctk_list_store_append (CTK_LIST_STORE (model), &iter);
	ctk_list_store_set (CTK_LIST_STORE (model), &iter,
			    NAME_COL, name,
			    ICON_COL, g_mount_get_icon (mount),
			    MOUNT_COL, mount,
			    -1);

	g_free (name);

	if (select_added != FALSE)
		ctk_combo_box_set_active (CTK_COMBO_BOX (cb), 0);

}

static gboolean
init (NstPlugin *plugin G_GNUC_UNUSED)
{
	g_print ("Init removable-devices plugin\n");

	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");

	vol_monitor = g_volume_monitor_get ();
	cb = ctk_combo_box_new ();

	return TRUE;
}

static CtkWidget*
get_contacts_widget (NstPlugin *plugin)
{
	CtkListStore *store;
	GList *l, *mounts;
	CtkTreeIter iter;
	CtkCellRenderer *text_renderer, *icon_renderer;

	mounts = g_volume_monitor_get_mounts (vol_monitor);

	store = ctk_list_store_new (NUM_COLS, G_TYPE_STRING, G_TYPE_ICON, G_TYPE_OBJECT);

	for (l = mounts; l != NULL; l = l->next) {
		char *name;

		if (g_mount_is_shadowed (l->data) != FALSE) {
			g_object_unref (l->data);
			continue;
		}

		name = g_mount_get_name (l->data);

		ctk_list_store_append (store, &iter);
		ctk_list_store_set (store, &iter,
				    NAME_COL, name,
				    ICON_COL, g_mount_get_icon (l->data),
				    MOUNT_COL, l->data,
				    -1);
		g_free (name);

		g_object_unref (l->data);
	}
	g_list_free (mounts);

	ctk_cell_layout_clear (CTK_CELL_LAYOUT (cb));
	ctk_combo_box_set_model (CTK_COMBO_BOX (cb), CTK_TREE_MODEL (store));

	text_renderer = ctk_cell_renderer_text_new ();
	icon_renderer = ctk_cell_renderer_pixbuf_new ();
	ctk_cell_layout_pack_start (CTK_CELL_LAYOUT (cb), icon_renderer, FALSE);
	ctk_cell_layout_pack_start (CTK_CELL_LAYOUT (cb), text_renderer, TRUE);

	ctk_cell_layout_set_attributes (CTK_CELL_LAYOUT (cb), text_renderer, "text", 0,  NULL);
	ctk_cell_layout_set_attributes (CTK_CELL_LAYOUT (cb), icon_renderer, "gicon", 1,  NULL);
	ctk_combo_box_set_active (CTK_COMBO_BOX (cb), 0);

	g_signal_connect (G_OBJECT (vol_monitor), "mount-removed", G_CALLBACK (cb_mount_removed), plugin);
	g_signal_connect (G_OBJECT (vol_monitor), "mount-added", G_CALLBACK (cb_mount_added), plugin);
	g_signal_connect (G_OBJECT (vol_monitor), "mount-changed", G_CALLBACK (cb_mount_changed), plugin);

	return cb;
}

static gboolean
send_files (NstPlugin *plugin G_GNUC_UNUSED,
	    CtkWidget *contact_widget,
	    GList     *file_list)
{
	CtkListStore *store;
	CtkTreeIter iter;
	GMount *dest_mount;
	GFile *mount_root;

	if (ctk_combo_box_get_active_iter (CTK_COMBO_BOX (contact_widget), &iter) == FALSE)
		return TRUE;

	store = CTK_LIST_STORE (ctk_combo_box_get_model (CTK_COMBO_BOX (cb)));
	ctk_tree_model_get (CTK_TREE_MODEL (store), &iter, MOUNT_COL, &dest_mount, -1);
	mount_root = g_mount_get_root (dest_mount);

	copy_files_to (file_list, mount_root);

	g_object_unref (mount_root);

	return TRUE;
}

static gboolean
destroy (NstPlugin *plugin G_GNUC_UNUSED)
{
	ctk_widget_destroy (cb);

	g_object_unref (vol_monitor);
	return TRUE;
}

static
NstPluginInfo plugin_info = {
	"folder-remote",
	"folder-remote",
	N_("Removable disks and shares"),
	NULL,
	BAUL_CAPS_SEND_DIRECTORIES,
	init,
	get_contacts_widget,
	NULL,
	send_files,
	destroy
};

NST_INIT_PLUGIN (plugin_info)

