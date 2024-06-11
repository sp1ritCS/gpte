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

#include "gpteproviders.h"
#include "gpteprovider-priv.h"

#define GPTE_CREATE_NAVITIA_PROVIDER_FUN(name, class) \
	GpteProvider* gpte_create_##name##_provider(GpteJvm* vm, const gchar* authorization) { \
		g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 4); \
		jclass provider_class = (*env)->FindClass(env, "de/schildbach/pte/" class); \
		jmethodID constructor = (*env)->GetMethodID(env, provider_class, "<init>", "(Ljava/lang/String;)V"); \
		jstring authorization_str = (*env)->NewStringUTF(env, authorization); \
		jobject object = (*env)->NewObject(env, provider_class, constructor, authorization_str); \
		return gpte_provider_new(#name, vm, object); \
	}
#define GPTE_CREATE_HAFAS_PROVIDER_FUN(name, class) \
	GpteProvider* gpte_create_##name##_provider(GpteJvm* vm, const gchar* api_authorization) { \
		g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 4); \
		jclass provider_class = (*env)->FindClass(env, "de/schildbach/pte/" class); \
		jmethodID constructor = (*env)->GetMethodID(env, provider_class, "<init>", "(Ljava/lang/String;)V"); \
		jstring authorization_str = (*env)->NewStringUTF(env, api_authorization); \
		jobject object = (*env)->NewObject(env, provider_class, constructor, authorization_str); \
		return gpte_provider_new(#name, vm, object); \
	}
#define GPTE_CREATE_EFA_PROVIDER_FUN(name, class) \
	GpteProvider* gpte_create_##name##_provider(GpteJvm* vm) { \
		g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 3); \
		jclass provider_class = (*env)->FindClass(env, "de/schildbach/pte/" class); \
		jmethodID constructor = (*env)->GetMethodID(env, provider_class, "<init>", "()V"); \
		jobject object = (*env)->NewObject(env, provider_class, constructor); \
		return gpte_provider_new(#name, vm, object); \
	}
#define GPTE_CREATE_HAFAS_COMPLEX_PROVIDER_FUN(name, class) \
	GpteProvider* gpte_create_##name##_provider(GpteJvm* vm, const gchar* api_authorization, const guchar* salt, gsize salt_len) { \
		g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 5); \
		jclass provider_class = (*env)->FindClass(env, "de/schildbach/pte/" class); \
		jmethodID constructor = (*env)->GetMethodID(env, provider_class, "<init>", "(Ljava/lang/String;[B)V"); \
		jstring authorization_str = (*env)->NewStringUTF(env, api_authorization); \
		jbyteArray jsalt = (*env)->NewByteArray(env, salt_len); \
		(*env)->SetByteArrayRegion(env, jsalt, 0, salt_len, (const jbyte*)salt); \
		jobject object = (*env)->NewObject(env, provider_class, constructor, authorization_str, jsalt); \
		return gpte_provider_new(#name, vm, object); \
	}
#define GPTE_CREATE_HAFAS_LEGACY_PROVIDER_FUN(name, class) \
	GpteProvider* gpte_create_##name##_provider(GpteJvm* vm) { \
		g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 3); \
		jclass provider_class = (*env)->FindClass(env, "de/schildbach/pte/" class); \
		jmethodID constructor = (*env)->GetMethodID(env, provider_class, "<init>", "()V"); \
		jobject object = (*env)->NewObject(env, provider_class, constructor); \
		return gpte_provider_new(#name, vm, object); \
	}

// BEGIN GPTE PROVIDERS SECTION

