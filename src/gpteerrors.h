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

#ifndef __GPTEERRORS_H__
#define __GPTEERRORS_H__

#include <glib-object.h>

G_BEGIN_DECLS

/**
 * GpteJavaError:
 * @GPTE_JAVA_ERROR_JVM_INIT_FAILED: initialization of the JVM failed
 * @GPTE_JAVA_ERROR_JVM_THREADING: threading routine issue
 * @GPTE_JAVA_ERROR_IO_EXCEPTION: the JVM has a pending [java.io.IOException](https://docs.oracle.com/en/java/javase/17/docs/api/java.base/java/io/IOException.html)
 * @GPTE_JAVA_ERROR_UNHANLED_EXCEPTION: the JVM has a pending exception
 *
 * Various errors that can occur either during
 * [JNI Invocation](https://docs.oracle.com/en/java/javase/17/docs/specs/jni/invocation.html)
 * initialization or over the runtime of the JVM.
 */

#define GPTE_JAVA_ERROR (gpte_java_error_quark())
#define GPTE_TYPE_JAVA_ERROR (gpte_java_error_get_type())
GQuark gpte_java_error_quark(void);
GType gpte_java_error_get_type(void);

typedef enum {
	GPTE_JAVA_ERROR_JVM_INIT_FAILED,
	GPTE_JAVA_ERROR_JVM_THREADING,
	GPTE_JAVA_ERROR_IO_EXCEPTION,
	GPTE_JAVA_ERROR_UNHANLED_EXCEPTION
} GpteJavaError;


/**
 * GptePteError:
 * @GPTE_PTE_ERROR_SERVICE_DOWN: provider is down
 * @GPTE_PTE_ERROR_INVALID_STATION: a station is invalid
 * @GPTE_PTE_ERROR_INVALID_ID: the identifier of a station is invalid
 * @GPTE_PTE_ERROR_INVALID_DATE: the passed date is invalid
 *
 * Errors that can be thrown by the [class@Gpte.Provider].`query_` set
 * of methods.
 */

#define GPTE_PTE_ERROR (gpte_pte_error_quark())
#define GPTE_TYPE_PTE_ERROR (gpte_pte_error_get_type())
GQuark gpte_pte_error_quark(void);
GType gpte_pte_error_get_type(void);

typedef enum {
	GPTE_PTE_ERROR_SERVICE_DOWN,
	GPTE_PTE_ERROR_INVALID_STATION,
	GPTE_PTE_ERROR_INVALID_ID,
	GPTE_PTE_ERROR_INVALID_DATE
} GptePteError;

/**
 * GptePteTripsError:
 * @GPTE_PTE_TRIPS_ERROR_NO_TRIPS: unable to find any trips
 * @GPTE_PTE_TRIPS_ERROR_TOO_CLOSE: departure and arrival locations are too close together
 * @GPTE_PTE_TRIPS_ERROR_UNKNOWN_FROM: unknown departure location
 * @GPTE_PTE_TRIPS_ERROR_UNKNOWN_LOCATION: unknown location
 * @GPTE_PTE_TRIPS_ERROR_UNKNOWN_TO: unknown arrival location
 * @GPTE_PTE_TRIPS_ERROR_UNKNOWN_VIA: unknown via location
 * @GPTE_PTE_TRIPS_ERROR_UNRESOLVABLE_ADDRESS: address location was unresolvable
 *
 * Additional errors only thrown by [method@Gpte.Provider.query_trips].
 */

#define GPTE_PTE_TRIPS_ERROR (gpte_pte_trips_error_quark())
#define GPTE_TYPE_PTE_TRIPS_ERROR (gpte_pte_trips_error_get_type())
GQuark gpte_pte_trips_error_quark(void);
GType gpte_pte_trips_error_get_type(void);

typedef enum {
	GPTE_PTE_TRIPS_ERROR_NO_TRIPS,
	GPTE_PTE_TRIPS_ERROR_TOO_CLOSE,
	GPTE_PTE_TRIPS_ERROR_UNKNOWN_FROM,
	GPTE_PTE_TRIPS_ERROR_UNKNOWN_LOCATION,
	GPTE_PTE_TRIPS_ERROR_UNKNOWN_TO,
	GPTE_PTE_TRIPS_ERROR_UNKNOWN_VIA,
	GPTE_PTE_TRIPS_ERROR_UNRESOLVABLE_ADDRESS
} GptePteTripsError;

G_END_DECLS

#endif // __GPTEERRORS_H__
