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

#ifndef __GPTEGTKLINEWIDGETUTILS_PRIV_H__
#define __GPTEGTKLINEWIDGETUTILS_PRIV_H__

#include <gptegtklinewidget.h>

G_BEGIN_DECLS

#define GPTE_GTK_COLOR_TO_GDK(color) \
	(GdkRGBA){ \
		.red = (color)->red / 255., \
		.green = (color)->green / 255., \
		.blue = (color)->blue / 255., \
		.alpha = (color)->alpha / 255., \
	}

const gchar* gpte_gtk_line_widget_utils_get_product_path(GpteProductCode product);
GskRenderNode* gpte_gtk_line_widget_utils_get_product_icon(GpteProductCode code, const GpteColor* fg);
GskRenderNode* gpte_gtk_line_widget_utils_draw_quartzite(GpteLine* line);

G_END_DECLS

#endif // __GPTEGTKLINEWIDGETUTILS_PRIV_H__
