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

#ifndef BAUL_WALLPAPER_EXTENSION_H
#define BAUL_WALLPAPER_EXTENSION_H

#include <glib-object.h>

G_BEGIN_DECLS

#define BAUL_TYPE_CWE  (baul_cwe_get_type ())
#define BAUL_CWE(o)    (G_TYPE_CHECK_INSTANCE_CAST ((o), BAUL_TYPE_CWE, BaulCwe))
#define BAUL_IS_CWE(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), BAUL_TYPE_CWE))

typedef struct _BaulCwe      BaulCwe;
typedef struct _BaulCweClass BaulCweClass;

struct _BaulCwe {
	GObject __parent;
};

struct _BaulCweClass {
	GObjectClass __parent;
};

GType baul_cwe_get_type      (void);
void  baul_cwe_register_type (GTypeModule *module);

G_END_DECLS

#endif /* BAUL_WALLPAPER_EXTENSION_H */
