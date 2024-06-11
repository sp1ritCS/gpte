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

#include "gptegtksearchentry.h"
#include "gptegtkbin-priv.h"

struct _GpteGtkSearchEntry {
	GpteGtkBin parent_instance;

	gboolean show_clear;
	gint max_suggestions;

	// owned
	GpteProvider* provider;
	GtkEntry* entry;
	GtkEntryCompletion* completion;
	GtkListStore* completions;

	GCancellable* current;

	guint entry_changed_sid;
	GpteLocation* active;
};

G_DEFINE_TYPE (GpteGtkSearchEntry, gpte_gtk_search_entry, GPTE_GTK_TYPE_BIN)

enum {
	PROP_PROVIDER = 1,
	PROP_ENTRY,
	PROP_PLACEHOLDER_TEXT,
	PROP_ICON_NAME,
	PROP_SHOW_CLEAR,
	PROP_MAX_SUGGESTIONS,
	N_PROPERTIES
};
static GParamSpec* obj_properties[N_PROPERTIES] = { 0, };

enum {
	SIGNAL_LOCATION_ENTERED,
	N_SIGNALS
};
static guint obj_signals[N_SIGNALS] = { 0 };

static void gpte_gtk_search_entry_dispose(GObject* object) {
	GpteGtkSearchEntry* self = GPTE_GTK_SEARCH_ENTRY(object);
	if (self->current)
		g_cancellable_cancel(self->current);
	g_clear_object(&self->current);
	g_clear_object(&self->entry);
	g_clear_object(&self->completion);
	g_clear_object(&self->completions);
	g_clear_object(&self->provider);
	G_OBJECT_CLASS(gpte_gtk_search_entry_parent_class)->dispose(object);
}

static void gpte_gtk_search_entry_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* pspec) {
	GpteGtkSearchEntry* self = GPTE_GTK_SEARCH_ENTRY(object);
	switch (prop_id) {
		case PROP_PROVIDER:
			g_value_set_object(value, gpte_gtk_search_entry_get_provider(self));
			break;
		case PROP_ENTRY:
			g_value_set_object(value, gpte_gtk_search_entry_get_entry(self));
			break;
		case PROP_PLACEHOLDER_TEXT:
			g_value_set_string(value, gtk_entry_get_placeholder_text(self->entry));
			break;
		case PROP_ICON_NAME:
			g_value_set_string(value, gtk_entry_get_icon_name(self->entry, GTK_ENTRY_ICON_PRIMARY));
			break;
		case PROP_SHOW_CLEAR:
			g_value_set_boolean(value, gpte_gtk_search_entry_get_show_clear(self));
			break;
		case PROP_MAX_SUGGESTIONS:
			g_value_set_int(value, gpte_gtk_search_entry_get_max_suggestions(self));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
	}
}
static void gpte_gtk_search_entry_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* pspec) {
 	GpteGtkSearchEntry* self = GPTE_GTK_SEARCH_ENTRY(object);
	switch (prop_id) {
		case PROP_PROVIDER:
			gpte_gtk_search_entry_set_provider(self, g_value_get_object(value));
			break;
		case PROP_PLACEHOLDER_TEXT:
			gtk_entry_set_placeholder_text(self->entry, g_value_get_string(value));
			break;
		case PROP_ICON_NAME:
			gtk_entry_set_icon_from_icon_name(self->entry, GTK_ENTRY_ICON_PRIMARY, g_value_get_string(value));
			break;
		case PROP_SHOW_CLEAR:
			gpte_gtk_search_entry_set_show_clear(self, g_value_get_boolean(value));
			break;
		case PROP_MAX_SUGGESTIONS:
			gpte_gtk_search_entry_set_max_suggestions(self, g_value_get_int(value));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
	}
}

