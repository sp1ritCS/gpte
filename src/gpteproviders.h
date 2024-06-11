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

// BEGIN GPTE PROVIDERS SECTION


/**
 * gpte_create_australia_provider:
 * @vm: the Java VM to create this provider in
 * @authorization: The Navitia authorization token
 *
 * Creates the australia provider from [Navitia](https://navitia.io/).
 * Returns: (transfer full): the australia provider
 */
GpteProvider* gpte_create_australia_provider(GpteJvm* vm, const gchar* authorization);

/**
 * gpte_create_avv_aachen_provider:
 * @vm: the Java VM to create this provider in
 * @api_authorization: The HAFAS authorization object
 *
 * Creates the avv_aachen provider.
 * Returns: (transfer full): the avv_aachen provider
 */
GpteProvider* gpte_create_avv_aachen_provider(GpteJvm* vm, const gchar* api_authorization);

/**
 * gpte_create_avv_augsburg_provider:
 * @vm: the Java VM to create this provider in
 * @api_authorization: The HAFAS authorization object
 *
 * Creates the avv_augsburg provider.
 * Returns: (transfer full): the avv_augsburg provider
 */
GpteProvider* gpte_create_avv_augsburg_provider(GpteJvm* vm, const gchar* api_authorization);

/**
 * gpte_create_bart_provider:
 * @vm: the Java VM to create this provider in
 * @api_authorization: The HAFAS authorization object
 *
 * Creates the bart provider.
 * Returns: (transfer full): the bart provider
 */
GpteProvider* gpte_create_bart_provider(GpteJvm* vm, const gchar* api_authorization);

/**
 * gpte_create_bayern_provider:
 * @vm: the Java VM to create this provider in
 *
 * Creates the bayern provider.
 * Returns: (transfer full): the bayern provider
 */
GpteProvider* gpte_create_bayern_provider(GpteJvm* vm);

/**
 * gpte_create_brazil_provider:
 * @vm: the Java VM to create this provider in
 * @authorization: The Navitia authorization token
 *
 * Creates the brazil provider from [Navitia](https://navitia.io/).
 * Returns: (transfer full): the brazil provider
 */
GpteProvider* gpte_create_brazil_provider(GpteJvm* vm, const gchar* authorization);

/**
 * gpte_create_british_columbia_provider:
 * @vm: the Java VM to create this provider in
 * @authorization: The Navitia authorization token
 *
 * Creates the british_columbia provider from [Navitia](https://navitia.io/).
 * Returns: (transfer full): the british_columbia provider
 */
GpteProvider* gpte_create_british_columbia_provider(GpteJvm* vm, const gchar* authorization);

/**
 * gpte_create_bsvag_provider:
 * @vm: the Java VM to create this provider in
 *
 * Creates the bsvag provider.
 * Returns: (transfer full): the bsvag provider
 */
GpteProvider* gpte_create_bsvag_provider(GpteJvm* vm);

/**
 * gpte_create_bvg_provider:
 * @vm: the Java VM to create this provider in
 * @api_authorization: The HAFAS authorization object
 *
 * Creates the bvg provider.
 * Returns: (transfer full): the bvg provider
 */
GpteProvider* gpte_create_bvg_provider(GpteJvm* vm, const gchar* api_authorization);

/**
 * gpte_create_cmta_provider:
 * @vm: the Java VM to create this provider in
 * @api_authorization: The HAFAS authorization object
 *
 * Creates the cmta provider.
 * Returns: (transfer full): the cmta provider
 */
GpteProvider* gpte_create_cmta_provider(GpteJvm* vm, const gchar* api_authorization);

/**
 * gpte_create_czech_republic_provider:
 * @vm: the Java VM to create this provider in
 * @authorization: The Navitia authorization token
 *
 * Creates the czech_republic provider from [Navitia](https://navitia.io/).
 * Returns: (transfer full): the czech_republic provider
 */
GpteProvider* gpte_create_czech_republic_provider(GpteJvm* vm, const gchar* authorization);

/**
 * gpte_create_db_provider:
 * @vm: the Java VM to create this provider in
 * @api_authorization: The HAFAS authorization object
 * @salt: (array length=salt_len): the salt blob passed to HAFAS
 * @salt_len: the langth of @salt in bytes
 *
 * Creates the db provider.
 * Returns: (transfer full): the db provider
 */
