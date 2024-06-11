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

#include "gpteutils-priv.h"

jobject gpte_date_to_java(GpteJvm* vm, GDateTime* date) {
	GpteScopeGuard env = gpte_jvm_enter_scope(vm, 3);

	jclass date_class = (*env)->FindClass(env, "java/util/Date");
	jmethodID constructor = (*env)->GetMethodID(env, date_class, "<init>", "(J)V");
	jobject jdate = (*env)->NewObject(env, date_class, constructor, (jlong)(g_date_time_to_unix_usec(date) / 1000));

	return gpte_scope_guard_leave_with_ref(&env, jdate);
}

GDateTime* gpte_date_from_java(GpteJvm* vm, jobject date) {
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 2);

	jclass class = (*env)->FindClass(env, "java/util/Date");
	jmethodID get_time = (*env)->GetMethodID(env, class, "getTime", "()J");
	jlong millis = (*env)->CallLongMethod(env, date, get_time);
	return g_date_time_new_from_unix_utc_usec(millis * 1000);
}
