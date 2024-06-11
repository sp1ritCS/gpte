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

#include "gptegeo.h"
#include "gptegeo-priv.h"

G_DEFINE_BOXED_TYPE(GpteGeoPoint, gpte_geo_point, gpte_geo_point_copy, gpte_geo_point_free)
GpteGeoPoint* gpte_geo_point_new(gdouble lat, gdouble lon) {
	GpteGeoPoint* self = g_new(GpteGeoPoint, 1);
	self->lat = lat;
	self->lon = lon;
	return self;
}
GpteGeoPoint* gpte_geo_point_copy(const GpteGeoPoint* self) {
	return gpte_geo_point_new(self->lat, self->lon);
}
void gpte_geo_point_free(GpteGeoPoint* self) {
	g_free(self);
}

GpteGeoPoint gpte_geo_point_from_java(GpteJvm* vm, jobject point) {
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 3);

	jclass point_class = (*env)->FindClass(env, "de/schildbach/pte/dto/Point");
	jmethodID get_lat = (*env)->GetMethodID(env, point_class, "getLatAsDouble", "()D");
	jmethodID get_lon = (*env)->GetMethodID(env, point_class, "getLonAsDouble", "()D");
	return (GpteGeoPoint){
		.lat = (*env)->CallDoubleMethod(env, point, get_lat),
		.lon = (*env)->CallDoubleMethod(env, point, get_lon)
	};
}