GpteProvider* gpte_create_db_provider(GpteJvm* vm, const gchar* api_authorization, const guchar* salt, gsize salt_len);

/**
 * gpte_create_ding_provider:
 * @vm: the Java VM to create this provider in
 *
 * Creates the ding provider.
 * Returns: (transfer full): the ding provider
 */
GpteProvider* gpte_create_ding_provider(GpteJvm* vm);

/**
 * gpte_create_dsb_provider:
 * @vm: the Java VM to create this provider in
 * @api_authorization: The HAFAS authorization object
 *
 * Creates the dsb provider.
 * Returns: (transfer full): the dsb provider
 */
GpteProvider* gpte_create_dsb_provider(GpteJvm* vm, const gchar* api_authorization);

/**
 * gpte_create_dub_provider:
 * @vm: the Java VM to create this provider in
 *
 * Creates the dub provider.
 * Returns: (transfer full): the dub provider
 */
GpteProvider* gpte_create_dub_provider(GpteJvm* vm);

/**
 * gpte_create_eireann_provider:
 * @vm: the Java VM to create this provider in
 *
 * Creates the eireann provider.
 * Returns: (transfer full): the eireann provider
 */
GpteProvider* gpte_create_eireann_provider(GpteJvm* vm);

/**
 * gpte_create_finland_provider:
 * @vm: the Java VM to create this provider in
 * @authorization: The Navitia authorization token
 *
 * Creates the finland provider from [Navitia](https://navitia.io/).
 * Returns: (transfer full): the finland provider
 */
GpteProvider* gpte_create_finland_provider(GpteJvm* vm, const gchar* authorization);

/**
 * gpte_create_france_ne_provider:
 * @vm: the Java VM to create this provider in
 * @authorization: The Navitia authorization token
 *
 * Creates the france_ne provider from [Navitia](https://navitia.io/).
 * Returns: (transfer full): the france_ne provider
 */
GpteProvider* gpte_create_france_ne_provider(GpteJvm* vm, const gchar* authorization);

/**
 * gpte_create_france_nw_provider:
 * @vm: the Java VM to create this provider in
 * @authorization: The Navitia authorization token
 *
 * Creates the france_nw provider from [Navitia](https://navitia.io/).
 * Returns: (transfer full): the france_nw provider
 */
GpteProvider* gpte_create_france_nw_provider(GpteJvm* vm, const gchar* authorization);

/**
 * gpte_create_france_se_provider:
 * @vm: the Java VM to create this provider in
 * @authorization: The Navitia authorization token
 *
 * Creates the france_se provider from [Navitia](https://navitia.io/).
 * Returns: (transfer full): the france_se provider
 */
GpteProvider* gpte_create_france_se_provider(GpteJvm* vm, const gchar* authorization);

/**
 * gpte_create_france_sw_provider:
 * @vm: the Java VM to create this provider in
 * @authorization: The Navitia authorization token
 *
 * Creates the france_sw provider from [Navitia](https://navitia.io/).
 * Returns: (transfer full): the france_sw provider
 */
GpteProvider* gpte_create_france_sw_provider(GpteJvm* vm, const gchar* authorization);

/**
 * gpte_create_ghana_provider:
 * @vm: the Java VM to create this provider in
 * @authorization: The Navitia authorization token
 *
 * Creates the ghana provider from [Navitia](https://navitia.io/).
 * Returns: (transfer full): the ghana provider
 */
GpteProvider* gpte_create_ghana_provider(GpteJvm* vm, const gchar* authorization);

/**
 * gpte_create_gvh_provider:
 * @vm: the Java VM to create this provider in
 *
 * Creates the gvh provider.
 * Returns: (transfer full): the gvh provider
 */
GpteProvider* gpte_create_gvh_provider(GpteJvm* vm);

/**
 * gpte_create_invg_provider:
 * @vm: the Java VM to create this provider in
 * @api_authorization: The HAFAS authorization object
 * @salt: (array length=salt_len): the salt blob passed to HAFAS
 * @salt_len: the langth of @salt in bytes
 *
 * Creates the invg provider.
 * Returns: (transfer full): the invg provider
 */
GpteProvider* gpte_create_invg_provider(GpteJvm* vm, const gchar* api_authorization, const guchar* salt, gsize salt_len);

