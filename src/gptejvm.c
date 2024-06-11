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

#include "gptejvm.h"
#include "gptejvm-priv.h"

#include "gpteerrors.h"

#include <gio/gio.h>
#include "gpte_res.h"

#include <sys/mman.h>

G_DEFINE_BOXED_TYPE(GpteThreadGuard, gpte_thread_guard, gpte_thread_guard_ref, gpte_thread_guard_unref)

struct _GpteThreadGuard {
	grefcount rc;
	GpteJvm* vm;
};

static GpteThreadGuard* gpte_thread_guard_create(GpteJvm* vm, const gchar* name) {
	JNIEnv* thread;
	gint rc = (*vm->vm)->AttachCurrentThread(vm->vm, (void**)&thread, &(JavaVMAttachArgs){
		.version = JNI_VERSION_21,
		.name = (gchar*)name,
		.group = NULL
	});
	g_return_val_if_fail(rc == JNI_OK, NULL);
	GpteThreadGuard* self = g_new(GpteThreadGuard, 1);
	g_ref_count_init(&self->rc);
	self->vm = gpte_jvm_ref(vm);
	return self;
}

GpteThreadGuard* gpte_thread_guard_ref(GpteThreadGuard* self) {
	g_ref_count_inc(&self->rc);
	return self;
}

void gpte_thread_guard_unref(GpteThreadGuard* self) {
	if (!self)
		return;
	if (g_ref_count_dec(&self->rc)) {
		gint rc = (*self->vm->vm)->DetachCurrentThread(self->vm->vm);
		if (rc != JNI_OK)
			g_critical("Gpte.ThreadGuard was unable to detach thread (error %d)", rc);
		gpte_jvm_unref(self->vm);
		g_free(self);
	}
}

void gpte_thread_guard_ping(GpteThreadGuard*) {}

void gpte_scope_guard_leave(GpteScopeGuard* self) {
	(**self)->PopLocalFrame(*self, NULL);
}
jobject gpte_scope_guard_leave_with_ref(GpteScopeGuard* self, jobject ref) {
	return (**self)->PopLocalFrame(*self, ref);
}


G_DEFINE_BOXED_TYPE(GpteJvm, gpte_jvm, gpte_jvm_ref, gpte_jvm_unref)

static void gpte_load_resources(void) {
	static int loaded = 0;
	if (loaded)
		return;
	g_resources_register(gpte_get_resource());
}

static gint gpte_expose_jar(GError** err) {
	gpte_load_resources();

	gint fd = memfd_create("pte.jar", 0);
	GBytes* bytes = g_resources_lookup_data("/arpa/sp1rit/gpte/dist.jar", G_RESOURCE_LOOKUP_FLAGS_NONE, err);
	if (!bytes)
		return -1;
	gsize jar_len;
	const guchar* jar_data = g_bytes_get_data(bytes, &jar_len);
	if (write(fd, jar_data, jar_len) != (gssize)jar_len) {
		g_set_error(err, GPTE_JAVA_ERROR, GPTE_JAVA_ERROR_JVM_INIT_FAILED, "Failed writing jar to memfd: %s", g_strerror(errno));
		return -1;
	}
	return fd;
}

static gboolean gpte_jvm_strv_contains(const gchar** strv, const gchar* value) {
	if (!strv)
		return FALSE;
	for (gsize i = 0; strv[i]; i++)
		if (g_ascii_strncasecmp(strv[i], value, strlen(value)) == 0)
			return TRUE;
	return FALSE;
}

