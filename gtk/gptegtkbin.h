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

#ifndef __GPTEGTKBIN_H__
#define __GPTEGTKBIN_H__

#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

/**
 * GpteGtkBin:
 * Private widget that holds a single child widget.
 *
 *
 * This is for internal use only, use
 * [`Adw.Bin`](https://gnome.pages.gitlab.gnome.org/libadwaita/doc/1-latest/class.Bin.html)
 * if [libadwaita](https://gnome.pages.gitlab.gnome.org/libadwaita/) is
 * available or build your own.
 */

#define GPTE_GTK_TYPE_BIN (gpte_gtk_bin_get_type())
G_DECLARE_DERIVABLE_TYPE (GpteGtkBin, gpte_gtk_bin, GPTE_GTK, BIN, GtkWidget)

struct _GpteGtkBinClass {
	GtkWidgetClass parent_class;
};

G_END_DECLS

#endif // __GPTEGTKBIN_PRIV_H__
