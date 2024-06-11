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

#include "gptegtklinewidgetutils-priv.h"

const gchar* gpte_gtk_line_widget_utils_get_product_path(GpteProductCode product) {
	switch (product) {
		case GPTE_PRODUCT_CODE_HIGH_SPEED_TRAIN:
			return "M4,14.5 C4,16.43,5.57,19,7.5,19 L5,20.5 L5,21 L19,21 L19,20.5 L16.5,19 C18.43,19,20,16.43,20,14.5 L20,12 C20,8.5,16.42,1,12,1 C7.58,1,4,8.5,4,12 Z M12,17 C10.9,17,10,16.1,10,15 C10,13.9,10.9,13,12,13 C13.1,13,14,13.9,14,15 C14,16.1,13.1,17,12,17 Z M18,10 L6,10 C7.38717,5.50455,8.44708,5.25665,9,5 L15,5 C16.4168,5.39432,17.2127,7.68416,18,10 Z";
		case GPTE_PRODUCT_CODE_REGIONAL_TRAIN:
			return "M4,15.5C4,17.43 5.57,19 7.5,19L6,20.5v0.5h12v-0.5L16.5,19c1.93,0 3.5,-1.57 3.5,-3.5L20,5c0,-3.5 -3.58,-4 -8,-4s-8,0.5 -8,4v10.5zM12,17c-1.1,0 -2,-0.9 -2,-2s0.9,-2 2,-2 2,0.9 2,2 -0.9,2 -2,2zM18,10L6,10L6,5h12v5z";
		case GPTE_PRODUCT_CODE_SUBURBAN_TRAIN:
			return "M12,2c-4.42,0 -8,0.5 -8,4v9.5C4,17.43 5.57,19 7.5,19L6,20.5v0.5h12v-0.5L16.5,19c1.93,0 3.5,-1.57 3.5,-3.5L20,6c0,-3.5 -3.58,-4 -8,-4zM7.5,17c-0.83,0 -1.5,-0.67 -1.5,-1.5S6.67,14 7.5,14s1.5,0.67 1.5,1.5S8.33,17 7.5,17zM11,11L6,11L6,6h5v5zM16.5,17c-0.83,0 -1.5,-0.67 -1.5,-1.5s0.67,-1.5 1.5,-1.5 1.5,0.67 1.5,1.5 -0.67,1.5 -1.5,1.5zM18,11h-5L13,6h5v5z";
		case GPTE_PRODUCT_CODE_SUBWAY:
			return "M12,4.04941 C7.58,4.04941,4,4.49548,4,7.61796 L4,16.0933 C4,17.8151,5.57,19.2158,7.5,19.2158 L6,20.5539 L6,21 L18,21 L18,20.5539 L16.5,19.2157 C18.43,19.2157,20,17.815,20,16.0932 L20,7.61796 C20,4.49548,16.42,4.04941,12,4.04941 Z M7.5,17.4315 C6.67,17.4315,6,16.8337,6,16.0933 C6,15.3528,6.67,14.7551,7.5,14.7551 C8.33,14.7551,9,15.3528,9,16.0933 C9,16.8338,8.33,17.4315,7.5,17.4315 Z M11,12.0786 L6,12.0786 L6,7.61792 L11,7.61792 L11,12.0786 Z M16.5,17.4314 C15.67,17.4314,15,16.8337,15,16.0932 C15,15.3527,15.67,14.755,16.5,14.755 C17.33,14.755,18,15.3527,18,16.0932 C18,16.8337,17.33,17.4314,16.5,17.4314 Z M18,12.0786 L13,12.0786 L13,7.61792 L18,7.61792 L18,12.0786 Z";
		case GPTE_PRODUCT_CODE_TRAM:
			return "M19,16.94 L19,8.5 C19,5.71,16.39,5.1,12.99,5.01 L13.75,3.5 L17,3.5 L17,2 L7,2 L7,3.5 L11.75,3.5 L10.99,5.02 C7.86,5.11,5,5.73,5,8.5 L5,16.94 C5,18.39,6.19,19.6,7.59,19.91 L6,21.5 L6,22 L8.23,22 L10.23,20 L14,20 L16,22 L18,22 L18,21.5 L16.5,20 L16.42,20 C18.11,20,19,18.63,19,16.94 Z M12,18.5 C11.17,18.5,10.5,17.83,10.5,17 S11.17,15.5,12,15.5 S13.5,16.17,13.5,17 S12.83,18.5,12,18.5 Z M17,14 L7,14 L7,9 L17,9 L17,14 Z";
		case GPTE_PRODUCT_CODE_BUS:
			return "M4,16c0,0.88 0.39,1.67 1,2.22L5,20c0,0.55 0.45,1 1,1h1c0.55,0 1,-0.45 1,-1v-1h8v1c0,0.55 0.45,1 1,1h1c0.55,0 1,-0.45 1,-1v-1.78c0.61,-0.55 1,-1.34 1,-2.22L20,6c0,-3.5 -3.58,-4 -8,-4s-8,0.5 -8,4v10zM7.5,17c-0.83,0 -1.5,-0.67 -1.5,-1.5S6.67,14 7.5,14s1.5,0.67 1.5,1.5S8.33,17 7.5,17zM16.5,17c-0.83,0 -1.5,-0.67 -1.5,-1.5s0.67,-1.5 1.5,-1.5 1.5,0.67 1.5,1.5 -0.67,1.5 -1.5,1.5zM18,11L6,11L6,6h12v5z";
		case GPTE_PRODUCT_CODE_FERRY:
			return "M20,21c-1.39,0 -2.78,-0.47 -4,-1.32 -2.44,1.71 -5.56,1.71 -8,0C6.78,20.53 5.39,21 4,21H2v2h2c1.38,0 2.74,-0.35 4,-0.99 2.52,1.29 5.48,1.29 8,0 1.26,0.65 2.62,0.99 4,0.99h2v-2h-2zM3.95,19H4c1.6,0 3.02,-0.88 4,-2 0.98,1.12 2.4,2 4,2s3.02,-0.88 4,-2c0.98,1.12 2.4,2 4,2h0.05l1.89,-6.68c0.08,-0.26 0.06,-0.54 -0.06,-0.78s-0.34,-0.42 -0.6,-0.5L20,10.62V6c0,-1.1 -0.9,-2 -2,-2h-3V1H9v3H6c-1.1,0 -2,0.9 -2,2v4.62l-1.29,0.42c-0.26,0.08 -0.48,0.26 -0.6,0.5s-0.15,0.52 -0.06,0.78L3.95,19zM6,6h12v3.97L12,8 6,9.97V6z";
		case GPTE_PRODUCT_CODE_CABLECAR:
			return "M22.6939,2.58158 L13.616,4.73637 L13.616,3.81344 L10.6626,4.50564 L10.6626,5.4055 L1.32944,7.59234 L1.30402,8.30526 L11.4009,5.98233 L11.4009,9.60485 L5.86329,9.60485 L4.38659,11.0815 L4.38659,20.68 L5.12494,21.4183 L19.1535,21.4183 L19.8918,20.68 L19.8918,11.0815 L18.4151,9.6048 L12.8775,9.6048 L12.8775,5.63618 L22.6957,3.26512 Z M5.125,11.8199 L8.81674,11.8199 L8.81674,16.4346 L5.125,16.4346 Z M9.55509,11.8199 L14.7236,11.8199 L14.7236,16.4346 L9.55517,16.4346 Z M15.4619,11.8199 L19.1536,11.8199 L19.1536,16.4346 L15.4619,16.4346 Z";
		case GPTE_PRODUCT_CODE_ON_DEMAND:
			return "M18.92,6.01C18.72,5.42 18.16,5 17.5,5L15,5L15,3L9,3v2L6.5,5c-0.66,0 -1.21,0.42 -1.42,1.01L3,12v8c0,0.55 0.45,1 1,1h1c0.55,0 1,-0.45 1,-1v-1h12v1c0,0.55 0.45,1 1,1h1c0.55,0 1,-0.45 1,-1v-8l-2.08,-5.99zM6.5,16c-0.83,0 -1.5,-0.67 -1.5,-1.5S5.67,13 6.5,13s1.5,0.67 1.5,1.5S7.33,16 6.5,16zM17.5,16c-0.83,0 -1.5,-0.67 -1.5,-1.5s0.67,-1.5 1.5,-1.5 1.5,0.67 1.5,1.5 -0.67,1.5 -1.5,1.5zM5,11l1.5,-4.5h11L19,11L5,11z";
		case GPTE_PRODUCT_CODE_NULL:
		default:
			return "M4,16c0,0.88 0.39,1.67 1,2.22L5,20c0,0.55 0.45,1 1,1h1c0.55,0 1,-0.45 1,-1v-1h8v1c0,0.55 0.45,1 1,1h1c0.55,0 1,-0.45 1,-1v-1.78c0.61,-0.55 1,-1.34 1,-2.22L20,6c0,-3.5 -3.58,-4 -8,-4s-8,0.5 -8,4v10zM7.5,17c-0.83,0 -1.5,-0.67 -1.5,-1.5S6.67,14 7.5,14s1.5,0.67 1.5,1.5S8.33,17 7.5,17zM16.5,17c-0.83,0 -1.5,-0.67 -1.5,-1.5s0.67,-1.5 1.5,-1.5 1.5,0.67 1.5,1.5 -0.67,1.5 -1.5,1.5zM18,11L6,11L6,6h12v5z";
		// TODO: better fallback?
	}
}

