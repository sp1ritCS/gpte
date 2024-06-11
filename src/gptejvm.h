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

#ifndef __GPTEJVM_H__
#define __GPTEJVM_H__

#include <glib-object.h>

G_BEGIN_DECLS

/**
 * GpteThreadGuard:
 * Object that needs to stay alive on any thread that isn't the main
 * thread.
 *
 *
 * Once this is freed, the Java VM can not accept calls from this thread
 * anymore.
 */

#define GPTE_TYPE_THREAD_GUARD (gpte_thread_guard_get_type())
GType gpte_thread_guard_get_type();
typedef struct _GpteThreadGuard GpteThreadGuard;

/**
 * gpte_thread_guard_ref:
 * @self: the thread guard
 *
 * Increments the reference counter of @self by one.
 * Returns: (transfer full): @self
 */
GpteThreadGuard* gpte_thread_guard_ref(GpteThreadGuard* self);

/**
 * gpte_thread_guard_unref:
 * @self: the thread guard
 *
 * Decrements the reference counter of @self by one.
 *
 * Once the counter hits zero, @self (and the corresponding JVM Thread)
 * will be freed. Do not call any more GPTE functions on this thread
 * after this.
 */
void gpte_thread_guard_unref(GpteThreadGuard* self);

/**
 * gpte_thread_guard_ping:
 * @self: the thread guard
 *
 * No-op.
 *
 * Use this to ensure that GpteThreadGuard is not freed prematurely when
 * a garbage collector is in use.
 */
void gpte_thread_guard_ping(GpteThreadGuard* self);

G_DEFINE_AUTOPTR_CLEANUP_FUNC(GpteThreadGuard, gpte_thread_guard_unref)


/**
 * GpteJvm:
 * Container that holds the instance of the
 * [Java Virtual Machine](https://docs.oracle.com/en/java/javase/17/vm/java-virtual-machine-technology-overview.html).
 *
 *
 * There may only be one instance alive at a time.
 */

#define GPTE_TYPE_JVM (gpte_jvm_get_type())
GType gpte_jvm_get_type(void);
typedef struct _GpteJvm GpteJvm;

/**
 * gpte_vm_create:
 * @err: a #GError, or %NULL
 *
 * Creates and initializes the Java VM.
 *
 * Returns: (transfer full): the JVM wrapper
 */
GpteJvm* gpte_jvm_create(GError** err);

/**
 * gpte_jvm_ref:
 * @self: the JVM wrapper
 *
 * Increments the reference count of @self by one.
 *
 * Returns: (transfer full): @self
 */
GpteJvm* gpte_jvm_ref(GpteJvm* self);

/**
 * gpte_vm_unref:
 * @self: the JVM wrapper
 *
 * Decrements the reference count of @self by one.
 *
 * Once the counter hits zero, the JVM will be deinitialized and @self
 * freed from memory. Do not use @self after this method was called.
 */
void gpte_jvm_unref(GpteJvm* self);

/**
 * gpte_jvm_attach_thread:
 * @self: the JVM wrapper
 * @name: (nullable): the name of this thread or %NULL
 *
 * Attaches the current thread to the JVM.
 *
 * This method must be called on any non-main thread before calling
 * other GPTE methods. Ensure that the returned
 * [struct@Gpte.ThreadGuard] is kept alive during this time.
 *
 * Returns: (transfer full): a thread guard for the current thread
 */
GpteThreadGuard* gpte_jvm_attach_thread(GpteJvm* self, const gchar* name);

/**
 * gpte_vm_error:
 * @self: the JVM wrapper
 * @err: a #GError or %NULL
 *
 * Checks if the JVM ran into an exception and rethrows it as @err.
 *
 * Returns: %TRUE if the JVM has a pending error, %FALSE otherwise
 */
gboolean gpte_jvm_error(GpteJvm* self, GError** err);

G_DEFINE_AUTOPTR_CLEANUP_FUNC(GpteJvm, gpte_jvm_unref)

G_END_DECLS

#endif // __GPTEJVM_H__

