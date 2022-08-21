/*
 *  baul-image-rotator.c
 *
 *  Copyright (C) 2004-2008 Jürg Billeter
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

#include "baul-image-rotator.h"

#include <string.h>

#include <glib/gi18n.h>
#include <gio/gio.h>
#include <ctk/ctk.h>

#include <libbaul-extension/baul-file-info.h>

typedef struct _BaulImageRotatorPrivate BaulImageRotatorPrivate;

struct _BaulImageRotatorPrivate {
	GList *files;

	gchar *suffix;

	int images_rotated;
	int images_total;
	gboolean cancelled;

	gchar *angle;

	CtkDialog *rotate_dialog;
	CtkRadioButton *default_angle_radiobutton;
	CtkComboBox *angle_combobox;
	CtkRadioButton *custom_angle_radiobutton;
	CtkSpinButton *angle_spinbutton;
	CtkRadioButton *append_radiobutton;
	CtkEntry *name_entry;
	CtkRadioButton *inplace_radiobutton;

	CtkWidget *progress_dialog;
	CtkWidget *progress_bar;
	CtkWidget *progress_label;
};

G_DEFINE_TYPE_WITH_PRIVATE (BaulImageRotator, baul_image_rotator, G_TYPE_OBJECT)

enum {
	PROP_FILES = 1,
};

typedef enum {
	/* Place Signal Types Here */
	SIGNAL_TYPE_EXAMPLE,
	LAST_SIGNAL
} BaulImageRotatorSignalType;

static void
baul_image_rotator_finalize(GObject *object)
{
	BaulImageRotator *dialog = BAUL_IMAGE_ROTATOR (object);
	BaulImageRotatorPrivate *priv = baul_image_rotator_get_instance_private (dialog);

	g_free (priv->suffix);

	G_OBJECT_CLASS(baul_image_rotator_parent_class)->finalize(object);
}

static void
baul_image_rotator_set_property (GObject      *object,
                        guint         property_id,
                        const GValue *value,
                        GParamSpec   *pspec)
{
	BaulImageRotator *dialog = BAUL_IMAGE_ROTATOR (object);
	BaulImageRotatorPrivate *priv = baul_image_rotator_get_instance_private (dialog);

	switch (property_id) {
	case PROP_FILES:
		priv->files = g_value_get_pointer (value);
		priv->images_total = g_list_length (priv->files);
		break;
	default:
		/* We don't have any other property... */
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object,property_id,pspec);
		break;
	}
}

static void
baul_image_rotator_get_property (GObject      *object,
                        guint         property_id,
                        GValue       *value,
                        GParamSpec   *pspec)
{
	BaulImageRotator *self = BAUL_IMAGE_ROTATOR (object);
	BaulImageRotatorPrivate *priv = baul_image_rotator_get_instance_private (self);

	switch (property_id) {
	case PROP_FILES:
		g_value_set_pointer (value, priv->files);
		break;
	default:
		/* We don't have any other property... */
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object,property_id,pspec);
		break;
	}
}

static void
baul_image_rotator_class_init(BaulImageRotatorClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);
	GParamSpec *files_param_spec;

	object_class->finalize = baul_image_rotator_finalize;
	object_class->set_property = baul_image_rotator_set_property;
	object_class->get_property = baul_image_rotator_get_property;

	files_param_spec = g_param_spec_pointer ("files",
	"Files",
	"Set selected files",
	G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);

	g_object_class_install_property (object_class,
	PROP_FILES,
	files_param_spec);
}

static void run_op (BaulImageRotator *rotator);

static GFile *
baul_image_rotator_transform_filename (BaulImageRotator *rotator, GFile *orig_file)
{
	BaulImageRotatorPrivate *priv = baul_image_rotator_get_instance_private (rotator);

	GFile *parent_file, *new_file;
	char *basename, *extension, *new_basename;

	g_return_val_if_fail (G_IS_FILE (orig_file), NULL);

	parent_file = g_file_get_parent (orig_file);

	basename = g_strdup (g_file_get_basename (orig_file));

	extension = g_strdup (strrchr (basename, '.'));
	if (extension != NULL)
		basename[strlen (basename) - strlen (extension)] = '\0';

	new_basename = g_strdup_printf ("%s%s%s", basename,
		priv->suffix == NULL ? ".tmp" : priv->suffix,
		extension == NULL ? "" : extension);
	g_free (basename);
	g_free (extension);

	new_file = g_file_get_child (parent_file, new_basename);

	g_object_unref (parent_file);
	g_free (new_basename);

	return new_file;
}

