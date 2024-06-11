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

#include "gptetripleg.h"
#include "gptejavaobject-priv.h"

#include "gpteutils-priv.h"
#include "gptelist-priv.h"
#include "gptegeo-priv.h"

G_DEFINE_ENUM_TYPE(GpteTripIndividualType, gpte_trip_individual_type,
	G_DEFINE_ENUM_VALUE(GPTE_TRIP_INDIVIDUAL_NULL, "null"),
	G_DEFINE_ENUM_VALUE(GPTE_TRIP_INDIVIDUAL_BIKE, "bike"),
	G_DEFINE_ENUM_VALUE(GPTE_TRIP_INDIVIDUAL_CAR, "car"),
	G_DEFINE_ENUM_VALUE(GPTE_TRIP_INDIVIDUAL_CHECK_IN, "check-in"),
	G_DEFINE_ENUM_VALUE(GPTE_TRIP_INDIVIDUAL_CHECK_OUT, "check-out"),
	G_DEFINE_ENUM_VALUE(GPTE_TRIP_INDIVIDUAL_TRANSFER, "transfer"),
	G_DEFINE_ENUM_VALUE(GPTE_TRIP_INDIVIDUAL_WALK, "walk")
)


typedef enum {
	GPTE_TRIP_LEG_CACHED_DEPARTURE = 1 << 0,
	GPTE_TRIP_LEG_CACHED_ARRIVAL = 1 << 1,
	GPTE_TRIP_LEG_CACHED_PATH = 1 << 2,
	GPTE_TRIP_LEG_CACHED_DEPARTURE_TIME = 1 << 3,
	GPTE_TRIP_LEG_CACHED_ARRIVAL_TIME = 1 << 4,
	GPTE_TRIP_LEG_CACHED_MIN_TIME = 1 << 5,
	GPTE_TRIP_LEG_CACHED_MAX_TIME = 1 << 6
} GpteTripLegCachedValues;

typedef struct {
	GpteTripLegCachedValues cached;

	GpteLocation* cached_departure;
	GpteLocation* cached_arrival;
	GArray* cached_path;
	GDateTime* cached_depature_time;
	GDateTime* cached_arrival_time;
	GDateTime* cached_min_time;
	GDateTime* cached_max_time;
} GpteTripLegPrivate;

// TODO: this could be ABSTRACT, but I think it'd needlessly confuse people
// It'd also cause a wierd error message in case some invalid jobject was passed in (or PTE adds a new trip type)
G_DEFINE_TYPE_WITH_PRIVATE (GpteTripLeg, gpte_trip_leg, GPTE_TYPE_JAVA_OBJECT)

static void gpte_trip_leg_dispose(GObject* object) {
	GpteTripLegPrivate* priv = gpte_trip_leg_get_instance_private(GPTE_TRIP_LEG(object));
	if (priv->cached & GPTE_TRIP_LEG_CACHED_DEPARTURE)
		g_object_unref(priv->cached_departure);
	if (priv->cached & GPTE_TRIP_LEG_CACHED_ARRIVAL)
		g_object_unref(priv->cached_arrival);
	if (priv->cached & GPTE_TRIP_LEG_CACHED_PATH)
		g_array_unref(priv->cached_path);
	if (priv->cached & GPTE_TRIP_LEG_CACHED_DEPARTURE_TIME)
		g_date_time_unref(priv->cached_depature_time);
	if (priv->cached & GPTE_TRIP_LEG_CACHED_ARRIVAL_TIME)
		g_date_time_unref(priv->cached_arrival_time);
	if (priv->cached & GPTE_TRIP_LEG_CACHED_MIN_TIME)
		g_date_time_unref(priv->cached_min_time);
	if (priv->cached & GPTE_TRIP_LEG_CACHED_MAX_TIME)
		g_date_time_unref(priv->cached_max_time);
	priv->cached = 0;
	G_OBJECT_CLASS(gpte_trip_leg_parent_class)->dispose(object);
}

