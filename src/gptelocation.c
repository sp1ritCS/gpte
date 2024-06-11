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

#include "gptelocation.h"
#include "gptelocation-priv.h"

#include "gptejavaobject-priv.h"
#include "gptegeo-priv.h"
#include "gpteproducts-priv.h"

G_DEFINE_ENUM_TYPE(GpteLocationType, gpte_location_type,
	G_DEFINE_ENUM_VALUE(GPTE_LOCATION_ANY, "any"),
	G_DEFINE_ENUM_VALUE(GPTE_LOCATION_STATION, "station"),
	G_DEFINE_ENUM_VALUE(GPTE_LOCATION_POI, "poi"),
	G_DEFINE_ENUM_VALUE(GPTE_LOCATION_ADDRESS, "address"),
	G_DEFINE_ENUM_VALUE(GPTE_LOCATION_COORD, "coord")
)

G_DEFINE_FLAGS_TYPE(GpteLocations, gpte_locations,
	G_DEFINE_ENUM_VALUE(GPTE_LOCATIONS_ANY, "any"),
	G_DEFINE_ENUM_VALUE(GPTE_LOCATIONS_STATION, "station"),
	G_DEFINE_ENUM_VALUE(GPTE_LOCATIONS_POI, "poi"),
	G_DEFINE_ENUM_VALUE(GPTE_LOCATIONS_ADDRESS, "address"),
	G_DEFINE_ENUM_VALUE(GPTE_LOCATIONS_COORD, "coord")
)


jobject gpte_locations_to_java(GpteJvm* vm, GpteLocations locations) {
	GpteScopeGuard env = gpte_jvm_enter_scope(vm, 15);

	jclass set_class = (*env)->FindClass(env, "java/util/HashSet");
	jmethodID set_constructor = (*env)->GetMethodID(env, set_class, "<init>", "()V");
	jmethodID add_to_set = (*env)->GetMethodID(env, set_class, "add", "(Ljava/lang/Object;)Z");

	jobject flags = (*env)->NewObject(env, set_class, set_constructor);
	jclass flags_enum = (*env)->FindClass(env, "de/schildbach/pte/dto/LocationType");
	if (locations & GPTE_LOCATIONS_ANY) {
		jobject any = (*env)->GetStaticObjectField(env, flags_enum, (*env)->GetStaticFieldID(env, flags_enum, "ANY", "Lde/schildbach/pte/dto/LocationType;"));
		(*env)->CallBooleanMethod(env, flags, add_to_set, any);
	}
	if (locations & GPTE_LOCATIONS_STATION) {
		jobject station = (*env)->GetStaticObjectField(env, flags_enum, (*env)->GetStaticFieldID(env, flags_enum, "STATION", "Lde/schildbach/pte/dto/LocationType;"));
		(*env)->CallBooleanMethod(env, flags, add_to_set, station);
	}
	if (locations & GPTE_LOCATIONS_POI) {
		jobject poi = (*env)->GetStaticObjectField(env, flags_enum, (*env)->GetStaticFieldID(env, flags_enum, "POI", "Lde/schildbach/pte/dto/LocationType;"));
		(*env)->CallBooleanMethod(env, flags, add_to_set, poi);
	}
	if (locations & GPTE_LOCATIONS_ADDRESS) {
		jobject address = (*env)->GetStaticObjectField(env, flags_enum, (*env)->GetStaticFieldID(env, flags_enum, "ADDRESS", "Lde/schildbach/pte/dto/LocationType;"));
		(*env)->CallBooleanMethod(env, flags, add_to_set, address);
	}
	if (locations & GPTE_LOCATIONS_COORD) {
		jobject coord = (*env)->GetStaticObjectField(env, flags_enum, (*env)->GetStaticFieldID(env, flags_enum, "COORD", "Lde/schildbach/pte/dto/LocationType;"));
		(*env)->CallBooleanMethod(env, flags, add_to_set, coord);
	}

	return gpte_scope_guard_leave_with_ref(&env, flags);
}

typedef struct {
	jstring string;
	const char* utf8;
} GpteCachedString;

