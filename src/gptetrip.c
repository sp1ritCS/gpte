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

#include "gptetrip.h"
#include "gptejavaobject-priv.h"

#include "gpteutils-priv.h"
#include "gptelist-priv.h"
#include "gpteproducts-priv.h"

typedef enum {
	GPTE_TRIP_CACHED_FROM_LOC = 1 << 0,
	GPTE_TRIP_CACHED_TO_LOC = 1 << 1,
	GPTE_TRIP_CACHED_LEGS = 1 << 2,
	GPTE_TRIP_CACHED_CHANGES = 1 << 3,
	GPTE_TRIP_CACHED_DURATION = 1 << 4,
	GPTE_TRIP_CACHED_FIRST_PUBLIC_LEG = 1 << 5,
	GPTE_TRIP_CACHED_LAST_PUBLIC_LEG = 1 << 6,
	GPTE_TRIP_CACHED_FIRST_DEPARTURE = 1 << 7,
	GPTE_TRIP_CACHED_LAST_ARRIVAL = 1 << 8,
	GPTE_TRIP_CACHED_MIN_TIME = 1 << 9,
	GPTE_TRIP_CACHED_MAX_TIME = 1 << 10,
	GPTE_TRIP_CACHED_TRAVELABLE = 1 << 11,
	GPTE_TRIP_CACHED_PRODUCTS = 1 << 12,
	GPTE_TRIP_CACHED_FARES = 1 << 13
} GpteTripCachedValues;

struct _GpteTrip {
	GpteJavaObject parent_instance;

	GpteTripCachedValues cached;
	GpteLocation* cached_from;
	GpteLocation* cached_to;
	GListModel* cached_legs;
	gint cached_changes;
	glong cached_duration;
	GpteTripPublic* cached_first_public_leg;
	GpteTripPublic* cached_last_public_leg;
	GDateTime* cached_first_departure;
	GDateTime* cached_last_arrival;
	GDateTime* cached_min_time;
	GDateTime* cached_max_time;
	gboolean cached_travelable;
	GpteProducts cached_products;
	GListModel* cached_fares;
};

G_DEFINE_TYPE (GpteTrip, gpte_trip, GPTE_TYPE_JAVA_OBJECT)

static void gpte_trip_dispose(GObject* object) {
	GpteTrip* self = GPTE_TRIP(object);
	if ((self->cached & GPTE_TRIP_CACHED_FROM_LOC) && self->cached_from)
		g_object_unref(self->cached_from);
	if ((self->cached & GPTE_TRIP_CACHED_TO_LOC) && self->cached_to)
		g_object_unref(self->cached_to);
	if ((self->cached & GPTE_TRIP_CACHED_LEGS) && self->cached_legs)
		g_object_unref(self->cached_legs);
	if ((self->cached & GPTE_TRIP_CACHED_FIRST_PUBLIC_LEG) && self->cached_first_public_leg)
		g_object_unref(self->cached_first_public_leg);
	if ((self->cached & GPTE_TRIP_CACHED_LAST_PUBLIC_LEG) && self->cached_last_public_leg)
		g_object_unref(self->cached_last_public_leg);
	if ((self->cached & GPTE_TRIP_CACHED_FIRST_DEPARTURE) && self->cached_first_departure)
		g_date_time_unref(self->cached_first_departure);
	if ((self->cached & GPTE_TRIP_CACHED_LAST_ARRIVAL) && self->cached_last_arrival)
		g_date_time_unref(self->cached_last_arrival);
	if ((self->cached & GPTE_TRIP_CACHED_MIN_TIME) && self->cached_min_time)
		g_date_time_unref(self->cached_min_time);
	if ((self->cached & GPTE_TRIP_CACHED_MAX_TIME) && self->cached_max_time)
		g_date_time_unref(self->cached_max_time);
	if ((self->cached & GPTE_TRIP_CACHED_FARES) && self->cached_fares)
		g_object_unref(self->cached_fares);
	self->cached = 0;
	G_OBJECT_CLASS(gpte_trip_parent_class)->dispose(object);
}

static void gpte_trip_class_init(GpteTripClass* class) {
	GObjectClass* object_class = G_OBJECT_CLASS(class);
	object_class->dispose = gpte_trip_dispose;
}

