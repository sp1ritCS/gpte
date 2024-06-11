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

#include "gpteprovider.h"
#include "gptejavaobject-priv.h"
#include "gpteutils-priv.h"
#include "gptelocation-priv.h"
#include "gpteproducts-priv.h"
#include "gptegeo-priv.h"
#include "gptestyle-priv.h"
#include "gptelist-priv.h"
#include "gptetrips-priv.h"
#include "gpteerrors.h"

G_DEFINE_FLAGS_TYPE(GpteProviderCapabilities, gpte_provider_capabilities,
	G_DEFINE_ENUM_VALUE(GPTE_PROVIDER_CAPABILITY_DEPARTURES, "departures"),
	G_DEFINE_ENUM_VALUE(GPTE_PROVIDER_CAPABILITY_NEARBY_LOCATIONS, "nearby-locations"),
	G_DEFINE_ENUM_VALUE(GPTE_PROVIDER_CAPABILITY_SUGGEST_LOCATIONS, "suggest-locations"),
	G_DEFINE_ENUM_VALUE(GPTE_PROVIDER_CAPABILITY_TRIPS, "trips"),
	G_DEFINE_ENUM_VALUE(GPTE_PROVIDER_CAPABILITY_TRIPS_VIA, "trips-via")
)

G_DEFINE_FLAGS_TYPE(GpteQueryDeparturesFlags, gpte_query_departures_flags,
	G_DEFINE_ENUM_VALUE(GPTE_QUERY_DEPARTURES_QUERY_EQUIVS, "query-equivs")
)

G_DEFINE_ENUM_TYPE(GpteTripsQueryRequest, gpte_trips_query_request,
	G_DEFINE_ENUM_VALUE(GPTE_TRIPS_QUERY_DEPARTURE, "departure"),
	G_DEFINE_ENUM_VALUE(GPTE_TRIPS_QUERY_ARRIVAL, "arrival")
)


struct _GpteProvider {
	GpteJavaObject parent_instance;

	gchar* id;
};

G_DEFINE_TYPE (GpteProvider, gpte_provider, GPTE_TYPE_JAVA_OBJECT)

enum {
	PROP_ID = 1,
	N_PROPERTIES
};
static GParamSpec* obj_properties[N_PROPERTIES] = { 0, };

static void gpte_provider_finalize(GObject* object) {
	GpteProvider* self = GPTE_PROVIDER(object);
	g_free(self->id);
	G_OBJECT_CLASS(gpte_provider_parent_class)->finalize(object);
}

static void gpte_provider_get_property(GObject* object, guint prop_id, GValue* val, GParamSpec* pspec) {
	GpteProvider* self = GPTE_PROVIDER(object);
	switch (prop_id) {
		case PROP_ID:
			g_value_set_string(val, gpte_provider_get_id(self));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
	}
}
static void gpte_provider_set_property(GObject* object, guint prop_id, const GValue* val, GParamSpec* pspec) {
	GpteProvider* self = GPTE_PROVIDER(object);
	switch (prop_id) {
		case PROP_ID:
			g_return_if_fail(self->id == NULL);
			self->id = g_value_dup_string(val);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
	}
}

static void gpte_provider_class_init(GpteProviderClass* class) {
	GObjectClass* object_class = G_OBJECT_CLASS(class);
	object_class->finalize = gpte_provider_finalize;
	object_class->get_property = gpte_provider_get_property;
	object_class->set_property = gpte_provider_set_property;

	obj_properties[PROP_ID] = g_param_spec_string("identifier", NULL, NULL, NULL, G_PARAM_STATIC_STRINGS | G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
	g_object_class_install_properties(object_class, N_PROPERTIES, obj_properties);
}

static void gpte_provider_init(GpteProvider* self) {
	self->id = NULL;
}

GpteProvider* gpte_provider_new(const gchar* identifier, GpteJvm* vm, jobject provider) {
	return g_object_new(GPTE_TYPE_PROVIDER, "vm", vm, "object", provider, "identifier", identifier, NULL);
}

const gchar* gpte_provider_get_id(GpteProvider* self) {
	g_return_val_if_fail(GPTE_IS_PROVIDER(self), NULL);
	return self->id;
}

GpteProducts gpte_provider_default_products(GpteProvider* self) {
	g_return_val_if_fail(GPTE_IS_PROVIDER(self), 0);
	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 3);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));

	jclass class = (*env)->FindClass(env, "de/schildbach/pte/NetworkProvider");
	jmethodID products_mid = (*env)->GetMethodID(env, class, "defaultProducts", "()Ljava/util/Set;");
	jobject default_products = (*env)->CallObjectMethod(env, this, products_mid);

	return gpte_products_from_set(vm, default_products);
}

