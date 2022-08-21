/*
 *  Baul SendTo extension
 *
 *  Copyright (C) 2005 Roberto Majadas
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

#ifndef BAUL_NSTE_H
#define BAUL_NSTE_H

#include <glib-object.h>

G_BEGIN_DECLS

#define BAUL_TYPE_NSTE  (baul_nste_get_type ())
#define BAUL_NSTE(o)    (G_TYPE_CHECK_INSTANCE_CAST ((o), BAUL_TYPE_NSTE, BaulNste))
#define BAUL_IS_NSTE(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), BAUL_TYPE_NSTE))

typedef struct _BaulNste      BaulNste;
typedef struct _BaulNsteClass BaulNsteClass;

struct _BaulNste {
	GObject __parent;
};

struct _BaulNsteClass {
	GObjectClass __parent;
};

GType baul_nste_get_type      (void);
void  baul_nste_register_type (GTypeModule *module);

G_END_DECLS

#endif /* BAUL_NSTE_H */
