--- render/allocator/allocator.c.orig
+++ render/allocator/allocator.c
@@ -1,6 +1,11 @@
 #include <assert.h>
 #include <fcntl.h>
 #include <stdlib.h>
+#ifdef __FreeBSD__
+#include <stdio.h>
+#include <string.h>
+#include <sys/sysctl.h>
+#endif
 #include <unistd.h>
 #include <wlr/backend.h>
 #include <wlr/config.h>
@@ -35,6 +40,44 @@ void wlr_allocator_init(struct wlr_allocator *alloc,
 /* Re-open the DRM node to avoid GEM handle ref'counting issues. See:
  * https://gitlab.freedesktop.org/mesa/drm/-/merge_requests/110
  */
+#ifdef __FreeBSD__
+/* Resolve a render node from the stock dev.drm.<minor> sysctl tree (render
+ * nodes are minors 128-191).  Inside a jail the fd cannot be mapped to a
+ * specific GPU: libdrm's drmGetRenderDeviceNameFromFd()/drmGetDevice2() scan
+ * devfs / open /dev/pci, and drmGetBusid() returns an empty string on modern
+ * KMS drivers (amdgpu).
+ *
+ * When exactly one render node exists it is unambiguously the right one
+ * (single GPU, or a display-only primary paired with one render GPU), so use
+ * it.  With several render nodes we can't tell which belongs to this fd, so
+ * fail loudly rather than render on the wrong GPU. */
+static char *freebsd_render_node(void) {
+	int found = -1, count = 0;
+	for (int m = 128; m < 192; m++) {
+		char mib[32];
+		size_t sz = 0;
+		snprintf(mib, sizeof(mib), "dev.drm.%d.PCI_ID", m);
+		if (sysctlbyname(mib, NULL, &sz, NULL, 0) != 0) {
+			continue;
+		}
+		if (count++ == 0) {
+			found = m;
+		}
+	}
+	if (count == 0) {
+		return NULL;
+	}
+	if (count > 1) {
+		wlr_log(WLR_ERROR, "Multiple DRM render nodes present; cannot select "
+			"the one for this GPU inside a jail");
+		return NULL;
+	}
+	char path[32];
+	snprintf(path, sizeof(path), "/dev/drm/%d", found);
+	return strdup(path);
+}
+#endif
+
 static int reopen_drm_node(int drm_fd, bool allow_render_node) {
 	if (drmIsMaster(drm_fd)) {
 		// Only recent kernels support empty leases
@@ -53,6 +96,11 @@ static int reopen_drm_node(int drm_fd, bool allow_render_node) {
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