static GObject* gpte_trip_leg_constructor(GType type, guint n_construct_props, GObjectConstructParam* construct_props) {
	if (type == GPTE_TYPE_TRIP_LEG && n_construct_props >= 2) {
		GpteJvm* vm = NULL;
		jobject this = NULL;
		for (guint i = 0; i < n_construct_props; i++) {
			if (g_strcmp0(construct_props[i].pspec->name, "vm") == 0)
				vm = g_value_get_boxed(construct_props[i].value);
			else if (g_strcmp0(construct_props[i].pspec->name, "object") == 0)
				this = g_value_get_pointer(construct_props[i].value);
		}

		if (vm && this) {
			g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 2);
			jclass individual = (*env)->FindClass(env, "de/schildbach/pte/dto/Trip$Individual");
			jclass public = (*env)->FindClass(env, "de/schildbach/pte/dto/Trip$Public");
			if ((*env)->IsInstanceOf(env, this, individual))
				type = GPTE_TYPE_TRIP_INDIVIDUAL;
			else if ((*env)->IsInstanceOf(env, this, public))
				type = GPTE_TYPE_TRIP_PUBLIC;
		}
	}

	return G_OBJECT_CLASS(gpte_trip_leg_parent_class)->constructor(type, n_construct_props, construct_props);
}

static void gpte_trip_leg_class_init(GpteTripLegClass* class) {
	GObjectClass* object_class = G_OBJECT_CLASS(class);
	object_class->dispose = gpte_trip_leg_dispose;
	object_class->constructor = gpte_trip_leg_constructor;
}
static void gpte_trip_leg_init(GpteTripLeg* self) {
	GpteTripLegPrivate* priv = gpte_trip_leg_get_instance_private(self);
	priv->cached = 0;
}

static GpteLocation* gpte_trip_leg_get_location_field(GpteTripLeg* self, const gchar* field, GpteLocation** cache, GpteTripLegCachedValues cache_field) {
	GpteTripLegPrivate* priv = gpte_trip_leg_get_instance_private(self);
	if (priv->cached & cache_field)
		return *cache;

	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 3);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));

	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/Trip$Leg");
	jfieldID id = (*env)->GetFieldID(env, class, field, "Lde/schildbach/pte/dto/Location;");
	jobject location = (*env)->GetObjectField(env, this, id);

	*cache = g_object_new(GPTE_TYPE_LOCATION, "vm", vm, "object", location, NULL);
	priv->cached |= cache_field;
	return *cache;
}

GpteLocation* gpte_trip_leg_get_departure(GpteTripLeg* self) {
	g_return_val_if_fail(GPTE_IS_TRIP_LEG(self), NULL);
	GpteTripLegPrivate* priv = gpte_trip_leg_get_instance_private(self);
	return gpte_trip_leg_get_location_field(self, "departure", &priv->cached_departure, GPTE_TRIP_LEG_CACHED_DEPARTURE);
}

GpteLocation* gpte_trip_leg_get_arrival(GpteTripLeg* self) {
	g_return_val_if_fail(GPTE_IS_TRIP_LEG(self), NULL);
	GpteTripLegPrivate* priv = gpte_trip_leg_get_instance_private(self);
	return gpte_trip_leg_get_location_field(self, "arrival", &priv->cached_arrival, GPTE_TRIP_LEG_CACHED_ARRIVAL);
}

