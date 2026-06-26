--- render/egl.c.orig
+++ render/egl.c
@@ -4,6 +4,10 @@
 #include <stdio.h>
 #include <stdlib.h>
 #include <unistd.h>
+#ifdef __FreeBSD__
+#include <string.h>
+#include <sys/sysctl.h>
+#endif
 #include <gbm.h>
 #include <wlr/render/egl.h>
 #include <wlr/util/log.h>
@@ -533,8 +537,51 @@ static EGLDeviceEXT get_egl_device_from_drm_fd(struct wlr_egl *egl,
 	return egl_device;
 }
 
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
