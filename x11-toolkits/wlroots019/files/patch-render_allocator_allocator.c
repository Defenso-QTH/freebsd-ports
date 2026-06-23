--- a/render/allocator/allocator.c
+++ b/render/allocator/allocator.c
@@ -1,8 +1,14 @@
 #include <assert.h>
 #include <fcntl.h>
 #include <stdlib.h>
+#include <sys/stat.h>
+#ifdef __FreeBSD__
+#include <stdio.h>
+#include <sys/sysctl.h>
+extern char *devname_r(dev_t, int, char *, int);
+#endif
 #include <unistd.h>
 #include <wlr/backend.h>
 #include <wlr/config.h>
 #include <wlr/interfaces/wlr_buffer.h>
 #include <wlr/render/allocator.h>
@@ -53,6 +59,40 @@
	char *name = NULL;
	if (allow_render_node) {
		name = drmGetRenderDeviceNameFromFd(drm_fd);
+#ifdef __FreeBSD__
+		/* drmGetRenderDeviceNameFromFd scans devfs and fails in jails.
+		 * Fall back to hw.dri.N.{primary,render}_devnum sysctls. */
+		if (name == NULL) {
+			struct stat st;
+			if (fstat(drm_fd, &st) == 0) {
+				char mib[64];
+				unsigned int devnum;
+				size_t sz;
+				int n;
+				for (n = 0; n <= 9; n++) {
+					snprintf(mib, sizeof(mib), "hw.dri.%d.primary_devnum", n);
+					sz = sizeof(devnum);
+					if (sysctlbyname(mib, &devnum, &sz, NULL, 0) < 0)
+						continue;
+					if ((dev_t)devnum != st.st_rdev)
+						continue;
+					snprintf(mib, sizeof(mib), "hw.dri.%d.render_devnum", n);
+					sz = sizeof(devnum);
+					if (sysctlbyname(mib, &devnum, &sz, NULL, 0) < 0)
+						break;
+					char devname[64];
+					if (devname_r((dev_t)devnum, 0, devname, sizeof(devname)) == NULL)
+						break;
+					char path[80];
+					snprintf(path, sizeof(path), "/dev/%s", devname);
+					name = strdup(path);
+					wlr_log(WLR_DEBUG,
+						"Found render node '%s' via hw.dri.%d sysctls", path, n);
+					break;
+				}
+			}
+		}
+#endif
	}
	if (name == NULL) {
		// Either the DRM device has no render node, either the caller wants
