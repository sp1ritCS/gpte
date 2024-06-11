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

#include "gpteerrors.h"

G_DEFINE_QUARK(gpte_java_error_quark, gpte_java_error)
G_DEFINE_ENUM_TYPE(GpteJavaError, gpte_java_error,
	G_DEFINE_ENUM_VALUE(GPTE_JAVA_ERROR_JVM_INIT_FAILED, "jvm-init-failed"),
	G_DEFINE_ENUM_VALUE(GPTE_JAVA_ERROR_JVM_THREADING, "jvm-threading"),
	G_DEFINE_ENUM_VALUE(GPTE_JAVA_ERROR_IO_EXCEPTION, "io-exception"),
	G_DEFINE_ENUM_VALUE(GPTE_JAVA_ERROR_UNHANLED_EXCEPTION, "unhandled-exception")
)

G_DEFINE_QUARK(gpte_pte_quark, gpte_pte_error)
G_DEFINE_ENUM_TYPE(GptePteError, gpte_pte_error,
	G_DEFINE_ENUM_VALUE(GPTE_PTE_ERROR_SERVICE_DOWN, "service-down"),
	G_DEFINE_ENUM_VALUE(GPTE_PTE_ERROR_INVALID_STATION, "invalid-station"),
	G_DEFINE_ENUM_VALUE(GPTE_PTE_ERROR_INVALID_ID, "invalid-id"),
	G_DEFINE_ENUM_VALUE(GPTE_PTE_ERROR_INVALID_DATE, "invalid-date")
)

G_DEFINE_QUARK(gpte_pte_trips_quark, gpte_pte_trips_error)
G_DEFINE_ENUM_TYPE(GptePteTripsError, gpte_pte_trips_error,
	G_DEFINE_ENUM_VALUE(GPTE_PTE_TRIPS_ERROR_NO_TRIPS, "no-trips"),
	G_DEFINE_ENUM_VALUE(GPTE_PTE_TRIPS_ERROR_TOO_CLOSE, "too-close"),
	G_DEFINE_ENUM_VALUE(GPTE_PTE_TRIPS_ERROR_UNKNOWN_FROM, "unknown-from"),
	G_DEFINE_ENUM_VALUE(GPTE_PTE_TRIPS_ERROR_UNKNOWN_LOCATION, "unknown-location"),
	G_DEFINE_ENUM_VALUE(GPTE_PTE_TRIPS_ERROR_UNKNOWN_TO, "unknown-to"),
	G_DEFINE_ENUM_VALUE(GPTE_PTE_TRIPS_ERROR_UNKNOWN_VIA, "unknown-via"),
	G_DEFINE_ENUM_VALUE(GPTE_PTE_TRIPS_ERROR_UNRESOLVABLE_ADDRESS, "unresolvable-address")
)
