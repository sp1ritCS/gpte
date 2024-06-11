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

#include "gptelist.h"
#include "gptelist-priv.h"
#include <gptejavaobject-priv.h>

static void gpte_list_g_object_unref_with_null_guard(GObject* object) {
	if (object)
		g_object_unref(object);
}

struct _GpteList {
	GpteJavaObject parent_instance;

	GTypeClass* child_kind;

	gint length;
	GPtrArray* cache;
};

static void gpte_list_iface_init(GListModelInterface* iface);
G_DEFINE_TYPE_WITH_CODE (GpteList, gpte_list, GPTE_TYPE_JAVA_OBJECT,
	G_IMPLEMENT_INTERFACE(G_TYPE_LIST_MODEL, gpte_list_iface_init)
)

enum {
	PROP_TYPE = 1,
	N_PROPERTIES
};
static GParamSpec* obj_properties[N_PROPERTIES] = { 0, };

static void gpte_list_finalize(GObject* object) {
	GpteList* self = GPTE_LIST(object);
	g_type_class_unref(self->child_kind);
	G_OBJECT_CLASS(gpte_list_parent_class)->finalize(object);
}
static void gpte_list_dispose(GObject* object) {
	GpteList* self = GPTE_LIST(object);
	g_ptr_array_unref(self->cache);
	G_OBJECT_CLASS(gpte_list_parent_class)->dispose(object);
}

static void gpte_list_get_property(GObject* object, guint prop_id, GValue* val, GParamSpec* pspec) {
	GpteList* self = GPTE_LIST(object);
	switch (prop_id) {
		case PROP_TYPE:
			g_value_set_gtype(val, G_TYPE_FROM_CLASS(self->child_kind));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
	}
}
static void gpte_list_set_property(GObject* object, guint prop_id, const GValue* val, GParamSpec* pspec) {
	GpteList* self = GPTE_LIST(object);
	switch (prop_id) {
		case PROP_TYPE: {
			g_return_if_fail(self->child_kind == NULL);
			GTypeClass* kind = g_type_class_ref(g_value_get_gtype(val));
			if (!g_type_check_class_is_a(kind, GPTE_TYPE_JAVA_OBJECT)) {
				g_critical("Gpte.List only accepts subclasses of Gpte.JavaObject");
				g_type_class_unref(kind);
			}
			self->child_kind = kind;
		} break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
	}
}

static void gpte_list_class_init(GpteListClass* class) {
	GObjectClass* object_class = G_OBJECT_CLASS(class);
	object_class->finalize = gpte_list_finalize;
	object_class->dispose = gpte_list_dispose;
	object_class->get_property = gpte_list_get_property;
	object_class->set_property = gpte_list_set_property;

	obj_properties[PROP_TYPE] = g_param_spec_gtype("type", NULL, NULL, GPTE_TYPE_JAVA_OBJECT, G_PARAM_STATIC_STRINGS | G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
	g_object_class_install_properties(object_class, N_PROPERTIES, obj_properties);
}

static void gpte_list_init(GpteList* self) {
	self->child_kind = NULL;
	self->length = -1;
	self->cache = g_ptr_array_new_with_free_func((GDestroyNotify)gpte_list_g_object_unref_with_null_guard);
}

static GType gpte_list_model_get_type(GListModel* model) {
	GpteList* self = GPTE_LIST(model);
	return G_TYPE_FROM_CLASS(self->child_kind);
}
static guint gpte_list_model_get_n_items(GListModel* model) {
	GpteList* self = GPTE_LIST(model);
	if (self->length >= 0)
		return self->length;
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self)), 2);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));

	jclass class = (*env)->FindClass(env, "java/util/List");
	jmethodID mid = (*env)->GetMethodID(env, class, "size", "()I");
	self->length = (*env)->CallIntMethod(env, this, mid);
	return self->length;
}
static gpointer gpte_list_model_get_item(GListModel* model, guint idx) {
	GpteList* self = GPTE_LIST(model);
	guint length = gpte_list_model_get_n_items(model);
	if (idx >= length)
		return NULL;
	if (idx < self->cache->len) {
		gpointer c = g_ptr_array_index(self->cache, idx);
		if (c)
			return g_object_ref(c);
	}
	GpteJvm* vm = gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self));
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 3);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));

	jclass class = (*env)->FindClass(env, "java/util/List");
	jmethodID mid = (*env)->GetMethodID(env, class, "get", "(I)Ljava/lang/Object;");
	jobject item = (*env)->CallObjectMethod(env, this, mid, idx);
	gpointer ret = g_object_new(G_TYPE_FROM_CLASS(self->child_kind), "vm", vm, "object", item, NULL);
	if (idx >= self->cache->len)
		g_ptr_array_insert(self->cache, idx, g_object_ref(ret));
	else
		g_ptr_array_index(self->cache, idx) = g_object_ref(ret);
	return ret;
}
static void gpte_list_iface_init(GListModelInterface* iface) {
	iface->get_item_type = gpte_list_model_get_type;
	iface->get_n_items = gpte_list_model_get_n_items;
	iface->get_item = gpte_list_model_get_item;
}

GListModel* gpte_list_new(GpteJvm* vm, GType type, jobject list) {
	return g_object_new(GPTE_TYPE_LIST, "vm", vm, "object", list, "type", type, NULL);
}

void gpte_list_prepend(GpteList* self, jobject list) {
	g_return_if_fail(GPTE_IS_LIST(self));

	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self)), 3);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));

	jclass class = (*env)->FindClass(env, "java/util/List");
	jmethodID mid = (*env)->GetMethodID(env, class, "size", "()I");
	jint length = (*env)->CallIntMethod(env, list, mid);

	jmethodID prepend = (*env)->GetMethodID(env, class, "addAll", "(ILjava/util/Collection;)Z");
	jboolean res = (*env)->CallBooleanMethod(env, this, prepend, 0, list);
	if (!res)
		return;

	GPtrArray* new_cache = g_ptr_array_new_with_free_func((GDestroyNotify)gpte_list_g_object_unref_with_null_guard);
	g_ptr_array_set_size(new_cache, self->length);
	g_ptr_array_extend_and_steal(new_cache, self->cache);
	self->cache = new_cache;
	self->length += length;

	g_list_model_items_changed(G_LIST_MODEL(self), 0, 0, length);
}

void gpte_list_append(GpteList* self, jobject list) {
	g_return_if_fail(GPTE_IS_LIST(self));

	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(gpte_java_object_get_vm(GPTE_JAVA_OBJECT(self)), 3);
	jobject this = gpte_java_object_get(GPTE_JAVA_OBJECT(self));

	jclass class = (*env)->FindClass(env, "java/util/List");
	jmethodID mid = (*env)->GetMethodID(env, class, "size", "()I");
	jint length = (*env)->CallIntMethod(env, list, mid);

	jmethodID append = (*env)->GetMethodID(env, class, "addAll", "(Ljava/util/Collection;)Z");
	jboolean res = (*env)->CallBooleanMethod(env, this, append, list);
	if (!res)
		return;

	guint old_length = self->length;
	self->length += length;
	g_list_model_items_changed(G_LIST_MODEL(self), old_length, 0, length);
}
