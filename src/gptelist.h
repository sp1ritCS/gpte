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

#ifndef __GPTELIST_H__
#define __GPTELIST_H__

#include <glib-object.h>
#include <gio/gio.h>
#include <gptejavaobject.h>

G_BEGIN_DECLS

/**
 * GpteList:
 * Object wrapping
 * [`java.util.List`](https://docs.oracle.com/en/java/javase/18/docs/api/java.base/java/util/List.html)
 * and exposing it as [iface@Gio.ListModel].
 */

#define GPTE_TYPE_LIST (gpte_list_get_type())
G_DECLARE_FINAL_TYPE (GpteList, gpte_list, GPTE, LIST, GpteJavaObject)

G_END_DECLS

#endif // __GPTELIST_H__
