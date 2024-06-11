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

#ifndef __GPTESTYLE_H__
#define __GPTESTYLE_H__

#include <glib-object.h>

G_BEGIN_DECLS

/**
 * GpteStyleShape:
 * @GPTE_STYLE_SHAPE_RECT: Rectangle line style
 * @GPTE_STYLE_SHAPE_ROUNDED: Rounded rectangle line style
 * @GPTE_STYLE_SHAPE_CIRCLE: Circular line style
 *
 * The shape of the line style.
 */

#define GPTE_TYPE_STYLE_SHAPE (gpte_style_shape_get_type())
GType gpte_style_shape_get_type(void);

typedef enum {
	GPTE_STYLE_SHAPE_RECT,
	GPTE_STYLE_SHAPE_ROUNDED,
	GPTE_STYLE_SHAPE_CIRCLE
} GpteStyleShape;

/**
 * GpteColor:
 * Container for a singular 32bit RGBA color.
 */

#define GPTE_TYPE_COLOR (gpte_color_get_type())
GType gpte_color_get_type(void);

typedef struct {
	guchar alpha;
	guchar red;
	guchar green;
	guchar blue;
} GpteColor;

/**
 * gpte_color_copy:
 * @self: the instance to duplicate
 *
 * Duplicates the [struct@Gpte.Color] instance.
 *
 * Returns: (transfer full): the copied instance
 */
GpteColor* gpte_color_copy(const GpteColor* self);

/**
 * gpte_color_free:
 * @self: the color instance
 *
 * Frees the [struct@Gpte.Color] instance from memory.
 *
 * After this function was called, you may not access the @self anymore.
 */
void gpte_color_free(GpteColor* self);

/**
 * GpteStyle:
 * This structure describes the visual style of a [class@Gpte.Line].
 */

#define GPTE_TYPE_STYLE (gpte_style_get_type())
GType gpte_style_get_type(void);

typedef struct {
	GpteStyleShape shape;
	GpteColor background, background2;
	GpteColor foreground;
	GpteColor border;
} GpteStyle;

/***
 * gpte_style_copy:
 * @self: the style instance to copy
 *
 * Duplicates an instance of [struct@Gpte.Style].
 *
 * Returns: (transfer full): the copied instance
 */
GpteStyle* gpte_style_copy(const GpteStyle* self);

/**
 * gpte_style_free:
 * @self: the style instance
 *
 * Frees the @self from memory.
 *
 * After this function was called, you may not access the structure
 * anymore.
 */
void gpte_style_free(GpteStyle* self);

G_DEFINE_AUTOPTR_CLEANUP_FUNC(GpteStyle, gpte_style_free)

G_END_DECLS

#endif // __GPTESTYLE_H__