static void gpte_trip_init(GpteTrip*) {}

static GpteLocation* gpte_trip_location_field(GpteTrip* self, const gchar* field, GpteLocation** location_cache, GpteTripCachedValues cache_value) {
	g_return_val_if_fail(GPTE_IS_TRIP(self), NULL);
	if (self->cached & cache_value)
		return *location_cache;

	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 3);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));

	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/Trip");
	jfieldID id = (*env)->GetFieldID(env, class, field, "Lde/schildbach/pte/dto/Location;");
	jobject location = (*env)->GetObjectField(env, this, id);

	*location_cache = location ? g_object_new(GPTE_TYPE_LOCATION, "vm", vm, "object", location, NULL) : NULL;
	self->cached |= cache_value;
	return *location_cache;
}
GpteLocation* gpte_trip_from(GpteTrip* self) {
	return gpte_trip_location_field(self, "from", &self->cached_from, GPTE_TRIP_CACHED_FROM_LOC);
}
GpteLocation* gpte_trip_to(GpteTrip* self) {
	return gpte_trip_location_field(self, "to", &self->cached_to, GPTE_TRIP_CACHED_TO_LOC);
}

GListModel* gpte_trip_get_legs(GpteTrip* self) {
	g_return_val_if_fail(GPTE_IS_TRIP(self), NULL);
	if (self->cached & GPTE_TRIP_CACHED_LEGS)
		return self->cached_legs;

	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 3);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));

	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/Trip");
	jfieldID legs_id = (*env)->GetFieldID(env, class, "legs", "Ljava/util/List;");
	jobject jlegs = (*env)->GetObjectField(env, this, legs_id);

	self->cached_legs = jlegs ? gpte_list_new(vm, GPTE_TYPE_TRIP_LEG, jlegs) : NULL;
	self->cached |= GPTE_TRIP_CACHED_LEGS;
	return self->cached_legs;
}

gint gpte_trip_get_num_changes(GpteTrip* self) {
	g_return_val_if_fail(GPTE_IS_TRIP(self), -1);
	if (self->cached & GPTE_TRIP_CACHED_CHANGES)
		return self->cached_changes;

	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self)), 5);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));

	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/Trip");
	jmethodID get_changes = (*env)->GetMethodID(env, class, "getNumChanges", "()Ljava/lang/Integer;");
	jobject jchanges = (*env)->CallObjectMethod(env, this, get_changes);
	if (!jchanges) {
		self->cached_changes = -1;
		self->cached |= GPTE_TRIP_CACHED_CHANGES;
		return -1;
	}

	jclass integer = (*env)->FindClass(env, "java/lang/Integer");
	jmethodID to_int = (*env)->GetMethodID(env, integer, "intValue", "()I");
	self->cached_changes = (*env)->CallIntMethod(env, jchanges, to_int);
	self->cached |= GPTE_TRIP_CACHED_CHANGES;
	return self->cached_changes;
}

glong gpte_trip_get_duration(GpteTrip* self) {
	g_return_val_if_fail(GPTE_IS_TRIP(self), -1);
	if (self->cached & GPTE_TRIP_CACHED_DURATION)
		return self->cached_duration;

	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self)), 2);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));

	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/Trip");
	jmethodID duration_mid = (*env)->GetMethodID(env, class, "getDuration", "()J");
	self->cached_duration = (*env)->CallLongMethod(env, this, duration_mid);
	self->cached |= GPTE_TRIP_CACHED_DURATION;
	return self->cached_duration;
}

static GpteTripPublic* gpte_trip_call_public_leg_meth(GpteTrip* self, const gchar* meth, GpteTripPublic** cached_leg, GpteTripCachedValues cache_value) {
	g_return_val_if_fail(GPTE_IS_TRIP(self), NULL);
	if (self->cached & cache_value)
		return *cached_leg;

	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 2);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));

	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/Trip");
	jmethodID duration_mid = (*env)->GetMethodID(env, class, meth, "()Lde/schildbach/pte/dto/Trip$Public;");
	jobject leg = (*env)->CallObjectMethod(env, this, duration_mid);

	*cached_leg = leg ? g_object_new(GPTE_TYPE_TRIP_PUBLIC, "vm", vm, "object", leg, NULL) : NULL;
	self->cached |= cache_value;
	return *cached_leg;
}