static void gpte_gtk_search_entry_class_init(GpteGtkSearchEntryClass* class) {
	GObjectClass* object_class = G_OBJECT_CLASS(class);

	object_class->dispose = gpte_gtk_search_entry_dispose;
	object_class->get_property = gpte_gtk_search_entry_get_property;
	object_class->set_property = gpte_gtk_search_entry_set_property;

	obj_properties[PROP_PROVIDER] = g_param_spec_object("provider", NULL, NULL, GPTE_TYPE_PROVIDER, G_PARAM_STATIC_STRINGS | G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);
	obj_properties[PROP_ENTRY] = g_param_spec_object("entry", NULL, NULL, GTK_TYPE_EDITABLE, G_PARAM_STATIC_STRINGS | G_PARAM_READABLE);
	obj_properties[PROP_PLACEHOLDER_TEXT] = g_param_spec_string("placeholder-text", NULL, NULL, NULL, G_PARAM_STATIC_STRINGS | G_PARAM_READWRITE);
	obj_properties[PROP_ICON_NAME] = g_param_spec_string("icon-name", NULL, NULL, NULL, G_PARAM_STATIC_STRINGS | G_PARAM_READWRITE);
	obj_properties[PROP_SHOW_CLEAR] = g_param_spec_boolean("show-clear", NULL, NULL, TRUE, G_PARAM_STATIC_STRINGS | G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_EXPLICIT_NOTIFY);
	obj_properties[PROP_MAX_SUGGESTIONS] = g_param_spec_int("max-suggestions", NULL, NULL, 0, G_MAXINT, 16, G_PARAM_STATIC_STRINGS | G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_EXPLICIT_NOTIFY);
	g_object_class_install_properties(object_class, N_PROPERTIES, obj_properties);

	obj_signals[SIGNAL_LOCATION_ENTERED] = g_signal_new(
		"location-entered", G_TYPE_FROM_CLASS(class),
		G_SIGNAL_RUN_LAST, 0,
		NULL, NULL, NULL,
		G_TYPE_NONE, 1,
		GPTE_TYPE_LOCATION
	);
}

static void gpte_gtk_search_entry_icon_released(GtkEntry*, GtkEntryIconPosition pos, GpteGtkSearchEntry* self) {
	if (pos == GTK_ENTRY_ICON_SECONDARY)
		gpte_gtk_search_entry_set_location(self, NULL);
}

static void gpte_gtk_search_entry_emit_current_location_cb(GpteProvider* provider, GAsyncResult* res, GpteGtkSearchEntry* self) {
	GError* err = NULL;
	g_autoptr(GListModel) results = gpte_provider_suggest_locations_finish(provider, res, &err);
	if (err) {
		if (!g_error_matches(err, G_IO_ERROR, G_IO_ERROR_CANCELLED))
			g_warning("Unable to get suggested locations: %s", err->message);
		g_error_free(err);
		return;
	}

	guint len = g_list_model_get_n_items(results);
	if (len > 0) {
		if (self->active)
			g_object_unref(self->active);
		// TODO: get the geographically closest location instead of the first
		self->active = g_list_model_get_item(results, 0);
		const gchar* name = gpte_location_get_name(self->active);
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
		gtk_list_store_clear(self->completions);
G_GNUC_END_IGNORE_DEPRECATIONS
		g_signal_handler_block(self->entry, self->entry_changed_sid);
		gtk_editable_set_text(GTK_EDITABLE(self->entry), name);
		g_signal_handler_unblock(self->entry, self->entry_changed_sid);
		g_signal_emit(self, obj_signals[SIGNAL_LOCATION_ENTERED], 0, self->active);
	}
}
static void gpte_gtk_search_entry_emit_current_location(GtkEntry* entry, GpteGtkSearchEntry* self) {
	const gchar* text = gtk_editable_get_text(GTK_EDITABLE(entry));
	if (!text || !*text)
		return;

	gpte_provider_suggest_locations_async(self->provider, text, 0, self->max_suggestions, self->current, (GAsyncReadyCallback)gpte_gtk_search_entry_emit_current_location_cb, self);
}

