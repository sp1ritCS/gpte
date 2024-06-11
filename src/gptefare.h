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

#ifndef __GPTEFARE_H__
#define __GPTEFARE_H__

#include <glib-object.h>
#include <gptejavaobject.h>

G_BEGIN_DECLS

/**
 * GpteFareType:
 * @GPTE_FARE_ADULT: the default fare price
 * @GPTE_FARE_BIKE: fare price for bikes
 * @GPTE_FARE_CHILD: fare price for children
 * @GPTE_FARE_DISABLED: fare price for handicapped people
 * @GPTE_FARE_MILITARY: fare price for military personel
 * @GPTE_FARE_SENIOR: fare price for seniors
 * @GPTE_FARE_STUDENT: fare price for students
 * @GPTE_FARE_YOUTH: fare price for the youth
 *
 * The fare type describes what group of people may be allowed to travel
 * using a specific [class@Gpte.Fare].
 */

#define GPTE_TYPE_FARE_TYPE (gpte_fare_type_get_type())
GType gpte_fare_type_get_type(void);

typedef enum {
	GPTE_FARE_ADULT,
	GPTE_FARE_BIKE,
	GPTE_FARE_CHILD,
	GPTE_FARE_DISABLED,
	GPTE_FARE_MILITARY,
	GPTE_FARE_SENIOR,
	GPTE_FARE_STUDENT,
	GPTE_FARE_YOUTH
} GpteFareType;


/**
 * GpteCurrency:
 * @symbol: the unicode symbol(s) of the currency
 * @name: the ISO 4217 code of the currency
 *
 * The currency struct contains information about how to display a
 * particular currency.
 */

#define GPTE_TYPE_CURRENTCY (gpte_currency_get_type())
GType gpte_currency_get_type(void);

typedef struct {
	gchar* symbol;
	gchar* name;
} GpteCurrency;

/**
 * gpte_currency_copy:
 * @self: the instance to duplicate
 *
 * Duplicates the [struct@Gpte.Currency] instance.
 *
 * Returns: (transfer full): the copied instance
 */
GpteCurrency* gpte_currency_copy(const GpteCurrency* self);

/**
 * gpte_currency_free:
 * @self: the currency instance
 *
 * Frees the [struct@Gpte.Currency] instance from memory.
 *
 * After this function was called, you may not access the @self anymore.
 */
void gpte_currency_free(GpteCurrency* self);

G_DEFINE_AUTOPTR_CLEANUP_FUNC(GpteCurrency, gpte_currency_free)

/**
 * GpteFare:
 * Represents a specific fare possibility.
 */

#define GPTE_TYPE_FARE (gpte_fare_get_type())
G_DECLARE_FINAL_TYPE (GpteFare, gpte_fare, GPTE, FARE, GpteJavaObject)

/**
 * gpte_fare_get_name:
 * @self: the fare
 *
 * Gets the name of the fare.
 * Returns: (transfer none): the name of the fare
 */
const gchar* gpte_fare_get_name(GpteFare* self);

/**
 * gpte_fare_get_fare_type:
 * @self: the fare
 *
 * Gets the type of the fare.
 * Returns: the fare type
 */
GpteFareType gpte_fare_get_fare_type(GpteFare* self);

/**
 * gpte_fare_get_currency:
 * @self: the fare
 *
 * Gets the currency that [method@Gpte.Fare.get_fare] is represented
 * in.
 *
 * Returns: (transfer none): the currency
 */
const GpteCurrency* gpte_fare_get_currency(GpteFare* self);

/**
 * gpte_fare_get_fare:
 * @self: the fare
 *
 * Gets the fare in decimal representation of the currency returned from
 * [method@Gpte.Fare.get_currency].
 *
 * Returns: fare in decimal representation
 */
gfloat gpte_fare_get_fare(GpteFare* self);

G_END_DECLS

#endif // __GPTEFARE_H__