GArray* gpte_trip_leg_get_path(GpteTripLeg* self) {
	g_return_val_if_fail(GPTE_IS_TRIP_LEG(self), NULL);
	GpteTripLegPrivate* priv = gpte_trip_leg_get_instance_private(self);
	if (priv->cached & GPTE_TRIP_LEG_CACHED_PATH)
		return priv->cached_path;

	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 7);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));

	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/Trip$Leg");
	jfieldID id = (*env)->GetFieldID(env, class, "path", "Ljava/util/List;");
	jobject path_list = (*env)->GetObjectField(env, this, id);
	if (!path_list)
		return NULL;

	jclass list = (*env)->FindClass(env, "java/util/List");
	jmethodID to_array = (*env)->GetMethodID(env, list, "toArray", "()[Ljava/lang/Object;");
	jobjectArray path_arr = (*env)->CallObjectMethod(env, path_list, to_array);
	jsize len = (*env)->GetArrayLength(env, path_arr);

	priv->cached_path = g_array_sized_new(FALSE, TRUE, sizeof(GpteGeoPoint), len);
	for (jsize i = 0; i < len; i++) {
		jobject jpoint = (*env)->GetObjectArrayElement(env, path_arr, i);
		GpteGeoPoint p = gpte_geo_point_from_java(vm, jpoint);
		g_array_append_val(priv->cached_path, p);
		(*env)->DeleteLocalRef(env, jpoint);
	}

	priv->cached |= GPTE_TRIP_LEG_CACHED_PATH;
	return priv->cached_path;
}

static GDateTime* gpte_trip_leg_call_date_getter(GpteTripLeg* self, const gchar* method, GDateTime** cache, GpteTripLegCachedValues cache_field) {
	GpteTripLegPrivate* priv = gpte_trip_leg_get_instance_private(self);
	if (priv->cached & cache_field)
		return *cache;

	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 3);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));

	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/Trip$Leg");
	jmethodID mid = (*env)->GetMethodID(env, class, method, "()Ljava/util/Date;");
	jobject jdate = (*env)->CallObjectMethod(env, this, mid);

	*cache = gpte_date_from_java(vm, jdate);
	priv->cached |= cache_field;
	return *cache;
}

GDateTime* gpte_trip_leg_get_departure_time(GpteTripLeg* self) {
	g_return_val_if_fail(GPTE_IS_TRIP_LEG(self), NULL);
	GpteTripLegPrivate* priv = gpte_trip_leg_get_instance_private(self);
	return gpte_trip_leg_call_date_getter(self, "getDepartureTime", &priv->cached_depature_time, GPTE_TRIP_LEG_CACHED_DEPARTURE_TIME);
}

GDateTime* gpte_trip_leg_get_arrival_time(GpteTripLeg* self) {
	g_return_val_if_fail(GPTE_IS_TRIP_LEG(self), NULL);
	GpteTripLegPrivate* priv = gpte_trip_leg_get_instance_private(self);
	return gpte_trip_leg_call_date_getter(self, "getArrivalTime", &priv->cached_arrival_time, GPTE_TRIP_LEG_CACHED_ARRIVAL_TIME);
}

GDateTime* gpte_trip_leg_get_min_time(GpteTripLeg* self) {
	g_return_val_if_fail(GPTE_IS_TRIP_LEG(self), NULL);
	GpteTripLegPrivate* priv = gpte_trip_leg_get_instance_private(self);
	return gpte_trip_leg_call_date_getter(self, "getMinTime", &priv->cached_min_time, GPTE_TRIP_LEG_CACHED_MIN_TIME);
}

GDateTime* gpte_trip_leg_get_max_time(GpteTripLeg* self) {
	g_return_val_if_fail(GPTE_IS_TRIP_LEG(self), NULL);
	GpteTripLegPrivate* priv = gpte_trip_leg_get_instance_private(self);
	return gpte_trip_leg_call_date_getter(self, "getMaxTime", &priv->cached_max_time, GPTE_TRIP_LEG_CACHED_MAX_TIME);
}

// BEGIN Trip.Individual

typedef enum {
	GPTE_TRIP_INDIVIDUAL_CACHED_TYPE = 1 << 0,
	GPTE_TRIP_INDIVIDUAL_CACHED_DISTANCE = 1 << 1
} GpteTripIndividualCachedValues;

