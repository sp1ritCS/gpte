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

#include "gptegtkprovidericon.h"
#include "gptegtkbin-priv.h"

#include "gpte_gtk_res.h"

static void gpte_gtk_load_resources(void) {
	static int loaded = 0;
	if (loaded)
		return;
	loaded = 1;
	g_resources_register(gpte_gtk_get_resource());
}

struct _GpteGtkProviderIcon {
	GpteGtkBin parent_instance;

	GpteProvider* provider;
	GtkImage* inner;
};

G_DEFINE_TYPE (GpteGtkProviderIcon, gpte_gtk_provider_icon, GPTE_GTK_TYPE_BIN)

enum {
	PROP_PROVIDER = 1,
	N_PROPERTIES
};
static GParamSpec* obj_properties[N_PROPERTIES] = { 0, };

static void gpte_gtk_provider_icon_dispose(GObject* object) {
	GpteGtkProviderIcon* self = GPTE_GTK_PROVIDER_ICON(object);
	g_clear_object(&self->inner);
	g_clear_object(&self->provider);
	G_OBJECT_CLASS(gpte_gtk_provider_icon_parent_class)->dispose(object);
}

static void gpte_gtk_provider_icon_get_property(GObject* object, guint prop_id, GValue* val, GParamSpec* pspec) {
	GpteGtkProviderIcon* self = GPTE_GTK_PROVIDER_ICON(object);
	switch (prop_id) {
		case PROP_PROVIDER:
			g_value_set_object(val, gpte_gtk_provider_icon_get_provider(self));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
	}
}
static void gpte_gtk_provider_icon_set_property(GObject* object, guint prop_id, const GValue* val, GParamSpec* pspec) {
	GpteGtkProviderIcon* self = GPTE_GTK_PROVIDER_ICON(object);
	switch (prop_id) {
		case PROP_PROVIDER:
			gpte_gtk_provider_icon_set_provider(self, g_value_get_object(val));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
	}
}

static void gpte_gtk_provider_icon_class_init(GpteGtkProviderIconClass* class) {
	gpte_gtk_load_resources();

	GObjectClass* object_class = G_OBJECT_CLASS(class);
	object_class->dispose = gpte_gtk_provider_icon_dispose;
	object_class->get_property = gpte_gtk_provider_icon_get_property;
	object_class->set_property = gpte_gtk_provider_icon_set_property;

	obj_properties[PROP_PROVIDER] = g_param_spec_object("provider", NULL, NULL, GPTE_TYPE_PROVIDER, G_PARAM_STATIC_STRINGS | G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);
	g_object_class_install_properties(object_class, N_PROPERTIES, obj_properties);
}
static void gpte_gtk_provider_icon_init(GpteGtkProviderIcon* self) {
	self->provider = NULL;

	self->inner = g_object_ref_sink(GTK_IMAGE(gtk_image_new()));
	gtk_image_set_icon_size(self->inner, GTK_ICON_SIZE_LARGE);
	gpte_gtk_bin_set_child(GPTE_GTK_BIN(self), GTK_WIDGET(self->inner));
}

GpteProvider* gpte_gtk_provider_icon_get_provider(GpteGtkProviderIcon* self) {
	g_return_val_if_fail(GPTE_GTK_IS_PROVIDER_ICON(self), NULL);
	return self->provider;
}
void gpte_gtk_provider_icon_set_provider(GpteGtkProviderIcon* self, GpteProvider* provider) {
	g_return_if_fail(GPTE_GTK_IS_PROVIDER_ICON(self));
	g_return_if_fail(!provider || GPTE_IS_PROVIDER(provider));
	if (self->provider)
		g_object_unref(self->provider);
	self->provider = provider;
	if (self->provider) {
		g_object_ref(self->provider);

		g_autofree gchar* resource_path = g_strdup_printf("/arpa/sp1rit/gpte/gtk/networks/%s.svg", gpte_provider_get_id(self->provider));
		GError* err = NULL;
		g_autoptr(GBytes) resource = g_resources_lookup_data(resource_path, G_RESOURCE_LOOKUP_FLAGS_NONE, &err);
		if (err) {
			if (g_error_matches(err, G_RESOURCE_ERROR, G_RESOURCE_ERROR_NOT_FOUND)) {
				gtk_image_set_from_resource(self->inner, "/arpa/sp1rit/gpte/gtk/networks/placeholder.svg");
			} else {
				g_warning("Failed loading provider icon: %s", err->message);
				gtk_image_set_from_resource(self->inner, NULL);
			}
			g_error_free(err);
		} else {
			gtk_image_set_from_resource(self->inner, resource_path);
		}
	} else {
		gtk_image_set_from_resource(self->inner, NULL);
	}

	g_object_notify_by_pspec(G_OBJECT(self), obj_properties[PROP_PROVIDER]);
}
