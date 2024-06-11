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

#include "gpteproducts.h"
#include "gpteproducts-priv.h"

G_DEFINE_ENUM_TYPE(GpteProductCode, gpte_product_code,
	G_DEFINE_ENUM_VALUE(GPTE_PRODUCT_CODE_NULL, "null"),
	G_DEFINE_ENUM_VALUE(GPTE_PRODUCT_CODE_HIGH_SPEED_TRAIN, "high-speed-train"),
	G_DEFINE_ENUM_VALUE(GPTE_PRODUCT_CODE_REGIONAL_TRAIN, "regional-train"),
	G_DEFINE_ENUM_VALUE(GPTE_PRODUCT_CODE_SUBURBAN_TRAIN, "suburban-train"),
	G_DEFINE_ENUM_VALUE(GPTE_PRODUCT_CODE_SUBWAY, "subway"),
	G_DEFINE_ENUM_VALUE(GPTE_PRODUCT_CODE_TRAM, "tram"),
	G_DEFINE_ENUM_VALUE(GPTE_PRODUCT_CODE_BUS, "bus"),
	G_DEFINE_ENUM_VALUE(GPTE_PRODUCT_CODE_FERRY, "ferry"),
	G_DEFINE_ENUM_VALUE(GPTE_PRODUCT_CODE_CABLECAR, "cablecar"),
	G_DEFINE_ENUM_VALUE(GPTE_PRODUCT_CODE_ON_DEMAND, "on-demand")
)

G_DEFINE_FLAGS_TYPE(GpteProducts, gpte_products,
	G_DEFINE_ENUM_VALUE(GPTE_PRODUCT_HIGH_SPEED_TRAIN, "high-speed-train"),
	G_DEFINE_ENUM_VALUE(GPTE_PRODUCT_REGIONAL_TRAIN, "regional-train"),
	G_DEFINE_ENUM_VALUE(GPTE_PRODUCT_SUBURBAN_TRAIN, "suburban-train"),
	G_DEFINE_ENUM_VALUE(GPTE_PRODUCT_SUBWAY, "subway"),
	G_DEFINE_ENUM_VALUE(GPTE_PRODUCT_TRAM, "tram"),
	G_DEFINE_ENUM_VALUE(GPTE_PRODUCT_BUS, "bus"),
	G_DEFINE_ENUM_VALUE(GPTE_PRODUCT_FERRY, "ferry"),
	G_DEFINE_ENUM_VALUE(GPTE_PRODUCT_CABLECAR, "cablecar"),
	G_DEFINE_ENUM_VALUE(GPTE_PRODUCT_ON_DEMAND, "on-demand")
)


GpteProductCode gpte_product_code_from_java(GpteJvm* vm, jobject product) {
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 2);

	jclass product_class = (*env)->FindClass(env, "de/schildbach/pte/dto/Product");
	jfieldID product_code = (*env)->GetFieldID(env, product_class, "code", "C");
	jchar code = (*env)->GetCharField(env, product, product_code);
	return (GpteProductCode)code;
}

gint gpte_product_code_to_priority(GpteProductCode code) {
	switch (code) {
		case GPTE_PRODUCT_CODE_HIGH_SPEED_TRAIN:
			return 100;
		case GPTE_PRODUCT_CODE_REGIONAL_TRAIN:
			return 80;
		case GPTE_PRODUCT_CODE_SUBURBAN_TRAIN:
			return 70;
		case GPTE_PRODUCT_CODE_SUBWAY:
			return 50;
		case GPTE_PRODUCT_CODE_TRAM:
			return 45;
		case GPTE_PRODUCT_CODE_BUS:
			return 35;
		case GPTE_PRODUCT_CODE_FERRY:
			return 30;
		case GPTE_PRODUCT_CODE_CABLECAR:
			return 20;
		case GPTE_PRODUCT_CODE_ON_DEMAND:
			return 10;
		case GPTE_PRODUCT_CODE_NULL:
		default:
			return -1;
	}
}

#define GPTE_PRODUCT_SWITCH(kw) \
	case GPTE_PRODUCT_CODE_##kw:\
		ret |= GPTE_PRODUCT_##kw;\
		break;

GpteProducts gpte_products_from_set(GpteJvm* vm, jobject set) {
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 6);

	jclass set_class = (*env)->FindClass(env, "java/util/Set");
	jmethodID set_to_array = (*env)->GetMethodID(env, set_class, "toArray", "()[Ljava/lang/Object;");
	jobjectArray elems = (*env)->CallObjectMethod(env, set, set_to_array);
	gint len = (*env)->GetArrayLength(env, elems);

	jclass product_class = (*env)->FindClass(env, "de/schildbach/pte/dto/Product");
	jfieldID product_code = (*env)->GetFieldID(env, product_class, "code", "C");

	GpteProducts ret = 0;
	for (gint i = 0; i < len; i++) {
		jobject obj = (*env)->GetObjectArrayElement(env, elems, i);
		jchar code = (*env)->GetCharField(env, obj, product_code);
		switch (code) {
			GPTE_PRODUCT_SWITCH(HIGH_SPEED_TRAIN)
			GPTE_PRODUCT_SWITCH(REGIONAL_TRAIN)
			GPTE_PRODUCT_SWITCH(SUBURBAN_TRAIN)
			GPTE_PRODUCT_SWITCH(SUBWAY)
			GPTE_PRODUCT_SWITCH(TRAM)
			GPTE_PRODUCT_SWITCH(BUS)
			GPTE_PRODUCT_SWITCH(FERRY)
			GPTE_PRODUCT_SWITCH(CABLECAR)
			GPTE_PRODUCT_SWITCH(ON_DEMAND)
			default:
				g_critical("Unknown product code: %c", code);
		}
		(*env)->DeleteLocalRef(env, obj);
	}
	return ret;
}

#define GPTE_PRODUCT_CONV(kw) \
	if (products & GPTE_PRODUCT_##kw) { \
		jobject itrain = (*env)->GetStaticObjectField(env, products_enum, (*env)->GetStaticFieldID(env, products_enum, #kw, "Lde/schildbach/pte/dto/Product;")); \
		(*env)->CallBooleanMethod(env, jproducts, add_to_set, itrain); \
	}

jobject gpte_products_to_java(GpteJvm* vm, GpteProducts products) {
	GpteScopeGuard env = gpte_jvm_enter_scope(vm, 14);

	jclass set_class = (*env)->FindClass(env, "java/util/HashSet");
	jmethodID constructor = (*env)->GetMethodID(env, set_class, "<init>", "()V");
	jmethodID add_to_set = (*env)->GetMethodID(env, set_class, "add", "(Ljava/lang/Object;)Z");

	jclass products_enum = (*env)->FindClass(env, "de/schildbach/pte/dto/Product");
	jobject jproducts = (*env)->NewObject(env, set_class, constructor);
	GPTE_PRODUCT_CONV(HIGH_SPEED_TRAIN)
	GPTE_PRODUCT_CONV(REGIONAL_TRAIN)
	GPTE_PRODUCT_CONV(SUBURBAN_TRAIN)
	GPTE_PRODUCT_CONV(SUBWAY)
	GPTE_PRODUCT_CONV(TRAM)
	GPTE_PRODUCT_CONV(BUS)
	GPTE_PRODUCT_CONV(FERRY)
	GPTE_PRODUCT_CONV(CABLECAR)
	GPTE_PRODUCT_CONV(ON_DEMAND)
	return gpte_scope_guard_leave_with_ref(&env, jproducts);
}

