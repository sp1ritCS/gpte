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

#include "gptejavaobject.h"
#include "gptejavaobject-priv.h"
#include "gptejvm-priv.h"

typedef struct {
	GpteJvm* vm;
	jobject object;
} GpteJavaObjectPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (GpteJavaObject, gpte_java_object, G_TYPE_OBJECT)

enum {
	PROP_VM = 1,
	PROP_OBJECT,
	N_PROPERTIES
};
static GParamSpec* obj_properties[N_PROPERTIES] = { 0, };

static void gpte_java_object_finalize(GObject* object) {
	GpteJavaObjectPrivate* priv = gpte_java_object_get_instance_private(GPTE_JAVA_OBJECT(object));
	JNIEnv* env = gpte_jvm_get_env(priv->vm);
	(*env)->DeleteGlobalRef(env, priv->object);
	gpte_jvm_unref(priv->vm);
	G_OBJECT_CLASS(gpte_java_object_parent_class)->finalize(object);
}

static void gpte_java_object_get_property(GObject* object, guint prop_id, GValue* val, GParamSpec* pspec) {
	GpteJavaObjectPrivate* priv = gpte_java_object_get_instance_private(GPTE_JAVA_OBJECT(object));
	switch (prop_id) {
		case PROP_VM:
			g_value_set_boxed(val, priv->vm);
			break;
		case PROP_OBJECT:
			g_value_set_pointer(val, priv->object);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
	}
}
static void gpte_java_object_set_property(GObject* object, guint prop_id, const GValue* val, GParamSpec* pspec) {
	GpteJavaObjectPrivate* priv = gpte_java_object_get_instance_private(GPTE_JAVA_OBJECT(object));
	switch (prop_id) {
		case PROP_VM:
			g_return_if_fail(priv->vm == NULL);
			priv->vm = g_value_dup_boxed(val);
			break;
		case PROP_OBJECT: {
			g_return_if_fail(priv->object == NULL);
			if (!priv->vm) {
				g_critical("GpteJavaObject: property 'vm' needs to be set before 'object'");
				return;
			}
			JNIEnv* env = gpte_jvm_get_env(priv->vm);
			priv->object = (*env)->NewGlobalRef(env, (jobject)g_value_get_pointer(val));
		} break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
	}
}

static void gpte_java_object_class_init(GpteJavaObjectClass* class) {
	GObjectClass* object_class = G_OBJECT_CLASS(class);
	object_class->finalize = gpte_java_object_finalize;
	object_class->get_property = gpte_java_object_get_property;
	object_class->set_property = gpte_java_object_set_property;

	obj_properties[PROP_VM] = g_param_spec_boxed("vm", NULL, NULL, GPTE_TYPE_JVM, G_PARAM_STATIC_STRINGS | G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
	obj_properties[PROP_OBJECT] = g_param_spec_pointer("object", NULL, NULL, G_PARAM_STATIC_STRINGS | G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
	g_object_class_install_properties(object_class, N_PROPERTIES, obj_properties);
}

static void gpte_java_object_init(GpteJavaObject* self) {
	GpteJavaObjectPrivate* priv = gpte_java_object_get_instance_private(self);
	priv->vm = NULL;
	priv->object = NULL;
}

GpteJvm* gpte_java_object_get_vm(GpteJavaObject* self) {
	g_return_val_if_fail(GPTE_IS_JAVA_OBJECT(self), NULL);
	GpteJavaObjectPrivate* priv = gpte_java_object_get_instance_private(self);
	return priv->vm;
}

jobject gpte_java_object_get(GpteJavaObject* self) {
	g_return_val_if_fail(GPTE_IS_JAVA_OBJECT(self), NULL);
	GpteJavaObjectPrivate* priv = gpte_java_object_get_instance_private(self);
	return priv->object;
}

JNIEnv* gpte_java_object_env(GpteJavaObject* self) {
	g_return_val_if_fail(GPTE_IS_JAVA_OBJECT(self), NULL);
	GpteJavaObjectPrivate* priv = gpte_java_object_get_instance_private(self);
	return gpte_jvm_get_env(priv->vm);
}

gboolean gpte_java_object_same(GpteJavaObject* a, GpteJavaObject* b) {
	g_return_val_if_fail(GPTE_IS_JAVA_OBJECT(a) && GPTE_IS_JAVA_OBJECT(b), FALSE);
	if (a == b)
		return TRUE;
	GpteJavaObjectPrivate* ap = gpte_java_object_get_instance_private(a);
	GpteJavaObjectPrivate* bp = gpte_java_object_get_instance_private(b);
	if (ap->vm != bp->vm)
		return FALSE;
	JNIEnv* env = gpte_jvm_get_env(ap->vm);
	return (*env)->IsSameObject(env, ap->object, bp->object);
}

gboolean gpte_java_object_equal(GpteJavaObject* a, GpteJavaObject* b) {
	g_return_val_if_fail(GPTE_IS_JAVA_OBJECT(a) && GPTE_IS_JAVA_OBJECT(b), FALSE);
	if (a == b)
		return TRUE;
	GpteJavaObjectPrivate* ap = gpte_java_object_get_instance_private(a);
	GpteJavaObjectPrivate* bp = gpte_java_object_get_instance_private(b);
	if (ap->vm != bp->vm)
		return FALSE;
	JNIEnv* env = gpte_jvm_get_env(ap->vm);
	if ((*env)->IsSameObject(env, ap->object, bp->object))
		return TRUE;
	g_auto(GpteScopeGuard) guard = gpte_jvm_enter_scope(ap->vm, 2);
	jclass object_class = (*env)->FindClass(env, "java/lang/Object");
	jmethodID equal_fun = (*env)->GetMethodID(env, object_class, "equals", "(Ljava/lang/Object;)Z");
	return (*env)->CallBooleanMethod(env, ap->object, equal_fun, bp->object);
}

typedef union {
	guint u;
	gint i;
} GpteJavaObjetSignedNess;
guint gpte_java_object_hash(GpteJavaObject* self) {
	g_return_val_if_fail(GPTE_IS_JAVA_OBJECT(self), 0);
	GpteJavaObjectPrivate* priv = gpte_java_object_get_instance_private(self);
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(priv->vm, 2);
	jclass object_class = (*env)->FindClass(env, "java/lang/Object");
	jmethodID hash_fun = (*env)->GetMethodID(env, object_class, "hashCode", "()I");
	GpteJavaObjetSignedNess hash;
	hash.i = (*env)->CallIntMethod(env, priv->object, hash_fun);
	return hash.u;
}
