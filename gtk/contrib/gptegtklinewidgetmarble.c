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

#include "gptegtklinewidgetmarble.h"
#include "gptegtklinewidgetutils-priv.h"

struct _GpteGtkLineWidgetMarble {
	GpteGtkLineWidget parent_instance;
};

G_DEFINE_TYPE (GpteGtkLineWidgetMarble, gpte_gtk_line_widget_marble, GPTE_GTK_TYPE_LINE_WIDGET)

static const gfloat gpte_gtk_line_widget_marble_safty_inset = 12.f;
static const gfloat gpte_gtk_line_widget_marble_padding_h = 5.f;
static const gfloat gpte_gtk_line_widget_marble_padding_v = 4.f;

static GskRenderNode* gpte_gtk_line_widget_marble_draw(GpteGtkLineWidget* widget) {
	GpteGtkLineWidgetMarble* self = GPTE_GTK_LINE_WIDGET_MARBLE(widget);

	GpteLine* line = gpte_gtk_line_widget_get_line(widget);
	GpteStyle* style = gpte_line_get_style(line);
	g_autoptr(GskRenderNode) quartzite = gpte_gtk_line_widget_utils_draw_quartzite(line);
	graphene_rect_t q_bounds;
	gsk_render_node_get_bounds(quartzite, &q_bounds);

	const gchar* label = gpte_line_get_label(line);
	if (!label)
		label = gpte_line_get_name(line);
	if (!label)
		label = "?"; // TODO: find something better
	PangoAttrList* attrs = pango_attr_list_new();
	pango_attr_list_insert(attrs, pango_attr_weight_new(PANGO_WEIGHT_BOLD));
	GList* list = pango_itemize(gtk_widget_get_pango_context(GTK_WIDGET(self)), label, 0, strlen(label), attrs, NULL);
	PangoGlyphString* glyphs = pango_glyph_string_new();
	pango_shape_item(list->data, label, -1, NULL, glyphs, PANGO_SHAPE_NONE);

	g_autoptr(GskRenderNode) text_node = gsk_text_node_new(((PangoItem*)list->data)->analysis.font, glyphs, &GPTE_GTK_COLOR_TO_GDK(&style->foreground), &GRAPHENE_POINT_INIT(0.f, 0.f));

	pango_glyph_string_free(glyphs);
	g_list_free_full(list, (GDestroyNotify)pango_item_free);
	pango_attr_list_unref(attrs);

	graphene_rect_t bg_base;
	gsk_render_node_get_bounds(text_node, &bg_base);

	g_autoptr(GskTransform) text_translate = gsk_transform_translate(NULL, &GRAPHENE_POINT_INIT(
		-graphene_rect_get_x(&bg_base) + gpte_gtk_line_widget_marble_padding_h + gpte_gtk_line_widget_marble_safty_inset,
		-graphene_rect_get_y(&bg_base) + gpte_gtk_line_widget_marble_padding_v
	));
	g_autoptr(GskRenderNode) transformed_text_node = gsk_transform_node_new(text_node, text_translate);

	//gsk_render_node_get_bounds(transformed_text_node, &bg_base);
	graphene_rect_inset(&bg_base, -gpte_gtk_line_widget_marble_padding_h, -gpte_gtk_line_widget_marble_padding_v);
	bg_base.size.width += gpte_gtk_line_widget_marble_safty_inset;
	bg_base.origin.x = 0.f;
	bg_base.origin.y = 0.f;

	g_autoptr(GskRenderNode) bg = gsk_color_node_new(&GPTE_GTK_COLOR_TO_GDK(&style->background), &bg_base);

	GskRoundedRect rect;
	gsk_rounded_rect_init_from_rect(&rect, &bg_base, 6.);
	g_autoptr(GskRenderNode) shape = gsk_rounded_clip_node_new(bg, &rect);

	g_autoptr(GskRenderNode) text_container = gsk_container_node_new((GskRenderNode*[]){shape, transformed_text_node}, 2);
	g_autoptr(GskTransform) container_translate = gsk_transform_translate(NULL, &GRAPHENE_POINT_INIT(
		graphene_rect_get_width(&q_bounds) - gpte_gtk_line_widget_marble_safty_inset,
		graphene_rect_get_height(&q_bounds) / 2.f - graphene_rect_get_height(&bg_base) / 2.f
	));
	g_autoptr(GskRenderNode) transformed_container = gsk_transform_node_new(text_container, container_translate);

	return gsk_container_node_new((GskRenderNode*[]){transformed_container, quartzite}, 2);
}

static void gpte_gtk_line_widget_marble_class_init(GpteGtkLineWidgetMarbleClass* class) {
	GPTE_GTK_LINE_WIDGET_CLASS(class)->draw = gpte_gtk_line_widget_marble_draw;
}
static void gpte_gtk_line_widget_marble_init(GpteGtkLineWidgetMarble*) {}

GtkWidget* gpte_gtk_line_widget_marble_new(GpteLine* line) {
	return g_object_new(GPTE_GTK_TYPE_LINE_WIDGET_MARBLE, "line", line, NULL);
}
