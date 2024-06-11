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

docdir = get_option('datadir') / 'doc'
docgen = find_program('gi-docgen', native: true, required: get_option('docs'))

if not get_option('docs').disabled() and docgen.found()
	if not get_option('introspection')
		error('introspection must be enabled to generate documentation')
	endif

	toml_conf = configuration_data()
	toml_conf.set('version', meson.project_version())

	gptedoc_toml = configure_file(
		input: 'gpte.toml.in',
		output: 'gpte.toml',
		configuration: toml_conf,
	)

	gptedoc = custom_target('gptedoc',
		input: [gptedoc_toml, gpte_introspection[0]],
		depend_files: [
			'gpte/debugging.md',
			'gpte/threading.md'
		],
		output: 'gpte',
		command: [
			docgen, 'generate',
			'--quiet', '--no-namespace-dir', '--fatal-warnings',
			'--config=@INPUT0@',
			'--output-dir=@OUTPUT@',
			'--content-dir=@0@'.format(meson.current_source_dir() / 'gpte'),
			'@INPUT1@'
		],

		install: true,
		install_dir: docdir
	)


	gptegtkdoc_toml = configure_file(
		input: 'gptegtk.toml.in',
		output: 'gptegtk.toml',
		configuration: toml_conf,
	)

	gptegtkdoc = custom_target('gptegtkdoc',
		input: [gptegtkdoc_toml, gpte_gtk_introspection[0]],
		output: 'gptegtk',
		command: [
			docgen, 'generate',
			'--quiet', '--no-namespace-dir', '--fatal-warnings',
			'--config=@INPUT0@',
			'--output-dir=@OUTPUT@',
			'--content-dir=@0@'.format(meson.current_source_dir() / 'gptegtk'),
			'--add-include-path=@0@'.format(fs.parent(gpte_introspection[0].full_path())),
			'@INPUT1@'
		],
		install: true,
		install_dir: docdir
	)
endif
