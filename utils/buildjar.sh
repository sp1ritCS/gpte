#!/bin/bash

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

set -e

usage() {
	echo "Usage: $0 [-o <output.jar>] <PTE checkout dir>" >> /dev/stderr;
	exit 1;
}

CLASSPATH_SEPERATOR=":"

OUTPUT="./dist.jar"
while getopts ":o:" o; do
    case "${o}" in
        o)
            OUTPUT=${OPTARG}
            ;;
        *)
            usage
            ;;
    esac
done

shift $((OPTIND-1))

if [ -z "$1" ]; then usage; fi

if [ ! -f "./JDEPS" ]; then
	echo "Missing JDEPS file in CWD" >> /dev/stderr
	exit 1
fi

DISTDIR=`mktemp -d`
CLASSPATH=`tr '\n' "$CLASSPATH_SEPERATOR" < JDEPS`

javac -d "$DISTDIR" -cp "$CLASSPATH" `find "$1/src/" -name '*.java' | tr '\n' ' '`

while read dep; do
	unzip -u "$dep" -d "$DISTDIR" -x "META-INF/MANIFEST.MF"
done < JDEPS

cat > "$DISTDIR/META-INF/MANIFEST.MF" << EOF
Manifest-Version: 1.0
Automatic-Module-Name: de.schildbach.pte
Archiver-Version: GPTE buildjar 0.1.0
Built-By: Florian "sp1rit" <sp1rit@disroot.org>
Created-By: $(id -un)
Build-Jdk: $(javac --version | awk '{ print $2 }')
EOF

rm -f "$OUTPUT"
jar --create --file "$OUTPUT" --manifest "$DISTDIR/META-INF/MANIFEST.MF" -C "$DISTDIR" .

rm -rf "$DISTDIR"
