--- render/egl.c.orig
+++ render/egl.c
@@ -4,6 +4,9 @@
 #include <stdio.h>
 #include <stdlib.h>
 #include <unistd.h>
+#ifdef __FreeBSD__
+#include <sys/sysctl.h>
+#endif
 #include <gbm.h>
 #include <wlr/render/egl.h>
 #include <wlr/util/log.h>
@@ -533,8 +536,33 @@ static EGLDeviceEXT get_egl_device_from_drm_fd(struct wlr_egl *egl,
 	return egl_device;
 }
 
+#ifdef __FreeBSD__
+/* Find a render node via the stock dev.drm.<minor> sysctl tree: render nodes
+ * are minors 128-191.  Single-GPU shortcut: return the first one present.
+ * Avoids libdrm's devfs scan, which fails inside a jail. */
+static char *freebsd_render_node(void) {
+	for (int m = 128; m < 192; m++) {
+		char mib[32];
+		size_t sz = 0;
+		snprintf(mib, sizeof(mib), "dev.drm.%d.PCI_ID", m);
+		if (sysctlbyname(mib, NULL, &sz, NULL, 0) != 0) {
+			continue;
+		}
+		char path[32];
+		snprintf(path, sizeof(path), "/dev/drm/%d", m);
+		return strdup(path);
+	}
+	return NULL;
+}
+#endif
+
 static int open_render_node(int drm_fd) {
 	char *render_name = drmGetRenderDeviceNameFromFd(drm_fd);
+#ifdef __FreeBSD__
+	if (render_name == NULL) {
+		render_name = freebsd_render_node();
+	}
+#endif
 	if (render_name == NULL) {
 		// This can happen on split render/display platforms, fallback to
 		// primary node