gboolean gpte_provider_has_capabilities(GpteProvider* self, GpteProviderCapabilities capabilities) {
	g_return_val_if_fail(GPTE_IS_PROVIDER(self), 0);
	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 3);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));

	jclass class = (*env)->FindClass(env, "de/schildbach/pte/NetworkProvider");
	jmethodID caps_mid = (*env)->GetMethodID(env, class, "hasCapabilities", "([Lde/schildbach/pte/NetworkProvider$Capability;)Z");

	jclass enum_class = (*env)->FindClass(env, "de/schildbach/pte/NetworkProvider$Capability");
	g_autoptr(GPtrArray) args = g_ptr_array_new();
	if (capabilities & GPTE_PROVIDER_CAPABILITY_DEPARTURES)
		g_ptr_array_add(args, (*env)->GetStaticObjectField(env, enum_class, (*env)->GetStaticFieldID(env, enum_class, "DEPARTURES", "Lde/schildbach/pte/NetworkProvider$Capability;")));
	if (capabilities & GPTE_PROVIDER_CAPABILITY_NEARBY_LOCATIONS)
		g_ptr_array_add(args, (*env)->GetStaticObjectField(env, enum_class, (*env)->GetStaticFieldID(env, enum_class, "NEARBY_LOCATIONS", "Lde/schildbach/pte/NetworkProvider$Capability;")));
	if (capabilities & GPTE_PROVIDER_CAPABILITY_SUGGEST_LOCATIONS)
		g_ptr_array_add(args, (*env)->GetStaticObjectField(env, enum_class, (*env)->GetStaticFieldID(env, enum_class, "SUGGEST_LOCATIONS", "Lde/schildbach/pte/NetworkProvider$Capability;")));
	if (capabilities & GPTE_PROVIDER_CAPABILITY_TRIPS)
		g_ptr_array_add(args, (*env)->GetStaticObjectField(env, enum_class, (*env)->GetStaticFieldID(env, enum_class, "TRIPS", "Lde/schildbach/pte/NetworkProvider$Capability;")));
	if (capabilities & GPTE_PROVIDER_CAPABILITY_TRIPS_VIA)
		g_ptr_array_add(args, (*env)->GetStaticObjectField(env, enum_class, (*env)->GetStaticFieldID(env, enum_class, "TRIPS_VIA", "Lde/schildbach/pte/NetworkProvider$Capability;")));

	jobjectArray jargs = (*env)->NewObjectArray(env, args->len, enum_class, NULL);
	for (guint i = 0; i < args->len; i++)
		(*env)->SetObjectArrayElement(env, jargs, i, g_ptr_array_index(args, i));

	return (*env)->CallBooleanMethod(env, this, caps_mid, jargs);
}

GpteGeoPoint* gpte_provider_get_area(GpteProvider* self, gsize* len) {
	g_return_val_if_fail(GPTE_IS_PROVIDER(self), NULL);
	g_return_val_if_fail(len != NULL, NULL);
	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 4);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));

	jclass class = (*env)->FindClass(env, "de/schildbach/pte/NetworkProvider");
	jmethodID area_mid = (*env)->GetMethodID(env, class, "getArea", "()[Lde/schildbach/pte/dto/Point;");
	jobjectArray jarea = (*env)->CallObjectMethod(env, this, area_mid);

	jsize jlen = (*env)->GetArrayLength(env, jarea);
	*len = jlen;
	GpteGeoPoint* ret = g_new(GpteGeoPoint, jlen);
	for (jsize i = 0; i < jlen; i++) {
		jobject point = (*env)->GetObjectArrayElement(env, jarea, i);
		ret[i] = gpte_geo_point_from_java(vm, point);
		(*env)->DeleteLocalRef(env, point);
	}
	return ret;
}

