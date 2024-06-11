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

#include "gptetrips.h"
#include "gptetrips-priv.h"

#include "gptejavaobject-priv.h"
#include "gptelist-priv.h"
#include "gpteproducts-priv.h"

#include "gpteprovider.h"
#include "gpteerrors.h"

G_DEFINE_ENUM_TYPE(GpteTripAccessibility, gpte_trip_accessibility,
	G_DEFINE_ENUM_VALUE(GPTE_ACCESSIBILITY_NEUTRAL, "neutral"),
	G_DEFINE_ENUM_VALUE(GPTE_ACCESSIBILITY_LIMITED, "limited"),
	G_DEFINE_ENUM_VALUE(GPTE_ACCESSIBILITY_BARRIER_FREE, "barrier-free")
)

G_DEFINE_FLAGS_TYPE(GpteTripFlags, gpte_trip_flags,
	G_DEFINE_ENUM_VALUE(GPTE_TRIP_FLAG_BIKE, "bike")
)

G_DEFINE_ENUM_TYPE(GpteTripOptimization, gpte_trip_optimization,
	G_DEFINE_ENUM_VALUE(GPTE_OPTIMIZE_LEAST_DURATION, "least-duration"),
	G_DEFINE_ENUM_VALUE(GPTE_OPTIMIZE_LEAST_CHANGES, "least-changes"),
	G_DEFINE_ENUM_VALUE(GPTE_OPTIMIZE_LEAST_WALKING, "least-walking")
)

G_DEFINE_ENUM_TYPE(GpteWalkSpeed, gpte_walk_speed,
	G_DEFINE_ENUM_VALUE(GPTE_WALK_SLOW, "slow"),
	G_DEFINE_ENUM_VALUE(GPTE_WALK_NORMAL, "normal"),
	G_DEFINE_ENUM_VALUE(GPTE_WALK_FAST, "fast")
)

G_DEFINE_BOXED_TYPE(GpteTripOptions, gpte_trip_options, gpte_trip_options_copy, gpte_trip_options_free);
GpteTripOptions* gpte_trip_options_copy(const GpteTripOptions* self) {
	return g_memdup2(self, sizeof(GpteTripOptions));
}
void gpte_trip_options_free(GpteTripOptions* self) {
	g_free(self);
}

