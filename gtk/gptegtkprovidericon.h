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

#ifndef __GPTEGTKPROVIDERICON_H__
#define __GPTEGTKPROVIDERICON_H__

#include <glib-object.h>
#include <gptegtkbin.h>

#include <gpteprovider.h>

G_BEGIN_DECLS

/**
 * GpteGtkProviderIcon:
 * Widget displaying the icon of a provider.
 *
 *
 * After this class was initialized, the icons are also available as
 * [struct@Gio.Resource] at
 * `/arpa/sp1rit/gpte/gtk/networks/<`[method@Gpte.Provider.get_id]`>.svg`.
 * Note that not every provider has an icon available, but you may use
 * `/arpa/sp1rit/gpte/gtk/networks/placeholder.svg` for those providers
 * instead. This matches the behaviour of [class@GpteGtk.ProviderIcon].
 */

#define GPTE_GTK_TYPE_PROVIDER_ICON (gpte_gtk_provider_icon_get_type())
G_DECLARE_FINAL_TYPE (GpteGtkProviderIcon, gpte_gtk_provider_icon, GPTE_GTK, PROVIDER_ICON, GpteGtkBin)

/**
 * gpte_gtk_provider_icon_get_provider:
 * @self: the provider icon
 *
 * Gets the currently active provider.
 *
 * Returns: (transfer none) (nullable): the provider
 */
GpteProvider* gpte_gtk_provider_icon_get_provider(GpteGtkProviderIcon* self);

/**
 * gpte_gtk_provider_icon_set_provider:
 * @self: the provider icon
 * @provider: (transfer none) (nullable): the new provider
 *
 * Updates to provider icon to @provider.
 */
void gpte_gtk_provider_icon_set_provider(GpteGtkProviderIcon* self, GpteProvider* provider);

G_END_DECLS

#endif // __GPTEGTKPROVIDERICON_H__