GpteStyle* gpte_provider_line_style(GpteProvider* self, const gchar* network, GpteProductCode product, const gchar* label) {
	g_return_val_if_fail(GPTE_IS_PROVIDER(self), NULL);
	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 8);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));

	jclass class = (*env)->FindClass(env, "de/schildbach/pte/NetworkProvider");
	jmethodID style_mid = (*env)->GetMethodID(env, class, "lineStyle", "(Ljava/lang/String;Lde/schildbach/pte/dto/Product;Ljava/lang/String;)Lde/schildbach/pte/dto/Style;");
	jstring network_str = network ? (*env)->NewStringUTF(env, network) : NULL;
	jstring label_str = label ? (*env)->NewStringUTF(env, label) : NULL;

	jclass product_class = (*env)->FindClass(env, "de/schildbach/pte/dto/Product");
	jmethodID from_code = (*env)->GetStaticMethodID(env, product_class, "fromCode", "(C)Lde/schildbach/pte/dto/Product;");
	jobject jproduct = product != GPTE_PRODUCT_CODE_NULL ? (*env)->CallStaticObjectMethod(env, product_class, from_code, (jchar)product) : NULL;

	jobject jstyle = (*env)->CallObjectMethod(env, this, style_mid, network_str, jproduct, label_str);
	if (!jstyle)
		return NULL;
	return gpte_style_from_java(vm, jstyle);
}