jobject gpte_trip_options_to_java(GpteJvm* vm, const GpteTripOptions* options) {
	if (!options)
		return NULL;

	GpteScopeGuard env = gpte_jvm_enter_scope(vm, 18);

	jclass accessibility_enum = (*env)->FindClass(env, "de/schildbach/pte/NetworkProvider$Accessibility");
	jobject accessibility = NULL;
	switch (options->accessibility) {
		case GPTE_ACCESSIBILITY_NEUTRAL:
			accessibility = (*env)->GetStaticObjectField(env, accessibility_enum, (*env)->GetStaticFieldID(env, accessibility_enum, "NEUTRAL", "Lde/schildbach/pte/NetworkProvider$Accessibility;"));
			break;
		case GPTE_ACCESSIBILITY_LIMITED:
			accessibility = (*env)->GetStaticObjectField(env, accessibility_enum, (*env)->GetStaticFieldID(env, accessibility_enum, "LIMITED", "Lde/schildbach/pte/NetworkProvider$Accessibility;"));
			break;
		case GPTE_ACCESSIBILITY_BARRIER_FREE:
			accessibility = (*env)->GetStaticObjectField(env, accessibility_enum, (*env)->GetStaticFieldID(env, accessibility_enum, "BARRIER_FREE", "Lde/schildbach/pte/NetworkProvider$Accessibility;"));
			break;
	}

	jclass set_class = (*env)->FindClass(env, "java/util/HashSet");
	jmethodID set_constructor = (*env)->GetMethodID(env, set_class, "<init>", "()V");
	jmethodID add_to_set = (*env)->GetMethodID(env, set_class, "add", "(Ljava/lang/Object;)Z");

	jobject flags = (*env)->NewObject(env, set_class, set_constructor);
	jclass flags_enum = (*env)->FindClass(env, "de/schildbach/pte/NetworkProvider$TripFlag");
	if (options->flags & GPTE_TRIP_FLAG_BIKE) {
		jobject bike = (*env)->GetStaticObjectField(env, flags_enum, (*env)->GetStaticFieldID(env, flags_enum, "BIKE", "Lde/schildbach/pte/NetworkProvider$TripFlag;"));
		(*env)->CallBooleanMethod(env, flags, add_to_set, bike);
	}

	jclass optimize_enum = (*env)->FindClass(env, "de/schildbach/pte/NetworkProvider$Optimize");
	jobject optimize = NULL;
	switch (options->optimize) {
		case GPTE_OPTIMIZE_LEAST_DURATION:
			optimize = (*env)->GetStaticObjectField(env, optimize_enum, (*env)->GetStaticFieldID(env, optimize_enum, "LEAST_DURATION", "Lde/schildbach/pte/NetworkProvider$Optimize;"));
			break;
		case GPTE_OPTIMIZE_LEAST_CHANGES:
			optimize = (*env)->GetStaticObjectField(env, optimize_enum, (*env)->GetStaticFieldID(env, optimize_enum, "LEAST_CHANGES", "Lde/schildbach/pte/NetworkProvider$Optimize;"));
			break;
		case GPTE_OPTIMIZE_LEAST_WALKING:
			optimize = (*env)->GetStaticObjectField(env, optimize_enum, (*env)->GetStaticFieldID(env, optimize_enum, "LEAST_WALKING", "Lde/schildbach/pte/NetworkProvider$Optimize;"));
			break;
	}

	jobject products = gpte_products_to_java(vm, options->products);

	jclass walk_speed_enum = (*env)->FindClass(env, "de/schildbach/pte/NetworkProvider$WalkSpeed");
	jobject walk_speed = NULL;
	switch (options->walk_speed) {
		case GPTE_WALK_SLOW:
			walk_speed = (*env)->GetStaticObjectField(env, walk_speed_enum, (*env)->GetStaticFieldID(env, walk_speed_enum, "SLOW", "Lde/schildbach/pte/NetworkProvider$WalkSpeed;"));
			break;
		case GPTE_WALK_NORMAL:
			walk_speed = (*env)->GetStaticObjectField(env, walk_speed_enum, (*env)->GetStaticFieldID(env, walk_speed_enum, "NORMAL", "Lde/schildbach/pte/NetworkProvider$WalkSpeed;"));
			break;
		case GPTE_WALK_FAST:
			walk_speed = (*env)->GetStaticObjectField(env, walk_speed_enum, (*env)->GetStaticFieldID(env, walk_speed_enum, "FAST", "Lde/schildbach/pte/NetworkProvider$WalkSpeed;"));
			break;
	}

	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/TripOptions");
	jmethodID constructor = (*env)->GetMethodID(env, class, "<init>", "("
		"Ljava/util/Set;"
		"Lde/schildbach/pte/NetworkProvider$Optimize;"
		"Lde/schildbach/pte/NetworkProvider$WalkSpeed;"
		"Lde/schildbach/pte/NetworkProvider$Accessibility;"
		"Ljava/util/Set;"
	")V");
	return gpte_scope_guard_leave_with_ref(&env, (*env)->NewObject(env, class, constructor, products, optimize, walk_speed, accessibility, flags));
}


G_DEFINE_ENUM_TYPE(GpteTripsQueryTime, gpte_trips_query_time,
	G_DEFINE_ENUM_VALUE(GPTE_TRIPS_QUERY_EARLIER, "earlier"),
	G_DEFINE_ENUM_VALUE(GPTE_TRIPS_QUERY_LATER, "later")
)

