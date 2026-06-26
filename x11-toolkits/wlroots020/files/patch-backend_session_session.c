--- backend/session/session.c.orig
+++ backend/session/session.c
@@ -16,6 +16,9 @@
 #include <xf86drmMode.h>
 #include "backend/session/session.h"
 #include "util/time.h"
+#ifdef __FreeBSD__
+#include <sys/sysctl.h>
+#endif
 
 #include <libseat.h>
 
@@ -473,6 +476,31 @@ static void find_gpus_handle_add(struct wl_listener *listener, void *data) {
 	handler->added = true;
 }
 
+#ifdef __FreeBSD__
+/* Enumerate primary DRM nodes from the stock dev.drm.<minor> sysctl tree
+ * (minors 0-63 are primary nodes), opening each through the session.  libudev
+ * drm enumeration scans devfs and finds nothing inside a jail. */
+static ssize_t freebsd_find_gpus(struct wlr_session *session,
+		size_t ret_len, struct wlr_device *ret[static ret_len]) {
+	size_t i = 0;
+	for (int m = 0; m < 64 && i < ret_len; m++) {
+		char mib[32];
+		size_t sz = 0;
+		snprintf(mib, sizeof(mib), "dev.drm.%d.PCI_ID", m);
+		if (sysctlbyname(mib, NULL, &sz, NULL, 0) != 0) {
+			continue;
+		}
+		char path[32];
+		snprintf(path, sizeof(path), "/dev/drm/%d", m);
+		struct wlr_device *dev = session_open_if_kms(session, path);
+		if (dev) {
+			ret[i++] = dev;
+		}
+	}
+	return i;
+}
+#endif
+
 ssize_t wlr_session_find_gpus(struct wlr_session *session,
 		size_t ret_len, struct wlr_device **ret) {
 	const char *explicit = getenv("WLR_DRM_DEVICES");
@@ -481,6 +509,13 @@ ssize_t wlr_session_find_gpus(struct wlr_session *session,
 		return explicit_find_gpus(session, ret_len, ret, explicit);
 	}
 
+#ifdef __FreeBSD__
+	ssize_t fbsd = freebsd_find_gpus(session, ret_len, ret);
+	if (fbsd > 0) {
+		return fbsd;
+	}
+#endif
+
 	struct udev_enumerate *en = enumerate_drm_cards(session->udev);
 	if (!en) {
 		return -1;
