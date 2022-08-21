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

#ifndef CAJA_XATTR_TAGS_EXTENSION_H
#define CAJA_XATTR_TAGS_EXTENSION_H

#include <glib-object.h>

G_BEGIN_DECLS

#define CAJA_TYPE_XATTR_TAGS  (baul_xattr_tags_get_type ())
#define CAJA_XATTR_TAGS(o)    (G_TYPE_CHECK_INSTANCE_CAST ((o), CAJA_TYPE_XATTR_TAGS, baulXattrTags))
#define CAJA_IS_XATTR_TAGS(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), CAJA_TYPE_XATTR_TAGS))

typedef struct _BaulXattrTags      BaulXattrTags;
typedef struct _BaulXattrTagsClass BaulXattrTagsClass;

struct _BaulXattrTags {
	GObject __parent;
};

struct _BaulXattrTagsClass {
	GObjectClass __parent;
};

GType baul_xattr_tags_get_type(void);
void  baul_xattr_tags_register_type(GTypeModule *module);

G_END_DECLS

#endif /* CAJA_XATTR_TAGS_EXTENSION_H */