G_DEFINE_ENUM_TYPE(GpteTripsResultType, gpte_trips_result_type,
	G_DEFINE_ENUM_VALUE(GPTE_TRIPS_RESULT_ERR, "err"),
	G_DEFINE_ENUM_VALUE(GPTE_TRIPS_RESULT_AMBIGUOUS, "ambiguous"),
	G_DEFINE_ENUM_VALUE(GPTE_TRIPS_RESULT_OK, "ok")
)

G_DEFINE_BOXED_TYPE(GpteTripsResult, gpte_trips_result, gpte_trips_result_copy, gpte_trips_result_free)
struct _GpteTripsResult {
	GpteTrips* inner;
};
GpteTripsResult* gpte_trips_result_copy(const GpteTripsResult* self) {
	GpteTripsResult* new = g_new(GpteTripsResult, 1);
	new->inner = g_object_ref(self->inner);
	return new;
}
void gpte_trips_result_free(GpteTripsResult* self) {
	g_object_unref(self->inner);
	g_free(self);
}

GpteTripsResultType gpte_trips_result_get(GpteTripsResult* self) {
	g_return_val_if_fail(self && GPTE_IS_TRIPS(self->inner), GPTE_TRIPS_RESULT_ERR);

	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self->inner));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 8);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self->inner));

	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/QueryTripsResult");
	jfieldID status_id = (*env)->GetFieldID(env, class, "status", "Lde/schildbach/pte/dto/QueryTripsResult$Status;");
	jobject status = (*env)->GetObjectField(env, this, status_id);

	jclass enum_class = (*env)->FindClass(env, "de/schildbach/pte/dto/QueryTripsResult$Status");
	if ((*env)->IsSameObject(env, status, (*env)->GetStaticObjectField(env, enum_class, (*env)->GetStaticFieldID(env, enum_class, "OK", "Lde/schildbach/pte/dto/QueryTripsResult$Status;"))))
		return GPTE_TRIPS_RESULT_OK;
	if ((*env)->IsSameObject(env, status, (*env)->GetStaticObjectField(env, enum_class, (*env)->GetStaticFieldID(env, enum_class, "AMBIGUOUS", "Lde/schildbach/pte/dto/QueryTripsResult$Status;"))))
		return GPTE_TRIPS_RESULT_AMBIGUOUS;
	return GPTE_TRIPS_RESULT_ERR;
}

static GListModel* gpte_trips_result_get_location_list_field(GpteTripsResult* self, const gchar* field) {
	g_return_val_if_fail(self && GPTE_IS_TRIPS(self->inner), NULL);

	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self->inner));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 3);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self->inner));

	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/QueryTripsResult");
	jfieldID field_id = (*env)->GetFieldID(env, class, field, "Ljava/util/List;");
	jobject list = (*env)->GetObjectField(env, this, field_id);
	return list ? gpte_list_new(vm, GPTE_TYPE_LOCATION, list) : NULL;
}
GListModel* gpte_trips_result_get_ambiguous_from(GpteTripsResult* self) {
	return gpte_trips_result_get_location_list_field(self, "ambiguousFrom");
}
GListModel* gpte_trips_result_get_ambiguous_via(GpteTripsResult* self) {
	return gpte_trips_result_get_location_list_field(self, "ambiguousVia");
}
GListModel* gpte_trips_result_get_ambiguous_to(GpteTripsResult* self) {
	return gpte_trips_result_get_location_list_field(self, "ambiguousTo");
}

GpteTrips* gpte_trips_result_get_trips(GpteTripsResult* self) {
	g_return_val_if_fail(gpte_trips_result_get(self) == GPTE_TRIPS_RESULT_OK, NULL);
	return g_object_ref(self->inner);
}


struct _GpteTrips {
	GpteJavaObject parent_instance;

	GMutex acp_lock;
	GCancellable* async_conflict_preventer;

	GpteProvider* provider;
	GListModel* trips;

	jobject earlier_ctx;
	jobject later_ctx;
};

