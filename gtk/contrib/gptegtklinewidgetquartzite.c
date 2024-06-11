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

#include "gptegtklinewidgetquartzite.h"
#include "gptegtklinewidgetutils-priv.h"

struct _GpteGtkLineWidgetQuartzite {
	GpteGtkLineWidget parent_instance;
};

G_DEFINE_TYPE (GpteGtkLineWidgetQuartzite, gpte_gtk_line_widget_quartzite, GPTE_GTK_TYPE_LINE_WIDGET)

static GskRenderNode* gpte_gtk_line_widget_quartzite_draw(GpteGtkLineWidget* widget) {
	return gpte_gtk_line_widget_utils_draw_quartzite(gpte_gtk_line_widget_get_line(widget));
}

static void gpte_gtk_line_widget_quartzite_class_init(GpteGtkLineWidgetQuartziteClass* class) {
	GPTE_GTK_LINE_WIDGET_CLASS(class)->draw = gpte_gtk_line_widget_quartzite_draw;
}

static void gpte_gtk_line_widget_quartzite_init(GpteGtkLineWidgetQuartzite*) {}

GtkWidget* gpte_gtk_line_widget_quartzite_new(GpteLine* line) {
	return g_object_new(GPTE_GTK_TYPE_LINE_WIDGET_QUARTZITE, "line", line, NULL);
}