GPTE_CREATE_NAVITIA_PROVIDER_FUN(australia, "AustraliaProvider")
GPTE_CREATE_HAFAS_PROVIDER_FUN(avv_aachen, "AvvAachenProvider")
GPTE_CREATE_HAFAS_PROVIDER_FUN(avv_augsburg, "AvvAugsburgProvider")
GPTE_CREATE_HAFAS_PROVIDER_FUN(bart, "BartProvider")
GPTE_CREATE_EFA_PROVIDER_FUN(bayern, "BayernProvider")
GPTE_CREATE_NAVITIA_PROVIDER_FUN(brazil, "BrazilProvider")
GPTE_CREATE_NAVITIA_PROVIDER_FUN(british_columbia, "BritishColumbiaProvider")
GPTE_CREATE_EFA_PROVIDER_FUN(bsvag, "BsvagProvider")
GPTE_CREATE_HAFAS_PROVIDER_FUN(bvg, "BvgProvider")
GPTE_CREATE_HAFAS_PROVIDER_FUN(cmta, "CmtaProvider")
GPTE_CREATE_NAVITIA_PROVIDER_FUN(czech_republic, "CzechRepublicProvider")
GPTE_CREATE_HAFAS_COMPLEX_PROVIDER_FUN(db, "DbProvider")
GPTE_CREATE_EFA_PROVIDER_FUN(ding, "DingProvider")
GPTE_CREATE_HAFAS_PROVIDER_FUN(dsb, "DsbProvider")
GPTE_CREATE_EFA_PROVIDER_FUN(dub, "DubProvider")
GPTE_CREATE_HAFAS_LEGACY_PROVIDER_FUN(eireann, "EireannProvider")
GPTE_CREATE_NAVITIA_PROVIDER_FUN(finland, "FinlandProvider")
GPTE_CREATE_NAVITIA_PROVIDER_FUN(france_ne, "FranceNorthEastProvider")
GPTE_CREATE_NAVITIA_PROVIDER_FUN(france_nw, "FranceNorthWestProvider")
GPTE_CREATE_NAVITIA_PROVIDER_FUN(france_se, "FranceSouthEastProvider")
GPTE_CREATE_NAVITIA_PROVIDER_FUN(france_sw, "FranceSouthWestProvider")
GPTE_CREATE_NAVITIA_PROVIDER_FUN(ghana, "GhanaProvider")
GPTE_CREATE_EFA_PROVIDER_FUN(gvh, "GvhProvider")
GPTE_CREATE_HAFAS_COMPLEX_PROVIDER_FUN(invg, "InvgProvider")
GPTE_CREATE_NAVITIA_PROVIDER_FUN(italy, "ItalyProvider")
GPTE_CREATE_EFA_PROVIDER_FUN(kvv, "KvvProvider")
GPTE_CREATE_EFA_PROVIDER_FUN(linz, "LinzProvider")
GPTE_CREATE_HAFAS_PROVIDER_FUN(lu, "LuProvider")
GPTE_CREATE_NAVITIA_PROVIDER_FUN(massachusetts, "MassachusettsProvider")
GPTE_CREATE_EFA_PROVIDER_FUN(mersey, "MerseyProvider")
GPTE_CREATE_EFA_PROVIDER_FUN(mvg, "MvgProvider")
GPTE_CREATE_EFA_PROVIDER_FUN(mvv, "MvvProvider")
GPTE_CREATE_HAFAS_PROVIDER_FUN(nasa, "NasaProvider")

GpteProvider* gpte_create_negentwee_provider(GpteJvm* vm, GpteNegentweeLanguage language) {
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 7);
	jclass provider_class = (*env)->FindClass(env, "de/schildbach/pte/NegentweeProvider");
	jmethodID constructor = (*env)->GetMethodID(env, provider_class, "<init>", "(Lde/schildbach/pte/NegentweeProvider$Language;)V");

	jclass language_class = (*env)->FindClass(env, "de/schildbach/pte/NegentweeProvider$Language");
	jobject jlanguage = language == GPTE_NEGENTWEE_LANG_NL_NL ?
		(*env)->GetStaticObjectField(env, language_class, (*env)->GetStaticFieldID(env, language_class, "NL_NL", "Lde/schildbach/pte/NegentweeProvider$Language;")) :
		(*env)->GetStaticObjectField(env, language_class, (*env)->GetStaticFieldID(env, language_class, "EN_GB", "Lde/schildbach/pte/NegentweeProvider$Language;"));

	jobject object = (*env)->NewObject(env, provider_class, constructor, jlanguage);
	return gpte_provider_new("negentwee", vm, object);
}

