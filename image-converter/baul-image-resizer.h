/*
 *  baul-image-resize-dialog.h
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

#ifndef CAJA_IMAGE_RESIZER_H
#define CAJA_IMAGE_RESIZER_H

#include <glib-object.h>

G_BEGIN_DECLS

#define CAJA_TYPE_IMAGE_RESIZER         (baul_image_resizer_get_type ())
#define CAJA_IMAGE_RESIZER(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), CAJA_TYPE_IMAGE_RESIZER, BaulImageResizer))
#define CAJA_IMAGE_RESIZER_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), CAJA_TYPE_IMAGE_RESIZER, BaulImageResizerClass))
#define CAJA_IS_IMAGE_RESIZER(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), CAJA_TYPE_IMAGE_RESIZER))
#define CAJA_IS_IMAGE_RESIZER_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), CAJA_TYPE_IMAGE_RESIZER))
#define CAJA_IMAGE_RESIZER_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), CAJA_TYPE_IMAGE_RESIZER, BaulImageResizerClass))

typedef struct _BaulImageResizer BaulImageResizer;
typedef struct _BaulImageResizerClass BaulImageResizerClass;

struct _BaulImageResizer {
	GObject parent;
};

struct _BaulImageResizerClass {
	GObjectClass parent_class;
	/* Add Signal Functions Here */
};

GType baul_image_resizer_get_type (void);
BaulImageResizer *baul_image_resizer_new (GList *files);
void baul_image_resizer_show_dialog (BaulImageResizer *dialog);

G_END_DECLS

#endif /* CAJA_IMAGE_RESIZER_H */
