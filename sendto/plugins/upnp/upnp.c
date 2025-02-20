/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/*
 * Copyright (C) 2008 Zeeshan Ali (Khattak)
 * Copyright (C) 2006 Peter Enseleit
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
 * Author:  Zeeshan Ali (Khattak) <zeeshanak@gnome.org>
 *          Peter Enseleit <penseleit@gmail.com>
 *          Roberto Majadas <telemaco@openshine.com>
 *
 */

#include "config.h"

#include <glib/gi18n-lib.h>
#include <libgupnp/gupnp.h>
#include "baul-sendto-plugin.h"

#define MEDIA_SERVER "urn:schemas-upnp-org:device:MediaServer:1"
#define CDS "urn:schemas-upnp-org:service:ContentDirectory"

enum {
	UDN_COL,
	NAME_COL,
	INTERFACE_COL,
	NUM_COLS
};

static CtkWidget *combobox;
static CtkTreeModel *model;
static GUPnPContextManager *context_manager;

static gboolean
find_device (const gchar *udn,
	     CtkTreeIter *iter)
{
	gboolean found = FALSE;

	if (!ctk_tree_model_get_iter_first (model, iter))
		return FALSE;

	do {
		gchar *tmp;

		ctk_tree_model_get (model,
				    iter,
				    UDN_COL, &tmp,
				    -1);

		if (tmp != NULL && strcmp (tmp, udn) == 0)
			found = TRUE;

		g_free (tmp);
	} while (!found && ctk_tree_model_iter_next (model, iter));

	return found;
}

static gboolean
check_required_actions (GUPnPServiceIntrospection *introspection)
{
	if (gupnp_service_introspection_get_action (introspection,
						    "CreateObject") == NULL)
		return FALSE;
	if (gupnp_service_introspection_get_action (introspection,
						    "ImportResource") == NULL)
		return FALSE;
	return TRUE;
}

static void
get_introspection_cb (GObject      *source,
		      GAsyncResult *res,
		      gpointer      user_data)
{
	GUPnPDeviceInfo *device_info;
	gchar *name;
	const gchar *udn, *interface;
	CtkTreeIter iter;
	GUPnPContext *context;
	GUPnPServiceIntrospection *introspection;

	GError *error = NULL;

	introspection = gupnp_service_info_introspect_finish (GUPNP_SERVICE_INFO (source),
							      res,
							      &error);

	device_info = GUPNP_DEVICE_INFO (user_data);

	if (introspection != NULL) {
		/* If introspection is available, make sure required actions
		 * are implemented.
		 */
		if (!check_required_actions (introspection))
			goto error;
	}

	udn = gupnp_device_info_get_udn (device_info);
	if (G_UNLIKELY (udn == NULL))
		goto error;

	/* First check if the device is already added */
	if (find_device (udn, &iter))
		goto error;

	name = gupnp_device_info_get_friendly_name (device_info);
	if (name == NULL)
		name = g_strdup (udn);

	context = gupnp_device_info_get_context (device_info);
	interface = gssdp_client_get_interface (GSSDP_CLIENT (context));

	ctk_list_store_insert_with_values (CTK_LIST_STORE (model), NULL, -1,
					   UDN_COL, udn,
					   NAME_COL, name,
					   INTERFACE_COL, interface,
					   -1);

	g_free (name);

error:
	/* We don't need the proxy objects anymore */
	g_object_unref (source);
	g_object_ref (device_info);
}

static void
device_proxy_available_cb (GUPnPControlPoint *cp G_GNUC_UNUSED,
			   GUPnPDeviceProxy  *proxy)
{
	GUPnPServiceInfo *info;

	info = gupnp_device_info_get_service (GUPNP_DEVICE_INFO (proxy), CDS);
	if (G_UNLIKELY (info == NULL)) {
		/* No ContentDirectory implemented? Not interesting. */
		return;
	}

	gupnp_service_info_introspect_async (info,
					     NULL,
					     get_introspection_cb,
					     g_object_ref (proxy));
}

static void
device_proxy_unavailable_cb (GUPnPControlPoint *cp G_GNUC_UNUSED,
			     GUPnPDeviceProxy  *proxy)
{
	CtkTreeIter iter;
	const gchar *udn;

	udn = gupnp_device_info_get_udn (GUPNP_DEVICE_INFO (proxy));
	if (udn == NULL)
		return;

	/* First check if the device is already added */
	if (find_device (udn, &iter))
		ctk_list_store_remove (CTK_LIST_STORE (model), &iter);
}

