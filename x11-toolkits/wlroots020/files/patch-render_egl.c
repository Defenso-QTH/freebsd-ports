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
@@ -533,8 +537,71 @@ static EGLDeviceEXT get_egl_device_from_drm_fd(struct wlr_egl *egl,
 	return egl_device;
 }
 
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
 static int open_render_node(int drm_fd) {
 	char *render_name = drmGetRenderDeviceNameFromFd(drm_fd);
+#ifdef __FreeBSD__
+	if (render_name == NULL) {
+		render_name = freebsd_render_node(drm_fd);
+	}
+#endif
 	if (render_name == NULL) {
 		// This can happen on split render/display platforms, fallback to
 		// primary node
