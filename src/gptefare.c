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

#include "gptefare.h"
#include "gptejavaobject-priv.h"

#include <math.h>

G_DEFINE_ENUM_TYPE(GpteFareType, gpte_fare_type,
	G_DEFINE_ENUM_VALUE(GPTE_FARE_ADULT, "adult"),
	G_DEFINE_ENUM_VALUE(GPTE_FARE_BIKE, "bike"),
	G_DEFINE_ENUM_VALUE(GPTE_FARE_DISABLED, "disabled"),
	G_DEFINE_ENUM_VALUE(GPTE_FARE_MILITARY, "military"),
	G_DEFINE_ENUM_VALUE(GPTE_FARE_SENIOR, "senior"),
	G_DEFINE_ENUM_VALUE(GPTE_FARE_STUDENT, "student"),
	G_DEFINE_ENUM_VALUE(GPTE_FARE_YOUTH, "youth")
)


G_DEFINE_BOXED_TYPE(GpteCurrency, gpte_currency, gpte_currency_copy, gpte_currency_free)

GpteCurrency* gpte_currency_copy(const GpteCurrency* self) {
	GpteCurrency* new = g_new(GpteCurrency, 1);
	new->name = g_strdup(self->name);
	new->symbol = g_strdup(self->symbol);
	return new;
}
void gpte_currency_free(GpteCurrency* self) {
	g_free(self->name);
	g_free(self->symbol);
	g_free(self);
}

typedef enum {
	GPTE_FARE_CACHED_NAME = 1 << 0,
	GPTE_FARE_CACHED_TYPE = 1 << 1,
	GPTE_FARE_CACHED_CURRENCY = 1 << 2,
	GPTE_FARE_CACHED_FARE = 1 << 3
} GpteFareCachedValues;

struct _GpteFare {
	GpteJavaObject parent_instance;

	GpteFareCachedValues cached;
	gchar* cached_name;
	GpteFareType cached_type;
	GpteCurrency cached_currency;
	gfloat cached_fare;
};

G_DEFINE_TYPE (GpteFare, gpte_fare, GPTE_TYPE_JAVA_OBJECT)

static void gpte_fare_finalize(GObject* object) {
	GpteFare* self = GPTE_FARE(object);
	if (self->cached & GPTE_FARE_CACHED_NAME)
		g_free(self->cached_name);
	if (self->cached & GPTE_FARE_CACHED_CURRENCY) {
		g_free(self->cached_currency.name);
		g_free(self->cached_currency.symbol);
	}
	G_OBJECT_CLASS(gpte_fare_parent_class)->finalize(object);
}

static void gpte_fare_class_init(GpteFareClass* class) {
	G_OBJECT_CLASS(class)->finalize = gpte_fare_finalize;
}

static void gpte_fare_init(GpteFare* self) {
	self->cached = 0;
}

const gchar* gpte_fare_get_name(GpteFare* self) {
	g_return_val_if_fail(GPTE_IS_FARE(self), NULL);
	if (self->cached & GPTE_FARE_CACHED_NAME)
		return self->cached_name;

	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 3);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));

	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/Fare");
	jfieldID name_id = (*env)->GetFieldID(env, class, "name", "Ljava/lang/String;");
	jstring jname = (*env)->GetObjectField(env, this, name_id);

	const char* utf = (*env)->GetStringUTFChars(env, jname, NULL);
	self->cached_name = g_strdup(utf);
	(*env)->ReleaseStringUTFChars(env, jname, utf);
	self->cached |= GPTE_FARE_CACHED_NAME;
	return self->cached_name;
}

