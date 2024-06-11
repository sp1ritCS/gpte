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

project('gpte', 'c', 'java',
	version: '0.1.0',
	default_options: ['warning_level=2', 'c_std=gnu2x']
)
add_project_arguments('-D_GNU_SOURCE', language: 'c')

cc = meson.get_compiler('c')
javac = meson.get_compiler('java')
gnome = import('gnome')
fs = import('fs')
pkg = import('pkgconfig')

subdir('data')
subdir('src')
subdir('gtk')

subdir('docs')
