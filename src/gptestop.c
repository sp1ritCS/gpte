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

#include "gptestop.h"

#include "gptejavaobject-priv.h"
#include "gpteutils-priv.h"

G_DEFINE_BOXED_TYPE(GptePosition, gpte_position, gpte_position_copy, gpte_position_free)

GptePosition* gpte_position_copy(const GptePosition* self) {
	if (!self)
		return NULL;
	GptePosition* new = g_new(GptePosition, 1);
	new->name = g_strdup(self->name);
	new->section = self->section ? g_strdup(self->section) : NULL;
	return new;
}
void gpte_position_free(GptePosition* self) {
	if (!self)
		return;
	g_free(self->name);
	if (self->section)
		g_free(self->section);
	g_free(self);
}

const gchar* gpte_position_get_name(const GptePosition* self) {
	g_return_val_if_fail(self != NULL, NULL);
	return self->name;
}
const gchar* gpte_position_get_section(const GptePosition* self) {
	g_return_val_if_fail(self != NULL, NULL);
	return self->section;
}

typedef enum {
	GPTE_STOP_CACHED_LOCATION = 1 << 0,
	GPTE_STOP_CACHED_ARRIVAL = 1 << 1,
	GPTE_STOP_CACHED_DEPARTURE = 1 << 2,
	GPTE_STOP_CACHED_ARRIVAL_DELAY = 1 << 3,
	GPTE_STOP_CACHED_DEPARTURE_DELAY = 1 << 4,
	GPTE_STOP_CACHED_TRUE_ARRIVAL_POS = 1 << 5,
	GPTE_STOP_CACHED_TRUE_DEPARTURE_POS = 1 << 6,
} GpteStopCachedValues;

struct _GpteStop {
	GpteJavaObject parent_instance;

	GpteStopCachedValues cached;
	GpteLocation* cached_location;
	GDateTime* cached_arrival;
	GDateTime* cached_departure;
	gboolean cached_arrival_time_predicted;
	gboolean cached_departure_time_predicted;

	glong cached_arrival_delay;
	glong cached_departure_delay;
	gboolean cached_has_arrival_delay;
	gboolean cached_has_departure_delay;

	gboolean cached_arrival_predicted;
	gboolean cached_departure_predicted;
	GptePosition* cached_true_arrival;
	GptePosition* cached_true_departure;
};

G_DEFINE_TYPE (GpteStop, gpte_stop, GPTE_TYPE_JAVA_OBJECT)

static void gpte_stop_dispose(GObject* object) {
	GpteStop* self = GPTE_STOP(object);
	if (self->cached & GPTE_STOP_CACHED_TRUE_ARRIVAL_POS)
		gpte_position_free(self->cached_true_arrival);
	if (self->cached & GPTE_STOP_CACHED_TRUE_DEPARTURE_POS)
		gpte_position_free(self->cached_true_departure);
	if ((self->cached & GPTE_STOP_CACHED_ARRIVAL) && self->cached_arrival)
		g_date_time_unref(self->cached_arrival);
	if ((self->cached & GPTE_STOP_CACHED_DEPARTURE) && self->cached_departure)
		g_date_time_unref(self->cached_departure);
	if (self->cached & GPTE_STOP_CACHED_LOCATION)
		g_object_unref(self->cached_location);
	self->cached = 0;
	G_OBJECT_CLASS(gpte_stop_parent_class)->dispose(object);
}

static void gpte_stop_class_init(GpteStopClass* class) {
	GObjectClass* object_class = G_OBJECT_CLASS(class);
	object_class->dispose = gpte_stop_dispose;
}

static void gpte_stop_init(GpteStop* self) {
	self->cached = 0;
}

GpteLocation* gpte_stop_get_location(GpteStop* self) {
	g_return_val_if_fail(GPTE_IS_STOP(self), NULL);
	if (self->cached & GPTE_STOP_CACHED_LOCATION)
		return self->cached_location;

	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 3);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));

	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/Stop");
	jfieldID id = (*env)->GetFieldID(env, class, "location", "Lde/schildbach/pte/dto/Location;");
	jobject location = (*env)->GetObjectField(env, this, id);

	self->cached_location = g_object_new(GPTE_TYPE_LOCATION, "vm", vm, "object", location, NULL);
	self->cached |= GPTE_STOP_CACHED_LOCATION;
	return self->cached_location;
}

static GDateTime* gpte_stop_call_date_and_pred_meth(GpteStop* self, gboolean* is_predicted, const gchar* method, const gchar* predicted, GDateTime** cache, gboolean* predicted_cache, GpteStopCachedValues cache_value) {
	if (self->cached & cache_value) {
		if (is_predicted)
			*is_predicted = *predicted_cache;
		return *cache;
	}

	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 3);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));

	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/Stop");
	jmethodID method_id = (*env)->GetMethodID(env, class, method, "()Ljava/util/Date;");
	jobject date = (*env)->CallObjectMethod(env, this, method_id);

	jmethodID get_is_predicted = (*env)->GetMethodID(env, class, predicted, "()Z");
	*predicted_cache = (*env)->CallBooleanMethod(env, this, get_is_predicted);

	*cache = date ? gpte_date_from_java(vm, date) : NULL;
	self->cached |= cache_value;
	if (is_predicted)
		*is_predicted = *predicted_cache;
	return *cache;
}
GDateTime* gpte_stop_get_arrival_time(GpteStop* self, gboolean* is_predicted) {
	g_return_val_if_fail(GPTE_IS_STOP(self), NULL);
	return gpte_stop_call_date_and_pred_meth(self, is_predicted, "getArrivalTime", "isArrivalTimePredicted", &self->cached_arrival, &self->cached_arrival_time_predicted, GPTE_STOP_CACHED_ARRIVAL);
}
GDateTime* gpte_stop_get_departure_time(GpteStop* self, gboolean* is_predicted) {
	g_return_val_if_fail(GPTE_IS_STOP(self), NULL);
	return gpte_stop_call_date_and_pred_meth(self, is_predicted, "getDepartureTime", "isDepartureTimePredicted", &self->cached_departure, &self->cached_departure_time_predicted, GPTE_STOP_CACHED_DEPARTURE);
}

