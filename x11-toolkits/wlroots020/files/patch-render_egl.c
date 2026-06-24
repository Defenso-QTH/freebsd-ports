--- render/egl.c.orig
+++ render/egl.c
@@ -4,6 +4,12 @@
 #include <stdio.h>
 #include <stdlib.h>
 #include <unistd.h>
+#ifdef __FreeBSD__
+#include <sys/stat.h>
+#include <sys/sysctl.h>
+/* devname_r(3) is guarded by __BSD_VISIBLE; forward-declare it explicitly. */
+extern char *devname_r(dev_t, int, char *, int);
+#endif
 #include <gbm.h>
 #include <wlr/render/egl.h>
 #include <wlr/util/log.h>
@@ -535,6 +541,40 @@ static EGLDeviceEXT get_egl_device_from_drm_fd(struct wlr_egl *egl,
 
 static int open_render_node(int drm_fd) {
 	char *render_name = drmGetRenderDeviceNameFromFd(drm_fd);
+#ifdef __FreeBSD__
+	/* drmGetRenderDeviceNameFromFd scans devfs and fails in jails.
+	 * Fall back to hw.dri.N.{primary,render}_devnum sysctls. */
+	if (render_name == NULL) {
+		struct stat st;
+		if (fstat(drm_fd, &st) == 0) {
+			char mib[64];
+			unsigned int devnum;
+			size_t sz;
+			int n;
+			for (n = 0; n <= 9; n++) {
+				snprintf(mib, sizeof(mib), "hw.dri.%d.primary_devnum", n);
+				sz = sizeof(devnum);
+				if (sysctlbyname(mib, &devnum, &sz, NULL, 0) < 0)
+					continue;
+				if ((dev_t)devnum != st.st_rdev)
+					continue;
+				snprintf(mib, sizeof(mib), "hw.dri.%d.render_devnum", n);
+				sz = sizeof(devnum);
+				if (sysctlbyname(mib, &devnum, &sz, NULL, 0) < 0)
+					break;
+				char devname[64];
+				if (devname_r((dev_t)devnum, 0, devname, sizeof(devname)) == NULL)
+					break;
+				char path[80];
+				snprintf(path, sizeof(path), "/dev/%s", devname);
+				render_name = strdup(path);
+				wlr_log(WLR_DEBUG,
+					"Found render node '%s' via hw.dri.%d sysctls", path, n);
+				break;
+			}
+		}
+	}
+#endif
 	if (render_name == NULL) {
 		// This can happen on split render/display platforms, fallback to
 		// primary node
