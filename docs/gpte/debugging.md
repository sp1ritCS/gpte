Title: Debugging with GPTE
SPDX-License-Identifier: GPL-3.0-or-later
SPDX-FileCopyrightText: 2024  Florian "sp1rit" <sp1rit@disroot.org>

Using the `GPTE_DEBUG` environment variable, the behaviour of the JVM
can be tweaked. It can contain multiple, comma seperated, options:

- `gdb`: Register GDB as error handler, spawning gdb once the JVM
	encountered a fatal error.
- `xgdb`: Like `gdb`, but force open a new
	[xterm](https://invisible-island.net/xterm/xterm.html) instance with
	the debugger instead of the using the the preexisting terminal.
- `verbose`: Log verbose JNI output
- `interpreter`: Disable JIT compilation for JVM code
- `help`: Show all available options and their description

::: important "Debugging with gdb"
	Once [func@Gpte.Jvm.create] has been called, it's not possible to
	debug an application normally anymore. This is due to Java "abusing"
	the SIGSEGV signal for internal purposes, making it impossible to
	debug. By setting `GPTE_DEBUG` to `gdb`, like described above, Java
	will call `gdb -p $$` once it encountered a segfault it did not
	induce itself, allowing you to debug your application.

	If you need to catch other signals, like `SIGTRAP` occurring from
	criticals/warnings when `G_DEBUG=fatal-criticals/fatal-warnings` is
	set, you must tell gdb to pass `SIGSEGV` to the application using:
	`handle SIGSEGV nostop noprint pass` before running the application
	in the debugger.
