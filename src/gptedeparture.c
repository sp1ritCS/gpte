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

#include "gptedeparture.h"
#include "gptejavaobject-priv.h"

#include "gpteutils-priv.h"
#include "gptegeo-priv.h"

struct _GpteDeparture {
	GpteJavaObject parent_instance;
};

G_DEFINE_TYPE (GpteDeparture, gpte_departure, GPTE_TYPE_JAVA_OBJECT)

static void gpte_departure_class_init(GpteDepartureClass*) {}
static void gpte_departure_init(GpteDeparture*) {}

GDateTime* gpte_departure_get_planned_time(GpteDeparture* self) {
	g_return_val_if_fail(GPTE_IS_DEPARTURE(self), NULL);

	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 3);

	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));
	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/Departure");

	jfieldID field_id = (*env)->GetFieldID(env, class, "plannedTime", "Ljava/util/Date;");
	jobject date = (*env)->GetObjectField(env, this, field_id);
	if (!date)
		return NULL;
	return gpte_date_from_java(vm, date);
}

GDateTime* gpte_departure_get_predicted_time(GpteDeparture* self) {
	g_return_val_if_fail(GPTE_IS_DEPARTURE(self), NULL);

	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 3);

	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));
	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/Departure");

	jfieldID field_id = (*env)->GetFieldID(env, class, "predictedTime", "Ljava/util/Date;");
	jobject date = (*env)->GetObjectField(env, this, field_id);
	if (!date)
		return NULL;
	return gpte_date_from_java(vm, date);
}

GDateTime* gpte_departure_get_time(GpteDeparture* self) {
	g_return_val_if_fail(GPTE_IS_DEPARTURE(self), NULL);

	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 3);

	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));
	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/Departure");

	jmethodID get_time = (*env)->GetMethodID(env, class, "getTime", "()Ljava/util/Date;");
	jobject date = (*env)->CallObjectMethod(env, this, get_time);
	if (!date)
		return NULL;
	return gpte_date_from_java(vm, date);
}

GpteLine* gpte_departure_get_line(GpteDeparture* self) {
	g_return_val_if_fail(GPTE_IS_DEPARTURE(self), NULL);

	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 3);

	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));
	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/Departure");

	jfieldID field_id = (*env)->GetFieldID(env, class, "line", "Lde/schildbach/pte/dto/Line;");
	jobject line = (*env)->GetObjectField(env, this, field_id);

	return g_object_new(GPTE_TYPE_LINE, "vm", vm, "object", line, NULL);
}

GpteGeoPoint* gpte_departure_get_position(GpteDeparture* self) {
	g_return_val_if_fail(GPTE_IS_DEPARTURE(self), NULL);

	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 3);

	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));
	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/Departure");

	jfieldID field_id = (*env)->GetFieldID(env, class, "destination", "Lde/schildbach/pte/dto/Position;");
	jobject pos = (*env)->GetObjectField(env, this, field_id);
	if (!pos)
		return NULL;
	GpteGeoPoint p = gpte_geo_point_from_java(vm, pos);
	return gpte_geo_point_copy(&p);
}

GpteLocation* gpte_departure_get_destination(GpteDeparture* self) {
	g_return_val_if_fail(GPTE_IS_DEPARTURE(self), NULL);

	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 3);

	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));
	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/Departure");

	jfieldID field_id = (*env)->GetFieldID(env, class, "destination", "Lde/schildbach/pte/dto/Location;");
	jobject dest = (*env)->GetObjectField(env, this, field_id);
	if (!dest)
		return NULL;

	return g_object_new(GPTE_TYPE_LOCATION, "vm", vm, "object", dest, NULL);
}

gchar* gpte_departure_get_message(GpteDeparture* self) {
	g_return_val_if_fail(GPTE_IS_DEPARTURE(self), NULL);

	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self)), 3);

	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));
	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/Departure");

	jfieldID field_id = (*env)->GetFieldID(env, class, "message", "Ljava/lang/String;");
	jobject msg = (*env)->GetObjectField(env, this, field_id);
	if (!msg)
		return NULL;
	const char* utf = (*env)->GetStringUTFChars(env, msg, NULL);
	gchar* copy = g_strdup(utf);
	(*env)->ReleaseStringUTFChars(env, msg, utf);
	return copy;
}