static void
on_context_available (GUPnPContextManager *context_manager,
		      GUPnPContext        *context,
		      gpointer             user_data G_GNUC_UNUSED)
{
	GUPnPControlPoint *cp;

	cp = gupnp_control_point_new (context, MEDIA_SERVER);

	g_signal_connect (cp,
			  "device-proxy-available",
			  G_CALLBACK (device_proxy_available_cb),
			  NULL);
	g_signal_connect (cp,
			  "device-proxy-unavailable",
			  G_CALLBACK (device_proxy_unavailable_cb),
			  NULL);

	gssdp_resource_browser_set_active (GSSDP_RESOURCE_BROWSER (cp), TRUE);

	/* Let context manager take care of the control point life cycle */
	gupnp_context_manager_manage_control_point (context_manager, cp);
	g_object_unref (cp);
}

static gboolean
init (NstPlugin *plugin G_GNUC_UNUSED)
{
	CtkListStore *store;
	CtkCellRenderer *renderer;
	char *upload_cmd;

	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");

	upload_cmd = g_find_program_in_path ("gupnp-upload");
	if (upload_cmd == NULL)
		return FALSE;
	g_free (upload_cmd);

	context_manager = gupnp_context_manager_create (0);

	g_assert (context_manager != NULL);
	g_signal_connect (context_manager, "context-available",
			  G_CALLBACK (on_context_available), NULL);

	combobox = ctk_combo_box_new ();

	store = ctk_list_store_new (NUM_COLS,
				    G_TYPE_STRING,   /* UDN  */
				    G_TYPE_STRING,   /* Name */
				    G_TYPE_STRING);  /* Network Interface */
	model = CTK_TREE_MODEL (store);
	ctk_combo_box_set_model (CTK_COMBO_BOX (combobox), model);

	renderer = ctk_cell_renderer_text_new ();

	ctk_cell_layout_pack_start (CTK_CELL_LAYOUT (combobox),
				    renderer,
				    TRUE);
	ctk_cell_layout_add_attribute (CTK_CELL_LAYOUT (combobox),
				       renderer,
				       "text", NAME_COL);

	return TRUE;
}

static CtkWidget*
get_contacts_widget (NstPlugin *plugin G_GNUC_UNUSED)
{
	return combobox;
}

static gboolean
send_files (NstPlugin *plugin G_GNUC_UNUSED,
	    CtkWidget *contact_widget G_GNUC_UNUSED,
	    GList     *file_list)
{
	gchar *upload_cmd, *udn, *interface;
	GPtrArray *argv;
	gboolean ret;
	GList *l;
	CtkTreeIter iter;
	GError *err = NULL;

	if (!ctk_combo_box_get_active_iter (CTK_COMBO_BOX (combobox), &iter))
		return FALSE;

	ctk_tree_model_get (model, &iter, UDN_COL, &udn, INTERFACE_COL,
			    &interface, -1);

	upload_cmd = g_find_program_in_path ("gupnp-upload");
	if (upload_cmd == NULL)
		return FALSE;

	argv = g_ptr_array_new ();
	g_ptr_array_add (argv, upload_cmd);
	g_ptr_array_add (argv, "-t");
	g_ptr_array_add (argv, "15"); /* discovery timeout (seconds) */
	g_ptr_array_add (argv, "-e");
	g_ptr_array_add (argv, interface);
	g_ptr_array_add (argv, udn);
	for (l = file_list ; l; l=l->next) {
		gchar *file_path;

		file_path = g_filename_from_uri (l->data, NULL, NULL);
		g_ptr_array_add (argv, file_path);
	}
	g_ptr_array_add (argv, NULL);

	ret = g_spawn_async (NULL, (gchar **) argv->pdata,
			     NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, &err);

	if (ret == FALSE) {
		g_warning ("Could not send files to MediaServer: %s",
			   err->message);
		g_error_free (err);
	}

	g_ptr_array_free (argv, TRUE);
	g_free (upload_cmd);
	g_free (udn);

	return ret;
}

static gboolean
destroy (NstPlugin *plugin G_GNUC_UNUSED)
{
	ctk_widget_destroy (combobox);
	g_object_unref (model);

	g_object_unref (context_manager);

	return TRUE;
}

static
NstPluginInfo plugin_info = {
	"folder-remote",
	"upnp",
	N_("UPnP Media Server"),
	NULL,
	BAUL_CAPS_NONE,
	init,
	get_contacts_widget,
	NULL,
	send_files,
	destroy
};

NST_INIT_PLUGIN (plugin_info)

