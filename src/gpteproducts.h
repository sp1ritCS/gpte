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

#ifndef __GPTEPRODUCTS_H__
#define __GPTEPRODUCTS_H__

#include <glib-object.h>

G_BEGIN_DECLS

/**
 * GpteProductCode:
 * @GPTE_PRODUCT_CODE_NULL: unset/unknown product
 * @GPTE_PRODUCT_CODE_HIGH_SPEED_TRAIN: High speed train
 * @GPTE_PRODUCT_CODE_REGIONAL_TRAIN: Regional train
 * @GPTE_PRODUCT_CODE_SUBURBAN_TRAIN: Suburban train
 * @GPTE_PRODUCT_CODE_SUBWAY: Subway
 * @GPTE_PRODUCT_CODE_TRAM: Tram
 * @GPTE_PRODUCT_CODE_BUS: Bus
 * @GPTE_PRODUCT_CODE_FERRY: Ferry
 * @GPTE_PRODUCT_CODE_CABLECAR: Cabelcar
 * @GPTE_PRODUCT_CODE_ON_DEMAND: On-demand service
 *
 * The product code describes the transport class.
 *
 *
 * See [flags@Gpte.Products] for the bitset version.
 */

#define GPTE_TYPE_PRODUCT_CODE (gpte_product_code_get_type())
GType gpte_product_code_get_type(void);

typedef enum {
	GPTE_PRODUCT_CODE_NULL = 0,
	GPTE_PRODUCT_CODE_HIGH_SPEED_TRAIN = 'I',
	GPTE_PRODUCT_CODE_REGIONAL_TRAIN = 'R',
	GPTE_PRODUCT_CODE_SUBURBAN_TRAIN = 'S',
	GPTE_PRODUCT_CODE_SUBWAY = 'U',
	GPTE_PRODUCT_CODE_TRAM = 'T',
	GPTE_PRODUCT_CODE_BUS = 'B',
	GPTE_PRODUCT_CODE_FERRY = 'F',
	GPTE_PRODUCT_CODE_CABLECAR = 'C',
	GPTE_PRODUCT_CODE_ON_DEMAND = 'P'
} GpteProductCode;

/**
 * gpte_product_code_to_priority:
 * @code: the product code
 *
 * Gets the sorting priority of @code used when sorting different
 * services.
 *
 * Returns: sorting priority
 */
gint gpte_product_code_to_priority(GpteProductCode code);


/**
 * GpteProducts:
 * @GPTE_PRODUCT_HIGH_SPEED_TRAIN: High speed train
 * @GPTE_PRODUCT_REGIONAL_TRAIN: Regional train
 * @GPTE_PRODUCT_SUBURBAN_TRAIN: Suburban train
 * @GPTE_PRODUCT_SUBWAY: Subway
 * @GPTE_PRODUCT_TRAM: Tram
 * @GPTE_PRODUCT_BUS: Bus
 * @GPTE_PRODUCT_FERRY: Ferry
 * @GPTE_PRODUCT_CABLECAR: Cabelcar
 * @GPTE_PRODUCT_ON_DEMAND: On-demand service
 *
 * Bitset of [enum@Gpte.ProductCode].
 */

#define GPTE_TYPE_PRODUCTS (gpte_products_get_type())
GType gpte_products_get_type(void);

typedef enum {
	GPTE_PRODUCT_HIGH_SPEED_TRAIN = 1 << 0,
	GPTE_PRODUCT_REGIONAL_TRAIN = 1 << 1,
	GPTE_PRODUCT_SUBURBAN_TRAIN = 1 << 2,
	GPTE_PRODUCT_SUBWAY = 1 << 3,
	GPTE_PRODUCT_TRAM = 1 << 4,
	GPTE_PRODUCT_BUS = 1 << 5,
	GPTE_PRODUCT_FERRY = 1 << 6,
	GPTE_PRODUCT_CABLECAR = 1 << 7,
	GPTE_PRODUCT_ON_DEMAND = 1 << 8,
	GPTE_PRODUCT_ANY = (GPTE_PRODUCT_ON_DEMAND << 1) - 1
} GpteProducts;

G_END_DECLS

#endif // __GPTEPROgDUCTS_H__