static void gpte_trips_list_init(GListModelInterface* iface);
G_DEFINE_TYPE_WITH_CODE (GpteTrips, gpte_trips, GPTE_TYPE_JAVA_OBJECT,
	G_IMPLEMENT_INTERFACE(G_TYPE_LIST_MODEL, gpte_trips_list_init)
)

enum {
	PROP_PROVIDER = 1,
	N_PROPERTIES
};
static GParamSpec* obj_properties[N_PROPERTIES] = { 0, };

static void gpte_trips_finalize(GObject* object) {
	GpteTrips* self = GPTE_TRIPS(object);

	JNIEnv* env = gpte_java_object_env(GPTE_JAVA_OBJECT(self));
	(*env)->DeleteGlobalRef(env, self->earlier_ctx);
	(*env)->DeleteGlobalRef(env, self->later_ctx);

	g_object_unref(self->trips);

	g_mutex_clear(&self->acp_lock);

	G_OBJECT_CLASS(gpte_trips_parent_class)->finalize(object);
}

static void gpte_trips_dispose(GObject* object) {
	GpteTrips* self = GPTE_TRIPS(object);
	g_mutex_lock(&self->acp_lock);
	g_clear_object(&self->async_conflict_preventer);
	g_mutex_unlock(&self->acp_lock);
	g_clear_object(&self->provider);
	G_OBJECT_CLASS(gpte_trips_parent_class)->dispose(object);
}

static void gpte_trips_list_changed(GListModel*, guint position, guint removed, guint added, GpteTrips* self) {
	g_list_model_items_changed(G_LIST_MODEL(self), position, removed, added);
}
static void gpte_trips_constructed(GObject* object) {
	GpteTrips* self = GPTE_TRIPS(object);

	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 5);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));
	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/QueryTripsResult");

	jfieldID trips_id = (*env)->GetFieldID(env, class, "trips", "Ljava/util/List;");
	jobject trips = (*env)->GetObjectField(env, this, trips_id);
	self->trips = gpte_list_new(vm, GPTE_TYPE_TRIP, trips);
	g_signal_connect(self->trips, "items-changed", G_CALLBACK(gpte_trips_list_changed), self);

	jfieldID ctx_id = (*env)->GetFieldID(env, class, "context", "Lde/schildbach/pte/dto/QueryTripsContext;");
	jobject ctx = (*env)->GetObjectField(env, this, ctx_id);
	self->earlier_ctx = (*env)->NewGlobalRef(env, ctx);
	self->later_ctx = (*env)->NewGlobalRef(env, ctx);

	G_OBJECT_CLASS(gpte_trips_parent_class)->constructed(object);
}

static void gpte_trips_get_property(GObject* object, guint prop_id, GValue* val, GParamSpec* pspec) {
	GpteTrips* self = GPTE_TRIPS(object);
	switch (prop_id) {
		case PROP_PROVIDER:
			g_value_set_object(val, self->provider);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
	}
}
static void gpte_trips_set_property(GObject* object, guint prop_id, const GValue* val, GParamSpec* pspec) {
	GpteTrips* self = GPTE_TRIPS(object);
	switch (prop_id) {
		case PROP_PROVIDER:
			g_return_if_fail(self->provider == NULL);
			self->provider = g_value_dup_object(val);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
	}
}