struct _GpteTripIndividual {
	GpteTripLeg parent_instance;

	GpteTripIndividualCachedValues cached;

	GpteTripIndividualType cached_type;
	gint cached_distance;
};
G_DEFINE_FINAL_TYPE (GpteTripIndividual, gpte_trip_individual, GPTE_TYPE_TRIP_LEG)

static void gpte_trip_individual_class_init(GpteTripIndividualClass*) {}
static void gpte_trip_individual_init(GpteTripIndividual* self) {
	self->cached = 0;
}

GpteTripIndividualType gpte_trip_individual_type(GpteTripIndividual* self) {
	g_return_val_if_fail(GPTE_IS_TRIP_INDIVIDUAL(self), GPTE_TRIP_INDIVIDUAL_NULL);
	if (self->cached & GPTE_TRIP_INDIVIDUAL_CACHED_TYPE)
		return self->cached_type;

	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 16);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));
	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/Trip$Individual");

	jfieldID field_id = (*env)->GetFieldID(env, class, "type", "Lde/schildbach/pte/dto/Trip$Individual$Type;");
	jobject jtype = (*env)->GetObjectField(env, this, field_id);
	jclass type_class = (*env)->FindClass(env, "de/schildbach/pte/dto/Trip$Individual$Type");

	if ((*env)->IsSameObject(env, jtype, (*env)->GetStaticObjectField(
		env, type_class,
		(*env)->GetStaticFieldID(env, type_class , "BIKE", "Lde/schildbach/pte/dto/Trip$Individual$Type;")
	)))
		self->cached_type = GPTE_TRIP_INDIVIDUAL_BIKE;
	else if ((*env)->IsSameObject(env, jtype, (*env)->GetStaticObjectField(
		env, type_class,
		(*env)->GetStaticFieldID(env, type_class , "CAR", "Lde/schildbach/pte/dto/Trip$Individual$Type;")
	)))
		self->cached_type = GPTE_TRIP_INDIVIDUAL_CAR;
	else if ((*env)->IsSameObject(env, jtype, (*env)->GetStaticObjectField(
		env, type_class,
		(*env)->GetStaticFieldID(env, type_class , "CHECK_IN", "Lde/schildbach/pte/dto/Trip$Individual$Type;")
	)))
		self->cached_type = GPTE_TRIP_INDIVIDUAL_CHECK_IN;
	else if ((*env)->IsSameObject(env, jtype, (*env)->GetStaticObjectField(
		env, type_class,
		(*env)->GetStaticFieldID(env, type_class , "CHECK_OUT", "Lde/schildbach/pte/dto/Trip$Individual$Type;")
	)))
		self->cached_type = GPTE_TRIP_INDIVIDUAL_CHECK_OUT;
	else if ((*env)->IsSameObject(env, jtype, (*env)->GetStaticObjectField(
		env, type_class,
		(*env)->GetStaticFieldID(env, type_class , "TRANSFER", "Lde/schildbach/pte/dto/Trip$Individual$Type;")
	)))
		self->cached_type = GPTE_TRIP_INDIVIDUAL_TRANSFER;
	else if ((*env)->IsSameObject(env, jtype, (*env)->GetStaticObjectField(
		env, type_class,
		(*env)->GetStaticFieldID(env, type_class , "WALK", "Lde/schildbach/pte/dto/Trip$Individual$Type;")
	)))
		self->cached_type = GPTE_TRIP_INDIVIDUAL_WALK;
	else {
		g_critical("Unknown ind. trip type: %p", jtype);
		return GPTE_TRIP_INDIVIDUAL_NULL;
	}
	self->cached |= GPTE_TRIP_INDIVIDUAL_CACHED_TYPE;
	return self->cached_type;
}

