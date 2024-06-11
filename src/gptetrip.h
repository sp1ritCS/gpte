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

#ifndef __GPTETRIP_H__
#define __GPTETRIP_H__

#include <glib-object.h>
#include <gptejavaobject.h>

#include <gio/gio.h>
#include <gptetripleg.h>

#include <gptelocation.h>
#include <gpteproducts.h>
#include <gptefare.h>

G_BEGIN_DECLS

/**
 * GpteTrip:
 * Represents a unique trip from a departure point to a destination.
 */

#define GPTE_TYPE_TRIP (gpte_trip_get_type())
G_DECLARE_FINAL_TYPE (GpteTrip, gpte_trip, GPTE, TRIP, GpteJavaObject)

/**
 * gpte_trip_from:
 * @self: the trip
 *
 * Gets the departure locaiton of the trip.
 *
 * Returns: (transfer none): departure location
 */
GpteLocation* gpte_trip_from(GpteTrip* self);

/**
 * gpte_trip_to:
 * @self: the trip
 *
 * Gets the destination of @self.
 *
 * Returns: (transfer none): the destination
 */
GpteLocation* gpte_trip_to(GpteTrip* self);

/**
 * gpte_trip_get_legs:
 * @self: the trip
 *
 * The list of legs the trip is split into.
 *
 * Returns: (transfer none): list of legs as #GListModel of [class@Gpte.TripLeg]
 */
GListModel* gpte_trip_get_legs(GpteTrip* self);

/**
 * gpte_trip_get_num_changes:
 * @self: the trip
 *
 * Gets the number of changes on the trip.
 *
 * The number of changes for a Trip consisting of only individual Legs is -1.
 * Returns: number of changes on the trip
 */
gint gpte_trip_get_num_changes(GpteTrip* self);

/**
 * gpte_trip_get_first_public_leg:
 * @self: the trip
 *
 * Gets the first public leg of the trip if it exists, %NULL otherwise.
 *
 * Returns: (transfer none) (nullable): the first public leg or %NULL
 */
GpteTripPublic* gpte_trip_get_first_public_leg(GpteTrip* self);
/**
 * gpte_trip_get_last_public_leg:
 * @self: the trip
 *
 * Gets the last public leg of the trip if it exists, %NULL otherwise.
 *
 * Returns: (transfer none) (nullable): the last public leg or %NULL
 */
GpteTripPublic* gpte_trip_get_last_public_leg(GpteTrip* self);

/**
 * gpte_trip_get_first_departure_time:
 * @self: the trip
 *
 * Gets the departure time of the first relevant leg.
 *
 * Returns: (transfer none): first departure time
 */
GDateTime* gpte_trip_get_first_departure_time(GpteTrip* self);

/**
 * gpte_trip_get_last_arrival_time:
 * @self: the trip
 *
 * Gets the arrival time of the last relevant leg.
 *
 * Returns: (transfer none): last arrival time
 */
GDateTime* gpte_trip_get_last_arrival_time(GpteTrip* self);

/**
 * gpte_trip_get_duration:
 * @self: the trip
 *
 * Gets the duration of whole trip in milliseconds, including leading
 * and trailing individual legs.
 *
 * Returns: duration of @self in milliseconds
 */
glong gpte_trip_get_duration(GpteTrip* self);

/**
 * gpte_trip_get_min_time:
 * @self: the trip
 *
 * Gets the minimum time occurring in this trip.
 *
 * Returns: (transfer none): minimum time
 */
GDateTime* gpte_trip_get_min_time(GpteTrip* self);

/**
 * gpte_trip_get_max_time:
 * @self: the trip
 *
 * Gets the maximum time occurring in this trip.
 *
 * Returns: (transfer none): maximum time
 */
GDateTime* gpte_trip_get_max_time(GpteTrip* self);

/**
 * gpte_trip_is_travelable:
 * @self: the trip
 *
 * Tests if it looks like the trip can be traveled or if legs overlap,
 * important departures or arrivals are cancelled and that sort of
 * thing.
 *
 * Returns: %TRUE if the trip can be traveled, %FALSE otherwise
 */
gboolean gpte_trip_is_travelable(GpteTrip* self);

/**
 * gpte_trip_get_get_products:
 * @self: the trip
 *
 * Gets the type of products that are used on this trip.
 * Returns: the products in use
 */
GpteProducts gpte_trip_get_products(GpteTrip* self);

/**
 * gpte_trip_get_fares:
 * @self: the trip
 *
 * Gets the list of [class@Gpte.Fare] for this trip.
 *
 * Returns: (transfer none): list of fares as #GListModel of [class@Gpte.Fare]
 */
GListModel* gpte_trip_get_fares(GpteTrip* self);

G_END_DECLS

#endif // __GPTETRIP_H__
