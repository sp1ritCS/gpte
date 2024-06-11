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

gpte_gtk_api_ver = '0'

gpte_gtk_src = [
	'gptegtkbin.c',
	'gptegtksearchentry.c',
	'gptegtkprovidericon.c',
	'gptegtklinewidget.c',

	'contrib/gptegtklinewidgetutils.c',
	'contrib/gptegtklinewidgetquartzite.c',
	'contrib/gptegtklinewidgetmarble.c',
]

gpte_gtk_headers = [
	'gptegtkbin.h',
	'gptegtklinewidget.h',
	'gptegtkprovidericon.h',
	'gptegtksearchentry.h',

	'contrib/gptegtklinewidgetquartzite.h',
	'contrib/gptegtklinewidgetmarble.h',
]

gpte_gtk_inc_dirs = [
	include_directories('.'),
	include_directories('contrib')
]

gpte_gtk_public_pkgs = [
	'gtk4'
]
gpte_gtk_public_deps = [ gpte_dep ]
foreach dep : gpte_gtk_public_pkgs
	gpte_gtk_public_deps += dependency(dep)
endforeach

gpte_gtk_public_pkgs += 'gpte@0@ >= @1@'.format(gpte_api_ver, meson.project_version())

gpte_gtk_dep_sources = []
gpte_gtk_lib = library('gptegtk', gpte_gtk_src, gpte_gtk_res,
	soversion: gpte_gtk_api_ver,
	include_directories: gpte_gtk_inc_dirs,
	dependencies: [
		gpte_gtk_public_deps,
		cc.find_library('m', required: false)
	],
	install: true
)

install_headers(gpte_gtk_headers, subdir: 'gpte/gtk/', preserve_path: true)

pkg.generate(gpte_gtk_lib,
	filebase: 'gpte-gtk@0@'.format(gpte_gtk_api_ver),
	description: 'GTK utilities for GPTE',
	requires: gpte_gtk_public_pkgs,
	subdirs: [ 'gpte/gtk', 'gpte/gtk/contrib' ]
)

if get_option('introspection')
	gpte_gtk_introspection = gnome.generate_gir(gpte_gtk_lib,
		sources: [gpte_gtk_headers],
		includes: [
			gpte_introspection[0],

			'GLib-2.0',
			'GObject-2.0',
			'Gio-2.0',
			'Gtk-4.0'
		],
		dependencies: gpte_gtk_public_deps,
		namespace: 'GpteGtk',
		nsversion: gpte_gtk_api_ver,
		install: true
	)
	gpte_gtk_dep_sources += gpte_gtk_introspection
endif

gpte_gtk_dep = declare_dependency(
	sources: gpte_gtk_dep_sources,
	link_with: gpte_gtk_lib,
	dependencies: gpte_gtk_public_deps,
	include_directories: gpte_gtk_inc_dirs
)
meson.override_dependency('gpte-gtk@0@'.format(gpte_gtk_api_ver), gpte_gtk_dep)