static const glong* gpte_stop_call_boxed_long_meth(GpteStop* self, const gchar* method, glong* value_cache, gboolean* has_value_cache, GpteStopCachedValues cache_value) {
	if (self->cached & cache_value) {
		if (*has_value_cache)
			return value_cache;
		return NULL;
	}

	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 5);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));

	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/Stop");
	jmethodID get_box = (*env)->GetMethodID(env, class, method, "()Ljava/lang/Long;");
	jobject boxed = (*env)->CallObjectMethod(env, this, get_box);
	if (!boxed) {
		*has_value_cache = FALSE;
		self->cached |= cache_value;
		return NULL;
	}

	jclass long_class = (*env)->FindClass(env, "java/lang/Long");
	jmethodID get_value = (*env)->GetMethodID(env, long_class, "longValue", "()J");
	*value_cache = (*env)->CallLongMethod(env, boxed, get_value);
	*has_value_cache = TRUE;
	self->cached |= cache_value;
	return value_cache;
}
const glong* gpte_stop_get_arrival_delay(GpteStop* self) {
	g_return_val_if_fail(GPTE_IS_STOP(self), NULL);
	return gpte_stop_call_boxed_long_meth(self, "getArrivalDelay", &self->cached_arrival_delay, &self->cached_has_arrival_delay, GPTE_STOP_CACHED_ARRIVAL_DELAY);
}
const glong* gpte_stop_get_departure_delay(GpteStop* self) {
	g_return_val_if_fail(GPTE_IS_STOP(self), NULL);
	return gpte_stop_call_boxed_long_meth(self, "getDepartureDelay", &self->cached_departure_delay, &self->cached_has_departure_delay, GPTE_STOP_CACHED_DEPARTURE_DELAY);
}

static const GptePosition* gpte_stop_call_position_and_pred_meth(GpteStop* self, gboolean* is_predicted, const gchar* position, const gchar* predicted, GptePosition** position_cache, gboolean* predicted_cache, GpteStopCachedValues cache_value) {
	if (self->cached & cache_value) {
		if (is_predicted)
			*is_predicted = *predicted_cache;
		return *position_cache;
	}

	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 11);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));

	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/Stop");
	jmethodID get_position = (*env)->GetMethodID(env, class, position, "()Lde/schildbach/pte/dto/Position;");
	jobject jposition = (*env)->CallObjectMethod(env, this, get_position);
	if (!jposition)
		return NULL;

	jmethodID get_is_predicted = (*env)->GetMethodID(env, class, predicted, "()Z");
	*predicted_cache = (*env)->CallBooleanMethod(env, this, get_is_predicted);

	jclass position_class = (*env)->FindClass(env, "de/schildbach/pte/dto/Position");
	jfieldID name_id = (*env)->GetFieldID(env, position_class, "name", "Ljava/lang/String;");
	jfieldID section_id = (*env)->GetFieldID(env, position_class, "section", "Ljava/lang/String;");
	jstring jname = (*env)->GetObjectField(env, jposition, name_id);
	jstring jsection = (*env)->GetObjectField(env, jposition, section_id);

	*position_cache = g_new0(GptePosition, 1);
	const char* jname_str = (*env)->GetStringUTFChars(env, jname, NULL);
	(*position_cache)->name = g_strdup(jname_str);
	(*env)->ReleaseStringUTFChars(env, jname, jname_str);
	if (jsection) {
		const char* jsection_str = (*env)->GetStringUTFChars(env, jsection, NULL);
		(*position_cache)->section = g_strdup(jsection_str);
		(*env)->ReleaseStringUTFChars(env, jsection, jsection_str);
	}

	self->cached |= cache_value;
	if (is_predicted)
		*is_predicted = *predicted_cache;
	return *position_cache;
}
const GptePosition* gpte_stop_get_arrival_position(GpteStop* self, gboolean* is_predicted) {
	g_return_val_if_fail(GPTE_IS_STOP(self), NULL);
	return gpte_stop_call_position_and_pred_meth(self, is_predicted, "getArrivalPosition", "isArrivalPositionPredicted", &self->cached_true_arrival, &self->cached_arrival_predicted, GPTE_STOP_CACHED_TRUE_ARRIVAL_POS);
}
const GptePosition* gpte_stop_get_departure_position(GpteStop* self, gboolean* is_predicted) {
	g_return_val_if_fail(GPTE_IS_STOP(self), NULL);
	return gpte_stop_call_position_and_pred_meth(self, is_predicted, "getDeparturePosition", "isDeparturePositionPredicted", &self->cached_true_departure, &self->cached_departure_predicted, GPTE_STOP_CACHED_TRUE_DEPARTURE_POS);
}
