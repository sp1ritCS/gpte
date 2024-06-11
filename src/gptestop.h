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

#ifndef __GPTESTOP_H__
#define __GPTESTOP_H__

#include <glib-object.h>
#include <gptejavaobject.h>

#include <gptelocation.h>

G_BEGIN_DECLS

#define GPTE_TYPE_POSITION (gpte_position_get_type())
GType gpte_position_get_type(void);

/**
 * GptePosition:
 * @name: the name of the position, commonly refering to the platform
 * @section: (nullable): a subdivision of @name if it exists, %NULL otherwise
 *
 * A position refers to a specific location of a station.
 */
typedef struct {
	gchar* name;
	gchar* section;
} GptePosition;

/**
 * gpte_position_copy:
 * @self: the instance to duplicate
 *
 * Duplicates an instance of [struct@Gpte.Position].
 * Returns: (transfer full): the duplicated instance
 */
GptePosition* gpte_position_copy(const GptePosition* self);

/**
 * gpte_position_free:
 * @self: the position
 *
 * Frees a position from memory.
 *
 * After this function was called, you may not access the structure
 * anymore.
 */
void gpte_position_free(GptePosition* self);

/**
 * gpte_position_get_name:
 * @self: the position
 *
 * Gets the name of the position.
 *
 * This is commonly a platform of a station.
 * Returns: (transfer none): the name
 */
const gchar* gpte_position_get_name(const GptePosition* self);

/**
 * gpte_position_get_section:
 * @self: the position
 *
 * Gets the subdivision of [method@Gpte.Position.get_name] if it exists.
 *
 * Returns: (transfer none) (nullable): the subdivision or %NULL
 */
const gchar* gpte_position_get_section(const GptePosition* self);

G_DEFINE_AUTOPTR_CLEANUP_FUNC(GptePosition, gpte_position_free)

/**
 * GpteStop:
 * Represents a singular stop of a public trip leg.
 */

#define GPTE_TYPE_STOP (gpte_stop_get_type())
G_DECLARE_FINAL_TYPE (GpteStop, gpte_stop, GPTE, STOP, GpteJavaObject)

/**
 * gpte_stop_get_location:
 * @self: the stop
 *
 * Gets the location where the stop is located.
 *
 * Returns: (transfer none): the location
 */
GpteLocation* gpte_stop_get_location(GpteStop* self);

/**
 * gpte_stop_get_arrival_time:
 * @self: the stop
 * @is_predicted: (out) (nullable): will be set to %TRUE if the time is not the planned time
 *
 * Gets the time at which the trip will arrive at this stop.
 *
 * Returns: (transfer none): the arrival time
 */
GDateTime* gpte_stop_get_arrival_time(GpteStop* self, gboolean* is_predicted);
/**
 * gpte_stop_get_departure_time:
 * @self: the stop
 * @is_predicted: (out) (nullable): will be set to %TRUE if the time is not the planned time

 *
 * Gets the time at which the trip will depart from this stop.
 *
 * Returns: (transfer none): the departure time
 */
GDateTime* gpte_stop_get_departure_time(GpteStop* self, gboolean* is_predicted);

/**
 * gpte_stop_get_arrival_delay:
 * @self: the stop
 *
 * Gets the arrival delay (in milliseconds).
 *
 * Returns: (transfer none) (nullable): arrival delay
 */
const glong* gpte_stop_get_arrival_delay(GpteStop* self);
/**
 * gpte_stop_get_departure_delay:
 * @self: the stop
 *
 * Gets the departure delay (in milliseconds).
 *
 * Returns: (transfer none) (nullable): departure delay
 */
const glong* gpte_stop_get_departure_delay(GpteStop* self);

/**
 * gpte_stop_get_arrival_position:
 * @self: the stop
 * @is_predicted: (out) (nullable): will be set to %TRUE if the position is not the planned position
 *
 * Gets the position where this trip leg will arrive at this stop.
 *
 * Returns: (transfer none): arrival position
 */
const GptePosition* gpte_stop_get_arrival_position(GpteStop* self, gboolean* is_predicted);
/**
 * gpte_stop_get_departure_position:
 * @self: the stop
 * @is_predicted: (out) (nullable): will be set to %TRUE if the position is not the planned position
 *
 * Gets the position where this trip leg will depart from this stop.
 *
 * Returns: (transfer none): departure position
 */
const GptePosition* gpte_stop_get_departure_position(GpteStop* self, gboolean* is_predicted);

G_END_DECLS

#endif // __GPTESTOP_H__