/**
 * gpte_create_italy_provider:
 * @vm: the Java VM to create this provider in
 * @authorization: The Navitia authorization token
 *
 * Creates the italy provider from [Navitia](https://navitia.io/).
 * Returns: (transfer full): the italy provider
 */
GpteProvider* gpte_create_italy_provider(GpteJvm* vm, const gchar* authorization);

/**
 * gpte_create_kvv_provider:
 * @vm: the Java VM to create this provider in
 *
 * Creates the kvv provider.
 * Returns: (transfer full): the kvv provider
 */
GpteProvider* gpte_create_kvv_provider(GpteJvm* vm);

/**
 * gpte_create_linz_provider:
 * @vm: the Java VM to create this provider in
 *
 * Creates the linz provider.
 * Returns: (transfer full): the linz provider
 */
GpteProvider* gpte_create_linz_provider(GpteJvm* vm);

/**
 * gpte_create_lu_provider:
 * @vm: the Java VM to create this provider in
 * @api_authorization: The HAFAS authorization object
 *
 * Creates the lu provider.
 * Returns: (transfer full): the lu provider
 */
GpteProvider* gpte_create_lu_provider(GpteJvm* vm, const gchar* api_authorization);

/**
 * gpte_create_massachusetts_provider:
 * @vm: the Java VM to create this provider in
 * @authorization: The Navitia authorization token
 *
 * Creates the massachusetts provider from [Navitia](https://navitia.io/).
 * Returns: (transfer full): the massachusetts provider
 */
GpteProvider* gpte_create_massachusetts_provider(GpteJvm* vm, const gchar* authorization);

/**
 * gpte_create_mersey_provider:
 * @vm: the Java VM to create this provider in
 *
 * Creates the mersey provider.
 * Returns: (transfer full): the mersey provider
 */
GpteProvider* gpte_create_mersey_provider(GpteJvm* vm);

/**
 * gpte_create_mvg_provider:
 * @vm: the Java VM to create this provider in
 *
 * Creates the mvg provider.
 * Returns: (transfer full): the mvg provider
 */
GpteProvider* gpte_create_mvg_provider(GpteJvm* vm);

/**
 * gpte_create_mvv_provider:
 * @vm: the Java VM to create this provider in
 *
 * Creates the mvv provider.
 * Returns: (transfer full): the mvv provider
 */
GpteProvider* gpte_create_mvv_provider(GpteJvm* vm);

/**
 * gpte_create_nasa_provider:
 * @vm: the Java VM to create this provider in
 * @api_authorization: The HAFAS authorization object
 *
 * Creates the nasa provider.
 * Returns: (transfer full): the nasa provider
 */
GpteProvider* gpte_create_nasa_provider(GpteJvm* vm, const gchar* api_authorization);


/**
 * gpte_create_negentwee_provider:
 * @vm: the Java VM to create this provider in
 * @language: [enum@Gpte.NegentweeLanguage] that this provider is supposed to use
 *
 * Creates the negentwee provider.
 * Returns: (transfer full): the negentwee provider
 */
GpteProvider* gpte_create_negentwee_provider(GpteJvm* vm, GpteNegentweeLanguage language);


/**
 * gpte_create_nicaragua_provider:
 * @vm: the Java VM to create this provider in
 * @authorization: The Navitia authorization token
 *
 * Creates the nicaragua provider from [Navitia](https://navitia.io/).
 * Returns: (transfer full): the nicaragua provider
 */
GpteProvider* gpte_create_nicaragua_provider(GpteJvm* vm, const gchar* authorization);

/**
 * gpte_create_ns_provider:
 * @vm: the Java VM to create this provider in
 *
 * Creates the ns provider.
 * Returns: (transfer full): the ns provider
 */
GpteProvider* gpte_create_ns_provider(GpteJvm* vm);

/**
 * gpte_create_nvbw_provider:
 * @vm: the Java VM to create this provider in
 *
 * Creates the nvbw provider.
 * Returns: (transfer full): the nvbw provider
 */
GpteProvider* gpte_create_nvbw_provider(GpteJvm* vm);

/**
 * gpte_create_nvv_provider:
 * @vm: the Java VM to create this provider in
 * @api_authorization: The HAFAS authorization object
 *
 * Creates the nvv provider.
 * Returns: (transfer full): the nvv provider
 */
