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

#include "gptestationdepartures.h"
#include "gptejavaobject-priv.h"

#include "gptelist-priv.h"

G_DEFINE_BOXED_TYPE(GpteLineDest, gpte_line_dest, gpte_line_dest_copy, gpte_line_dest_free)

GpteLineDest* gpte_line_dest_copy(const GpteLineDest* self) {
	GpteLineDest* new = g_new(GpteLineDest, 1);
	new->line = g_object_ref(self->line);
	new->destination = self->destination ? g_object_ref(self->destination) : NULL;
	return new;
}
void gpte_line_dest_free(GpteLineDest* self) {
	if (!self)
		return;
	g_object_unref(self->destination);
	g_object_unref(self->line);
	g_free(self);
}

struct _GpteStationDepartures {
	GpteJavaObject parent_instance;
};

G_DEFINE_TYPE (GpteStationDepartures, gpte_station_departures, GPTE_TYPE_JAVA_OBJECT)

static void gpte_station_departures_class_init(GpteStationDeparturesClass*) {}
static void gpte_station_departures_init(GpteStationDepartures*) {}

GpteLocation* gpte_station_departures_get_location(GpteStationDepartures* self) {
	g_return_val_if_fail(GPTE_IS_STATION_DEPARTURES(self), NULL);

	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 3);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));
	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/StationDepartures");

	jfieldID location_id = (*env)->GetFieldID(env, class, "location", "Lde/schildbach/pte/dto/Location;");
	jobject jlocation = (*env)->GetObjectField(env, this, location_id);

	return g_object_new(GPTE_TYPE_LOCATION, "vm", vm, "object", jlocation, NULL);
}

GListModel* gpte_station_departures_get_departures(GpteStationDepartures* self) {
	g_return_val_if_fail(GPTE_IS_STATION_DEPARTURES(self), NULL);

	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 3);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));
	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/StationDepartures");

	jfieldID depas_id = (*env)->GetFieldID(env, class, "departures", "Ljava/util/List;");
	jobject jdepas = (*env)->GetObjectField(env, this, depas_id);

	return gpte_list_new(vm, GPTE_TYPE_DEPARTURE, jdepas);
}

GPtrArray* gpte_station_departures_get_lines(GpteStationDepartures* self) {
	g_return_val_if_fail(GPTE_IS_STATION_DEPARTURES(self), NULL);

	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 11);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));
	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/StationDepartures");

	jfieldID lines_id = (*env)->GetFieldID(env, class, "lines", "Ljava/util/List;");
	jobject lines = (*env)->GetObjectField(env, this, lines_id);
	if (!lines)
		return NULL;

	jclass list_class = (*env)->FindClass(env, "java/util/List");
	jmethodID to_arr = (*env)->GetMethodID(env, list_class, "toArray", "()[Ljava/lang/Object;");
	jobjectArray lines_arr = (*env)->CallObjectMethod(env, lines, to_arr);

	jclass line_dest_class = (*env)->FindClass(env, "de/schildbach/pte/dto/LineDestination");
	jfieldID line_id = (*env)->GetFieldID(env, line_dest_class, "line", "Lde/schildbach/pte/dto/Line;");
	jfieldID dest_id = (*env)->GetFieldID(env, line_dest_class, "line", "Lde/schildbach/pte/dto/Location;");

	jsize len = (*env)->GetArrayLength(env, lines_arr);
	GPtrArray* ret = g_ptr_array_new_full(len, (GDestroyNotify)gpte_line_dest_free);
	for (jsize i = 0; i < len; i++) {
		jobject line_dest = (*env)->GetObjectArrayElement(env, lines_arr, i);
		GpteLineDest* ld = g_new(GpteLineDest, 1);
		ld->line = g_object_new(GPTE_TYPE_LINE, "vm", vm, "object", (*env)->GetObjectField(env, line_dest, line_id), NULL);
		jobject dest = (*env)->GetObjectField(env, line_dest, dest_id);
		ld->destination = dest ? g_object_new(GPTE_TYPE_LINE, "vm", vm, "object", dest, NULL) : NULL;
		g_ptr_array_add(ret, ld);
		(*env)->DeleteLocalRef(env, line_dest);
		(*env)->DeleteLocalRef(env, dest);
	}
	return ret;
}
