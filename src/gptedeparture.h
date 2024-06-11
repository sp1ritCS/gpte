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

#ifndef __GPTEDEPARTURE_H__
#define __GPTEDEPARTURE_H__

#include <glib-object.h>
#include <gptejavaobject.h>

#include <gpteline.h>
#include <gptegeo.h>
#include <gptelocation.h>

G_BEGIN_DECLS

/**
 * GpteDeparture:
 * Represents a singular departure of a line at a specific station.
 */

#define GPTE_TYPE_DEPARTURE (gpte_departure_get_type())
G_DECLARE_FINAL_TYPE (GpteDeparture, gpte_departure, GPTE, DEPARTURE, GpteJavaObject)

/**
 * gpte_departure_get_planned_time:
 * @self: the departure
 *
 * Gets the time the departure was planned to take off.
 * Returns: (nullable) (transfer full): planned departure time
 */
GDateTime* gpte_departure_get_planned_time(GpteDeparture* self);

/**
 * gpte_departure_get_predicted_time:
 * @self: the departure
 *
 * Gets the time the departure is predicted to take off.
 * Returns: (nullable) (transfer full): predicted departure time
 */
GDateTime* gpte_departure_get_predicted_time(GpteDeparture* self);

/**
 * gpte_departure_get_time:
 * @self: the departure
 *
 * Gets the time the departure will take off.
 * Returns: (transfer full): actual departure time
 */
GDateTime* gpte_departure_get_time(GpteDeparture* self);

/**
 * gpte_departure_get_line:
 * @self: the departure
 *
 * Gets the [class@Gpte.Line] that will depart based on this departure.
 * Returns: (transfer full): the departing line
 */
GpteLine* gpte_departure_get_line(GpteDeparture* self);

/**
 * gpte_departure_get_position:
 * @self: the departure
 *
 * Gets the position where the product will depart at.
 * Returns: (nullable) (transfer full): the position of the departure
 */
GpteGeoPoint* gpte_departure_get_position(GpteDeparture* self);

/**
 * gpte_departure_get_destination:
 * @self: the departure
 *
 * Gets the terminus for the departing line.
 * Returns: (nullable) (transfer full): the terminus
 */
GpteLocation* gpte_departure_get_destination(GpteDeparture* self);

/**
 * gpte_departure_get_message:
 * @self: the departure
 *
 * Gets the custom free-text message assigned to this departure, if such
 * a message exists.
 *
 * Returns: (nullable) (transfer full): the message or %NULL
 */
gchar* gpte_departure_get_message(GpteDeparture* self);

G_END_DECLS

#endif // __GPTEDEPARTURE_H__