GpteProvider* gpte_create_nvv_provider(GpteJvm* vm, const gchar* api_authorization);

/**
 * gpte_create_nz_provider:
 * @vm: the Java VM to create this provider in
 * @authorization: The Navitia authorization token
 *
 * Creates the nz provider from [Navitia](https://navitia.io/).
 * Returns: (transfer full): the nz provider
 */
GpteProvider* gpte_create_nz_provider(GpteJvm* vm, const gchar* authorization);

/**
 * gpte_create_oebb_provider:
 * @vm: the Java VM to create this provider in
 * @api_authorization: The HAFAS authorization object
 *
 * Creates the oebb provider.
 * Returns: (transfer full): the oebb provider
 */
GpteProvider* gpte_create_oebb_provider(GpteJvm* vm, const gchar* api_authorization);

/**
 * gpte_create_ontario_provider:
 * @vm: the Java VM to create this provider in
 * @authorization: The Navitia authorization token
 *
 * Creates the ontario provider from [Navitia](https://navitia.io/).
 * Returns: (transfer full): the ontario provider
 */
GpteProvider* gpte_create_ontario_provider(GpteJvm* vm, const gchar* authorization);

/**
 * gpte_create_ooevv_provider:
 * @vm: the Java VM to create this provider in
 * @api_authorization: The HAFAS authorization object
 *
 * Creates the ooevv provider.
 * Returns: (transfer full): the ooevv provider
 */
GpteProvider* gpte_create_ooevv_provider(GpteJvm* vm, const gchar* api_authorization);

/**
 * gpte_create_oregon_provider:
 * @vm: the Java VM to create this provider in
 * @authorization: The Navitia authorization token
 *
 * Creates the oregon provider from [Navitia](https://navitia.io/).
 * Returns: (transfer full): the oregon provider
 */
GpteProvider* gpte_create_oregon_provider(GpteJvm* vm, const gchar* authorization);

/**
 * gpte_create_paris_provider:
 * @vm: the Java VM to create this provider in
 * @authorization: The Navitia authorization token
 *
 * Creates the paris provider from [Navitia](https://navitia.io/).
 * Returns: (transfer full): the paris provider
 */
GpteProvider* gpte_create_paris_provider(GpteJvm* vm, const gchar* authorization);

/**
 * gpte_create_pl_navitia_provider:
 * @vm: the Java VM to create this provider in
 * @authorization: The Navitia authorization token
 *
 * Creates the pl_navitia provider from [Navitia](https://navitia.io/).
 * Returns: (transfer full): the pl_navitia provider
 */
GpteProvider* gpte_create_pl_navitia_provider(GpteJvm* vm, const gchar* authorization);

/**
 * gpte_create_pl_provider:
 * @vm: the Java VM to create this provider in
 * @api_authorization: The HAFAS authorization object
 *
 * Creates the pl provider.
 * Returns: (transfer full): the pl provider
 */
GpteProvider* gpte_create_pl_provider(GpteJvm* vm, const gchar* api_authorization);

/**
 * gpte_create_quebec_provider:
 * @vm: the Java VM to create this provider in
 * @authorization: The Navitia authorization token
 *
 * Creates the quebec provider from [Navitia](https://navitia.io/).
 * Returns: (transfer full): the quebec provider
 */
GpteProvider* gpte_create_quebec_provider(GpteJvm* vm, const gchar* authorization);

/**
 * gpte_create_rta_chicago_provider:
 * @vm: the Java VM to create this provider in
 *
 * Creates the rta_chicago provider.
 * Returns: (transfer full): the rta_chicago provider
 */
GpteProvider* gpte_create_rta_chicago_provider(GpteJvm* vm);

/**
 * gpte_create_rt_provider:
 * @vm: the Java VM to create this provider in
 *
 * Creates the rt provider.
 * Returns: (transfer full): the rt provider
 */
GpteProvider* gpte_create_rt_provider(GpteJvm* vm);

/**
 * gpte_create_se_provider:
 * @vm: the Java VM to create this provider in
 * @api_authorization: The HAFAS authorization object
 *
 * Creates the se provider.
 * Returns: (transfer full): the se provider
 */
GpteProvider* gpte_create_se_provider(GpteJvm* vm, const gchar* api_authorization);

