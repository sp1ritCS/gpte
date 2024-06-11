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

#ifndef __GPTETRIPLEG_H__
#define __GPTETRIPLEG_H__

#include <glib-object.h>
#include <gptejavaobject.h>

#include <gio/gio.h>

#include <gptelocation.h>
#include <gpteline.h>
#include <gptestop.h>

G_BEGIN_DECLS

/**
 * GpteTripIndividualType:
 * @GPTE_TRIP_INDIVIDUAL_NULL: unset/unknown individual trip type
 * @GPTE_TRIP_INDIVIDUAL_BIKE: bike trip
 * @GPTE_TRIP_INDIVIDUAL_CAR: car trip
 * @GPTE_TRIP_INDIVIDUAL_CHECK_IN: check-in section
 * @GPTE_TRIP_INDIVIDUAL_CHECK_OUT: check-out section
 * @GPTE_TRIP_INDIVIDUAL_TRANSFER: transfer section
 * @GPTE_TRIP_INDIVIDUAL_WALK: walk trip
 *
 * The indivual trip type describes the "transportation method" of an
 * individual trip.
 */

#define GPTE_TYPE_TRIP_INDIVIDUAL_TYPE (gpte_trip_individual_type_get_type())
GType gpte_trip_individual_type_get_type(void);

typedef enum {
	GPTE_TRIP_INDIVIDUAL_NULL = 0,
	GPTE_TRIP_INDIVIDUAL_BIKE,
	GPTE_TRIP_INDIVIDUAL_CAR,
	GPTE_TRIP_INDIVIDUAL_CHECK_IN,
	GPTE_TRIP_INDIVIDUAL_CHECK_OUT,
	GPTE_TRIP_INDIVIDUAL_TRANSFER,
	GPTE_TRIP_INDIVIDUAL_WALK
} GpteTripIndividualType;


/**
 * GpteTripLeg:
 * Represents a unqiue leg of a Trip.
 *
 *
 * Should actually either be [class@Gpte.TripIndividual] or
 * [class@Gpte.TripPublic].
 */

#define GPTE_TYPE_TRIP_LEG (gpte_trip_leg_get_type())
G_DECLARE_DERIVABLE_TYPE (GpteTripLeg, gpte_trip_leg, GPTE, TRIP_LEG, GpteJavaObject)

struct _GpteTripLegClass {
	GpteJavaObjectClass parent_class;
};

/**
 * gpte_trip_leg_get_departure:
 * @self: the trip leg
 *
 * Gets the location where this leg will depart from.
 *
 * Returns: (transfer none): departing location
 */
GpteLocation* gpte_trip_leg_get_departure(GpteTripLeg* self);

/**
 * gpte_trip_leg_get_arrival:
 * @self: the trip leg
 *
 * Gets the location where this leg will arrive at.
 *
 * Returns: (transfer none): arrival location
 */
GpteLocation* gpte_trip_leg_get_arrival(GpteTripLeg* self);


/**
 * gpte_trip_leg_get_path:
 * @self: the trip leg
 *
 * Gets the path that this leg will move at.
 *
 * In cases this returns %NULL, you might want to use the path from
 * [method@Gpte.TripLeg.get_departure] to [method@Gpte.TripLeg.get_arrival].
 *
 * Returns: (transfer none) (element-type Gpte.GeoPoint) (nullable): list of coordinates making up the path, or %NULL
 */
GArray* gpte_trip_leg_get_path(GpteTripLeg* self);


/**
 * gpte_trip_leg_get_departure_time:
 * @self: the trip leg
 *
 * Gets the coarse departure time.
 *
 * Returns: (transfer none): the departure time
 */
GDateTime* gpte_trip_leg_get_departure_time(GpteTripLeg* self);

/**
 * gpte_trip_leg_get_arrival_time:
 * @self: the trip leg
 *
 * Gets the coarse arrival time.
 *
 * Returns: (transfer none): the arrival time
 */
GDateTime* gpte_trip_leg_get_arrival_time(GpteTripLeg* self);

