#!/bin/sh

cat << EOF
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

#ifndef __GPTEPROVIDERS_H__
#define __GPTEPROVIDERS_H__

#include <gpteprovider.h>

G_BEGIN_DECLS

typedef enum {
	GPTE_NEGENTWEE_LANG_EN_GB,
	GPTE_NEGENTWEE_LANG_NL_NL
} GpteNegentweeLanguage;

EOF

awk '/BEGIN GPTE PROVIDERS SECTION/,/END GPTE PROVIDERS SECTION/' gpteproviders.c | \
	sed -E 's$GPTE_CREATE_NAVITIA_PROVIDER_FUN\(([a-z_]*), .*\)$\
/**\
 * gpte_create_\1_provider:\
 * @vm: the Java VM to create this provider in\
 * @authorization: The Navitia authorization token\
 *\
 * Creates the \1 provider from [Navitia](https://navitia.io/).\
 * Returns: (transfer full): the \1 provider\
 */\
GpteProvider* gpte_create_\1_provider(GpteJvm* vm, const gchar* authorization);$' | \
	sed -E 's$GPTE_CREATE_HAFAS_PROVIDER_FUN\(([a-z_]*), .*\)$\
/**\
 * gpte_create_\1_provider:\
 * @vm: the Java VM to create this provider in\
 * @api_authorization: The HAFAS authorization object\
 *\
 * Creates the \1 provider.\
 * Returns: (transfer full): the \1 provider\
 */\
GpteProvider* gpte_create_\1_provider(GpteJvm* vm, const gchar* api_authorization);$' | \
	sed -E 's$GPTE_CREATE_EFA_PROVIDER_FUN\(([a-z_]*), .*\)$\
/**\
 * gpte_create_\1_provider:\
 * @vm: the Java VM to create this provider in\
 *\
 * Creates the \1 provider.\
 * Returns: (transfer full): the \1 provider\
 */\
GpteProvider* gpte_create_\1_provider(GpteJvm* vm);$' | \
	sed -E 's$GPTE_CREATE_HAFAS_COMPLEX_PROVIDER_FUN\(([a-z_]*), .*\)$\
/**\
 * gpte_create_\1_provider:\
 * @vm: the Java VM to create this provider in\
 * @api_authorization: The HAFAS authorization object\
 * @salt: (array length=salt_len): the salt blob passed to HAFAS\
 * @salt_len: the langth of @salt in bytes\
 *\
 * Creates the \1 provider.\
 * Returns: (transfer full): the \1 provider\
 */\
GpteProvider* gpte_create_\1_provider(GpteJvm* vm, const gchar* api_authorization, const guchar* salt, gsize salt_len);$' | \
	sed -E 's$GPTE_CREATE_HAFAS_LEGACY_PROVIDER_FUN\(([a-z_]*), .*\)$\
/**\
 * gpte_create_\1_provider:\
 * @vm: the Java VM to create this provider in\
 *\
 * Creates the \1 provider.\
 * Returns: (transfer full): the \1 provider\
 */\
GpteProvider* gpte_create_\1_provider(GpteJvm* vm);$' | \
	perl -0777 -pe 's!(GpteProvider\* gpte_create_([a-z_]*)_provider\(GpteJvm\* vm, GpteNegentweeLanguage language\)) ?\{$(\n|.)*?^\}!\
/**\
 * gpte_create_\2_provider:\
 * \@vm: the Java VM to create this provider in\
 * \@language: [enum\@Gpte.NegentweeLanguage] that this provider is supposed to use\
 *\
 * Creates the \2 provider.\
 * Returns: (transfer full): the \2 provider\
 */\
\1;!m' | \
	perl -0777 -pe 's!(GpteProvider\* gpte_create_([a-z_]*)_provider\(GpteJvm\* vm, const guchar\* client_cert, gsize client_cert_len\)) ?\{$(\n|.)*?^\}!\
/**\
 * gpte_create_\2_provider:\
 * \@vm: the Java VM to create this provider in\
 * \@client_cert: (array length=client_cert_len): client certificate blob\
 * \@client_cert_len: length of @client_cert in bytes\
 *\
 * Creates the \2 provider.\
 * Returns: (transfer full): the \2 provider\
 */\
\1;!m'

cat << EOF

G_END_DECLS

#endif // __GPTEPROVIDERS_H__
EOF