GpteJvm* gpte_jvm_create(GError** err) {
	g_return_val_if_fail(!err || !*err, NULL);

	GpteJvm* self = g_new(GpteJvm, 1);
	g_atomic_ref_count_init(&self->rc);

	self->jar_fd = gpte_expose_jar(err);
	if (self->jar_fd < 0)
		//goto err;
		return NULL;


	GStrvBuilder* builder = g_strv_builder_new();
	g_strv_builder_take(builder, g_strdup_printf("-Djava.class.path=/proc/%d/fd/%d", getpid(), self->jar_fd));

	const gchar* debug = g_getenv("GPTE_DEBUG");
	if (debug) {
		g_auto(GStrv) options = g_strsplit(debug, ",", -1);
		if (gpte_jvm_strv_contains((const gchar**)options, "help")) {
			g_print("Supported GPTE_DEBUG values:\n"
				"  gdb                Register GDB as error handler\n"
				"  xgdb               Force the GDB error handler into xterm\n"
				"  verbose            Write verbose JNI output\n"
				"  interpreter        Disable JIT compilation for JVM code\n"
				"  help               Print this help\n"
				"\n"
				"Multiple values can be given by separating them by comma.\n"
			);
		}

		gboolean xgdb = gpte_jvm_strv_contains((const gchar**)options, "xgdb");
		gboolean gdb = gpte_jvm_strv_contains((const gchar**)options, "gdb");
		if (xgdb || (gdb && !isatty(STDIN_FILENO)))
			g_strv_builder_add(builder, "-XX:OnError=xterm -e gdb -p %p");
		else if (gdb)
			g_strv_builder_add(builder, "-XX:OnError=gdb -p %p");
		else
			g_strv_builder_add(builder, "-XX:OnError=kill -9 %p");

		if (gpte_jvm_strv_contains((const gchar**)options, "interpreter"))
			g_strv_builder_add(builder, "-Xint");

		if (gpte_jvm_strv_contains((const gchar**)options, "verbose"))
			g_strv_builder_add(builder, "-verbose:jni");
	} else {
		g_strv_builder_add(builder, "-XX:OnError=kill -9 %p");
	}

	g_auto(GStrv) options_data = g_strv_builder_end(builder);
	gsize n_options = 0;
	while (options_data[n_options])
		n_options++;
	g_autofree JavaVMOption* options = g_new0(JavaVMOption, n_options);
	for (gsize i = 0; i < n_options; i++)
		options[i].optionString = options_data[i];

	JavaVMInitArgs args = (JavaVMInitArgs){
		.version = JNI_VERSION_21,
		.options = options,
		.nOptions =  n_options,
		.ignoreUnrecognized = TRUE
	};

	gint rc = JNI_CreateJavaVM(&self->vm, (void**)&self->main_env, &args);
	if (rc) {
		g_set_error(err, GPTE_JAVA_ERROR, GPTE_JAVA_ERROR_JVM_INIT_FAILED, "Failed creating Java VM (error %d)", rc);
		goto err;
	}
	return self;
err:
	g_free(self);
	return NULL;
}

GpteJvm* gpte_jvm_ref(GpteJvm* self) {
	g_atomic_ref_count_inc(&self->rc);
	return self;
}
void gpte_jvm_unref(GpteJvm* self) {
	if (g_atomic_ref_count_dec(&self->rc)) {
		(*self->vm)->DestroyJavaVM(self->vm);
		close(self->jar_fd);
		free(self);
	}
}

JNIEnv* gpte_jvm_get_env(GpteJvm* self) {
	JNIEnv* env;
	if ((*self->vm)->GetEnv(self->vm, (void**)&env, JNI_VERSION_21) != JNI_OK)
		return NULL;
	return env;
}

GpteThreadGuard* gpte_jvm_attach_thread(GpteJvm* self, const gchar* name) {
	return gpte_thread_guard_create(self, name);
}

GpteScopeGuard gpte_jvm_enter_scope(GpteJvm* self, gint capacity) {
	JNIEnv* env = gpte_jvm_get_env(self);
	if ((*env)->PushLocalFrame(env, capacity) < 0)
		g_error("GPTE out of stack memory");
	return env;
}

gboolean gpte_jvm_error(GpteJvm* self, GError** err) {
	g_return_val_if_fail(!err || !*err, FALSE);
	gboolean ret = FALSE;

	g_auto(GpteScopeGuard) guard = gpte_jvm_enter_scope(self, 5);
	JNIEnv* env = gpte_jvm_get_env(self);
	jthrowable exception = (*env)->ExceptionOccurred(env);
	if (exception) {
		ret = TRUE;

		jclass throwable = (*env)->FindClass(env, "java/lang/Throwable");
		jmethodID get_msg = (*env)->GetMethodID(env, throwable, "getMessage", "()Ljava/lang/String;");
		jstring msg = (*env)->CallObjectMethod(env, exception, get_msg);
		const char* utf = msg ? (*env)->GetStringUTFChars(env, msg, NULL) : NULL;

		jclass ioexc = (*env)->FindClass(env, "java/io/IOException");
		GpteJavaError det_err;
		if ((*env)->IsInstanceOf(env, exception, ioexc))
			det_err = GPTE_JAVA_ERROR_IO_EXCEPTION;
		else
			det_err = GPTE_JAVA_ERROR_UNHANLED_EXCEPTION;
		g_set_error(err, GPTE_JAVA_ERROR, det_err, "%s", utf);
		if (msg)
			(*env)->ReleaseStringUTFChars(env, msg, utf);
	}
	return ret;
}
