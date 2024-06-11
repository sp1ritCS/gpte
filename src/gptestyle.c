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

#include "gptestyle.h"
#include "gptestyle-priv.h"

G_DEFINE_ENUM_TYPE(GpteStyleShape, gpte_style_shape,
	G_DEFINE_ENUM_VALUE(GPTE_STYLE_SHAPE_RECT, "rect"),
	G_DEFINE_ENUM_VALUE(GPTE_STYLE_SHAPE_ROUNDED, "rounded"),
	G_DEFINE_ENUM_VALUE(GPTE_STYLE_SHAPE_CIRCLE, "circle")
)


G_DEFINE_BOXED_TYPE(GpteColor, gpte_color, gpte_color_copy, gpte_color_free)

GpteColor* gpte_color_copy(const GpteColor* self) {
	return g_memdup2(self, sizeof(GpteColor));
}
void gpte_color_free(GpteColor* self) {
	g_free(self);
}

G_DEFINE_BOXED_TYPE(GpteStyle, gpte_style, gpte_style_copy, gpte_style_free)

GpteStyle* gpte_style_copy(const GpteStyle* self) {
	return g_memdup2(self, sizeof(GpteStyle));
}
void gpte_style_free(GpteStyle* self) {
	g_free(self);
}

static GpteStyleShape gpte_style_shape_from_java(GpteJvm* vm, jobject shape) {
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 7);
	jclass type_class = (*env)->FindClass(env, "de/schildbach/pte/dto/Style$Shape");

	if ((*env)->IsSameObject(env, shape, (*env)->GetStaticObjectField(
		env, type_class,
		(*env)->GetStaticFieldID(env, type_class , "RECT", "Lde/schildbach/pte/dto/Style$Shape;")
	)))
		return GPTE_STYLE_SHAPE_RECT;
	else if ((*env)->IsSameObject(env, shape, (*env)->GetStaticObjectField(
		env, type_class,
		(*env)->GetStaticFieldID(env, type_class , "ROUNDED", "Lde/schildbach/pte/dto/Style$Shape;")
	)))
		return GPTE_STYLE_SHAPE_ROUNDED;
	else if ((*env)->IsSameObject(env, shape, (*env)->GetStaticObjectField(
		env, type_class,
		(*env)->GetStaticFieldID(env, type_class , "CIRCLE", "Lde/schildbach/pte/dto/Style$Shape;")
	)))
		return GPTE_STYLE_SHAPE_CIRCLE;
	else {
		g_critical("Unknown shape type: %p", shape);
		return 0;
	}
}

typedef union {
	jint jcolor;
	guint32 color;
} GpteStyleJcolorUnsigned;
static GpteColor gpte_style_color_from_jcolor(jint color) {
	GpteStyleJcolorUnsigned c;
	c.jcolor = color;
	// TODO: investigate if it's worth to swap the layout of GpteColor on LE systems to save a bswap
	return (GpteColor){
		.alpha = c.color >> 24,
		.red   = c.color >> 16,
		.green = c.color >>  8,
		.blue  = c.color >>  0
	};
}

GpteStyle* gpte_style_from_java(GpteJvm* vm, jobject jstyle) {
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 11);

	jclass class = (*env)->FindClass(env, "de/schildbach/pte/dto/Style");
	jfieldID shape_id = (*env)->GetFieldID(env, class, "shape", "Lde/schildbach/pte/dto/Style$Shape;");
	jfieldID bg_id = (*env)->GetFieldID(env, class, "backgroundColor", "I");
	jfieldID bg2_id = (*env)->GetFieldID(env, class, "backgroundColor2", "I");
	jfieldID fg_id = (*env)->GetFieldID(env, class, "foregroundColor", "I");
	jfieldID border_id = (*env)->GetFieldID(env, class, "borderColor", "I");

	GpteStyle* self = g_new0(GpteStyle, 1);
	self->shape = gpte_style_shape_from_java(vm, (*env)->GetObjectField(env, jstyle, shape_id));
	self->background = gpte_style_color_from_jcolor((*env)->GetIntField(env, jstyle, bg_id));
	self->background2 = gpte_style_color_from_jcolor((*env)->GetIntField(env, jstyle, bg2_id));
	self->foreground = gpte_style_color_from_jcolor((*env)->GetIntField(env, jstyle, fg_id));
	self->border = gpte_style_color_from_jcolor((*env)->GetIntField(env, jstyle, border_id));
	return self;
}