GpteTripPublic* gpte_trip_get_first_public_leg(GpteTrip* self) {
	return gpte_trip_call_public_leg_meth(self, "getFirstPublicLeg", &self->cached_first_public_leg, GPTE_TRIP_CACHED_FIRST_PUBLIC_LEG);
}
GpteTripPublic* gpte_trip_get_last_public_leg(GpteTrip* self) {
	return gpte_trip_call_public_leg_meth(self, "getLastPublicLeg", &self->cached_last_public_leg, GPTE_TRIP_CACHED_LAST_PUBLIC_LEG);
}

static GDateTime* gpte_trip_call_date_meth(GpteTrip* self, const gchar* meth, GDateTime** cached_date, GpteTripCachedValues cache_value) {
	g_return_val_if_fail(GPTE_IS_TRIP(self), NULL);
	if (self->cached & cache_value)
		return *cached_date;

	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 2);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));

	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/Trip");
	jmethodID duration_mid = (*env)->GetMethodID(env, class, meth, "()Ljava/util/Date;");
	jobject jdate = (*env)->CallObjectMethod(env, this, duration_mid);

	*cached_date = gpte_date_from_java(vm, jdate);
	self->cached |= cache_value;
	return *cached_date;
}
GDateTime* gpte_trip_get_first_departure_time(GpteTrip* self) {
	return gpte_trip_call_date_meth(self, "getFirstDepartureTime", &self->cached_first_departure, GPTE_TRIP_CACHED_FIRST_DEPARTURE);
}
GDateTime* gpte_trip_get_last_arrival_time(GpteTrip* self) {
	return gpte_trip_call_date_meth(self, "getLastArrivalTime", &self->cached_last_arrival, GPTE_TRIP_CACHED_LAST_ARRIVAL);
}
GDateTime* gpte_trip_get_min_time(GpteTrip* self) {
	return gpte_trip_call_date_meth(self, "getMinTime", &self->cached_min_time, GPTE_TRIP_CACHED_MIN_TIME);
}
GDateTime* gpte_trip_get_max_time(GpteTrip* self) {
	return gpte_trip_call_date_meth(self, "getMaxTime", &self->cached_max_time, GPTE_TRIP_CACHED_MAX_TIME);
}

gboolean gpte_trip_is_travelable(GpteTrip* self) {
	g_return_val_if_fail(GPTE_IS_TRIP(self), -1);
	if (self->cached & GPTE_TRIP_CACHED_TRAVELABLE)
		return self->cached_travelable;

	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self)), 2);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));

	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/Trip");
	jmethodID trabelable_mid = (*env)->GetMethodID(env, class, "isTravelable", "()Z");

	self->cached_travelable = (*env)->CallBooleanMethod(env, this, trabelable_mid);
	self->cached |= GPTE_TRIP_CACHED_TRAVELABLE;
	return self->cached_travelable;
}

GpteProducts gpte_trip_get_products(GpteTrip* self) {
	g_return_val_if_fail(GPTE_IS_TRIP(self), -1);
	if (self->cached & GPTE_TRIP_CACHED_PRODUCTS)
		return self->cached_products;

	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 3);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));

	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/Trip");
	jmethodID products_mid = (*env)->GetMethodID(env, class, "products", "()Ljava/util/Set;");
	jobject jproducts = (*env)->CallObjectMethod(env, this, products_mid);

	self->cached_products = jproducts ? gpte_products_from_set(vm, jproducts) : 0;
	self->cached |= GPTE_TRIP_CACHED_PRODUCTS;
	return self->cached_products;
}

GListModel* gpte_trip_get_fares(GpteTrip* self) {
	g_return_val_if_fail(GPTE_IS_TRIP(self), NULL);
	if (self->cached & GPTE_TRIP_CACHED_FARES)
		return self->cached_fares;

	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 3);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));

	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/Trip");
	jfieldID fares_id = (*env)->GetFieldID(env, class, "fares", "Ljava/util/List;");
	jobject jfares = (*env)->GetObjectField(env, this, fares_id);

	self->cached_fares = jfares ? gpte_list_new(vm, GPTE_TYPE_FARE, jfares) : NULL;
	self->cached |= GPTE_TRIP_CACHED_FARES;
	return self->cached_fares;
}
