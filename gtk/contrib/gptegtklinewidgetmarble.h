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

#ifndef __GPTEGTKLINEWIDGETMARBLE_H__
#define __GPTEGTKLINEWIDGETMARBLE_H__

#include <glib-object.h>
#include <gptegtklinewidget.h>

G_BEGIN_DECLS

/**
 * GpteGtkLineWidgetMarble:
 * The *Mable* style for [class@GpteGtk.LineWidget] uses extends
 * [class@GpteGtk.LineWidgetQuartzite] by adding the line label
 * next to the icon box from Quartzite.
 *
 *
 * Unlike [class@GpteGtk.LineWidgetQuartzite], this widget is always
 * wider than tall, as it additionally contains the label of the line
 * as text. The text shares the same foreground/background color as
 * the icon from the *Quartzite* style.
 *
 * ![Screenshot showing various lines using the Marble style](GpteGtkLineWidgetMarble.png)
 */

#define GPTE_GTK_TYPE_LINE_WIDGET_MARBLE (gpte_gtk_line_widget_marble_get_type())
G_DECLARE_FINAL_TYPE (GpteGtkLineWidgetMarble, gpte_gtk_line_widget_marble, GPTE_GTK, LINE_WIDGET_MARBLE, GpteGtkLineWidget)

/**
 * gpte_gtk_line_widget_marble_new:
 * @line: (transfer none) (nullable): the line that should be shown
 *
 * Creats a new [class@GpteGtk.LineWidget] using the *Marble* style.
 *
 * Returns: (transfer floating): the line widget
 */
GtkWidget* gpte_gtk_line_widget_marble_new(GpteLine* line);

G_END_DECLS

#endif // __GPTEGTKLINEWIDGETMARBLE_H__
