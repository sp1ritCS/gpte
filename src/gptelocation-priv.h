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

#ifndef __GPTELOCATION_PRIV_H__
#define __GPTELOCATION_PRIV_H__

#include <gptelocation.h>
#include <gptejvm-priv.h>

G_BEGIN_DECLS

jobject gpte_locations_to_java(GpteJvm* vm, GpteLocations locations);

G_END_DECLS

#endif // __GPTELOCATION_PRIV_H__
