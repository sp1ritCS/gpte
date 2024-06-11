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

#ifndef __GPTEJAVAOBJECT_H__
#define __GPTEJAVAOBJECT_H__

#include <glib-object.h>
#include <gptejvm.h>

G_BEGIN_DECLS

/**
 * GpteJavaObject:
 * Base class for all objects that internally wrap a Java object.
 */

#define GPTE_TYPE_JAVA_OBJECT (gpte_java_object_get_type())
G_DECLARE_DERIVABLE_TYPE (GpteJavaObject, gpte_java_object, GPTE, JAVA_OBJECT, GObject)

struct _GpteJavaObjectClass {
	GObjectClass parent_class;
};

/**
 * gpte_java_object_get_vm:
 * @self: a java object wrapper
 *
 * Gets the Java VM wrapper that created/contains the object.
 * Returns: (transfer none): the Java VM wrapper
 */
GpteJvm* gpte_java_object_get_vm(GpteJavaObject* self);

/**
 * gpte_java_object_same:
 * @a: the left hand side of the comparison
 * @b: the right hand side of the comparison
 *
 * Tests if both objects are the same object.
 *
 * This is equivalent to the `==` operator in Java.
 *
 * Returns: %TRUE if a and b are the same, %FALSE if not
 */
gboolean gpte_java_object_same(GpteJavaObject* a, GpteJavaObject* b);

/**
 * gpte_java_object_equal:
 * @a: the left hand side of the comparison
 * @b: the right hand side of the comparison
 *
 * Tests if the objects are equal to each other.
 *
 * This is equivalent to the
 * [`equals`](https://docs.oracle.com/en/java/javase/18/docs/api/java.base/java/lang/Object.html#equals(java.lang.Object))
 * method in Java.
 *
 * Returns: %TRUE if a and b are equal, %FALSE if not
 */
gboolean gpte_java_object_equal(GpteJavaObject* a, GpteJavaObject* b);

/**
 * gpte_java_object_hash:
 * @self: a java object wrapper
 *
 * Calculates a hash value of the object.
 *
 * Returns: hash value for self
 */
guint gpte_java_object_hash(GpteJavaObject* self);

G_END_DECLS

#endif // __GPTEJAVAOBJECT_H__