static void
op_finished (GPid pid, gint status, gpointer data)
{
	BaulImageRotator *rotator = BAUL_IMAGE_ROTATOR (data);
	BaulImageRotatorPrivate *priv = baul_image_rotator_get_instance_private (rotator);

	gboolean retry = TRUE;

	BaulFileInfo *file = BAUL_FILE_INFO (priv->files->data);

	if (status != 0) {
		/* rotating failed */
		char *name = baul_file_info_get_name (file);

		CtkWidget *msg_dialog = ctk_message_dialog_new (GTK_WINDOW (priv->progress_dialog),
			GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR,
			GTK_BUTTONS_NONE,
			"'%s' cannot be rotated. Check whether you have permission to write to this folder.",
			name);
		g_free (name);

		ctk_dialog_add_button (GTK_DIALOG (msg_dialog), _("_Skip"), 1);
		ctk_dialog_add_button (GTK_DIALOG (msg_dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);
		ctk_dialog_add_button (GTK_DIALOG (msg_dialog), _("_Retry"), 0);
		ctk_dialog_set_default_response (GTK_DIALOG (msg_dialog), 0);

		int response_id = ctk_dialog_run (GTK_DIALOG (msg_dialog));
		ctk_widget_destroy (msg_dialog);
		if (response_id == 0) {
			retry = TRUE;
		} else if (response_id == GTK_RESPONSE_CANCEL) {
			priv->cancelled = TRUE;
		} else if (response_id == 1) {
			retry = FALSE;
		}

	} else if (priv->suffix == NULL) {
		/* rotate image in place */
		GFile *orig_location = baul_file_info_get_location (file);
		GFile *new_location = baul_image_rotator_transform_filename (rotator, orig_location);
		g_file_move (new_location, orig_location, G_FILE_COPY_OVERWRITE, NULL, NULL, NULL, NULL);
		g_object_unref (orig_location);
		g_object_unref (new_location);
	}

	if (status == 0 || !retry) {
		/* image has been successfully rotated (or skipped) */
		priv->images_rotated++;
		priv->files = priv->files->next;
	}

	if (!priv->cancelled && priv->files != NULL) {
		/* process next image */
		run_op (rotator);
	} else {
		/* cancel/terminate operation */
		ctk_widget_destroy (priv->progress_dialog);
	}
}

static void
run_op (BaulImageRotator *rotator)
{
	BaulImageRotatorPrivate *priv = baul_image_rotator_get_instance_private (rotator);

	g_return_if_fail (priv->files != NULL);

	BaulFileInfo *file = BAUL_FILE_INFO (priv->files->data);

	GFile *orig_location = baul_file_info_get_location (file);
	char *filename = g_file_get_path (orig_location);
	GFile *new_location = baul_image_rotator_transform_filename (rotator, orig_location);
	char *new_filename = g_file_get_path (new_location);
	g_object_unref (orig_location);
	g_object_unref (new_location);

	/* FIXME: check whether new_uri already exists and provide "Replace _All", "_Skip", and "_Replace" options */

	gchar *argv[8];
	argv[0] = "/usr/bin/convert";
	argv[1] = filename;
	argv[2] = "-rotate";
	argv[3] = priv->angle;
	argv[4] = "-orient";
	argv[5] = "TopLeft";
	argv[6] = new_filename;
	argv[7] = NULL;

	pid_t pid;

	if (!g_spawn_async (NULL, argv, NULL, G_SPAWN_DO_NOT_REAP_CHILD, NULL, NULL, &pid, NULL)) {
		// FIXME: error handling
		return;
	}

	g_free (filename);
	g_free (new_filename);

	g_child_watch_add (pid, op_finished, rotator);

	char *tmp;

	ctk_progress_bar_set_fraction (GTK_PROGRESS_BAR (priv->progress_bar), (double) (priv->images_rotated + 1) / priv->images_total);
	tmp = g_strdup_printf (_("Rotating image: %d of %d"), priv->images_rotated + 1, priv->images_total);
	ctk_progress_bar_set_text (GTK_PROGRESS_BAR (priv->progress_bar), tmp);
	g_free (tmp);

	char *name = baul_file_info_get_name (file);
	tmp = g_strdup_printf (_("<i>Rotating \"%s\"</i>"), name);
	g_free (name);
	ctk_label_set_markup (GTK_LABEL (priv->progress_label), tmp);
	g_free (tmp);

}

static void
baul_image_rotator_response_cb (CtkDialog *dialog, gint response_id, gpointer user_data)
{
	BaulImageRotator *rotator = BAUL_IMAGE_ROTATOR (user_data);
	BaulImageRotatorPrivate *priv = baul_image_rotator_get_instance_private (rotator);

	if (response_id == GTK_RESPONSE_OK) {
		if (ctk_toggle_button_get_active (GTK_TOGGLE_BUTTON (priv->append_radiobutton))) {
			if (strlen (ctk_entry_get_text (priv->name_entry)) == 0) {
				CtkWidget *msg_dialog = ctk_message_dialog_new (GTK_WINDOW (dialog),
					GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR,
					GTK_BUTTONS_OK, _("Please enter a valid filename suffix!"));
				ctk_dialog_run (GTK_DIALOG (msg_dialog));
				ctk_widget_destroy (msg_dialog);
				return;
			}
			priv->suffix = g_strdup (ctk_entry_get_text (priv->name_entry));
		}
		if (ctk_toggle_button_get_active (GTK_TOGGLE_BUTTON (priv->default_angle_radiobutton))) {
			switch (ctk_combo_box_get_active (GTK_COMBO_BOX (priv->angle_combobox))) {
			case 0:
				priv->angle = g_strdup_printf ("90");
				break;
			case 1:
				priv->angle = g_strdup_printf ("-90");
				break;
			case 2:
				priv->angle = g_strdup_printf ("180");
				break;
			default:
				g_assert_not_reached ();
			}
		} else if (ctk_toggle_button_get_active (GTK_TOGGLE_BUTTON (priv->custom_angle_radiobutton))) {
			priv->angle = g_strdup_printf ("%d", (int) ctk_spin_button_get_value (priv->angle_spinbutton));
		} else {
			g_assert_not_reached ();
		}

		run_op (rotator);
	}

	ctk_widget_destroy (GTK_WIDGET (dialog));
}

static void
baul_image_rotator_init(BaulImageRotator *rotator)
{
	BaulImageRotatorPrivate *priv = baul_image_rotator_get_instance_private (rotator);

	CtkBuilder *ui;
	gchar      *path;
	guint       result;
	GError     *err = NULL;

	/* Let's create our ctkbuilder and load the xml file */
	ui = ctk_builder_new ();
	ctk_builder_set_translation_domain (ui, GETTEXT_PACKAGE);
	path = g_build_filename (DATADIR, PACKAGE, "baul-image-rotate.ui", NULL);
	result = ctk_builder_add_from_file (ui, path, &err);
	g_free (path);

	/* If we're unable to load the xml file */
	if (result == 0) {
		g_warning ("%s", err->message);
		g_error_free (err);
		return;
	}

	/* Grab some widgets */
	priv->rotate_dialog = GTK_DIALOG (ctk_builder_get_object (ui, "rotate_dialog"));
	priv->default_angle_radiobutton =
		GTK_RADIO_BUTTON (ctk_builder_get_object (ui, "default_angle_radiobutton"));
	priv->angle_combobox = GTK_COMBO_BOX (ctk_builder_get_object (ui, "angle_combobox"));
	priv->custom_angle_radiobutton =
		GTK_RADIO_BUTTON (ctk_builder_get_object (ui, "custom_angle_radiobutton"));
	priv->angle_spinbutton =
		GTK_SPIN_BUTTON (ctk_builder_get_object (ui, "angle_spinbutton"));
	priv->append_radiobutton =
		GTK_RADIO_BUTTON (ctk_builder_get_object (ui, "append_radiobutton"));
	priv->name_entry = GTK_ENTRY (ctk_builder_get_object (ui, "name_entry"));
	priv->inplace_radiobutton =
		GTK_RADIO_BUTTON (ctk_builder_get_object (ui, "inplace_radiobutton"));

	/* Set default value for combobox */
	ctk_combo_box_set_active  (priv->angle_combobox, 0); /* 90° clockwise */

	/* Connect the signal */
	g_signal_connect (G_OBJECT (priv->rotate_dialog), "response",
			  (GCallback) baul_image_rotator_response_cb,
			  rotator);
}

BaulImageRotator *
baul_image_rotator_new (GList *files)
{
	return g_object_new (BAUL_TYPE_IMAGE_ROTATOR, "files", files, NULL);
}

void
baul_image_rotator_show_dialog (BaulImageRotator *rotator)
{
	BaulImageRotatorPrivate *priv = baul_image_rotator_get_instance_private (rotator);

	ctk_widget_show (GTK_WIDGET (priv->rotate_dialog));
}
