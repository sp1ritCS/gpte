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

#include "gptegtklinewidget.h"

typedef struct {
	GpteLine* line;
	GskRenderNode* cache;
} GpteGtkLineWidgetPrivate;
G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE (GpteGtkLineWidget, gpte_gtk_line_widget, GTK_TYPE_WIDGET)

enum {
	PROP_LINE = 1,
	N_PROPERTIES
};
static GParamSpec* obj_properties[N_PROPERTIES] = { 0, };

static void gpte_gtk_line_widget_dispose(GObject* object) {
	GpteGtkLineWidgetPrivate* priv = gpte_gtk_line_widget_get_instance_private(GPTE_GTK_LINE_WIDGET(object));
	g_clear_pointer(&priv->cache, gsk_render_node_unref);
	g_clear_object(&priv->line);
	G_OBJECT_CLASS(gpte_gtk_line_widget_parent_class)->dispose(object);
}

static void gpte_gtk_line_widget_get_property(GObject* object, guint prop_id, GValue* val, GParamSpec* pspec) {
	GpteGtkLineWidget* self = GPTE_GTK_LINE_WIDGET(object);
	switch (prop_id) {
		case PROP_LINE:
			g_value_set_object(val, gpte_gtk_line_widget_get_line(self));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
	}
}
static void gpte_gtk_line_widget_set_property(GObject* object, guint prop_id, const GValue* val, GParamSpec* pspec) {
	GpteGtkLineWidget* self = GPTE_GTK_LINE_WIDGET(object);
	switch (prop_id) {
		case PROP_LINE:
			gpte_gtk_line_widget_set_line(self, g_value_get_object(val));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
	}
}

static GtkSizeRequestMode gpte_gtk_line_widget_request_mode(GtkWidget*) {
	return GTK_SIZE_REQUEST_CONSTANT_SIZE;
}
static void gpte_gtk_line_widget_measure(GtkWidget* widget, GtkOrientation orientation, gint, gint* min, gint* nat, gint*, gint*) {
	GpteGtkLineWidgetPrivate* priv = gpte_gtk_line_widget_get_instance_private(GPTE_GTK_LINE_WIDGET(widget));
	if (!priv->cache) {
		*min = 0;
		*nat = 0;
		return;
	}
	graphene_rect_t bounds;
	gsk_render_node_get_bounds(priv->cache, &bounds);
	if (orientation == GTK_ORIENTATION_VERTICAL) {
		gint height = ceil(graphene_rect_get_height(&bounds));
		*min = height;
		*nat = height;
	} else {
		gint width = ceil(graphene_rect_get_width(&bounds));
		*min = width;
		*nat = width;
	}
}
static void gpte_gtk_line_widget_snapshot(GtkWidget* widget, GtkSnapshot* snapshot) {
	GpteGtkLineWidgetPrivate* priv = gpte_gtk_line_widget_get_instance_private(GPTE_GTK_LINE_WIDGET(widget));
	if (priv->cache)
		gtk_snapshot_append_node(snapshot, priv->cache);
}

static void gpte_gtk_line_widget_class_init(GpteGtkLineWidgetClass* class) {
	GObjectClass* object_class = G_OBJECT_CLASS(class);
	GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(class);

	object_class->dispose = gpte_gtk_line_widget_dispose;
	object_class->get_property = gpte_gtk_line_widget_get_property;
	object_class->set_property = gpte_gtk_line_widget_set_property;
	widget_class->get_request_mode = gpte_gtk_line_widget_request_mode;
	widget_class->measure = gpte_gtk_line_widget_measure;
	widget_class->snapshot = gpte_gtk_line_widget_snapshot;
	class->draw = NULL;

	obj_properties[PROP_LINE] = g_param_spec_object("line", NULL, NULL, GPTE_TYPE_LINE, G_PARAM_STATIC_STRINGS | G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);
	g_object_class_install_properties(object_class, N_PROPERTIES, obj_properties);

	gtk_widget_class_set_accessible_role (widget_class, GTK_ACCESSIBLE_ROLE_LABEL);
}
static void gpte_gtk_line_widget_init(GpteGtkLineWidget* self) {
	GpteGtkLineWidgetPrivate* priv = gpte_gtk_line_widget_get_instance_private(self);
	priv->line = NULL;
	priv->cache = NULL;
}

GpteLine* gpte_gtk_line_widget_get_line(GpteGtkLineWidget* self) {
	g_return_val_if_fail(GPTE_GTK_IS_LINE_WIDGET(self), NULL);
	GpteGtkLineWidgetPrivate* priv = gpte_gtk_line_widget_get_instance_private(self);
	return priv->line;
}

void gpte_gtk_line_widget_set_line(GpteGtkLineWidget* self, GpteLine* line) {
	g_return_if_fail(GPTE_GTK_IS_LINE_WIDGET(self));
	g_return_if_fail(!line || GPTE_IS_LINE(line));
	GpteGtkLineWidgetClass* class = GPTE_GTK_LINE_WIDGET_GET_CLASS(self);
	g_return_if_fail(class->draw != NULL);
	GpteGtkLineWidgetPrivate* priv = gpte_gtk_line_widget_get_instance_private(self);
	if (priv->line) {
		gsk_render_node_unref(priv->cache);
		g_object_unref(priv->line);
	}
	priv->line = line;
	if (priv->line) {
		g_object_ref(priv->line);

		const gchar* name = gpte_line_get_name(priv->line);
		gtk_accessible_update_property(GTK_ACCESSIBLE(self), GTK_ACCESSIBLE_PROPERTY_LABEL, name, -1);

		priv->cache = class->draw(self);
	} else {
		priv->cache = NULL;
		gtk_accessible_update_property(GTK_ACCESSIBLE(self), GTK_ACCESSIBLE_PROPERTY_LABEL, NULL, -1);
	}
	gtk_widget_queue_resize(GTK_WIDGET(self));
	g_object_notify_by_pspec(G_OBJECT(self), obj_properties[PROP_LINE]);
}
