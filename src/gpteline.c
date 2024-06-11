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

#include "gpteline.h"
#include "gptejavaobject-priv.h"

#include "gpteproducts-priv.h"
#include "gptestyle-priv.h"

G_DEFINE_FLAGS_TYPE(GpteLineAttrs, gpte_line_attrs,
	G_DEFINE_ENUM_VALUE(GPTE_LINE_ATTR_CIRCLE_CLOCKWISE, "circle-clockwise"),
	G_DEFINE_ENUM_VALUE(GPTE_LINE_ATTR_CIRCLE_ANTICLOCKWISE, "circle-anticlockwise"),
	G_DEFINE_ENUM_VALUE(GPTE_LINE_ATTR_SERVICE_REPLACEMENT, "service-replacement"),
	G_DEFINE_ENUM_VALUE(GPTE_LINE_ATTR_LINE_AIRPORT, "line-airport"),
	G_DEFINE_ENUM_VALUE(GPTE_LINE_ATTR_WHEEL_CHAIR_ACCESS, "wheel-chair-access"),
	G_DEFINE_ENUM_VALUE(GPTE_LINE_ATTR_BICYCLE_CARRIAGE, "bicycle-carriage")
)

typedef struct {
	jstring string;
	const char* utf8;
} GpteCachedString;

typedef enum {
	GPTE_LINE_CACHED_ID = 1 << 0,
	GPTE_LINE_CACHED_NETWORK = 1 << 1,
	GPTE_LINE_CACHED_PRODUCT = 1 << 2,
	GPTE_LINE_CACHED_LABEL = 1 << 3,
	GPTE_LINE_CACHED_NAME = 1 << 4,
	GPTE_LINE_CACHED_STYLE = 1 << 5,
	GPTE_LINE_CACHED_ATTRS = 1 << 6,
	GPTE_LINE_CACHED_MESSAGE = 1 << 7
} GpteLineCachedValues;

struct _GpteLine {
	GpteJavaObject parent_instance;

	GpteLineCachedValues cached;
	GpteCachedString id;
	GpteCachedString network;
	GpteProductCode product;
	GpteCachedString label;
	GpteCachedString name;
	GpteStyle* style;
	GpteLineAttrs attrs;
	GpteCachedString message;
};

G_DEFINE_TYPE (GpteLine, gpte_line, GPTE_TYPE_JAVA_OBJECT)

#define GPTE_LINE_FREE_CACHED_STRING(env,fn,ce) \
	if ((self->cached & (ce)) && self->fn.string) { \
		(*(env))->ReleaseStringUTFChars((env), self->fn.string, self->fn.utf8); \
		(*(env))->DeleteGlobalRef((env), self->fn.string); \
	}

static void gpte_location_finalize(GObject* object) {
	GpteLine* self = GPTE_LINE(object);
	JNIEnv* env = gpte_java_object_env(GPTE_JAVA_OBJECT(self));
	GPTE_LINE_FREE_CACHED_STRING(env, id, GPTE_LINE_CACHED_ID)
	GPTE_LINE_FREE_CACHED_STRING(env, network, GPTE_LINE_CACHED_NETWORK)
	GPTE_LINE_FREE_CACHED_STRING(env, label, GPTE_LINE_CACHED_LABEL)
	GPTE_LINE_FREE_CACHED_STRING(env, name, GPTE_LINE_CACHED_NAME)
	//GpteStyle* gpte_line_get_style(GpteLine* self);
	GPTE_LINE_FREE_CACHED_STRING(env, message, GPTE_LINE_CACHED_MESSAGE)
	G_OBJECT_CLASS(gpte_line_parent_class)->finalize(object);
}

static void gpte_line_class_init(GpteLineClass* class) {
	G_OBJECT_CLASS(class)->finalize = gpte_location_finalize;
}

static void gpte_line_init(GpteLine* self) {
	self->cached = 0;
}

#define GPTE_LINE_STRING_GETTER(fn,ce) \
	const gchar* gpte_line_get_##fn(GpteLine* self) { \
		g_return_val_if_fail(GPTE_IS_LINE(self), NULL); \
		if (self->cached & (ce)) \
			return self->fn.utf8; \
		GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self)); \
		g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 3); \
		jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self)); \
		jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/Line"); \
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