/**
 * gpte_create_sh_provider:
 * @vm: the Java VM to create this provider in
 * @api_authorization: The HAFAS authorization object
 *
 * Creates the sh provider.
 * Returns: (transfer full): the sh provider
 */
GpteProvider* gpte_create_sh_provider(GpteJvm* vm, const gchar* api_authorization);

/**
 * gpte_create_spain_provider:
 * @vm: the Java VM to create this provider in
 * @authorization: The Navitia authorization token
 *
 * Creates the spain provider from [Navitia](https://navitia.io/).
 * Returns: (transfer full): the spain provider
 */
GpteProvider* gpte_create_spain_provider(GpteJvm* vm, const gchar* authorization);

/**
 * gpte_create_stv_provider:
 * @vm: the Java VM to create this provider in
 *
 * Creates the stv provider.
 * Returns: (transfer full): the stv provider
 */
GpteProvider* gpte_create_stv_provider(GpteJvm* vm);

/**
 * gpte_create_svv_provider:
 * @vm: the Java VM to create this provider in
 * @api_authorization: The HAFAS authorization object
 *
 * Creates the svv provider.
 * Returns: (transfer full): the svv provider
 */
GpteProvider* gpte_create_svv_provider(GpteJvm* vm, const gchar* api_authorization);

/**
 * gpte_create_sydney_provider:
 * @vm: the Java VM to create this provider in
 *
 * Creates the sydney provider.
 * Returns: (transfer full): the sydney provider
 */
GpteProvider* gpte_create_sydney_provider(GpteJvm* vm);

/**
 * gpte_create_tlem_provider:
 * @vm: the Java VM to create this provider in
 *
 * Creates the tlem provider.
 * Returns: (transfer full): the tlem provider
 */
GpteProvider* gpte_create_tlem_provider(GpteJvm* vm);

/**
 * gpte_create_vao_provider:
 * @vm: the Java VM to create this provider in
 * @api_authorization: The HAFAS authorization object
 *
 * Creates the vao provider.
 * Returns: (transfer full): the vao provider
 */
GpteProvider* gpte_create_vao_provider(GpteJvm* vm, const gchar* api_authorization);

/**
 * gpte_create_vbb_provider:
 * @vm: the Java VM to create this provider in
 * @api_authorization: The HAFAS authorization object
 * @salt: (array length=salt_len): the salt blob passed to HAFAS
 * @salt_len: the langth of @salt in bytes
 *
 * Creates the vbb provider.
 * Returns: (transfer full): the vbb provider
 */
GpteProvider* gpte_create_vbb_provider(GpteJvm* vm, const gchar* api_authorization, const guchar* salt, gsize salt_len);

/**
 * gpte_create_vbl_provider:
 * @vm: the Java VM to create this provider in
 *
 * Creates the vbl provider.
 * Returns: (transfer full): the vbl provider
 */
GpteProvider* gpte_create_vbl_provider(GpteJvm* vm);

/**
 * gpte_create_vbn_provider:
 * @vm: the Java VM to create this provider in
 * @api_authorization: The HAFAS authorization object
 * @salt: (array length=salt_len): the salt blob passed to HAFAS
 * @salt_len: the langth of @salt in bytes
 *
 * Creates the vbn provider.
 * Returns: (transfer full): the vbn provider
 */
GpteProvider* gpte_create_vbn_provider(GpteJvm* vm, const gchar* api_authorization, const guchar* salt, gsize salt_len);

/**
 * gpte_create_vgn_provider:
 * @vm: the Java VM to create this provider in
 *
 * Creates the vgn provider.
 * Returns: (transfer full): the vgn provider
 */
GpteProvider* gpte_create_vgn_provider(GpteJvm* vm);

/**
 * gpte_create_vgs_provider:
 * @vm: the Java VM to create this provider in
 * @api_authorization: The HAFAS authorization object
 * @salt: (array length=salt_len): the salt blob passed to HAFAS
 * @salt_len: the langth of @salt in bytes
 *
 * Creates the vgs provider.
 * Returns: (transfer full): the vgs provider
 */
GpteProvider* gpte_create_vgs_provider(GpteJvm* vm, const gchar* api_authorization, const guchar* salt, gsize salt_len);

