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

#include "gptegtkbin.h"
#include "gptegtkbin-priv.h"

typedef struct {
	GtkWidget* child;
} GpteGtkBinPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (GpteGtkBin, gpte_gtk_bin, GTK_TYPE_WIDGET)

enum {
	PROP_CHILD = 1,
	N_PROPERTIES
};
static GParamSpec* obj_properties[N_PROPERTIES] = { 0, };

static void gpte_gtk_bin_dispose(GObject* object) {
	GpteGtkBinPrivate* priv = gpte_gtk_bin_get_instance_private(GPTE_GTK_BIN(object));
	g_clear_pointer(&priv->child, gtk_widget_unparent);
	G_OBJECT_CLASS(gpte_gtk_bin_parent_class)->dispose(object);
}

static void gpte_gtk_bin_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* pspec) {
	GpteGtkBin* self = GPTE_GTK_BIN(object);
	switch (prop_id) {
		case PROP_CHILD:
			g_value_set_object(value, gpte_gtk_bin_get_child(self));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
	}
}
static void gpte_gtk_bin_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* pspec) {
 	GpteGtkBin* self = GPTE_GTK_BIN(object);
	switch (prop_id) {
		case PROP_CHILD:
			gpte_gtk_bin_set_child(self, g_value_get_object(value));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
	}
}

static void gpte_gtk_bin_compute_expand(GtkWidget* widget, gboolean* hexpand_p, gboolean* vexpand_p) {
	GpteGtkBinPrivate* priv = gpte_gtk_bin_get_instance_private(GPTE_GTK_BIN(widget));
	if (priv->child) {
		*hexpand_p = gtk_widget_compute_expand(priv->child, GTK_ORIENTATION_HORIZONTAL);
		*vexpand_p = gtk_widget_compute_expand(priv->child, GTK_ORIENTATION_VERTICAL);
	} else {
		*hexpand_p = FALSE;
		*vexpand_p = FALSE;
	}
}

static gboolean gpte_gtk_bin_focus(GtkWidget* widget, GtkDirectionType direction) {
	GpteGtkBinPrivate* priv = gpte_gtk_bin_get_instance_private(GPTE_GTK_BIN(widget));
	if (priv->child)
		return gtk_widget_child_focus(priv->child, direction);
	else
		return FALSE;
}

static void gpte_gtk_bin_class_init(GpteGtkBinClass* class) {
	GObjectClass *object_class = G_OBJECT_CLASS(class);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(class);

	object_class->dispose = gpte_gtk_bin_dispose;
	object_class->get_property = gpte_gtk_bin_get_property;
	object_class->set_property = gpte_gtk_bin_set_property;
	widget_class->compute_expand = gpte_gtk_bin_compute_expand;
	widget_class->focus = gpte_gtk_bin_focus;

	obj_properties[PROP_CHILD] = g_param_spec_object("child", NULL, NULL, GTK_TYPE_WIDGET, G_PARAM_STATIC_STRINGS | G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);
	g_object_class_install_properties(object_class, N_PROPERTIES, obj_properties);

	gtk_widget_class_set_layout_manager_type (widget_class, GTK_TYPE_BIN_LAYOUT);
}

static void gpte_gtk_bin_init(GpteGtkBin* self) {
	GpteGtkBinPrivate* priv = gpte_gtk_bin_get_instance_private(self);
	priv->child = NULL;
}

GtkWidget* gpte_gtk_bin_new(GtkWidget* child) {
	return g_object_new(GPTE_GTK_TYPE_BIN, "child", child, NULL);
}

GtkWidget* gpte_gtk_bin_get_child(GpteGtkBin* self) {
	g_return_val_if_fail(GPTE_GTK_IS_BIN(self), NULL);
	GpteGtkBinPrivate* priv = gpte_gtk_bin_get_instance_private(self);
	return priv->child;
}

void gpte_gtk_bin_set_child(GpteGtkBin* self, GtkWidget* child) {
	g_return_if_fail(GPTE_GTK_IS_BIN (self));
	g_return_if_fail(!child || GTK_IS_WIDGET(child));

	if (child)
		g_return_if_fail(gtk_widget_get_parent(child) == NULL);

	GpteGtkBinPrivate* priv = gpte_gtk_bin_get_instance_private(self);
	if (priv->child == child)
		return;
	if (priv->child)
		gtk_widget_unparent(priv->child);
	priv->child = child;
	if (priv->child)
		gtk_widget_set_parent(priv->child, GTK_WIDGET(self));

	g_object_notify_by_pspec(G_OBJECT(self), obj_properties[PROP_CHILD]);
}
