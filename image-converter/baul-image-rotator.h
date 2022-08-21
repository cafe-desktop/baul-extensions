/*
 *  baul-image-rotator.h
 *
 *  Copyright (C) 2004-2006 Jürg Billeter
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

#ifndef BAUL_IMAGE_ROTATOR_H
#define BAUL_IMAGE_ROTATOR_H

#include <glib-object.h>

G_BEGIN_DECLS

#define BAUL_TYPE_IMAGE_ROTATOR         (baul_image_rotator_get_type ())
#define BAUL_IMAGE_ROTATOR(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), BAUL_TYPE_IMAGE_ROTATOR, BaulImageRotator))
#define BAUL_IMAGE_ROTATOR_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), BAUL_TYPE_IMAGE_ROTATOR, BaulImageRotatorClass))
#define BAUL_IS_IMAGE_ROTATOR(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), BAUL_TYPE_IMAGE_ROTATOR))
#define BAUL_IS_IMAGE_ROTATOR_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), BAUL_TYPE_IMAGE_ROTATOR))
#define BAUL_IMAGE_ROTATOR_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), BAUL_TYPE_IMAGE_ROTATOR, BaulImageRotatorClass))

typedef struct _BaulImageRotator BaulImageRotator;
typedef struct _BaulImageRotatorClass BaulImageRotatorClass;

struct _BaulImageRotator {
	GObject parent;
};

struct _BaulImageRotatorClass {
	GObjectClass parent_class;
	/* Add Signal Functions Here */
};

GType baul_image_rotator_get_type (void);
BaulImageRotator *baul_image_rotator_new (GList *files);
void baul_image_rotator_show_dialog (BaulImageRotator *dialog);

G_END_DECLS

#endif /* BAUL_IMAGE_ROTATOR_H */
