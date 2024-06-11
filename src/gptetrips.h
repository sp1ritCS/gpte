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

#ifndef __GPTETRIPS_H__
#define __GPTETRIPS_H__

#include <glib-object.h>
#include <gptejavaobject.h>

#include <gptetrip.h>

#include <gpteproducts.h>
#include <gptelist.h>

G_BEGIN_DECLS

/**
 * GpteTripAccessibility:
 * @GPTE_ACCESSIBILITY_NEUTRAL: No accessibility demands specified
 * @GPTE_ACCESSIBILITY_LIMITED: Limited accessibility demands
 * @GPTE_ACCESSIBILITY_BARRIER_FREE: Barrier-free accessibility required
 *
 * The accessibility requirement of a trip.
 */

#define GPTE_TYPE_TRIP_ACCESSIBILITY (gpte_trip_accessibility_get_type())
GType gpte_trip_accessibility_get_type(void);

typedef enum {
	GPTE_ACCESSIBILITY_NEUTRAL,
	GPTE_ACCESSIBILITY_LIMITED,
	GPTE_ACCESSIBILITY_BARRIER_FREE
} GpteTripAccessibility;


/**
 * GpteTripFlags:
 * @GPTE_TRIP_FLAG_BIKE: Bicycle transport required
 *
 * Additional query parameters for a trip request.
 */

#define GPTE_TYPE_TRIP_FLAGS (gpte_trip_flags_get_type())
GType gpte_trip_flags_get_type(void);

typedef enum {
	GPTE_TRIP_FLAG_BIKE = 1 << 0
} GpteTripFlags;


/**
 * GpteTripOptimization:
 * @GPTE_OPTIMIZE_LEAST_DURATION: Optimize for least duration
 * @GPTE_OPTIMIZE_LEAST_CHANGES: Optimize for least changes
 * @GPTE_OPTIMIZE_LEAST_WALKING: Optimize for least walking
 *
 * Optimization target of a trip request.
 */

#define GPTE_TYPE_TRIP_OPTIMIZATION (gpte_trip_optimization_get_type())
GType gpte_trip_optimization_get_type(void);

typedef enum {
	GPTE_OPTIMIZE_LEAST_DURATION,
	GPTE_OPTIMIZE_LEAST_CHANGES,
	GPTE_OPTIMIZE_LEAST_WALKING
} GpteTripOptimization;


/**
 * GpteWalkSpeed:
 * @GPTE_WALK_SLOW: Slow walk speed
 * @GPTE_WALK_NORMAL: Normal walk speed
 * @GPTE_WALK_FAST: Fast walk speed
 *
 * The walk speed needed to calculate duration of individual walking and
 * transfer legs.
 */

#define GPTE_TYPE_WALK_SPEED (gpte_walk_speed_get_type())
GType gpte_walk_speed_get_type(void);

typedef enum {
	GPTE_WALK_SLOW,
	GPTE_WALK_NORMAL,
	GPTE_WALK_FAST
} GpteWalkSpeed;


/**
 * GpteTripOptions:
 * @accessibility: the accessibility requirements of a trip
 * @flags: additional query parameters
 * @optimize: optimization target
 * @products: bitset of acceptable  products
 * @walk_speed: the walking speed
 *
 * Additional request options used to limit the space of possible trips
 * and aid in calculation for better results.
 */

#define GPTE_TYPE_TRIP_OPTIONS (gpte_trip_options_get_type())
GType gpte_trip_options_get_type(void);

typedef struct {
	GpteTripAccessibility accessibility;
	GpteTripFlags flags;
	GpteTripOptimization optimize;
	GpteProducts products;
	GpteWalkSpeed walk_speed;
} GpteTripOptions;

/**
 * gpte_trip_options_copy:
 * @self: the instance to duplicate
 *
 * Duplicates the [struct@Gpte.TripOptions] instance.
 *
 * Returns: (transfer full): the copied instance
 */
GpteTripOptions* gpte_trip_options_copy(const GpteTripOptions* self);

/**
 * gpte_trip_options_free:
 * @self: the trip options
 *
 * Frees the [struct@Gpte.TripOptions] instance from memory.
 *
 * After this function was called, you may not access the @self anymore.
 */
void gpte_trip_options_free(GpteTripOptions* self);


typedef struct _GpteProvider GpteProvider;
typedef struct _GpteTrips GpteTrips;


/**
 * GpteTripsQueryTime:
 * @GPTE_TRIPS_QUERY_EARLIER: query earlier trips
 * @GPTE_TRIPS_QUERY_LATER: query later trips
 *
 * Parameter for [method@Gpte.Trips.query_more] for deciding if it
 * should query more trips before or after the current ones.
 */

#define GPTE_TYPE_TRIPS_QUERY_TIME (gpte_trips_query_time_get_type())
GType gpte_trips_query_time_get_type(void);

typedef enum {
	GPTE_TRIPS_QUERY_EARLIER,
	GPTE_TRIPS_QUERY_LATER
} GpteTripsQueryTime;


/**
 * GpteTripsResultType:
 * @GPTE_TRIPS_RESULT_ERR: the result contained an error
 * @GPTE_TRIPS_RESULT_AMBIGUOUS: detected ambiguous location parameters
 * @GPTE_TRIPS_RESULT_OK: successful trips query
 *
 * The resulting type of a [struct@Gpte.TripsResult].
 */

#define GPTE_TYPE_TRIPS_RESULT_TYPE (gpte_trips_result_type_get_type())
GType gpte_trips_result_type_get_type(void);

typedef enum {
	GPTE_TRIPS_RESULT_ERR,
	GPTE_TRIPS_RESULT_AMBIGUOUS,
	GPTE_TRIPS_RESULT_OK
} GpteTripsResultType;