gint gpte_trip_individual_get_distance(GpteTripIndividual* self) {
	g_return_val_if_fail(GPTE_IS_TRIP_INDIVIDUAL(self), GPTE_TRIP_INDIVIDUAL_NULL);
	if (self->cached & GPTE_TRIP_INDIVIDUAL_CACHED_DISTANCE)
			return self->cached_type;

	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 2);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));
	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/Trip$Individual");

	jfieldID field_id = (*env)->GetFieldID(env, class, "distance", "I");
	self->cached_distance = (*env)->GetIntField(env, this, field_id);

	self->cached |= GPTE_TRIP_INDIVIDUAL_CACHED_DISTANCE;
	return self->cached_distance;
}

// BEGIN Trip.Public

typedef enum {
	GPTE_TRIP_PUBLIC_CACHED_DEPARTURE = 1 << 0,
	GPTE_TRIP_PUBLIC_CACHED_ARRIVAL = 1 << 1,
	GPTE_TRIP_PUBLIC_CACHED_INTERMEDIATE = 1 << 2,
	GPTE_TRIP_PUBLIC_CACHED_DESTINATION = 1 << 3,
	GPTE_TRIP_PUBLIC_CACHED_LINE = 1 << 4,
	GPTE_TRIP_PUBLIC_CACHED_MESSAGE = 1 << 5
} GpteTripPublicCachedValues;

struct _GpteTripPublic {
	GpteTripLeg parent_instance;

	GpteTripPublicCachedValues cached;
	GpteStop* cached_departure;
	GpteStop* cached_arrival;
	GListModel* cached_intermediate;
	GpteLocation* cached_destination;
	GpteLine* cached_line;
	gchar* cached_message;
};
G_DEFINE_FINAL_TYPE (GpteTripPublic, gpte_trip_public, GPTE_TYPE_TRIP_LEG)

static void gpte_trip_public_dispose(GObject* object) {
	GpteTripPublic* self = GPTE_TRIP_PUBLIC(object);
	if (self->cached & GPTE_TRIP_PUBLIC_CACHED_DEPARTURE)
		g_object_unref(self->cached_departure);
	if (self->cached & GPTE_TRIP_PUBLIC_CACHED_ARRIVAL)
		g_object_unref(self->cached_arrival);
	if (self->cached & GPTE_TRIP_PUBLIC_CACHED_INTERMEDIATE)
		g_object_unref(self->cached_intermediate);
	if (self->cached & GPTE_TRIP_PUBLIC_CACHED_DESTINATION)
		g_object_unref(self->cached_destination);
	if (self->cached & GPTE_TRIP_PUBLIC_CACHED_LINE)
		g_object_unref(self->cached_line);
	if (self->cached & GPTE_TRIP_PUBLIC_CACHED_MESSAGE)
		g_free(self->cached_message);
	self->cached = 0;
	G_OBJECT_CLASS(gpte_trip_leg_parent_class)->dispose(object);
}

static void gpte_trip_public_class_init(GpteTripPublicClass* class) {
	GObjectClass* object_class = G_OBJECT_CLASS(class);
	object_class->dispose = gpte_trip_public_dispose;
}

static void gpte_trip_public_init(GpteTripPublic* self) {
	self->cached = 0;
}