/**
 * gpte_trip_leg_get_min_time:
 * @self: the trip leg
 *
 * Gets the minimum time occurring in this leg.
 *
 * Returns: (transfer none): minimum time
 */
GDateTime* gpte_trip_leg_get_min_time(GpteTripLeg* self);

/**
 * gpte_trip_leg_get_max_time:
 * @self: the trip leg
 *
 * Gets the maximum time occurring in this leg.
 *
 * Returns: (transfer none): maximum time
 */

GDateTime* gpte_trip_leg_get_max_time(GpteTripLeg* self);


/**
 * GpteTripIndividual:
 * Represents a [class@Gpte.TripLeg] taken using a form of "individual"
 * transport (this includes walking).
 *
 *
 * See [enum@Gpte.TripIndividualType] for all possible individual
 * transport types.
 */

#define GPTE_TYPE_TRIP_INDIVIDUAL (gpte_trip_individual_get_type())
G_DECLARE_FINAL_TYPE(GpteTripIndividual, gpte_trip_individual, GPTE, TRIP_INDIVIDUAL, GpteTripLeg)

/**
 * gpte_trip_individual_type:
 * @self: the individual trip leg
 *
 * Gets the type of individual transportation taken on this leg.
 *
 * Returns: type of individual transportation
 */
GpteTripIndividualType gpte_trip_individual_type(GpteTripIndividual* self);

/**
 * gpte_trip_individual_get_distance:
 * @self: the individual trip leg
 *
 * Gets the distance of this individual trip leg.
 *
 * Returns: the distance
 */
gint gpte_trip_individual_get_distance(GpteTripIndividual* self);


/**
 * GpteTripPublic:
 * Represents a [class@Gpte.TripLeg] taken using a form of public
 * transport.
 *
 *
 * See [enum@Gpte.ProductCode] for all possible public transport types.
 */

#define GPTE_TYPE_TRIP_PUBLIC (gpte_trip_public_get_type())
G_DECLARE_FINAL_TYPE(GpteTripPublic, gpte_trip_public, GPTE, TRIP_PUBLIC, GpteTripLeg)

/**
 * gpte_trip_public_get_departure:
 * @self: the public trip leg
 *
 * Gets the stop where this leg will depart from.
 *
 * Returns: (transfer none): departing stop
 */
GpteStop* gpte_trip_public_get_departure(GpteTripPublic* self);

/**
 * gpte_trip_public_get_arrival:
 * @self: the public trip leg
 *
 * Gets the stop where this leg will arrive at.
 *
 * Returns: (transfer none): arrival stop
 */
GpteStop* gpte_trip_public_get_arrival(GpteTripPublic* self);

/**
 * gpte_trip_public_get_intermediate:
 * @self: the public trip leg
 *
 * Gets intermediate stops that this leg will stop at.
 *
 * Returns: (transfer none) (nullable): list  of intermediate stops as a #GListModel of [class@Gpte.Stop]
 */
GListModel* gpte_trip_public_get_intermediate(GpteTripPublic* self);

/**
 * gpte_trip_public_get_destination:
 * @self: the public trip leg
 *
 * Gets the terminus of [method@Gpte.TripPublic.get_line] from this trip
 * leg.
 *
 * Returns: (transfer none) (nullable): the terminus of the line or %NULL
 */
GpteLocation* gpte_trip_public_get_destination(GpteTripPublic* self);

/**
 * gpte_trip_public_get_line:
 * @self: the public trip leg
 *
 * Gets the line that is used to traverse this trip leg.
 *
 * Returns: (transfer none): the line
 */
GpteLine* gpte_trip_public_get_line(GpteTripPublic* self);

/**
 * gpte_trip_public_get_message:
 * @self: the public trip leg
 *
 * Gets the custom free-text message assigned to this leg, if such a
 * message exists.
 *
 * Returns: (transfer none) (nullable): the message or %NULL
 */
const gchar* gpte_trip_public_get_message(GpteTripPublic* self);

G_END_DECLS

#endif // __GPTETRIPLEG_H__
