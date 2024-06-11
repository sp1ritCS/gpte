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

#ifndef __GPTEPROVIDER_H__
#define __GPTEPROVIDER_H__

#include <glib-object.h>
#include <gptejavaobject.h>

#include <gio/gio.h>
#include <gpteproducts.h>
#include <gptestyle.h>
#include <gptestationdepartures.h>
#include <gptetrips.h>
#include <gptelocation.h>

G_BEGIN_DECLS

/**
 * GpteProviderCapabilities:
 * @GPTE_PROVIDER_CAPABILITY_DEPARTURES: provider can query departures
 * @GPTE_PROVIDER_CAPABILITY_NEARBY_LOCATIONS: provider can query nearby locations
 * @GPTE_PROVIDER_CAPABILITY_SUGGEST_LOCATIONS: provider can query suggested locations
 * @GPTE_PROVIDER_CAPABILITY_TRIPS: provider can query trips
 * @GPTE_PROVIDER_CAPABILITY_TRIPS_VIA: provider can query trips with an additional via location
 *
 * Used by [method@Gpte.Provider.has_capabilities] to determine if a
 * provider is capable of of the required capabilities.
 */

#define GPTE_TYPE_PROVIDER_CAPABILITIES (gpte_provider_capabilities_get_type())
GType gpte_provider_capabilities_get_type(void);

typedef enum {
	GPTE_PROVIDER_CAPABILITY_DEPARTURES = 1 << 0,
	GPTE_PROVIDER_CAPABILITY_NEARBY_LOCATIONS = 1 << 1,
	GPTE_PROVIDER_CAPABILITY_SUGGEST_LOCATIONS = 1 << 2,
	GPTE_PROVIDER_CAPABILITY_TRIPS = 1 << 3,
	GPTE_PROVIDER_CAPABILITY_TRIPS_VIA =  1 << 4,
} GpteProviderCapabilities;


/**
 * GpteQueryDeparturesFlags:
 * @GPTE_QUERY_DEPARTURES_QUERY_EQUIVS: also query equivalent stations
 *
 * Additional query parameters for [method@Gpte.Provider.query_departures].
 */

#define GPTE_TYPE_QUERY_DEPARTURES_FLAGS (gpte_query_departures_flags_get_type())
GType gpte_query_departures_flags_get_type(void);

typedef enum {
	GPTE_QUERY_DEPARTURES_NONE = 0,
	GPTE_QUERY_DEPARTURES_QUERY_EQUIVS = 1 << 0
} GpteQueryDeparturesFlags;


/**
 * GpteTripsQueryRequest:
 * @GPTE_TRIPS_QUERY_DEPARTURE: the `date` describes departure
 * @GPTE_TRIPS_QUERY_ARRIVAL: the `date` describes arrival
 *
 * Parameter for [method@Gpte.Provider.query_trips] used for setting the
 * behaviour of the `date` parameter.
 */

#define GPTE_TYPE_TRIPS_QUERY_REQUEST (gpte_trips_query_request_get_type())
GType gpte_trips_query_request_get_type(void);

typedef enum {
	GPTE_TRIPS_QUERY_DEPARTURE,
	GPTE_TRIPS_QUERY_ARRIVAL
} GpteTripsQueryRequest;


/**
 * GpteProvider:
 * Object wrapping a de.schildbach.pte.NetworkProvider, which can be
 * used to query data from an abitrary transportation netowork.
 *
 *
 * To receive a Provider, look at the `gpte_create` family of functions.
 */

#define GPTE_TYPE_PROVIDER (gpte_provider_get_type())
G_DECLARE_FINAL_TYPE (GpteProvider, gpte_provider, GPTE, PROVIDER, GpteJavaObject)

/**
 * gpte_provider_get_id:
 * @self: the transportation network
 *
 * Gets a unique identifier for each transportation network, matching
 * its constructor function.
 * Returns: (transfer none): a unique identifier of the netowrk
 */
const gchar* gpte_provider_get_id(GpteProvider* self);

/**
 * gpte_provider_default_products:
 * @self: the transportation network
 *
 * Gets typical products for the network.
 * Returns: products
 */
GpteProducts gpte_provider_default_products(GpteProvider* self);

/**
 * gpte_provider_has_capabilities:
 * @self: the transportation network
 * @capabilities: capabilities to check for
 *
 * Check if the specific transporation network provider supports a
 * particular set of capabilities.
 * Returns: %TRUE if the network supports all the capabilites, %FALSE if not
 */
gboolean gpte_provider_has_capabilities(GpteProvider* self, GpteProviderCapabilities capabilities);

/**
 * gpte_provider_get_area:
 * @self: the transportation network
 * @len: (out): length of the returned array
 *
 * Gets the primary covered area of the particular transportation
 * network.
 * Returns: (transfer full) (array length=len):
 * 	array containing points of a polygon (special case: just one coordinate defines just a center point)
 */
