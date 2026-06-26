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
@@ -35,6 +40,64 @@ void wlr_allocator_init(struct wlr_allocator *alloc,
 /* Re-open the DRM node to avoid GEM handle ref'counting issues. See:
  * https://gitlab.freedesktop.org/mesa/drm/-/merge_requests/110
  */
+#ifdef __FreeBSD__
+/* Resolve the render node for a primary DRM fd inside a jail using only an
+ * ioctl plus stock sysctls.  libdrm's drmGetRenderDeviceNameFromFd() scans
+ * devfs and drmGetDevice2() opens /dev/pci, both of which fail in a jail.
+ *
+ *   1. drmGetBusid(fd)            -> PCI busid           (DRM_IOCTL_GET_UNIQUE)
+ *   2. hw.dri.N.busid match       -> card index N        (sysctl)
+ *   3. dev.drm.N.PCI_ID           -> primary's PCI_ID     (sysctl)
+ *   4. dev.drm.<m>.PCI_ID (m>=128) matching PCI_ID -> /dev/drm/<m>
+ *
+ * Pairs primary and render of the same GPU by PCI_ID (correct for hybrid
+ * iGPU+dGPU).  Returns NULL -- rather than guessing -- if the GPU cannot be
+ * identified, so a broken busid lookup fails loudly instead of silently
+ * selecting the wrong render node. */
+static char *freebsd_render_node(int drm_fd) {
+	char pci_id[64];
+	int have_pci = 0;
+	char *busid = drmGetBusid(drm_fd);
+	if (busid != NULL) {
+		for (int n = 0; n < 64 && !have_pci; n++) {
+			char mib[32], val[256];
+			size_t sz = sizeof(val);
+			snprintf(mib, sizeof(mib), "hw.dri.%d.busid", n);
+			if (sysctlbyname(mib, val, &sz, NULL, 0) != 0 ||
+					strcmp(val, busid) != 0) {
+				continue;
+			}
+			snprintf(mib, sizeof(mib), "dev.drm.%d.PCI_ID", n);
+			sz = sizeof(pci_id);
+			if (sysctlbyname(mib, pci_id, &sz, NULL, 0) == 0) {
+				have_pci = 1;
+			}
+		}
+	}
+	if (!have_pci) {
+		wlr_log(WLR_ERROR, "Cannot identify GPU for DRM fd (busid '%s'): "
+			"no matching hw.dri.N.busid / dev.drm.N.PCI_ID",
+			busid ? busid : "(null)");
+		drmFreeBusid(busid);
+		return NULL;
+	}
+	drmFreeBusid(busid);
+	for (int m = 128; m < 192; m++) {
+		char mib[32], val[64];
+		size_t sz = sizeof(val);
+		snprintf(mib, sizeof(mib), "dev.drm.%d.PCI_ID", m);
+		if (sysctlbyname(mib, val, &sz, NULL, 0) != 0 ||
+				strcmp(val, pci_id) != 0) {
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
@@ -53,6 +116,11 @@ static int reopen_drm_node(int drm_fd, bool allow_render_node) {
 	char *name = NULL;
 	if (allow_render_node) {
 		name = drmGetRenderDeviceNameFromFd(drm_fd);
+#ifdef __FreeBSD__
+		if (name == NULL) {
+			name = freebsd_render_node(drm_fd);
+		}
+#endif
 	}
 	if (name == NULL) {
 		// Either the DRM device has no render node, either the caller wants