static void gpte_gtk_search_entry_query_callback(GpteProvider* provider, GAsyncResult* res, GpteGtkSearchEntry* self) {
	GError* err = NULL;
	g_autoptr(GListModel) results = gpte_provider_suggest_locations_finish(provider, res, &err);
	if (err) {
		if (!g_error_matches(err, G_IO_ERROR, G_IO_ERROR_CANCELLED)) {
			g_warning("Unable to get suggested locations: %s", err->message);
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
			gtk_list_store_clear(self->completions);
G_GNUC_END_IGNORE_DEPRECATIONS
		}
		g_error_free(err);
		return;
	}

G_GNUC_BEGIN_IGNORE_DEPRECATIONS
	gtk_list_store_clear(self->completions);
G_GNUC_END_IGNORE_DEPRECATIONS
	g_autoptr(GHashTable) location_set = g_hash_table_new_full((GHashFunc)gpte_java_object_hash, (GEqualFunc)gpte_java_object_equal, g_object_unref, NULL);
	guint len = g_list_model_get_n_items(results);
	for (guint i = 0; i < len; i++) {
		GpteLocation* location = g_list_model_get_item(results, i);
		if (g_hash_table_contains(location_set, location)) {
			g_object_unref(location);
			continue;
		} else {
			g_hash_table_add(location_set, location);
		}
		const gchar* name = gpte_location_get_name(location);
		GtkTreeIter iter;
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
		gtk_list_store_append(self->completions, &iter);
		gtk_list_store_set(self->completions, &iter, 0, location, 1, name, -1);
G_GNUC_END_IGNORE_DEPRECATIONS
	}

	// As Gtk.EntryCompletion usually wants the whole list and does the searching itself, we
	// have to hack arround a bit to make it work. Part of it is overwriting the search
	// function to always return TRUE (see gpte_gtk_search_entry_completion_match), the other
	// part is here, where Gtk.EntryCompletion only shows the popup after a character was
	// inserted. We work arround this here by emitting the "changed" signal ourselves.
	g_signal_handler_block(self->entry, self->entry_changed_sid);
	GtkEditable* delegate = gtk_editable_get_delegate(GTK_EDITABLE(self->entry));
	g_signal_emit_by_name(delegate, "changed");
	g_signal_handler_unblock(self->entry, self->entry_changed_sid);
}
static void gpte_gtk_search_entry_changed(GtkEditable* editable, GpteGtkSearchEntry* self) {
	self->active = NULL;
	if (!self->provider)
		return;
	if (self->current) {
		g_cancellable_cancel(self->current);
		g_object_unref(self->current);
	}
	self->current = g_cancellable_new();

	// TODO: provide ability to set the GpteLocations value via property
	gpte_provider_suggest_locations_async(self->provider, gtk_editable_get_text(editable), GPTE_LOCATIONS_ANY, self->max_suggestions, self->current, (GAsyncReadyCallback)gpte_gtk_search_entry_query_callback, self);
}

static gboolean gpte_gtk_search_entry_completion_match(GtkEntryCompletion*, const gchar*, GtkTreeIter*, gpointer) {
	return TRUE;
}

static gboolean gpte_gkte_search_entry_match_selected(GtkEntryCompletion*, GtkTreeModel* model, GtkTreeIter* iter, GpteGtkSearchEntry* self) {
	GpteLocation* location;
	gchar* name;
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
	gtk_tree_model_get(model, iter, 0, &location, 1, &name, -1);
G_GNUC_END_IGNORE_DEPRECATIONS

	g_signal_handler_block(self->entry, self->entry_changed_sid);
	gtk_editable_set_text(GTK_EDITABLE(self->entry), name);
	g_signal_handler_unblock(self->entry, self->entry_changed_sid);
	if (self->active)
		g_object_unref(self->active);
	self->active = location;

	g_signal_emit(self, obj_signals[SIGNAL_LOCATION_ENTERED], 0, self->active);

	g_free(name);
	return TRUE;
}

static void gpte_gtk_search_entry_changed_update_icon(GtkEditable* editable, GpteGtkSearchEntry* self) {
	const gchar* icon = NULL;
	if (self->show_clear) {
		const gchar* text = gtk_editable_get_text(editable);
		if (text && *text)
			icon = "edit-clear-symbolic";
	}
	gtk_entry_set_icon_from_icon_name(self->entry, GTK_ENTRY_ICON_SECONDARY, icon);
}

static void gpte_gtk_search_entry_init(GpteGtkSearchEntry* self) {
	self->provider = NULL;
	self->current = NULL;
	self->active = NULL;
	self->show_clear = FALSE;
	self->max_suggestions = 0;

	self->entry = g_object_ref_sink(GTK_ENTRY(gtk_entry_new()));
	gpte_gtk_bin_set_child(GPTE_GTK_BIN(self), GTK_WIDGET(self->entry));

	g_signal_connect(self->entry, "icon-release", G_CALLBACK(gpte_gtk_search_entry_icon_released), self);

G_GNUC_BEGIN_IGNORE_DEPRECATIONS
	self->completions = gtk_list_store_new(2, G_TYPE_OBJECT, G_TYPE_STRING);
	self->completion = gtk_entry_completion_new();
	gtk_entry_completion_set_model(self->completion, GTK_TREE_MODEL(self->completions));
	gtk_entry_completion_set_text_column(self->completion, 1);
	gtk_entry_completion_set_match_func(self->completion, gpte_gtk_search_entry_completion_match, NULL, NULL);
	gtk_entry_set_completion(self->entry, self->completion);
G_GNUC_END_IGNORE_DEPRECATIONS

	g_signal_connect(self->entry, "activate", G_CALLBACK(gpte_gtk_search_entry_emit_current_location), self);

	self->entry_changed_sid = g_signal_connect(self->entry, "changed", G_CALLBACK(gpte_gtk_search_entry_changed), self);
	g_signal_connect(self->completion, "match-selected", G_CALLBACK(gpte_gkte_search_entry_match_selected), self);
	g_signal_connect(self->entry, "changed", G_CALLBACK(gpte_gtk_search_entry_changed_update_icon), self);
}