GpteGeoPoint* gpte_provider_get_area(GpteProvider* self, gsize* len);

/**
 * gpte_provider_line_style:
 * @self: the transportation network
 * @network: (nullable): network to disambiguate line
 * @product: (nullable): line product to get style of,
 * @label: (nullable): line label to get style of
 *
 * Get style of line
 * Returns: (transfer full): style containing background, foreground and optional border colors
 */
GpteStyle* gpte_provider_line_style(GpteProvider* self, const gchar* network, GpteProductCode product, const gchar* label);

/**
 * gpte_provider_query_departures:
 * @self: the transportation network
 * @id: identifier of the station
 * @time: (nullable): desired time for departing, or %NULL for the provider default
 * @max: maximum number of departures to get or 0,
 * @flags: additional parameters for the query
 * @err: a #GError, or %NULL
 *
 * Gets departures at a given station, probably live.
 *
 * Ought to throw exceptions from the [error@Gpte.PteError] or
 * [error@Gpte.JavaError] domain.
 *
 * Returns: (transfer full): list of departures as a #GListModel of [class@Gpte.StationDepartures]
 */
GListModel* gpte_provider_query_departures(GpteProvider* self, const gchar* id, GDateTime* time, gint max, GpteQueryDeparturesFlags flags, GError** err);

/**
 * gpte_provider_query_departures_async:
 * @self: the transportation network
 * @id: identifier of the station
 * @time: (nullable): desired time for departing, or %NULL for the
 *  provider default
 * @max: maximum number of departures to get or 0,
 * @flags: additional parameters for the query
 * @cancellable: (nullable): optional #GCancellable object, %NULL to
 *  ignore
 * @callback: (scope async) (closure user_data): a #GAsyncReadyCallback
 *  to call when the request is satisfied
 * @user_data: the data to pass to callback function
 *
 * Asynchronously gets departures at a given station
 *
 * For more details, see gpte_provider_query_departures() which is the
 * synchronous version of this call.
 */
void gpte_provider_query_departures_async(
	GpteProvider* self,
	const gchar* id,
	GDateTime* time,
	gint max,
	GpteQueryDeparturesFlags flags,
	GCancellable* cancellable,
	GAsyncReadyCallback callback, gpointer user_data
);
/**
 * gpte_provider_query_departures_finish:
 * @self: the transportation network
 * @result: a #GAsyncResult
 * @error: a #GError, or %NULL
 *
 * Finishes an asynchronous departure query started with
 * gpte_provider_query_departures_async().
 *
 * Returns: (transfer full): list of departures as a #GListModel of [class@Gpte.StationDepartures]
 */
GListModel* gpte_provider_query_departures_finish(GpteProvider* self, GAsyncResult* result, GError** error);

/**
 * gpte_provider_query_trips:
 * @self: the transportation network
 * @from: #GpteLocation to start the trip from
 * @via: (nullable): #GpteLocation to reach during the trip
 * @to: #GpteLocation to end the trip at
 * @date: desired date for departing
 * @request: additional parameters for the query
 * @options: (transfer none) (nullable): additional trip options, or %NULL for the
 *  provider default
 * @err: a #GError, or %NULL
 *
 * Query trips, asking for any ambiguousnesses.
 *
 * Ought to throw exceptions from the [error@Gpte.PteError],
 * [error@Gpte.PteTripsError] or [error@Gpte.JavaError] domain.
 *
 * Returns: (transfer full): result of the query
 */
GpteTripsResult* gpte_provider_query_trips(GpteProvider* self, GpteLocation* from, GpteLocation* via, GpteLocation* to, GDateTime* date, GpteTripsQueryRequest request, const GpteTripOptions* options, GError** err);

/**
 * gpte_provider_query_trips_async:
 * @self: the transportation network
 * @from: #GpteLocation to start the trip from
 * @via: (nullable): #GpteLocation to reach during the trip
 * @to: #GpteLocation to end the trip at
 * @date: desired date for departing
 * @request: additional parameters for the query
 * @options: (transfer none) (nullable): additional trip options, or %NULL for the
 *  provider default
 * @cancellable: (nullable): optional #GCancellable object, %NULL to
 *  ignore
 * @callback: (scope async) (closure user_data): a #GAsyncReadyCallback
 *  to call when the request is satisfied
 * @user_data: the data to pass to callback function
 *
 * Query trips asyncronously.
 *
 * For more details, see gpte_provider_query_trips() which is the
 * synchronous version of this call.
 */
