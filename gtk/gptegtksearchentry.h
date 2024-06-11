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

#ifndef __GPTEGTKSEARCHENTRY_H__
#define __GPTEGTKSEARCHENTRY_H__

#include <glib-object.h>
#include <gptegtkbin.h>

#include <gpteprovider.h>

G_BEGIN_DECLS

/**
 * GpteGtkSearchEntry:
 * This widget provides a text field with autocompletion for location
 * entry.
 *
 *
 * In order to get notified about a selected location, connect to
 * [signal@GpteGtk.SearchEntry::location-entered].
 *
 * ![Screenshot showing the search entry with a set of suggested locations below it](GpteGtkSearchEntry.png)
 *
 * Note: as this is using [class@Gtk.EntryCompletion] (deprecated since
 * 4.10), the behaviour of this widget might change in future versions
 * (especially once GTK 5 is released). Ideally this API should be
 * flexible enough to cope with whatever GTK plans to provide as
 * replacement.
 */

#define GPTE_GTK_TYPE_SEARCH_ENTRY (gpte_gtk_search_entry_get_type())
G_DECLARE_FINAL_TYPE (GpteGtkSearchEntry, gpte_gtk_search_entry, GPTE_GTK, SEARCH_ENTRY, GpteGtkBin)

/**
 * gpte_gtk_search_entry_new:
 * @provider: (transfer none) (nullable): the provider used to lookup locations
 *
 * Creats a new search entry widget using @provider.
 * Returns: (transfer floating): search entry widget
 */
GtkWidget* gpte_gtk_search_entry_new(GpteProvider* provider);

/**
 * gpte_gtk_search_entry_get_provider:
 * @self: the search entry widget
 *
 * Gets the provider currently in use to lookup locations.
 *
 * Returns: (transfer none) (nullable): currently in-use location
 */
GpteProvider* gpte_gtk_search_entry_get_provider(GpteGtkSearchEntry* self);

/**
 * gpte_gtk_search_entry_set_provider
 * @self: the search entry widget
 * @provider: (transfer none) (nullable): new provider to use
 *
 * Updates the provider that will be used to lookup locations.
 *
 * If @provider is %NULL, no location lookups are performed.
 */
void gpte_gtk_search_entry_set_provider(GpteGtkSearchEntry* self, GpteProvider* provider);

/**
 * gpte_gtk_search_entry_get_entry:
 * @self: the search entry widget
 *
 * Gets the [iface@Gtk.Editable] in use by @self.
 *
 * This is useful for [method@Gtk.SearchBar.connect_entry].
 *
 * Returns: (transfer none): the editable in use by @self
 */
GtkEditable* gpte_gtk_search_entry_get_entry(GpteGtkSearchEntry* self);

/**
 * gpte_gtk_search_entry_get_max_suggestions:
 * @self: the search entry widget
 *
 * Gets the maximal amount of location suggestions to lookup.
 *
 * Returns: maximal amount of suggestions
 */
gint gpte_gtk_search_entry_get_max_suggestions(GpteGtkSearchEntry* self);

/**
 * gpte_gtk_search_entry_set_max_suggestions:
 * @self: the search entry widget
 * @max_suggestions: the maximal amout of suggestions
 *
 * Sets the maximal amount of location suggestions to lookup.
 *
 * Set to `0` to use the provider default.
 */
void gpte_gtk_search_entry_set_max_suggestions(GpteGtkSearchEntry* self, gint max_suggestions);

/**
 * gpte_gtk_search_entry_get_show_clear:
 * @self: the search entry widget
 *
 * Gets if this widget shows a clear button.
 *
 * Returns: %TRUE if it shows a clear button, %FALSE if it doesn't
 */
gboolean gpte_gtk_search_entry_get_show_clear(GpteGtkSearchEntry* self);

/**
 * gpte_gtk_search_entry_set_show_clear:
 * @self: the search entry widget
 * @show_clear: if @self ought to show a clear button
 *
 * Sets if @self should show a clear button besides it.
 */
void gpte_gtk_search_entry_set_show_clear(GpteGtkSearchEntry* self, gboolean show_clear);

/**
 * gpte_gtk_search_entry_get_location:
 * @self: the search entry widget
 *
 * Gets the currently selected location of this search entry, or %NULL
 * if no such location exists.
 *
 * Returns: (transfer none) (nullable): currently selected location
 */
GpteLocation* gpte_gtk_search_entry_get_location(GpteGtkSearchEntry* self);

/**
 * gpte_gtk_search_entry_set_location:
 * @self: the search entry widget
 * @location: the new location
 *
 * Overwrite the selected location for this entry.
 */
void gpte_gtk_search_entry_set_location(GpteGtkSearchEntry* self, GpteLocation* location);

G_END_DECLS

#endif // __GPTEGTKSEARCHENTRY_H__
