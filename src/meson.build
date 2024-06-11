# gpte - GObject bindings for public-transport-enabler
# Copyright (C) 2024  Florian "sp1rit" <sp1rit@disroot.org>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

gpte_api_ver = '0'

gpte_src = [
	'gpteerrors.c',
	'gptejvm.c',
	'gptejavaobject.c',
	'gptelist.c',
	'gpteutils.c',

	'gptegeo.c',
	'gpteproducts.c',
	'gptestyle.c',
	'gptelocation.c',
	'gpteline.c',
	'gptedeparture.c',
	'gptestationdepartures.c',
	'gptestop.c',
	'gptetrip.c',
	'gptefare.c',
	'gptetripleg.c',
	'gptetrips.c',
	'gpteprovider.c',

	'gpteproviders.c'
]

gpte_headers = [
	'gpteerrors.h',
	'gptejvm.h',
	'gptejavaobject.h',
	'gptelist.h',

	'gptegeo.h',
	'gpteproducts.h',
	'gptestyle.h',
	'gptelocation.h',
	'gpteline.h',
	'gptedeparture.h',
	'gptestationdepartures.h',
	'gptestop.h',
	'gptetrip.h',
	'gptefare.h',
	'gptetripleg.h',
	'gptetrips.h',
	'gpteprovider.h',

	'gpteproviders.h'
]

gpte_public_pkgs = [
	'gio-2.0'
]
gpte_public_deps = []
foreach dep : gpte_public_pkgs
	gpte_public_deps += dependency(dep)
endforeach

run_command(javac.cmd_array(),
	'-d', meson.current_build_dir() / 'GetJavaHome.p',
	meson.project_source_root() / 'build-aux' / 'GetJavaHome.java',
	check: true
)
# meson doesn't provide a way to run java software. This is a bit
# of a hack that makes the assumption that by just replacing javac
# with java makes it work.
java_invoc = []
foreach arg : javac.cmd_array()
	java_invoc += arg.replace('javac', 'java')
endforeach
java_home = run_command(
	java_invoc,
	'-cp', meson.current_build_dir() / 'GetJavaHome.p',
	'GetJavaHome',
	capture: true,
	check: true
).stdout().strip()

if host_machine.system() == 'windows'
	jvm_rpath = java_home / 'lib'
else
	if javac.version().version_compare('<= 1.8.0')
		arch = host_machine.cpu_family()
		if arch == 'x86_64'
			arch = 'amd64'
		endif
		jvm_rpath = java_home / 'lib' / arch / 'server'
	else
		jvm_rpath = java_home / 'lib' / 'server'
	endif
endif

gpte_dep_sources = []
gpte_lib = library('gpte', gpte_src, gpte_res,
	soversion: gpte_api_ver,
	dependencies: [
		gpte_public_deps,
		dependency('jni', version: '>= 1.8.0', modules: ['jvm']),
	],
	build_rpath: jvm_rpath,
	install_rpath: jvm_rpath,
	install: true
)

install_headers(gpte_headers, subdir: 'gpte')

pkg.generate(gpte_lib,
	filebase: 'gpte@0@'.format(gpte_api_ver),
	description: 'Unleash public transport data',
	requires: gpte_public_pkgs,
	subdirs: 'gpte'
)

if get_option('introspection')
	gpte_introspection = gnome.generate_gir(gpte_lib,
		sources: [gpte_headers],
		includes: [
			'GLib-2.0',
			'GObject-2.0',
			'Gio-2.0'
		],
		dependencies: gpte_public_deps,
		namespace: 'Gpte',
		nsversion: gpte_api_ver,
		install: true
	)

	gpte_dep_sources += gpte_introspection
endif

gpte_dep = declare_dependency(
	sources: gpte_dep_sources,
	link_with: gpte_lib,
	dependencies: gpte_public_deps,
	include_directories: include_directories('.')
)
meson.override_dependency('gpte@0@'.format(gpte_api_ver), gpte_dep)