/**
 * gpte_create_vmobil_provider:
 * @vm: the Java VM to create this provider in
 * @api_authorization: The HAFAS authorization object
 *
 * Creates the vmobil provider.
 * Returns: (transfer full): the vmobil provider
 */
GpteProvider* gpte_create_vmobil_provider(GpteJvm* vm, const gchar* api_authorization);

/**
 * gpte_create_vmt_provider:
 * @vm: the Java VM to create this provider in
 * @api_authorization: The HAFAS authorization object
 *
 * Creates the vmt provider.
 * Returns: (transfer full): the vmt provider
 */
GpteProvider* gpte_create_vmt_provider(GpteJvm* vm, const gchar* api_authorization);

/**
 * gpte_create_vmv_provider:
 * @vm: the Java VM to create this provider in
 *
 * Creates the vmv provider.
 * Returns: (transfer full): the vmv provider
 */
GpteProvider* gpte_create_vmv_provider(GpteJvm* vm);

/**
 * gpte_create_vor_provider:
 * @vm: the Java VM to create this provider in
 * @api_authorization: The HAFAS authorization object
 *
 * Creates the vor provider.
 * Returns: (transfer full): the vor provider
 */
GpteProvider* gpte_create_vor_provider(GpteJvm* vm, const gchar* api_authorization);

/**
 * gpte_create_vrn_provider:
 * @vm: the Java VM to create this provider in
 *
 * Creates the vrn provider.
 * Returns: (transfer full): the vrn provider
 */
GpteProvider* gpte_create_vrn_provider(GpteJvm* vm);

/**
 * gpte_create_vrr_provider:
 * @vm: the Java VM to create this provider in
 *
 * Creates the vrr provider.
 * Returns: (transfer full): the vrr provider
 */
GpteProvider* gpte_create_vrr_provider(GpteJvm* vm);


/**
 * gpte_create_vrs_provider:
 * @vm: the Java VM to create this provider in
 * @client_cert: (array length=client_cert_len): client certificate blob
 * @client_cert_len: length of  in bytes
 *
 * Creates the vrs provider.
 * Returns: (transfer full): the vrs provider
 */
GpteProvider* gpte_create_vrs_provider(GpteJvm* vm, const guchar* client_cert, gsize client_cert_len);


/**
 * gpte_create_vvm_provider:
 * @vm: the Java VM to create this provider in
 *
 * Creates the vvm provider.
 * Returns: (transfer full): the vvm provider
 */
GpteProvider* gpte_create_vvm_provider(GpteJvm* vm);

/**
 * gpte_create_vvo_provider:
 * @vm: the Java VM to create this provider in
 *
 * Creates the vvo provider.
 * Returns: (transfer full): the vvo provider
 */
GpteProvider* gpte_create_vvo_provider(GpteJvm* vm);

/**
 * gpte_create_vvs_provider:
 * @vm: the Java VM to create this provider in
 *
 * Creates the vvs provider.
 * Returns: (transfer full): the vvs provider
 */
GpteProvider* gpte_create_vvs_provider(GpteJvm* vm);

/**
 * gpte_create_vvt_provider:
 * @vm: the Java VM to create this provider in
 * @api_authorization: The HAFAS authorization object
 *
 * Creates the vvt provider.
 * Returns: (transfer full): the vvt provider
 */
GpteProvider* gpte_create_vvt_provider(GpteJvm* vm, const gchar* api_authorization);

/**
 * gpte_create_vvv_provider:
 * @vm: the Java VM to create this provider in
 *
 * Creates the vvv provider.
 * Returns: (transfer full): the vvv provider
 */
GpteProvider* gpte_create_vvv_provider(GpteJvm* vm);

/**
 * gpte_create_wien_provider:
 * @vm: the Java VM to create this provider in
 *
 * Creates the wien provider.
 * Returns: (transfer full): the wien provider
 */
GpteProvider* gpte_create_wien_provider(GpteJvm* vm);

/**
 * gpte_create_zvv_provider:
 * @vm: the Java VM to create this provider in
 * @api_authorization: The HAFAS authorization object
 *
 * Creates the zvv provider.
 * Returns: (transfer full): the zvv provider
 */
GpteProvider* gpte_create_zvv_provider(GpteJvm* vm, const gchar* api_authorization);

// END GPTE PROVIDERS SECTION

G_END_DECLS

#endif // __GPTEPROVIDERS_H__
