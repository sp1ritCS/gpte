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

#ifndef __GPTEGTKLINEWIDGET_H__
#define __GPTEGTKLINEWIDGET_H__

#include <glib-object.h>
#include <gtk/gtk.h>

#include <gpteline.h>

G_BEGIN_DECLS

/**
 * GpteGtkLineWidget:
 * Abstract base class for all line widgets.
 *
 *
 * Line widgets show a unique marker based on information from
 * [class@Gpte.Line].
 */

#define GPTE_GTK_TYPE_LINE_WIDGET (gpte_gtk_line_widget_get_type())
G_DECLARE_DERIVABLE_TYPE (GpteGtkLineWidget, gpte_gtk_line_widget, GPTE_GTK, LINE_WIDGET, GtkWidget)

struct _GpteGtkLineWidgetClass {
	GtkWidgetClass parent_class;
	GskRenderNode*(*draw)(GpteGtkLineWidget* self);
};

/**
 * gpte_gtk_line_widget_get_line:
 * @self: the line widget
 *
 * Gets the current line displayed by this widget.
 *
 * Returns: (transfer none) (nullable): current line
 */
GpteLine* gpte_gtk_line_widget_get_line(GpteGtkLineWidget* self);

/**
 * gpte_gtk_line_widget_set_line:
 * @self: the line widget
 * @line: (transfer none) (nullable): the new line to display
 *
 * Updates the line that this widget is supposed to display.
 */
void gpte_gtk_line_widget_set_line(GpteGtkLineWidget* self, GpteLine* line);

G_END_DECLS

#endif // __GPTEGTKLINEWIDGET_H__