GPTE_LINE_STRING_GETTER(id, GPTE_LINE_CACHED_ID)
GPTE_LINE_STRING_GETTER(network, GPTE_LINE_CACHED_NETWORK)
GpteProductCode gpte_line_get_product(GpteLine* self) {
	g_return_val_if_fail(GPTE_IS_LINE(self), GPTE_PRODUCT_CODE_NULL);
	if (self->cached & GPTE_LINE_CACHED_PRODUCT)
		return self->product;
	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 3);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));
	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/Line");
	jfieldID field_id = (*env)->GetFieldID(env, class, "product", "Lde/schildbach/pte/dto/Product;");
	jobject jproduct = (*env)->GetObjectField(env, this, field_id);
	if (!jproduct) {
		self->product = GPTE_PRODUCT_CODE_NULL;
		self->cached |= GPTE_LINE_CACHED_PRODUCT;
		return GPTE_PRODUCT_CODE_NULL;
	}
	self->product = gpte_product_code_from_java(vm, jproduct);
	self->cached |= GPTE_LINE_CACHED_PRODUCT;
	return self->product;
}
GPTE_LINE_STRING_GETTER(label, GPTE_LINE_CACHED_LABEL)
GPTE_LINE_STRING_GETTER(name, GPTE_LINE_CACHED_NAME)
GpteStyle* gpte_line_get_style(GpteLine* self) {
	g_return_val_if_fail(GPTE_IS_LINE(self), NULL);
	if (self->cached & GPTE_LINE_CACHED_STYLE)
		return self->style;
	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 3);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));
	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/Line");
	jfieldID field_id = (*env)->GetFieldID(env, class, "style", "Lde/schildbach/pte/dto/Style;");
	jobject jstyle = (*env)->GetObjectField(env, this, field_id);
	if (!jstyle) {
		self->style = NULL;
		self->cached |= GPTE_LINE_CACHED_STYLE;
		return NULL;
	}
	self->style = gpte_style_from_java(vm, jstyle);
	self->cached |= GPTE_LINE_CACHED_STYLE;
	return self->style;
}
GpteLineAttrs gpte_line_get_attrs(GpteLine* self) {
	g_return_val_if_fail(GPTE_IS_LINE(self), 0);
	if (self->cached & GPTE_LINE_CACHED_ATTRS)
		return self->attrs;
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self)), 13);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));
	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/Line");
	jfieldID field_id = (*env)->GetFieldID(env, class, "attrs", "Ljava/util/Set;");
	jobject jattrs = (*env)->GetObjectField(env, this, field_id);
	if (!jattrs) {
		self->attrs = 0;
		self->cached |= GPTE_LINE_CACHED_ATTRS;
		return 0;
	}

	self->attrs = 0;
	jclass set_class = (*env)->FindClass(env, "java/util/Set");
	jmethodID set_to_array = (*env)->GetMethodID(env, set_class, "toArray", "()[Ljava/lang/Object;");
	jobjectArray elems = (*env)->CallObjectMethod(env, jattrs, set_to_array);
	jsize len = (*env)->GetArrayLength(env, elems);

	jclass attrs_class = (*env)->FindClass(env, "de/schildbach/pte/dto/Line$Attr");
	jobject clockwise = (*env)->GetStaticObjectField(
		env, attrs_class,
		(*env)->GetStaticFieldID(env, attrs_class, "CIRCLE_CLOCKWISE", "Lde/schildbach/pte/dto/Line$Attr;")
	);
	jobject anticlockwise = (*env)->GetStaticObjectField(
		env, attrs_class,
		(*env)->GetStaticFieldID(env, attrs_class, "CIRCLE_ANTICLOCKWISE", "Lde/schildbach/pte/dto/Line$Attr;")
	);
	jobject replacement = (*env)->GetStaticObjectField(
		env, attrs_class,
		(*env)->GetStaticFieldID(env, attrs_class, "SERVICE_REPLACEMENT", "Lde/schildbach/pte/dto/Line$Attr;")
	);
	jobject airport = (*env)->GetStaticObjectField(
		env, attrs_class,
		(*env)->GetStaticFieldID(env, attrs_class, "LINE_AIRPORT", "Lde/schildbach/pte/dto/Line$Attr;")
	);
	jobject bicylces = (*env)->GetStaticObjectField(
		env, attrs_class,
		(*env)->GetStaticFieldID(env, attrs_class, "BICYCLE_CARRIAGE", "Lde/schildbach/pte/dto/Line$Attr;")
	);

	for (jsize i = 0; i < len; i++) {
		jobject entry = (*env)->GetObjectArrayElement(env, elems, i);
		if ((*env)->IsSameObject(env, entry, clockwise))
			self->attrs |= GPTE_LINE_ATTR_CIRCLE_CLOCKWISE;
		else if ((*env)->IsSameObject(env, entry, anticlockwise))
			self->attrs |= GPTE_LINE_ATTR_CIRCLE_ANTICLOCKWISE;
		else if ((*env)->IsSameObject(env, entry, replacement))
			self->attrs |= GPTE_LINE_ATTR_SERVICE_REPLACEMENT;
		else if ((*env)->IsSameObject(env, entry, airport))
			self->attrs |= GPTE_LINE_ATTR_LINE_AIRPORT;
		else if ((*env)->IsSameObject(env, entry, bicylces))
			self->attrs |= GPTE_LINE_ATTR_BICYCLE_CARRIAGE;
		else
			g_critical("Unknown line attribute: %p", entry);
		(*env)->DeleteLocalRef(env, entry);
	}

	self->cached |= GPTE_LINE_CACHED_ATTRS;
	return self->attrs;
}
GPTE_LINE_STRING_GETTER(message, GPTE_LINE_CACHED_MESSAGE)