GpteFareType gpte_fare_get_fare_type(GpteFare* self) {
	g_return_val_if_fail(GPTE_IS_FARE(self), -1);
	if (self->cached & GPTE_FARE_CACHED_TYPE)
		return self->cached_type;

	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 20);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));

	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/Fare");
	jfieldID type_id = (*env)->GetFieldID(env, class, "type", "Lde/schildbach/pte/dto/Fare$Type;");
	jobject type = (*env)->GetObjectField(env, this, type_id);

	jclass enum_class = (*env)->FindClass(env, "de/schildbach/pte/dto/Fare$Type");
	if ((*env)->IsSameObject(env, type, (*env)->GetStaticObjectField(env, enum_class, (*env)->GetStaticFieldID(env, enum_class, "ADULT", "Lde/schildbach/pte/dto/Fare$Type;"))))
		self->cached_type = GPTE_FARE_ADULT;
	else if ((*env)->IsSameObject(env, type, (*env)->GetStaticObjectField(env, enum_class, (*env)->GetStaticFieldID(env, enum_class, "BIKE", "Lde/schildbach/pte/dto/Fare$Type;"))))
		self->cached_type = GPTE_FARE_BIKE;
	else if ((*env)->IsSameObject(env, type, (*env)->GetStaticObjectField(env, enum_class, (*env)->GetStaticFieldID(env, enum_class, "CHILD", "Lde/schildbach/pte/dto/Fare$Type;"))))
		self->cached_type = GPTE_FARE_CHILD;
	else if ((*env)->IsSameObject(env, type, (*env)->GetStaticObjectField(env, enum_class, (*env)->GetStaticFieldID(env, enum_class, "DISABLED", "Lde/schildbach/pte/dto/Fare$Type;"))))
		self->cached_type = GPTE_FARE_DISABLED;
	else if ((*env)->IsSameObject(env, type, (*env)->GetStaticObjectField(env, enum_class, (*env)->GetStaticFieldID(env, enum_class, "MILITARY", "Lde/schildbach/pte/dto/Fare$Type;"))))
		self->cached_type = GPTE_FARE_MILITARY;
	else if ((*env)->IsSameObject(env, type, (*env)->GetStaticObjectField(env, enum_class, (*env)->GetStaticFieldID(env, enum_class, "SENIOR", "Lde/schildbach/pte/dto/Fare$Type;"))))
		self->cached_type = GPTE_FARE_SENIOR;
	else if ((*env)->IsSameObject(env, type, (*env)->GetStaticObjectField(env, enum_class, (*env)->GetStaticFieldID(env, enum_class, "STUDENT", "Lde/schildbach/pte/dto/Fare$Type;"))))
		self->cached_type = GPTE_FARE_STUDENT;
	else if ((*env)->IsSameObject(env, type, (*env)->GetStaticObjectField(env, enum_class, (*env)->GetStaticFieldID(env, enum_class, "YOUTH", "Lde/schildbach/pte/dto/Fare$Type;"))))
		self->cached_type = GPTE_FARE_YOUTH;
	else {
		g_critical("Unknown fare type: %p", type);
		return -1;
	}

	self->cached |= GPTE_FARE_CACHED_TYPE;
	return self->cached_type;
}

const GpteCurrency* gpte_fare_get_currency(GpteFare* self) {
	g_return_val_if_fail(GPTE_IS_FARE(self), NULL);
	if (self->cached & GPTE_FARE_CACHED_CURRENCY)
		return &self->cached_currency;

	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 8);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));

	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/Fare");
	jfieldID currency_id = (*env)->GetFieldID(env, class, "currency", "Ljava/util/Currency;");
	jobject currency = (*env)->GetObjectField(env, this, currency_id);

	jclass currency_class = (*env)->FindClass(env, "java/util/Currency");
	jmethodID get_code = (*env)->GetMethodID(env, currency_class, "getCurrencyCode", "()Ljava/lang/String;");
	jstring code = (*env)->CallObjectMethod(env, currency, get_code);
	const char* code_utf = (*env)->GetStringUTFChars(env, code, NULL);
	self->cached_currency.name = g_strdup(code_utf);
	(*env)->ReleaseStringUTFChars(env, code, code_utf);

	jmethodID get_symbol = (*env)->GetMethodID(env, currency_class, "getSymbol", "()Ljava/lang/String;");
	jstring symbol = (*env)->CallObjectMethod(env, currency, get_symbol);
	const char* symbol_utf = (*env)->GetStringUTFChars(env, symbol, NULL);
	self->cached_currency.symbol = g_strdup(symbol_utf);
	(*env)->ReleaseStringUTFChars(env, symbol, symbol_utf);

	self->cached |= GPTE_FARE_CACHED_CURRENCY;
	return &self->cached_currency;
}

gfloat gpte_fare_get_fare(GpteFare* self) {
	g_return_val_if_fail(GPTE_IS_FARE(self), NAN);
	if (self->cached & GPTE_FARE_CACHED_FARE)
		return self->cached_fare;

	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 2);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));

	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/Fare");
	jfieldID fare_id = (*env)->GetFieldID(env, class, "fare", "F");
	self->cached_fare = (*env)->GetFloatField(env, this, fare_id);

	self->cached |= GPTE_FARE_CACHED_FARE;
	return self->cached_fare;
}