GskRenderNode* gpte_gtk_line_widget_utils_get_product_icon(GpteProductCode code, const GpteColor* fg) {
	g_autoptr(GskRenderNode) color = gsk_color_node_new(&GPTE_GTK_COLOR_TO_GDK(fg), &GRAPHENE_RECT_INIT(0, 0, 24, 24));
	g_autoptr(GskPath) path = gsk_path_parse(gpte_gtk_line_widget_utils_get_product_path(code));
	return gsk_fill_node_new(color, path, GSK_FILL_RULE_WINDING);
}

static const guint quartzite_icon_size = 32;
GskRenderNode* gpte_gtk_line_widget_utils_draw_quartzite(GpteLine* line) {
	GpteStyle* style = gpte_line_get_style(line);
	g_autoptr(GskRenderNode) bg = gsk_color_node_new(&GPTE_GTK_COLOR_TO_GDK(&style->background), &GRAPHENE_RECT_INIT(0, 0, quartzite_icon_size, quartzite_icon_size));

	graphene_size_t size;
	switch (style->shape) {
		case GPTE_STYLE_SHAPE_RECT:
			graphene_size_init(&size, 0.f, 0.f);
			break;
		case GPTE_STYLE_SHAPE_ROUNDED:
			graphene_size_init(&size, 6.f, 6.f);
			break;
		case GPTE_STYLE_SHAPE_CIRCLE:
			graphene_size_init(&size, quartzite_icon_size / 2.f, quartzite_icon_size / 2.f);
			break;
	}
	GskRoundedRect rect = GSK_ROUNDED_RECT_INIT(0, 0, quartzite_icon_size, quartzite_icon_size);
	for (guint i = 0; i < 4; i++)
		rect.corner[i] = size;
	g_autoptr(GskRenderNode) shape = gsk_rounded_clip_node_new(bg, &rect);

	g_autoptr(GskRenderNode) icon = gpte_gtk_line_widget_utils_get_product_icon(gpte_line_get_product(line), &style->foreground);
	graphene_rect_t icon_bounds;
	gsk_render_node_get_bounds(icon, &icon_bounds);
	g_autoptr(GskTransform) icon_translate = gsk_transform_translate(NULL, &GRAPHENE_POINT_INIT(
		-graphene_rect_get_x(&icon_bounds) + quartzite_icon_size / 2.f - graphene_rect_get_width(&icon_bounds) / 2.f,
		-graphene_rect_get_y(&icon_bounds) + quartzite_icon_size / 2.f - graphene_rect_get_height(&icon_bounds) / 2.f
	));
	g_autoptr(GskRenderNode) transformed_icon = gsk_transform_node_new(icon, icon_translate);

	return gsk_container_node_new((GskRenderNode*[]){shape, transformed_icon}, 2);
}