/**
 * GpteTripsResult:
 * Union wrapper for the result of a trips query.
 */

#define GPTE_TYPE_TRIPS_RESULT (gpte_trips_result_get_type())
GType gpte_trips_result_get_type(void);

typedef struct _GpteTripsResult GpteTripsResult;

/**
 * gpte_trips_result_copy:
 * @self: the trips result wrapper
 *
 * Create a duplicate copy of @self.
 * Returns: (transfer full): copy of @self
 */
GpteTripsResult* gpte_trips_result_copy(const GpteTripsResult* self);

/**
 * gpte_trips_result_free:
 * @self: the trips result wrapper
 *
 * Frees the result from memory.
 *
 * After this function was called, you may not access @self anymore.
 */
void gpte_trips_result_free(GpteTripsResult* self);

/**
 * gpte_trips_result_get:
 * @self: the trips result wrapper
 *
 * Gets if the result was successful or the failure kind.
 *
 * Returns: the result kind
 */
GpteTripsResultType gpte_trips_result_get(GpteTripsResult* self);

/**
 * gpte_trips_result_get_ambiguous_from:
 * @self: the trips result wrapper
 *
 * Gets the ambiguous starting locations.
 *
 * You may only call this if [method@Gpte.TripsResult.get] returned [enum@Gpte.TripsResultType.AMBIGUOUS].
 * Returns: (transfer full) (nullable): ambiguous starting locations
 */
GListModel* gpte_trips_result_get_ambiguous_from(GpteTripsResult* self);
/**
 * gpte_trips_result_get_ambiguous_via:
 * @self: the trips result wrapper
 *
 * Gets the ambiguous via locations.
 *
 * You may only call this if [method@Gpte.TripsResult.get] returned [enum@Gpte.TripsResultType.AMBIGUOUS].
 * Returns: (transfer full) (nullable): ambiguous via locations
 */
GListModel* gpte_trips_result_get_ambiguous_via(GpteTripsResult* self);
/**
 * gpte_trips_result_get_ambiguous_to:
 * @self: the trips result wrapper
 *
 * Gets the ambiguous destination locations.
 *
 * You may only call this if [method@Gpte.TripsResult.get] returned [enum@Gpte.TripsResultType.AMBIGUOUS].
 * Returns: (transfer full) (nullable): ambiguous destination locations
 */
GListModel* gpte_trips_result_get_ambiguous_to(GpteTripsResult* self);

/**
 * gpte_trips_result_get_trips:
 * @self: the trips result wrapper
 *
 * Get the trips stored in a successful wrapper.
 *
 * You may only call this if [method@Gpte.TripsResult.get] returned [enum@Gpte.TripsResultType.OK].
 * Returns: (transfer full) (nullable): trips from this result
 */
GpteTrips* gpte_trips_result_get_trips(GpteTripsResult* self);

G_DEFINE_AUTOPTR_CLEANUP_FUNC(GpteTripsResult, gpte_trips_result_free)

/**
 * GpteTrips:
 * Extendible list model containing trips from a query.
 */

#define GPTE_TYPE_TRIPS (gpte_trips_get_type())
G_DECLARE_FINAL_TYPE (GpteTrips, gpte_trips, GPTE, TRIPS, GpteJavaObject)

/**
 * gpte_trips_get_from:
 * @self: the trips model
 *
 * Gets the location the trips depart from.
 *
 * Returns: (transfer full): starting location
 */
GpteLocation* gpte_trips_get_from(GpteTrips* self);
/**
 * gpte_trips_get_via:
 * @self: the trips model
 *
 * Gets the via location the trips cross trough or %NULL.
 *
 * Returns: (transfer full) (nullable): intermediate location
 */
GpteLocation* gpte_trips_get_via(GpteTrips* self);
/**
 * gpte_trips_get_to:
 * @self: the trips model
 *
 * Gets the location the trips end at.
 *
 * Returns: (transfer full): end location
 */
GpteLocation* gpte_trips_get_to(GpteTrips* self);

/**
 * gpte_trips_query_more:
 * @self: the trips model
 * @time: query trips earlier or later
 * @err: a #GError, or %NULL
 *
 * Query additional trips earlier or later and append it to this model.
 *
 * You should connect to [signal@Gio.ListModel::items-changed] to receive updates.
 *
 * Returns: %TRUE if successful, %FALSE otherwise
 */
gboolean gpte_trips_query_more(GpteTrips* self, GpteTripsQueryTime time, GError** err);

/**
 * gpte_trips_query_more_async:
 * @self: the trips model
 * @time: query trips earlier or later
 * @callback: (scope async) (closure user_data): a #GAsyncReadyCallback
 *  to call when the request is satisfied
 * @user_data: the data to pass to callback function
 *
 * Asyncronously query additional trips earlier or later.
 *
 * For more details, see [method@Gpte.Trips.query_more] which is the
 * synchronous version of this call.
 */
void gpte_trips_query_more_async(GpteTrips* self, GpteTripsQueryTime time, GAsyncReadyCallback callback, gpointer user_data);

/**
 * gpte_trips_query_more_finish:
 * @self: the trips model
 * @result: a #GAsyncResult
 * @error: a #GError, or %NULL
 *
 * Finishes an asynchronous more trips query started with
 * [method@Gpte.Trips.query_more_async].
 *
 * Returns: %TRUE if successful, %FALSE otherwise
 */
gboolean gpte_trips_query_more_finish(GpteTrips* self, GAsyncResult* result, GError** error);

G_END_DECLS

#endif // __GPTETRIPS_H__
