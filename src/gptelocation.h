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

#ifndef __GPTELOCATION_H__
#define __GPTELOCATION_H__

#include <glib-object.h>
#include <gptejavaobject.h>

#include <gptegeo.h>
#include <gpteproducts.h>

G_BEGIN_DECLS

/**
 * GpteLocationType:
 * @GPTE_LOCATION_ANY: any/unknown location
 * @GPTE_LOCATION_STATION: location is a station
 * @GPTE_LOCATION_POI: location is a point of interest
 * @GPTE_LOCATION_ADDRESS: location is an address
 * @GPTE_LOCATION_COORD: location is a coordinate
 *
 * The specific kind of a location.
 *
 *
 * See [flags@Gpte.Locations] for the bitset variant.
 */

#define GPTE_TYPE_LOCATION_TYPE (gpte_location_type_get_type())
GType gpte_location_type_get_type(void);

typedef enum {
	GPTE_LOCATION_ANY,
	GPTE_LOCATION_STATION,
	GPTE_LOCATION_POI,
	GPTE_LOCATION_ADDRESS,
	GPTE_LOCATION_COORD
} GpteLocationType;


/**
 * GpteLocations:
 * @GPTE_LOCATIONS_ANY: any/unknown locations
 * @GPTE_LOCATIONS_STATION: station locations
 * @GPTE_LOCATIONS_POI: point of interest locations
 * @GPTE_LOCATIONS_ADDRESS: address locations
 * @GPTE_LOCATIONS_COORD: coordinate locations
 *
 * Bitset of [enum@Gpte.LocationType].
 */

#define GPTE_TYPE_LOCATIONS (gpte_locations_get_type())
GType gpte_locations_get_type(void);

typedef enum {
	GPTE_LOCATIONS_ANY = 1 << 0,
	GPTE_LOCATIONS_STATION = 1 << 1,
	GPTE_LOCATIONS_POI = 1 << 2,
	GPTE_LOCATIONS_ADDRESS = 1 << 3,
	GPTE_LOCATIONS_COORD = 1 << 4
} GpteLocations;


/**
 * GpteLocation:
 * Specifies a well-known location.
 */

#define GPTE_TYPE_LOCATION (gpte_location_get_type())
G_DECLARE_FINAL_TYPE (GpteLocation, gpte_location, GPTE, LOCATION, GpteJavaObject)

/**
 * gpte_location_from_coords:
 * @vm: the JVM to create the location in
 * @point: the coordinates of the location
 *
 * Creates a new [class@Gpte.Location] from angular coordinates.
 * Returns: (transfer full): the location at %point
 */
GpteLocation* gpte_location_from_coords(GpteJvm* vm, const GpteGeoPoint* point);

/**
 * gpte_location_get_coords:
 * @self: the location
 *
 * Gets the coordinates of this location (if known).
 *
 * Returns: (transfer none) (nullable): coordnates of @self or %NULL
 */
const GpteGeoPoint* gpte_location_get_coords(GpteLocation* self);

/**
 * gpte_location_get_id:
 * @self: the location
 *
 * Gets a unique identifier for this location.
 *
 * Note: this identifier is unique per provider. While it might work for
 * others, it likely doesn't.
 *
 * Returns: (transfer none) (nullable): unique identifier or %NULL
 */
const char* gpte_location_get_id(GpteLocation* self);

/**
 * gpte_location_get_name:
 * @self: the location
 *
 * Gets the human readable name for this location.
 *
 * Returns: (transfer none) (nullable): human readable name
 */
const char* gpte_location_get_name(GpteLocation* self);

/**
 * gpte_location_get_place:
 * @self: the location
 *
 * Gets the place where the location is located as human readable text.
 *
 * Returns: (transfer none) (nullable): the place
 */
const char* gpte_location_get_place(GpteLocation* self);

/**
 * gpte_location_get_products:
 * @self: the location
 *
 * Gets the set of products that stop at this location.
 *
 * Returns: set of [flags@Gpte.Products]
 */
GpteProducts gpte_location_get_products(GpteLocation* self);

/**
 * gpte_location_get_location_type:
 * @self: the location
 *
 * Gets the kind of this location.
 *
 * Returns: the location kind
 */
GpteLocationType gpte_location_get_location_type(GpteLocation* self);

G_END_DECLS

#endif // __GPTELOCATION_H__
