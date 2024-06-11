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

#ifndef __GPTEJVM_PRIV_H__
#define __GPTEJVM_PRIV_H__

#include <gptejvm.h>
#include <jni.h>

G_BEGIN_DECLS

typedef JNIEnv* GpteScopeGuard;

void gpte_scope_guard_leave(GpteScopeGuard* self);
jobject gpte_scope_guard_leave_with_ref(GpteScopeGuard* self, jobject ref);

G_DEFINE_AUTO_CLEANUP_CLEAR_FUNC(GpteScopeGuard, gpte_scope_guard_leave)

struct _GpteJvm {
	gatomicrefcount rc;

	gint jar_fd;
	JavaVM* vm;
	JNIEnv* main_env;
};

JNIEnv* gpte_jvm_get_env(GpteJvm* self);
GpteScopeGuard gpte_jvm_enter_scope(GpteJvm* self, gint capacity);

G_END_DECLS

#endif // __GPTEJVM_PRIV_H__
