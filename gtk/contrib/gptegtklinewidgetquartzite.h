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

#ifndef __GPTEGTKLINEWIDGETQUARTZITE_H__
#define __GPTEGTKLINEWIDGETQUARTZITE_H__

#include <glib-object.h>
#include <gptegtklinewidget.h>

G_BEGIN_DECLS

/**
 * GpteGtkLineWidgetQuartzite:
 * The *Quartzite* style for [class@GpteGtk.LineWidget] uses the
 * information from [struct@Gpte.Style] to create a line widget.
 *
 *
 * The Quartzite widget is always a square, containing an icon matching
 * the [enum@Gpte.ProductCode] in the foreground color specified by the
 * style, while the background is either a (rounded) rectangle or a
 * circle, depending on [enum@Gpte.StyleShape]. Note that the
 * `background2` field of [struct@Gpte.Style] is currently not used,
 * instread the background is colored exclusively using the color
 * specified in `background`.
 *
 * ![Screenshot showing various lines using the Quartzite style](GpteGtkLineWidgetQuartzite.png)
 */

#define GPTE_GTK_TYPE_LINE_WIDGET_QUARTZITE (gpte_gtk_line_widget_quartzite_get_type())
G_DECLARE_FINAL_TYPE (GpteGtkLineWidgetQuartzite, gpte_gtk_line_widget_quartzite, GPTE_GTK, LINE_WIDGET_QUARTZITE, GpteGtkLineWidget)

/**
 * gpte_gtk_line_widget_quartzite_new:
 * @line: (transfer none) (nullable): the line that should be shown
 *
 * Creats a new [class@GpteGtk.LineWidget] using the *Quartzite* style.
 *
 * Returns: (transfer floating): the line widget
 */
GtkWidget* gpte_gtk_line_widget_quartzite_new(GpteLine* line);

G_END_DECLS

#endif // __GPTEGTKLINEWIDGETQUARTZITE_H__
