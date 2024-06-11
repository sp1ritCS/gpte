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

#ifndef __GPTELINE_H__
#define __GPTELINE_H__

#include <glib-object.h>
#include <gptejavaobject.h>

#include <gpteproducts.h>
#include <gptestyle.h>

G_BEGIN_DECLS

/**
 * GpteLineAttrs:
 * @GPTE_LINE_ATTR_CIRCLE_CLOCKWISE: clockwise circle line
 * @GPTE_LINE_ATTR_CIRCLE_ANTICLOCKWISE: anitclockwise circle line
 * @GPTE_LINE_ATTR_SERVICE_REPLACEMENT: line is replacement
 * @GPTE_LINE_ATTR_LINE_AIRPORT: line heads to/from the airport
 * @GPTE_LINE_ATTR_WHEEL_CHAIR_ACCESS: line is wheelchair accesible
 * @GPTE_LINE_ATTR_BICYCLE_CARRIAGE: line has a bicycle carriage
 *
 * Special attributes of a line.
 *
 * Received from [method@Gpte.Line.get_attrs].
 */

#define GPTE_TYPE_LINE_ATTRS (gpte_line_attrs_get_type())
GType gpte_line_attrs_get_type(void);

typedef enum {
	GPTE_LINE_ATTR_CIRCLE_CLOCKWISE = 1 << 0,
	GPTE_LINE_ATTR_CIRCLE_ANTICLOCKWISE = 1 << 1,
	GPTE_LINE_ATTR_SERVICE_REPLACEMENT = 1 << 2,
	GPTE_LINE_ATTR_LINE_AIRPORT = 1 << 3,
	GPTE_LINE_ATTR_WHEEL_CHAIR_ACCESS = 1 << 4,
	GPTE_LINE_ATTR_BICYCLE_CARRIAGE = 1 << 5
} GpteLineAttrs;


/**
 * GpteLine:
 * Models a particular line of a transit network.
 */

#define GPTE_TYPE_LINE (gpte_line_get_type())
G_DECLARE_FINAL_TYPE (GpteLine, gpte_line, GPTE, LINE, GpteJavaObject)

/**
 * gpte_line_get_id:
 * @self: the line
 *
 * Gets the identifier of a line.
 *
 * Returns: (transfer none) (nullable): line identifier
 */
const gchar* gpte_line_get_id(GpteLine* self);

/**
 * gpte_line_get_network:
 * @self: the line
 *
 * Gets the network the line is part of.
 * Returns: (transfer none) (nullable): network identifier
 */
const gchar* gpte_line_get_network(GpteLine* self);

/**
 * gpte_line_get_product:
 * @self: the line
 *
 * Gets the product in-use by the line.
 * Returns: the product
 */
GpteProductCode gpte_line_get_product(GpteLine* self);

/**
 * gpte_line_get_label:
 * @self: the line
 *
 * Gets the human readable label of the line.
 *
 * Returns: (transfer none) (nullable): the label
 */
const gchar* gpte_line_get_label(GpteLine* self);

/**
 * gpte_line_get_name:
 * @self: the line
 *
 * Gets the name of the line.
 *
 * In most cases, is probably what you'd want to try
 * [method@GpteLine.get_label] first.
 *
 * Returns: (transfer none) (nullable): the name
 */
const gchar* gpte_line_get_name(GpteLine* self);

/**
 * gpte_line_get_style:
 * @self: the line
 *
 * Gets the visual style parameters for displaying the line.
 *
 * Returns: (transfer none) (nullable): visual style
 */
GpteStyle* gpte_line_get_style(GpteLine* self);

/**
 * gpte_line_get_attrs:
 * @self: the line
 *
 * Gets the attributes set of this line.
 *
 * Returns: attributes bitset
 */
GpteLineAttrs gpte_line_get_attrs(GpteLine* self);

/**
 * gpte_line_get_message:
 * @self: the line
 *
 * Gets the custom free-text message assigned to this line, if such a
 * message exists.
 *
 * Returns: (transfer none) (nullable): the message or %NULL
 */
const gchar* gpte_line_get_message(GpteLine* self);

G_END_DECLS

#endif // __GPTELINE_H__
