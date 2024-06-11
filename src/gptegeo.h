/*
 * gpte - GObject bindings for public-transport-enabler
 * Copyright (C) 2024  Florian "sp1rit" <sp1rit@disroot.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef __GPTEGEO_H__
#define __GPTEGEO_H__

#include <glib-object.h>

G_BEGIN_DECLS

/**
 * GpteGeoPoint:
 * This structure represents a unique point on earth using angular
 * coordinates.
 */

#define GPTE_TYPE_GEO_POINT (gpte_geo_point_get_type())
GType gpte_geo_point_get_type(void);

typedef struct {
	gdouble lat;
	gdouble lon;
} GpteGeoPoint;

/**
 * gpte_geo_point_new:
 * @lat: the latitude of the point
 * @lon: the longitude of the point
 *
 * Creates a new #GpteGeoPoint.
 * Returns: (transfer full): a new point
 */
GpteGeoPoint* gpte_geo_point_new(gdouble lat, gdouble lon);

/**
 * gpte_geo_point_copy:
 * @self: the point to copy
 *
 * Copy a #GpteGeoPoint into a new owned structure.
 * Returns: (transfer full): the copied point
 */
GpteGeoPoint* gpte_geo_point_copy(const GpteGeoPoint* self);

/**
 * gpte_geo_point_free:
 * @self: the point
 *
 * Frees a point from memory.
 *
 * After this function was called, you may not access the point anymore.
 */
void gpte_geo_point_free(GpteGeoPoint* self);

G_DEFINE_AUTOPTR_CLEANUP_FUNC(GpteGeoPoint, gpte_geo_point_free)

G_END_DECLS

#endif // __GPTEGEO_H__