static void gpte_trips_class_init(GpteTripsClass* class) {
	GObjectClass* object_class = G_OBJECT_CLASS(class);
	object_class->finalize = gpte_trips_finalize;
	object_class->dispose = gpte_trips_dispose;
	object_class->constructed = gpte_trips_constructed;
	object_class->get_property = gpte_trips_get_property;
	object_class->set_property = gpte_trips_set_property;

	obj_properties[PROP_PROVIDER] = g_param_spec_object("provider", NULL, NULL, GPTE_TYPE_PROVIDER, G_PARAM_STATIC_STRINGS | G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
	g_object_class_install_properties(object_class, N_PROPERTIES, obj_properties);
}
static void gpte_trips_init(GpteTrips* self) {
	self->provider = NULL;
	self->trips = NULL;
	self->earlier_ctx = NULL;
	self->later_ctx = NULL;
	g_mutex_init(&self->acp_lock);
	self->async_conflict_preventer = g_cancellable_new();
}

static GType gpte_trips_list_get_item_type(GListModel*) {
	return GPTE_TYPE_TRIP;
}
static guint gpte_trips_list_get_n_items(GListModel* model) {
	return g_list_model_get_n_items(GPTE_TRIPS(model)->trips);
}
static gpointer gpte_trips_list_get_item(GListModel* model, guint idx) {
	return g_list_model_get_item(GPTE_TRIPS(model)->trips, idx);
}
static void gpte_trips_list_init(GListModelInterface* iface) {
	iface->get_item_type = gpte_trips_list_get_item_type;
	iface->get_n_items = gpte_trips_list_get_n_items;
	iface->get_item = gpte_trips_list_get_item;
}

static GpteTrips* gpte_trips_new(GpteJvm* vm, jobject this, GpteProvider* provider) {
	return g_object_new(GPTE_TYPE_TRIPS, "vm", vm, "object", this, "provider", provider, NULL);
}

static GpteLocation* gpte_trips_get_location_field(GpteTrips* self, const gchar* field) {
	g_return_val_if_fail(GPTE_IS_TRIPS(self), NULL);

	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 7);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));

	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/QueryTripsResult");
	jfieldID field_id = (*env)->GetFieldID(env, class, field, "Lde/schildbach/pte/dto/Location;");
	jobject location = (*env)->GetObjectField(env, this, field_id);
	return location ? g_object_new(GPTE_TYPE_LOCATION, "vm", vm, "object", location, NULL) : NULL;
}
GpteLocation* gpte_trips_get_from(GpteTrips* self) {
	return gpte_trips_get_location_field(self, "from");
}
GpteLocation* gpte_trips_get_via(GpteTrips* self) {
	return gpte_trips_get_location_field(self, "via");
}
GpteLocation* gpte_trips_get_to(GpteTrips* self) {
	return gpte_trips_get_location_field(self, "to");
}

static void gpte_trips_acp_refresh(GpteTrips* self) {
	g_mutex_lock(&self->acp_lock);
	g_cancellable_cancel(self->async_conflict_preventer);
	g_object_unref(self->async_conflict_preventer);
	self->async_conflict_preventer = g_cancellable_new();
	g_mutex_unlock(&self->acp_lock);
}

static jobject gpte_trips_query_more_query_obj(GpteTrips* self, GpteTripsQueryTime time, GError** err) {
	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	GpteScopeGuard env = gpte_jvm_enter_scope(vm, 3);

	jobject jprovider = gpte_java_object_get(GPTE_JAVA_OBJECT(self->provider));
	jclass provider_class = (*env)->FindClass(env, "de/schildbach/pte/NetworkProvider");
	jmethodID query_more = (*env)->GetMethodID(env, provider_class, "queryMoreTrips", "(Lde/schildbach/pte/dto/QueryTripsContext;Z)Lde/schildbach/pte/dto/QueryTripsResult;");

	jobject new = (*env)->CallObjectMethod(env, jprovider, query_more,
		time == GPTE_TRIPS_QUERY_EARLIER ? self->earlier_ctx : self->later_ctx,
		(jboolean)(time == GPTE_TRIPS_QUERY_LATER)
	);

	// TODO: error checking
	if (gpte_jvm_error(vm, err)) {
		gpte_scope_guard_leave(&env);
		return NULL;
	}

	return gpte_scope_guard_leave_with_ref(&env, new);
}
static void gpte_trips_push_more_result(GpteTrips* self, GpteTripsQueryTime time, jobject result) {
	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 5);
	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/QueryTripsResult");

	jfieldID ctx_id = (*env)->GetFieldID(env, class, "context", "Lde/schildbach/pte/dto/QueryTripsContext;");
	jobject ctx = (*env)->GetObjectField(env, result, ctx_id);
	jfieldID trips_id = (*env)->GetFieldID(env, class, "trips", "Ljava/util/List;");
	jobject trips = (*env)->GetObjectField(env, result, trips_id);
	if (time == GPTE_TRIPS_QUERY_EARLIER) {
		(*env)->DeleteGlobalRef(env, self->earlier_ctx);
		self->earlier_ctx = (*env)->NewGlobalRef(env, ctx);
		gpte_list_prepend(GPTE_LIST(self->trips), trips);
	} else {
		(*env)->DeleteGlobalRef(env, self->later_ctx);
		self->later_ctx = (*env)->NewGlobalRef(env, ctx);
		gpte_list_append(GPTE_LIST(self->trips), trips);
	}
}
gboolean gpte_trips_query_more(GpteTrips* self, GpteTripsQueryTime time, GError** err) {
	g_return_val_if_fail(GPTE_IS_TRIPS(self), FALSE);
	gpte_trips_acp_refresh(self);

	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) guard = gpte_jvm_enter_scope(vm, 1);

	jobject new = gpte_trips_query_more_query_obj(self, time, err);
	if (!new)
		return FALSE;

	gpte_trips_push_more_result(self, time, new);
	return TRUE;
}

