/* baul-share -- Baul File Sharing Extension
 *
 * Sebastien Estienne <sebastien.estienne@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * (C) Copyright 2005 Ethium, Inc.
 */

#ifndef BAUL_SHARE_H
#define BAUL_SHARE_H

#include <glib-object.h>

G_BEGIN_DECLS

/* Declarations for the Share extension object.  This object will be
 * instantiated by baul.  It implements the GInterfaces
 * exported by libbaul. */


typedef struct _BaulShare      BaulShare;
typedef struct _BaulShareClass BaulShareClass;

struct _BaulShare {
	GObject parent_slot;
};

struct _BaulShareClass {
	GObjectClass parent_slot;

	/* No extra class members */
};


typedef struct _BaulShareData      BaulShareData;

struct _BaulShareData {
  gchar		*fullpath;
  gchar		*section;
  BaulFileInfo *fileinfo;
};

G_END_DECLS

typedef enum {
  BAUL_SHARE_NOT_SHARED,
  BAUL_SHARE_SHARED_RO,
  BAUL_SHARE_SHARED_RW
} BaulShareStatus;

#endif