#define GPTE_PROVIDER_RESULT_STATUS_ENUM_ENTRY(f,cn) \
	(*env)->GetStaticObjectField(env, status_class, (*env)->GetStaticFieldID(env, status_class, (#f), "Lde/schildbach/pte/dto/" cn "$Status;"))
#define GPTE_PROVIDER_RESULT_STATUS_PROPAGTE_ERROR(dom,f,cn,ret,msgf,...) \
	if ((*env)->IsSameObject(env, status, GPTE_PROVIDER_RESULT_STATUS_ENUM_ENTRY(f, cn))) { \
		g_set_error(err, dom, dom##_##f, msgf __VA_OPT__(,) __VA_ARGS__); \
		return (ret); \
	}

GListModel* gpte_provider_query_departures(GpteProvider* self, const gchar* id, GDateTime* time, gint max, GpteQueryDeparturesFlags flags, GError** err) {
	g_return_val_if_fail(GPTE_IS_PROVIDER(self), NULL);
	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 10);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));

	jclass class = (*env)->FindClass(env, "de/schildbach/pte/NetworkProvider");
	jmethodID query_mid = (*env)->GetMethodID(env, class, "queryDepartures", "(Ljava/lang/String;Ljava/util/Date;IZ)Lde/schildbach/pte/dto/QueryDeparturesResult;");

	jstring id_str = (*env)->NewStringUTF(env, id);
	jobject jtime = time ? gpte_date_to_java(vm, time) : NULL;
	jobject res = (*env)->CallObjectMethod(env, this, query_mid, id_str, jtime, (jint)max, (jboolean)((flags & GPTE_QUERY_DEPARTURES_QUERY_EQUIVS) != 0));

	if (gpte_jvm_error(vm, err))
		return NULL;

	jclass result_class = (*env)->FindClass(env, "de/schildbach/pte/dto/QueryDeparturesResult");

	jfieldID status_id = (*env)->GetFieldID(env, result_class, "status", "Lde/schildbach/pte/dto/QueryDeparturesResult$Status;");
	jobject status = (*env)->GetObjectField(env, res, status_id);

	jclass status_class = (*env)->FindClass(env, "de/schildbach/pte/dto/QueryDeparturesResult$Status");
	if (!(*env)->IsSameObject(env, status, GPTE_PROVIDER_RESULT_STATUS_ENUM_ENTRY(OK, "QueryDeparturesResult"))) {
		GPTE_PROVIDER_RESULT_STATUS_PROPAGTE_ERROR(GPTE_PTE_ERROR, SERVICE_DOWN, "QueryDeparturesResult", NULL, "Service Down")
		GPTE_PROVIDER_RESULT_STATUS_PROPAGTE_ERROR(GPTE_PTE_ERROR, INVALID_STATION, "QueryDeparturesResult", NULL, "Invalid Station")
	}

	jfieldID depas_id = (*env)->GetFieldID(env, result_class, "stationDepartures", "Ljava/util/List;");
	jobject depas = (*env)->GetObjectField(env, res, depas_id);
	GListModel* ret = gpte_list_new(vm, GPTE_TYPE_STATION_DEPARTURES, depas);
	return ret;
}

typedef struct {
	gchar* id;
	GDateTime* time;
	gint max;
	GpteQueryDeparturesFlags flags;
} GpteQueryDeparturesData;
static void gpte_provider_query_depatures_data_free(GpteQueryDeparturesData* self) {
	g_free(self->id);
	g_date_time_unref(self->time);
	g_free(self);
}
static void gpte_provider_query_depatures_thread(GTask* task, GpteProvider* self, GpteQueryDeparturesData* data, GCancellable*) {
	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_autoptr(GpteThreadGuard) guard = gpte_jvm_attach_thread(vm, "gpte-query-depatures-async");
	if (!guard) {
		g_task_return_error(task, g_error_new(GPTE_JAVA_ERROR, GPTE_JAVA_ERROR_JVM_THREADING, "Unable to attach thread"));
		return;
	}

	GError* err = NULL;
	GListModel* model = gpte_provider_query_departures(self, data->id, data->time, data->max, data->flags, &err);
	if (model)
		g_task_return_pointer(task, model, g_object_unref);
	else
		g_task_return_error(task, err);
}
void gpte_provider_query_departures_async(
	GpteProvider* self,
	const gchar* id,
	GDateTime* time,
	gint max,
	GpteQueryDeparturesFlags flags,
	GCancellable* cancellable,
	GAsyncReadyCallback callback, gpointer user_data
) {
	g_return_if_fail(GPTE_IS_PROVIDER(self));
	g_autoptr(GTask) task = g_task_new(self, cancellable, callback, user_data);
	GpteQueryDeparturesData* data = g_new(GpteQueryDeparturesData, 1);
	data->id = id ? g_strdup(id) : NULL;
	data->time = g_date_time_ref(time);
	data->max = max;
	data->flags = flags;
	g_task_set_task_data(task, data, (GDestroyNotify)gpte_provider_query_depatures_data_free);
	g_task_run_in_thread(task, (GTaskThreadFunc)gpte_provider_query_depatures_thread);
}

GListModel* gpte_provider_query_departures_finish(GpteProvider* self, GAsyncResult* result, GError** error) {
	g_return_val_if_fail(g_task_is_valid(result, self), NULL);
	return g_task_propagate_pointer(G_TASK(result), error);
}


GpteTripsResult* gpte_provider_query_trips(GpteProvider* self, GpteLocation* from, GpteLocation* via, GpteLocation* to, GDateTime* date, GpteTripsQueryRequest request, const GpteTripOptions* options, GError** err) {
	g_return_val_if_fail(GPTE_IS_PROVIDER(self), NULL);
	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 10);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));

	jclass class = (*env)->FindClass(env, "de/schildbach/pte/NetworkProvider");
	jmethodID query_mid = (*env)->GetMethodID(env, class, "queryTrips", "("
		"Lde/schildbach/pte/dto/Location;"
		"Lde/schildbach/pte/dto/Location;"
		"Lde/schildbach/pte/dto/Location;"
		"Ljava/util/Date;"
		"Z"
		"Lde/schildbach/pte/dto/TripOptions;"
	")Lde/schildbach/pte/dto/QueryTripsResult;");


	jobject jfrom = gpte_java_object_get(GPTE_JAVA_OBJECT(from));
	jobject jvia = via ? gpte_java_object_get(GPTE_JAVA_OBJECT(via)) : NULL;
	jobject jto = gpte_java_object_get(GPTE_JAVA_OBJECT(to));
	jobject jdate = gpte_date_to_java(vm, date);
	jobject joptions = gpte_trip_options_to_java(vm, options);

	jobject res = (*env)->CallObjectMethod(env, this, query_mid, jfrom, jvia, jto, jdate, (jboolean)(request == GPTE_TRIPS_QUERY_DEPARTURE), joptions);

	if (gpte_jvm_error(vm, err))
		return NULL;

	jclass result_class = (*env)->FindClass(env, "de/schildbach/pte/dto/QueryTripsResult");
	jfieldID status_id = (*env)->GetFieldID(env, result_class, "status", "Lde/schildbach/pte/dto/QueryTripsResult$Status;");
	jobject status = (*env)->GetObjectField(env, res, status_id);

	jclass status_class = (*env)->FindClass(env, "de/schildbach/pte/dto/QueryTripsResult$Status");
	if (!(*env)->IsSameObject(env, status, GPTE_PROVIDER_RESULT_STATUS_ENUM_ENTRY(OK, "QueryTripsResult"))) {
		GPTE_PROVIDER_RESULT_STATUS_PROPAGTE_ERROR(GPTE_PTE_ERROR, SERVICE_DOWN, "QueryTripsResult", NULL, "Service Down")
		GPTE_PROVIDER_RESULT_STATUS_PROPAGTE_ERROR(GPTE_PTE_ERROR, INVALID_DATE, "QueryTripsResult", NULL, "Invalid Date")
		GPTE_PROVIDER_RESULT_STATUS_PROPAGTE_ERROR(GPTE_PTE_TRIPS_ERROR, NO_TRIPS, "QueryTripsResult", NULL, "No available trips")
		GPTE_PROVIDER_RESULT_STATUS_PROPAGTE_ERROR(GPTE_PTE_TRIPS_ERROR, TOO_CLOSE, "QueryTripsResult", NULL, "Locations too close by eachother")
		GPTE_PROVIDER_RESULT_STATUS_PROPAGTE_ERROR(GPTE_PTE_TRIPS_ERROR, UNKNOWN_FROM, "QueryTripsResult", NULL, "Departure location unknown")
		GPTE_PROVIDER_RESULT_STATUS_PROPAGTE_ERROR(GPTE_PTE_TRIPS_ERROR, UNKNOWN_LOCATION, "QueryTripsResult", NULL, "Unknown location")
		GPTE_PROVIDER_RESULT_STATUS_PROPAGTE_ERROR(GPTE_PTE_TRIPS_ERROR, UNKNOWN_TO, "QueryTripsResult", NULL, "Destination location unknown")
		GPTE_PROVIDER_RESULT_STATUS_PROPAGTE_ERROR(GPTE_PTE_TRIPS_ERROR, UNKNOWN_VIA, "QueryTripsResult", NULL, "Via location unknown")
		GPTE_PROVIDER_RESULT_STATUS_PROPAGTE_ERROR(GPTE_PTE_TRIPS_ERROR, UNRESOLVABLE_ADDRESS, "QueryTripsResult", NULL, "Address unresolvable")

	}

	return gpte_trips_result_new(vm, res, self);
}