typedef struct {
	GpteTripsQueryTime query_time;
} GpteTripsQueryMoreData;
static void gpte_trips_query_more_data_free(GpteTripsQueryMoreData* self) {
	g_free(self);
}
static void gpte_trips_query_nearby_thread(GTask* task, GpteTrips* self, GpteTripsQueryMoreData* data, GCancellable*) {
	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_autoptr(GpteThreadGuard) guard = gpte_jvm_attach_thread(vm, "gpte-query-more-async");
	if (!guard) {
		g_task_return_error(task, g_error_new(GPTE_JAVA_ERROR, GPTE_JAVA_ERROR_JVM_THREADING, "Unable to attach thread"));
		return;
	}
	g_auto(GpteScopeGuard) scope_guard = gpte_jvm_enter_scope(vm, 1);

	GError* err = NULL;
	jobject obj = gpte_trips_query_more_query_obj(self, data->query_time, &err);
	if (obj)
		g_task_return_pointer(task, g_object_new(GPTE_TYPE_JAVA_OBJECT, "vm", vm, "object", obj, NULL), g_object_unref);
	else
		g_task_return_error(task, err);
}
void gpte_trips_query_more_async(GpteTrips* self, GpteTripsQueryTime time, GAsyncReadyCallback callback, gpointer user_data) {
	g_return_if_fail(GPTE_IS_TRIPS(self));
	gpte_trips_acp_refresh(self);

	g_mutex_lock(&self->acp_lock);
	g_autoptr(GTask) task = g_task_new(self, self->async_conflict_preventer, callback, user_data);
	g_mutex_unlock(&self->acp_lock);

	GpteTripsQueryMoreData* data = g_new(GpteTripsQueryMoreData, 1);
	data->query_time = time;
	g_task_set_task_data(task, data, (GDestroyNotify)gpte_trips_query_more_data_free);
	g_task_run_in_thread(task, (GTaskThreadFunc)gpte_trips_query_nearby_thread);
}
gboolean gpte_trips_query_more_finish(GpteTrips* self, GAsyncResult* result, GError** error) {
	g_return_val_if_fail(g_task_is_valid(result, self), FALSE);
	g_autoptr(GpteJavaObject) obj = g_task_propagate_pointer(G_TASK(result), error);
	if (!obj)
		return FALSE;

	GpteTripsQueryMoreData* data = g_task_get_task_data(G_TASK(result));
	gpte_trips_push_more_result(self, data->query_time, gpte_java_object_get(obj));
	return TRUE;
}

GpteTripsResult* gpte_trips_result_new(GpteJvm* vm, jobject obj, GpteProvider* provider) {
	GpteTripsResult* new = g_new(GpteTripsResult, 1);
	new->inner = gpte_trips_new(vm, obj, provider);
	return new;
}