typedef enum {
	GPTE_LOCATION_CACHED_COORDS = 1 << 0,
	GPTE_LOCATION_CACHED_ID = 1 << 1,
	GPTE_LOCATION_CACHED_NAME = 1 << 2,
	GPTE_LOCATION_CACHED_PLACE = 1 << 3,
	GPTE_LOCATION_CACHED_PRODUCTS = 1 << 4,
	GPTE_LOCATION_CACHED_LOCATION_TYPE = 1 << 5
} GpteLocationCachedValues;

struct _GpteLocation {
	GpteJavaObject parent_instance;

	GpteLocationCachedValues cached;
	GpteGeoPoint* coords;
	GpteCachedString id;
	GpteCachedString name;
	GpteCachedString place;
	GpteProducts products;
	GpteLocationType type;
};

G_DEFINE_TYPE (GpteLocation, gpte_location, GPTE_TYPE_JAVA_OBJECT)

#define GPTE_LOCATION_FREE_CACHED_STRING(env,fn,ce) \
	if ((self->cached & (ce)) && self->fn.string) { \
		(*(env))->ReleaseStringUTFChars((env), self->fn.string, self->fn.utf8); \
		(*(env))->DeleteGlobalRef((env), self->fn.string); \
	}

static void gpte_location_finalize(GObject* object) {
	GpteLocation* self = GPTE_LOCATION(object);
	JNIEnv* env = gpte_java_object_env(GPTE_JAVA_OBJECT(self));
	if ((self->cached & GPTE_LOCATION_CACHED_COORDS) && self->coords)
		gpte_geo_point_free(self->coords);
	GPTE_LOCATION_FREE_CACHED_STRING(env, id, GPTE_LOCATION_CACHED_ID)
	GPTE_LOCATION_FREE_CACHED_STRING(env, name, GPTE_LOCATION_CACHED_NAME)
	GPTE_LOCATION_FREE_CACHED_STRING(env, place, GPTE_LOCATION_CACHED_PLACE)
	G_OBJECT_CLASS(gpte_location_parent_class)->finalize(object);
}

static void gpte_location_class_init(GpteLocationClass* class) {
	G_OBJECT_CLASS(class)->finalize = gpte_location_finalize;
}

static void gpte_location_init(GpteLocation* self) {
	self->cached = 0;
}

GpteLocation* gpte_location_from_coords(GpteJvm* vm, const GpteGeoPoint* point) {
	g_return_val_if_fail(vm != NULL, NULL);
	g_return_val_if_fail(point != NULL, NULL);

	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 3);

	jclass point_class = (*env)->FindClass(env, "de/schildbach/pte/dto/Point");
	jmethodID create_point = (*env)->GetStaticMethodID(env, point_class, "fromDouble", "(DD)Lde/schildbach/pte/dto/Point;");
	jobject jpoint = (*env)->CallStaticObjectMethod(env, point_class, create_point, point->lat, point->lon);

	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/Location");
	jmethodID mid = (*env)->GetStaticMethodID(env, class, "coord", "(Lde/schildbach/pte/dto/Point;)Lde/schildbach/pte/dto/Location;");
	jobject created = (*env)->CallStaticObjectMethod(env, class, mid, jpoint);

	return g_object_new(GPTE_TYPE_LOCATION, "vm", vm, "object", created, NULL);
}

const GpteGeoPoint* gpte_location_get_coords(GpteLocation* self) {
	g_return_val_if_fail(GPTE_IS_LOCATION(self), NULL);
	if (self->cached & GPTE_LOCATION_CACHED_COORDS)
		return self->coords;
	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 3);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));
	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/Location");
	jfieldID field_id = (*env)->GetFieldID(env, class, "coord", "Lde/schildbach/pte/dto/Point;");
	g_return_val_if_fail(field_id, NULL);
	jstring point = (*env)->GetObjectField(env, this, field_id);
	if (!point) {
		self->coords = NULL;
		self->cached |= GPTE_LOCATION_CACHED_COORDS;
		return NULL;
	}
	GpteGeoPoint cpoint = gpte_geo_point_from_java(vm, point);
	self->coords = gpte_geo_point_copy(&cpoint);
	self->cached |= GPTE_LOCATION_CACHED_COORDS;
	return self->coords;
}

