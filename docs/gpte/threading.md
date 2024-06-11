Title: Multithreading
SPDX-License-Identifier: GPL-3.0-or-later
SPDX-FileCopyrightText: 2024  Florian "sp1rit" <sp1rit@disroot.org>

By default, you may only call gpte methods on the same thread that
called [func@Gpte.Jvm.create]. Instead, its recommended that you use
the `_async` methods of [class@Gpte.Provider] and [class@Gpte.Trips]
which sets up threading itself and calls you back on the main thread,
but this requires  a running [struct@GLib.MainLoop] for it. 

If still have the need to access gpte from more than one thread youself,
each additional thread must hold a [struct@Gpte.ThreadGuard] received
from [method@Gpte.Jvm.attach_thread] while its calling gpte methods.

Take special caution when using a garbage collector, as the
[struct@Gpte.ThreadGuard] might be freed prematurely. Ensure that the
no-op [method@Gpte.ThreadGuard.ping] gets called once the thread won't
call any gpte methods anymore.

::: important "Thread Safety"
	Do not attempt to create a [struct@Gpte.ThreadGuard] for the initial
	thread or multiple [struct@Gpte.ThreadGuard] per thread, as
	*bad things* might happen otherwise.

	Also, a [struct@Gpte.ThreadGuard] is not thread safe in any way, do
	not attempt to transfer them between threads.