typedef struct {
	GpteLocation* from;
	GpteLocation* via;
	GpteLocation* to;
	GDateTime* date;
	GpteTripsQueryRequest request;
	gboolean has_options;
	GpteTripOptions options;
} GpteProviderQueryTripsData;
static void gpte_provider_query_trips_data_free(GpteProviderQueryTripsData* self) {
	g_object_unref(self->from);
	if (self->via)
		g_object_unref(self->via);
	g_object_unref(self->to);
	g_date_time_unref(self->date);
	g_free(self);
}
static void gpte_provider_query_trips_thread(GTask* task, GpteProvider* self, GpteProviderQueryTripsData* data, GCancellable*) {
	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_autoptr(GpteThreadGuard) guard = gpte_jvm_attach_thread(vm, "gpte-suggest-locations-async");
	if (!guard) {
		g_task_return_error(task, g_error_new(GPTE_JAVA_ERROR, GPTE_JAVA_ERROR_JVM_THREADING, "Unable to attach thread"));
		return;
	}

	GError* err = NULL;
	GpteTripsResult* res = gpte_provider_query_trips(self, data->from, data->via, data->to, data->date, data->request, data->has_options ? &data->options : NULL, &err);
	if (res)
		g_task_return_pointer(task, res, (GDestroyNotify)gpte_trips_result_free);
	else
		g_task_return_error(task, err);
}
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
) {
	g_return_if_fail(GPTE_IS_PROVIDER(self));
	g_autoptr(GTask) task = g_task_new(self, cancellable, callback, user_data);
	GpteProviderQueryTripsData* data = g_new(GpteProviderQueryTripsData, 1);
	data->from = g_object_ref(from);
	data->from = g_object_ref(from);
	data->via = via ? g_object_ref(via) : NULL;
	data->to = g_object_ref(to);
	data->date = g_date_time_ref(date);
	data->request = request;
	if (options) {
		data->options = *options;
		data->has_options = TRUE;
	} else {
		data->has_options = FALSE;
	}
	g_task_set_task_data(task, data, (GDestroyNotify)gpte_provider_query_trips_data_free);
	g_task_run_in_thread(task, (GTaskThreadFunc)gpte_provider_query_trips_thread);
}
GpteTripsResult* gpte_provider_query_trips_finish(GpteProvider* self, GAsyncResult* result, GError** error) {
	g_return_val_if_fail(g_task_is_valid(result, self), NULL);
	return g_task_propagate_pointer(G_TASK(result), error);
}