GtkWidget* gpte_gtk_search_entry_new(GpteProvider* provider) {
	return g_object_new(GPTE_GTK_TYPE_SEARCH_ENTRY, "provider", provider, NULL);
}

GpteProvider* gpte_gtk_search_entry_get_provider(GpteGtkSearchEntry* self) {
	g_return_val_if_fail(GPTE_GTK_IS_SEARCH_ENTRY(self), NULL);
	return self->provider;
}
void gpte_gtk_search_entry_set_provider(GpteGtkSearchEntry* self, GpteProvider* provider) {
	g_return_if_fail(GPTE_GTK_IS_SEARCH_ENTRY(self));
	g_return_if_fail(!provider || GPTE_IS_PROVIDER(provider));

	if (self->provider == provider)
		return;

	if (self->provider)
		g_object_unref(self->provider);
	self->provider = provider;
	if (self->provider)
		g_object_ref(self->provider);

G_GNUC_BEGIN_IGNORE_DEPRECATIONS
	gtk_list_store_clear(self->completions);
G_GNUC_END_IGNORE_DEPRECATIONS

	g_object_notify_by_pspec(G_OBJECT(self), obj_properties[PROP_PROVIDER]);
}

GtkEditable* gpte_gtk_search_entry_get_entry(GpteGtkSearchEntry* self) {
	g_return_val_if_fail(GPTE_GTK_IS_SEARCH_ENTRY(self), NULL);
	return GTK_EDITABLE(self->entry);
}

gint gpte_gtk_search_entry_get_max_suggestions(GpteGtkSearchEntry* self) {
	g_return_val_if_fail(GPTE_GTK_IS_SEARCH_ENTRY(self), -1);
	return self->max_suggestions;
}
void gpte_gtk_search_entry_set_max_suggestions(GpteGtkSearchEntry* self, gint max_suggestions) {
	g_return_if_fail(GPTE_GTK_IS_SEARCH_ENTRY(self));
	g_return_if_fail(max_suggestions >= 0);

	if (self->max_suggestions == max_suggestions)
		return;
	self->max_suggestions = max_suggestions;
	g_object_notify_by_pspec(G_OBJECT(self), obj_properties[PROP_MAX_SUGGESTIONS]);
}

gboolean gpte_gtk_search_entry_get_show_clear(GpteGtkSearchEntry* self) {
	g_return_val_if_fail(GPTE_GTK_IS_SEARCH_ENTRY(self), FALSE);
	return self->show_clear;
}
void gpte_gtk_search_entry_set_show_clear(GpteGtkSearchEntry* self, gboolean show_clear) {
	g_return_if_fail(GPTE_GTK_IS_SEARCH_ENTRY(self));
	if (self->show_clear == show_clear)
		return;
	self->show_clear = show_clear;

	const gchar* icon = NULL;
	if (self->show_clear) {
		const gchar* text = gtk_editable_get_text(GTK_EDITABLE(self->entry));
		if (text && *text)
			icon = "edit-clear-symbolic";
	}
	gtk_entry_set_icon_from_icon_name(self->entry, GTK_ENTRY_ICON_SECONDARY, icon);

	g_object_notify_by_pspec(G_OBJECT(self), obj_properties[PROP_SHOW_CLEAR]);
}


GpteLocation* gpte_gtk_search_entry_get_location(GpteGtkSearchEntry* self) {
	g_return_val_if_fail(GPTE_GTK_IS_SEARCH_ENTRY(self), NULL);
	return self->active;
}
void gpte_gtk_search_entry_set_location(GpteGtkSearchEntry* self, GpteLocation* location) {
	g_return_if_fail(GPTE_GTK_IS_SEARCH_ENTRY(self));
	g_return_if_fail(!location || GPTE_IS_LOCATION(location));

	if (self->current) {
		g_cancellable_cancel(self->current);
		g_clear_object(&self->current);
	}

G_GNUC_BEGIN_IGNORE_DEPRECATIONS
	gtk_list_store_clear(self->completions);
G_GNUC_END_IGNORE_DEPRECATIONS

	if (self->active)
		g_object_unref(self->active);
	self->active = location;
	if (self->active)
		g_object_ref(self->active);

	const gchar* new = self->active ? gpte_location_get_name(self->active) : "";
	g_signal_handler_block(self->entry, self->entry_changed_sid);
	gtk_editable_set_text(GTK_EDITABLE(self->entry), new);
	g_signal_handler_unblock(self->entry, self->entry_changed_sid);
}