#define GPTE_LOCATION_STRING_GETTER(fn,ce) \
	const gchar* gpte_location_get_##fn(GpteLocation* self) { \
		g_return_val_if_fail(GPTE_IS_LOCATION(self), NULL); \
		if (self->cached & (ce)) \
			return self->fn.utf8; \
		g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self)), 3); \
		jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self)); \
		jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/Location"); \
		jfieldID field_id = (*env)->GetFieldID(env, class, #fn, "Ljava/lang/String;"); \
		g_return_val_if_fail(field_id, NULL); \
		self->fn.string = (*env)->GetObjectField(env, this, field_id); \
		if (!self->fn.string) { \
			self->fn.utf8 = NULL; \
			self->cached |= (ce); \
			return NULL; \
		} \
		self->fn = (GpteCachedString){ \
			.string = (*env)->NewGlobalRef(env, self->fn.string), \
			.utf8 = (*env)->GetStringUTFChars(env, self->fn.string, NULL) \
		}; \
		self->cached |= (ce); \
		return self->fn.utf8; \
	}

GPTE_LOCATION_STRING_GETTER(id, GPTE_LOCATION_CACHED_ID)
GPTE_LOCATION_STRING_GETTER(name, GPTE_LOCATION_CACHED_NAME)
GPTE_LOCATION_STRING_GETTER(place, GPTE_LOCATION_CACHED_PLACE)

GpteProducts gpte_location_get_products(GpteLocation* self) {
	g_return_val_if_fail(GPTE_IS_LOCATION(self), 0);
	if (self->cached & GPTE_LOCATION_CACHED_PRODUCTS)
		return self->products;
	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 3);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));
	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/Location");
	jfieldID field_id = (*env)->GetFieldID(env, class, "products", "Ljava/util/Set;");
	g_return_val_if_fail(field_id, 0);
	jobject jproducts = (*env)->GetObjectField(env, this, field_id);
	if (!jproducts) {
		self->products = 0;
		self->cached |= GPTE_LOCATION_CACHED_PRODUCTS;
		return 0;
	}
	self->products = gpte_products_from_set(vm, jproducts);
	return self->products;
}

GpteLocationType gpte_location_get_location_type(GpteLocation* self) {
	g_return_val_if_fail(GPTE_IS_LOCATION(self), GPTE_LOCATION_ANY);
	if (self->cached & GPTE_LOCATION_CACHED_LOCATION_TYPE)
		return self->type;
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self)), 9);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));
	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/Location");
	jfieldID field_id = (*env)->GetFieldID(env, class, "type", "Lde/schildbach/pte/dto/LocationType;");
	g_return_val_if_fail(field_id, GPTE_LOCATION_ANY);
	jobject jtype = (*env)->GetObjectField(env, this, field_id);
	jclass type_class = (*env)->FindClass(env, "de/schildbach/pte/dto/LocationType");

	if ((*env)->IsSameObject(env, jtype, (*env)->GetStaticObjectField(
		env, type_class,
		(*env)->GetStaticFieldID(env, type_class , "ANY", "Lde/schildbach/pte/dto/LocationType;")
	)))
		self->type = GPTE_LOCATION_ANY;
	else if ((*env)->IsSameObject(env, jtype, (*env)->GetStaticObjectField(
		env, type_class,
		(*env)->GetStaticFieldID(env, type_class , "STATION", "Lde/schildbach/pte/dto/LocationType;")
	)))
		self->type = GPTE_LOCATION_STATION;
	else if ((*env)->IsSameObject(env, jtype, (*env)->GetStaticObjectField(
		env, type_class,
		(*env)->GetStaticFieldID(env, type_class , "POI", "Lde/schildbach/pte/dto/LocationType;")
	)))
		self->type = GPTE_LOCATION_POI;
	else if ((*env)->IsSameObject(env, jtype, (*env)->GetStaticObjectField(
		env, type_class,
		(*env)->GetStaticFieldID(env, type_class , "ADDRESS", "Lde/schildbach/pte/dto/LocationType;")
	)))
		self->type = GPTE_LOCATION_ADDRESS;
	else if ((*env)->IsSameObject(env, jtype, (*env)->GetStaticObjectField(
		env, type_class,
		(*env)->GetStaticFieldID(env, type_class , "COORD", "Lde/schildbach/pte/dto/LocationType;")
	)))
		self->type = GPTE_LOCATION_COORD;
	else {
		g_critical("Unknown location type: %p", jtype);
		self->type = GPTE_LOCATION_ANY;
	}
	return self->type;
}