GListModel* gpte_provider_query_nearby(GpteProvider* self, GpteLocations locations, GpteLocation* location, gint max_dist, gint max, GError** err) {
	g_return_val_if_fail(GPTE_IS_PROVIDER(self), NULL);
	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 10);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));

	jclass class = (*env)->FindClass(env, "de/schildbach/pte/NetworkProvider");
	jmethodID nearby_mid = (*env)->GetMethodID(env, class, "queryNearbyLocations", "(Ljava/util/Set;Lde/schildbach/pte/dto/Location;II)Lde/schildbach/pte/dto/NearbyLocationsResult;");

	jobject jlocations = gpte_locations_to_java(vm, locations);
	jobject jlocation = gpte_java_object_get(GPTE_JAVA_OBJECT(location));
	jobject res = (*env)->CallObjectMethod(env, this, nearby_mid, jlocations, jlocation, (jint)max_dist, (jint)max);

	if (gpte_jvm_error(vm, err))
		return NULL;

	jclass result_class = (*env)->FindClass(env, "de/schildbach/pte/dto/NearbyLocationsResult");

	jfieldID status_id = (*env)->GetFieldID(env, result_class, "status", "Lde/schildbach/pte/dto/NearbyLocationsResult$Status;");
	jobject status = (*env)->GetObjectField(env, res, status_id);

	jclass status_class = (*env)->FindClass(env, "de/schildbach/pte/dto/NearbyLocationsResult$Status");
	if (!(*env)->IsSameObject(env, status, GPTE_PROVIDER_RESULT_STATUS_ENUM_ENTRY(OK, "NearbyLocationsResult"))) {
		GPTE_PROVIDER_RESULT_STATUS_PROPAGTE_ERROR(GPTE_PTE_ERROR, SERVICE_DOWN, "NearbyLocationsResult", NULL, "Service Down")
		GPTE_PROVIDER_RESULT_STATUS_PROPAGTE_ERROR(GPTE_PTE_ERROR, INVALID_ID, "NearbyLocationsResult", NULL, "Invalid station identifier")
	}

	jfieldID locations_id = (*env)->GetFieldID(env, result_class, "locations", "Ljava/util/List;");
	jobject locations_list = (*env)->GetObjectField(env, res, locations_id);
	return gpte_list_new(vm, GPTE_TYPE_LOCATION, locations_list);
}

typedef struct {
	GpteLocations locations;
	GpteLocation* location;
	gint max_dist;
	gint max;
} GpteProviderQueryNearbyData;
static void gpte_provider_query_nearby_data_free(GpteProviderQueryNearbyData* self) {
	g_object_unref(self->location);
	g_free(self);
}
static void gpte_provider_query_nearby_thread(GTask* task, GpteProvider* self, GpteProviderQueryNearbyData* data, GCancellable*) {
	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_autoptr(GpteThreadGuard) guard = gpte_jvm_attach_thread(vm, "gpte-suggest-locations-async");
	if (!guard) {
		g_task_return_error(task, g_error_new(GPTE_JAVA_ERROR, GPTE_JAVA_ERROR_JVM_THREADING, "Unable to attach thread"));
		return;
	}

	GError* err = NULL;
	GListModel* model = gpte_provider_query_nearby(self, data->locations, data->location, data->max_dist, data->max, &err);
	if (model)
		g_task_return_pointer(task, model, g_object_unref);
	else
		g_task_return_error(task, err);
}
void gpte_provider_query_nearby_async(
	GpteProvider* self,
	GpteLocations locations,
	GpteLocation* location,
	gint max_dist,
	gint max,
	GCancellable* cancellable,
	GAsyncReadyCallback callback, gpointer user_data
) {
	g_return_if_fail(GPTE_IS_PROVIDER(self));
	g_autoptr(GTask) task = g_task_new(self, cancellable, callback, user_data);
	GpteProviderQueryNearbyData* data = g_new(GpteProviderQueryNearbyData, 1);
	data->locations = locations;
	data->location = g_object_ref(location);
	data->max_dist = max_dist;
	data->max = max;
	g_task_set_task_data(task, data, (GDestroyNotify)gpte_provider_query_nearby_data_free);
	g_task_run_in_thread(task, (GTaskThreadFunc)gpte_provider_query_nearby_thread);
}