void gpte_provider_query_trips_async(
	GpteProvider* self,
	GpteLocation* from,
	GpteLocation* via,
	GpteLocation* to,
	GDateTime* date,
	GpteTripsQueryRequest request,
	const GpteTripOptions* options,
	GCancellable* cancellable,
	GAsyncReadyCallback callback, gpointer user_data
);
/**
 * gpte_provider_query_trips_finish:
 * @self: the transportation network
 * @result: a #GAsyncResult
 * @error: a #GError, or %NULL
 *
 * Finishes an asynchronous trips query started with
 * gpte_provider_query_trips_async().
 *
 * Returns: (transfer full): result of the query
 */
GpteTripsResult* gpte_provider_query_trips_finish(GpteProvider* self, GAsyncResult* result, GError** error);

/**
 * gpte_provider_query_nearby:
 * @self: the transportation network
 * @locations: set of types of locations to look for
 * @location: location to query nearby locations from
 * @max_dist: maximum distance in meters, or 0
 * @max: maximum distance in meters, or 0
 * @err: a #GError, or %NULL
 *
 * Find locations near to given location.
 *
 * At least one of lat/lon pair or station id must be present in that location.
 *
 * Ought to throw exceptions from the [error@Gpte.PteError] or
 * [error@Gpte.JavaError] domain.
 *
 * Returns: (transfer full): nearby locations as a #GListModel of [class@Gpte.Location]
 */
GListModel* gpte_provider_query_nearby(GpteProvider* self, GpteLocations locations, GpteLocation* location, gint max_dist, gint max, GError** err);

/**
 * gpte_provider_query_nearby_async:
 * @self: the transportation network
 * @locations: set of types of locations to look for
 * @location: location to query nearby locations from
 * @max_dist: maximum distance in meters, or 0
 * @max: maximum number of locations, or 0
 * @cancellable: (nullable): optional #GCancellable object, %NULL to
 *  ignore
 * @callback: (scope async) (closure user_data): a #GAsyncReadyCallback
 *  to call when the request is satisfied
 * @user_data: the data to pass to callback function
 *
 * Asynchronously query locations near a given location.
 *
 * For more details, see gpte_provider_query_nearby() which is the
 * synchronous version of this call.
 */
void gpte_provider_query_nearby_async(
	GpteProvider* self,
	GpteLocations locations,
	GpteLocation* location,
	gint max_dist,
	gint max,
	GCancellable* cancellable,
	GAsyncReadyCallback callback, gpointer user_data
);
/**
 * gpte_provider_query_nearby_finish:
 * @self: the transportation network
 * @result: a #GAsyncResult
 * @error: a #GError, or %NULL
 *
 * Finishes an asynchronous nearby locations query started with
 * gpte_provider_query_nearby_async().
 *
 * Returns: (transfer full): nearby locations as a #GListModel of [class@Gpte.Location]
 */
GListModel* gpte_provider_query_nearby_finish(GpteProvider* self, GAsyncResult* result, GError** error);

/**
 * gpte_provider_suggest_locations:
 * @self: the transportation network
 * @constraint: string to match locations by
 * @locations: set of types of locations to look for
 * @max: maximum number of locations, or 0
 * @err: a #GError, or %NULL
 *
 * Suggest possibile locations based on `constraint`.
 *
 * Meant for auto-completion of location names, like in an Android AutoCompleteTextView.
 *
 * Ought to throw exceptions from the [error@Gpte.PteError] or
 * [error@Gpte.JavaError] domain.
 *
 * Returns: (transfer full): matching locations as a #GListModel of [class@Gpte.Location]
 */
GListModel* gpte_provider_suggest_locations(GpteProvider* self, const gchar* constraint, GpteLocations locations, gint max, GError** err);

/**
 * gpte_provider_suggest_locations_async:
 * @self: the transportation network
 * @constraint: string to match locations by
 * @locations: set of types of locations to look for
 * @max: maximum number of locations, or 0
 * @cancellable: (nullable): optional #GCancellable object, %NULL to
 *  ignore
 * @callback: (scope async) (closure user_data): a #GAsyncReadyCallback
 *  to call when the request is satisfied
 * @user_data: the data to pass to callback function
 *
 * Asynchronously match possibly locations based on `constraint`.
 *
 * For more details, see gpte_provider_suggest_locations() which is the
 * synchronous version of this call.
 */
void gpte_provider_suggest_locations_async(
	GpteProvider* self,
	const gchar* constraint,
	GpteLocations locations,
	gint max,
	GCancellable* cancellable,
	GAsyncReadyCallback callback, gpointer user_data
);
/**
 * gpte_provider_suggest_locations_finish:
 * @self: the transportation network
 * @result: a #GAsyncResult
 * @error: a #GError, or %NULL
 *
 * Finishes an asynchronous nearby locations query started with
 * gpte_provider_suggest_locations_async().
 *
 * Returns: (transfer full): matching locations as a #GListModel of [class@Gpte.Location]
 */
GListModel* gpte_provider_suggest_locations_finish(GpteProvider* self, GAsyncResult* result, GError** error);

G_END_DECLS

#endif // __GPTEPROVIDER_H__
