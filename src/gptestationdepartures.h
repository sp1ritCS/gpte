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

#ifndef __GPTESTATIONDEPARTURES_H__
#define __GPTESTATIONDEPARTURES_H__

#include <glib-object.h>
#include <gptejavaobject.h>

#include <gpteline.h>
#include <gptelocation.h>
#include <gptedeparture.h>

#include <gio/gio.h>

G_BEGIN_DECLS

/**
 * GpteLineDest:
 * @line: the departing line
 * @destination: (nullable): terminus location of @line
 *
 * (@line, @destination) tupel received from
 * [method@Gpte.StationDepartures.get_lines] by some providers.
 */

#define GPTE_TYPE_LINE_DEST (gpte_line_dest_get_type())
GType gpte_line_dest_get_type(void);

typedef struct {
	GpteLine* line;
	// (nullable)
	GpteLocation* destination;
} GpteLineDest;

GpteLineDest* gpte_line_dest_copy(const GpteLineDest* self);
void gpte_line_dest_free(GpteLineDest* self);

G_DEFINE_AUTOPTR_CLEANUP_FUNC(GpteLineDest, gpte_line_dest_free)

/**
 * GpteStationDepartures:
 * Container that holds all departures at a particular station.
 */

#define GPTE_TYPE_STATION_DEPARTURES (gpte_station_departures_get_type())
G_DECLARE_FINAL_TYPE (GpteStationDepartures, gpte_station_departures, GPTE, STATION_DEPARTURES, GpteJavaObject)

/**
 * gpte_station_departures_get_location:
 * @self: the station departures
 *
 * Gets the location of the station that this container holds the
 * departures of.
 *
 * Returns: (transfer full): the location of the station
 */
GpteLocation* gpte_station_departures_get_location(GpteStationDepartures* self);

/**
 * gpte_station_departures_get_departures:
 * @self: the station departures
 *
 * Gets the departures at this station.
 *
 * Returns: (transfer full): Departures as a #GListModel of [class@Gpte.Departure]
 */
GListModel* gpte_station_departures_get_departures(GpteStationDepartures* self);

/**
 * gpte_station_departures_get_lines:
 * @self: the station departures
 *
 * Gets the lines that depart at this station.
 *
 * Returns: (transfer full) (element-type GpteLineDest) (nullable): departing lines or %NULL
 */
GPtrArray* gpte_station_departures_get_lines(GpteStationDepartures* self);

G_END_DECLS

#endif // __GPTESTATIONDEPARTURES_H__
