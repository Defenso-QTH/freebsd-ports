--- render/allocator/allocator.c.orig
+++ render/allocator/allocator.c
@@ -1,6 +1,10 @@
 #include <assert.h>
 #include <fcntl.h>
 #include <stdlib.h>
+#ifdef __FreeBSD__
+#include <stdio.h>
+#include <sys/sysctl.h>
+#endif
 #include <unistd.h>
 #include <wlr/backend.h>
 #include <wlr/config.h>
@@ -35,6 +39,26 @@ void wlr_allocator_init(struct wlr_allocator *alloc,
 /* Re-open the DRM node to avoid GEM handle ref'counting issues. See:
  * https://gitlab.freedesktop.org/mesa/drm/-/merge_requests/110
  */
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
 static int reopen_drm_node(int drm_fd, bool allow_render_node) {
 	if (drmIsMaster(drm_fd)) {
 		// Only recent kernels support empty leases
@@ -53,6 +77,11 @@ static int reopen_drm_node(int drm_fd, bool allow_render_node) {
 	char *name = NULL;
 	if (allow_render_node) {
 		name = drmGetRenderDeviceNameFromFd(drm_fd);
+#ifdef __FreeBSD__
+		if (name == NULL) {
+			name = freebsd_render_node();
+		}
+#endif
 	}
 	if (name == NULL) {
 		// Either the DRM device has no render node, either the caller wants
