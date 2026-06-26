--- backend/drm/backend.c.orig
+++ backend/drm/backend.c
@@ -9,6 +9,9 @@
 #include <wlr/interfaces/wlr_output.h>
 #include <wlr/util/log.h>
 #include <xf86drm.h>
+#ifdef __FreeBSD__
+#include <sys/sysctl.h>
+#endif
 #include "backend/drm/drm.h"
 #include "backend/drm/fb.h"
 #include "render/drm_format_set.h"
@@ -211,6 +214,23 @@ struct wlr_backend *wlr_drm_backend_create(struct wlr_session *session,
 	assert(!parent || wlr_backend_is_drm(parent));
 
 	char *name = drmGetDeviceNameFromFd2(dev->fd);
+#ifdef __FreeBSD__
+	if (name == NULL) {
+		/* drmGetDeviceNameFromFd2 scans devfs and fails in jails.  Use the
+		 * first primary node from the stock dev.drm tree (minors 0-63); the
+		 * name is only used for logging and multi-GPU matching. */
+		for (int m = 0; m < 64 && name == NULL; m++) {
+			char mib[32];
+			size_t sz = 0;
+			snprintf(mib, sizeof(mib), "dev.drm.%d.PCI_ID", m);
+			if (sysctlbyname(mib, NULL, &sz, NULL, 0) == 0) {
+				char path[32];
+				snprintf(path, sizeof(path), "/dev/drm/%d", m);
+				name = strdup(path);
+			}
+		}
+	}
+#endif
 	if (name == NULL) {
 		wlr_log_errno(WLR_ERROR, "drmGetDeviceNameFromFd2() failed");
 		return NULL;