static GpteStop* gpte_trip_public_get_stop_field(GpteTripPublic* self, const gchar* field, GpteStop** cache, GpteTripPublicCachedValues cache_field) {
	if (self->cached & cache_field)
		return *cache;
	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 3);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));

	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/Trip$Public");
	jfieldID id = (*env)->GetFieldID(env, class, field, "Lde/schildbach/pte/dto/Stop;");
	jobject stop = (*env)->GetObjectField(env, this, id);
	*cache = g_object_new(GPTE_TYPE_STOP, "vm", vm, "object", stop, NULL);
	self->cached |= cache_field;
	return *cache;
}
GpteStop* gpte_trip_public_get_departure(GpteTripPublic* self) {
	g_return_val_if_fail(GPTE_IS_TRIP_PUBLIC(self), NULL);
	return gpte_trip_public_get_stop_field(self, "departureStop", &self->cached_departure, GPTE_TRIP_PUBLIC_CACHED_DEPARTURE);
}
GpteStop* gpte_trip_public_get_arrival(GpteTripPublic* self) {
	g_return_val_if_fail(GPTE_IS_TRIP_PUBLIC(self), NULL);
	return gpte_trip_public_get_stop_field(self, "arrivalStop", &self->cached_arrival, GPTE_TRIP_PUBLIC_CACHED_ARRIVAL);
}
GListModel* gpte_trip_public_get_intermediate(GpteTripPublic* self) {
	g_return_val_if_fail(GPTE_IS_TRIP_PUBLIC(self), NULL);
	if (self->cached & GPTE_TRIP_PUBLIC_CACHED_INTERMEDIATE)
		return self->cached_intermediate;

	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 3);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));

	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/Trip$Public");
	jfieldID id = (*env)->GetFieldID(env, class, "intermediateStops", "Ljava/util/List;");
	jobject list = (*env)->GetObjectField(env, this, id);
	self->cached_intermediate = list ? gpte_list_new(vm, GPTE_TYPE_STOP, list) : NULL;
	self->cached |= GPTE_TRIP_PUBLIC_CACHED_INTERMEDIATE;
	return self->cached_intermediate;
}

GpteLocation* gpte_trip_public_get_destination(GpteTripPublic* self) {
	g_return_val_if_fail(GPTE_IS_TRIP_PUBLIC(self), NULL);
	if (self->cached & GPTE_TRIP_PUBLIC_CACHED_DESTINATION)
		return self->cached_destination;

	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 3);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));

	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/Trip$Public");
	jfieldID id = (*env)->GetFieldID(env, class, "destination", "Lde/schildbach/pte/dto/Location;");
	jobject location = (*env)->GetObjectField(env, this, id);
	self->cached_destination = location ? g_object_new(GPTE_TYPE_LOCATION, "vm", vm, "object", location, NULL) : NULL;
	self->cached |= GPTE_TRIP_PUBLIC_CACHED_DESTINATION;
	return self->cached_destination;
}

GpteLine* gpte_trip_public_get_line(GpteTripPublic* self) {
	g_return_val_if_fail(GPTE_IS_TRIP_PUBLIC(self), NULL);
	if (self->cached & GPTE_TRIP_PUBLIC_CACHED_LINE)
		return self->cached_line;

	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 3);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));

	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/Trip$Public");
	jfieldID id = (*env)->GetFieldID(env, class, "line", "Lde/schildbach/pte/dto/Line;");
	jobject location = (*env)->GetObjectField(env, this, id);
	self->cached_line = g_object_new(GPTE_TYPE_LINE, "vm", vm, "object", location, NULL);
	self->cached |= GPTE_TRIP_PUBLIC_CACHED_LINE;
	return self->cached_line;
}

const gchar* gpte_trip_public_get_message(GpteTripPublic* self) {
	g_return_val_if_fail(GPTE_IS_TRIP_PUBLIC(self), NULL);
	if (self->cached & GPTE_TRIP_PUBLIC_CACHED_MESSAGE)
		return self->cached_message;

	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 3);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));

	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/Trip$Public");
	jfieldID id = (*env)->GetFieldID(env, class, "message", "Ljava/lang/String;");
	jstring msg = (*env)->GetObjectField(env, this, id);
	if (!msg) {
		self->cached_message = NULL;
		self->cached |= GPTE_TRIP_PUBLIC_CACHED_MESSAGE;
		return NULL;
	}

	const char* utf = (*env)->GetStringUTFChars(env, msg, NULL);
	self->cached_message = g_strdup(utf);
	self->cached |= GPTE_TRIP_PUBLIC_CACHED_MESSAGE;
	(*env)->ReleaseStringUTFChars(env, msg, utf);
	return self->cached_message;
}