GPTE_CREATE_NAVITIA_PROVIDER_FUN(nicaragua, "NicaraguaProvider")
GPTE_CREATE_HAFAS_LEGACY_PROVIDER_FUN(ns, "NsProvider")
GPTE_CREATE_EFA_PROVIDER_FUN(nvbw, "NvbwProvider")
GPTE_CREATE_HAFAS_PROVIDER_FUN(nvv, "NvvProvider")
GPTE_CREATE_NAVITIA_PROVIDER_FUN(nz, "NzProvider")
GPTE_CREATE_HAFAS_PROVIDER_FUN(oebb, "OebbProvider")
GPTE_CREATE_NAVITIA_PROVIDER_FUN(ontario, "OntarioProvider")
GPTE_CREATE_HAFAS_PROVIDER_FUN(ooevv, "OoevvProvider")
GPTE_CREATE_NAVITIA_PROVIDER_FUN(oregon, "OregonProvider")
GPTE_CREATE_NAVITIA_PROVIDER_FUN(paris, "ParisProvider")
GPTE_CREATE_NAVITIA_PROVIDER_FUN(pl_navitia, "PlNavitiaProvider")
GPTE_CREATE_HAFAS_PROVIDER_FUN(pl, "PlProvider")
GPTE_CREATE_NAVITIA_PROVIDER_FUN(quebec, "QuebecProvider")
GPTE_CREATE_EFA_PROVIDER_FUN(rta_chicago, "RtaChicagoProvider")
GPTE_CREATE_HAFAS_LEGACY_PROVIDER_FUN(rt, "RtProvider")
GPTE_CREATE_HAFAS_PROVIDER_FUN(se, "SeProvider")
GPTE_CREATE_HAFAS_PROVIDER_FUN(sh, "ShProvider")
GPTE_CREATE_NAVITIA_PROVIDER_FUN(spain, "SpainProvider")
GPTE_CREATE_EFA_PROVIDER_FUN(stv, "StvProvider")
GPTE_CREATE_HAFAS_PROVIDER_FUN(svv, "SvvProvider")
GPTE_CREATE_EFA_PROVIDER_FUN(sydney, "SydneyProvider")
GPTE_CREATE_EFA_PROVIDER_FUN(tlem, "TlemProvider")
GPTE_CREATE_HAFAS_PROVIDER_FUN(vao, "VaoProvider")
GPTE_CREATE_HAFAS_COMPLEX_PROVIDER_FUN(vbb, "VbbProvider")
GPTE_CREATE_EFA_PROVIDER_FUN(vbl, "VblProvider")
GPTE_CREATE_HAFAS_COMPLEX_PROVIDER_FUN(vbn, "VbnProvider")
GPTE_CREATE_EFA_PROVIDER_FUN(vgn, "VgnProvider")
GPTE_CREATE_HAFAS_COMPLEX_PROVIDER_FUN(vgs, "VgsProvider")
GPTE_CREATE_HAFAS_PROVIDER_FUN(vmobil, "VmobilProvider")
GPTE_CREATE_HAFAS_PROVIDER_FUN(vmt, "VmtProvider")
GPTE_CREATE_EFA_PROVIDER_FUN(vmv, "VmvProvider")
GPTE_CREATE_HAFAS_PROVIDER_FUN(vor, "VorProvider")
GPTE_CREATE_EFA_PROVIDER_FUN(vrn, "VrnProvider")
GPTE_CREATE_EFA_PROVIDER_FUN(vrr, "VrrProvider")

GpteProvider* gpte_create_vrs_provider(GpteJvm* vm, const guchar* client_cert, gsize client_cert_len) {
	g_auto(GpteScopeGuard) env = gpte_jvm_enter_scope(vm, 4);
	jclass provider_class = (*env)->FindClass(env, "de/schildbach/pte/VrsProvider");
	jmethodID constructor = (*env)->GetMethodID(env, provider_class, "<init>", "([B)V");
	jbyteArray jccert = (*env)->NewByteArray(env, client_cert_len);
	(*env)->SetByteArrayRegion(env, jccert, 0, client_cert_len, (const jbyte*)client_cert);
	jobject object = (*env)->NewObject(env, provider_class, constructor, jccert);
	return gpte_provider_new("vrs", vm, object);
}

GPTE_CREATE_EFA_PROVIDER_FUN(vvm, "VvmProvider")
GPTE_CREATE_EFA_PROVIDER_FUN(vvo, "VvoProvider")
GPTE_CREATE_EFA_PROVIDER_FUN(vvs, "VvsProvider")
GPTE_CREATE_HAFAS_PROVIDER_FUN(vvt, "VvtProvider")
GPTE_CREATE_EFA_PROVIDER_FUN(vvv, "VvvProvider")
GPTE_CREATE_EFA_PROVIDER_FUN(wien, "WienProvider")
GPTE_CREATE_HAFAS_PROVIDER_FUN(zvv, "ZvvProvider")

// END GPTE PROVIDERS SECTION