GListModel* gpte_provider_query_nearby_finish(GpteProvider* self, GAsyncResult* result, GError** error) {
	g_return_val_if_fail(g_task_is_valid(result, self), NULL);
	return g_task_propagate_pointer(G_TASK(result), error);
}


GListModel* gpte_provider_suggest_locations(GpteProvider* self, const gchar* constraint, GpteLocations locations, gint max, GError** err) {
	g_return_val_if_fail(GPTE_IS_PROVIDER(self), NULL);
	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 7);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));

	jclass class = (*env)->FindClass(env, "de/schildbach/pte/NetworkProvider");
	jmethodID suggest_mid = (*env)->GetMethodID(env, class, "suggestLocations", "(Ljava/lang/CharSequence;Ljava/util/Set;I)Lde/schildbach/pte/dto/SuggestLocationsResult;");
	jstring jconstraint = (*env)->NewStringUTF(env, constraint);
	jobject types = gpte_locations_to_java(vm, locations);
	jobject result = (*env)->CallObjectMethod(env, this, suggest_mid, jconstraint, types, max);

	if (gpte_jvm_error(vm, err))
		return NULL;

	jclass result_class = (*env)->FindClass(env, "de/schildbach/pte/dto/SuggestLocationsResult");

	jfieldID status_id = (*env)->GetFieldID(env, result_class, "status", "Lde/schildbach/pte/dto/SuggestLocationsResult$Status;");
	jobject status = (*env)->GetObjectField(env, result, status_id);

	jclass status_class = (*env)->FindClass(env, "de/schildbach/pte/dto/SuggestLocationsResult$Status");
	if (!(*env)->IsSameObject(env, status, GPTE_PROVIDER_RESULT_STATUS_ENUM_ENTRY(OK, "SuggestLocationsResult"))) {
		GPTE_PROVIDER_RESULT_STATUS_PROPAGTE_ERROR(GPTE_PTE_ERROR, SERVICE_DOWN, "SuggestLocationsResult", NULL, "Service Down")
	}

	jmethodID loc_mid = (*env)->GetMethodID(env, result_class, "getLocations", "()Ljava/util/List;");
	jobject locations_list = (*env)->CallObjectMethod(env, result, loc_mid);
	return gpte_list_new(vm, GPTE_TYPE_LOCATION, locations_list);
}

typedef struct {
	gchar* constraint;
	GpteLocations locations;
	gint max;
} GpteProviderSuggestLocationsData;
static void gpte_provider_suggest_locations_data_free(GpteProviderSuggestLocationsData* self) {
	g_free(self->constraint);
	g_free(self);
}
static void gpte_provider_suggest_locations_thread(GTask* task, GpteProvider* self, GpteProviderSuggestLocationsData* data, GCancellable*) {
	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_autoptr(GpteThreadGuard) guard = gpte_jvm_attach_thread(vm, "gpte-suggest-locations-async");
	if (!guard) {
		g_task_return_error(task, g_error_new(GPTE_JAVA_ERROR, GPTE_JAVA_ERROR_JVM_THREADING, "Unable to attach thread"));
		return;
	}

	GError* err = NULL;
	GListModel* model = gpte_provider_suggest_locations(self, data->constraint, data->locations, data->max, &err);
	if (model)
		g_task_return_pointer(task, model, g_object_unref);
	else
		g_task_return_error(task, err);
}
void gpte_provider_suggest_locations_async(
	GpteProvider* self,
	const gchar* constraint,
	GpteLocations locations,
	gint max,
	GCancellable* cancellable,
	GAsyncReadyCallback callback, gpointer user_data
) {
	g_return_if_fail(GPTE_IS_PROVIDER(self));
	g_autoptr(GTask) task = g_task_new(self, cancellable, callback, user_data);
	GpteProviderSuggestLocationsData* data = g_new(GpteProviderSuggestLocationsData, 1);
	data->constraint = constraint ? g_strdup(constraint) : NULL;
	data->locations = locations;
	data->max = max;
	g_task_set_task_data(task, data, (GDestroyNotify)gpte_provider_suggest_locations_data_free);
	g_task_run_in_thread(task, (GTaskThreadFunc)gpte_provider_suggest_locations_thread);
}

GListModel* gpte_provider_suggest_locations_finish(GpteProvider* self, GAsyncResult* result, GError** error) {
	g_return_val_if_fail(g_task_is_valid(result, self), NULL);
	return g_task_propagate_pointer(G_TASK(result), error);
}
