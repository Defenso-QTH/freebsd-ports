diff --git a/drivers/gpu/drm/drm_sysctl_freebsd.c b/drivers/gpu/drm/drm_sysctl_freebsd.c
index b9eca7a53c..f1e3c465b6 100644
--- a/drivers/gpu/drm/drm_sysctl_freebsd.c
+++ b/drivers/gpu/drm/drm_sysctl_freebsd.c
@@ -52,6 +52,8 @@ extern unsigned int drm_timestamp_precision;
 static int	   drm_name_info DRM_SYSCTL_HANDLER_ARGS;
 static int	   drm_clients_info DRM_SYSCTL_HANDLER_ARGS;
 static int	   drm_vblank_info DRM_SYSCTL_HANDLER_ARGS;
+static int	   drm_primary_devnum_handler DRM_SYSCTL_HANDLER_ARGS;
+static int	   drm_render_devnum_handler DRM_SYSCTL_HANDLER_ARGS;
 
 struct drm_sysctl_list {
 	const char *name;
@@ -140,6 +142,15 @@ drm_sysctl_init(struct drm_device *dev)
 
 	drm_add_busid_modesetting(dev, &info->ctx, top);
 
+	SYSCTL_ADD_PROC(&info->ctx, SYSCTL_CHILDREN(top), OID_AUTO,
+	    "primary_devnum", CTLTYPE_UINT | CTLFLAG_RD,
+	    dev, 0, drm_primary_devnum_handler, "IU",
+	    "user-visible dev_t of the primary DRM node");
+	SYSCTL_ADD_PROC(&info->ctx, SYSCTL_CHILDREN(top), OID_AUTO,
+	    "render_devnum", CTLTYPE_UINT | CTLFLAG_RD,
+	    dev, 0, drm_render_devnum_handler, "IU",
+	    "user-visible dev_t of the render DRM node");
+
 	SYSCTL_ADD_INT(&info->ctx, SYSCTL_CHILDREN(drioid), OID_AUTO,
 	    "vblank_offdelay", CTLFLAG_RW, &drm_vblank_offdelay,
 	    sizeof(drm_vblank_offdelay),
@@ -303,3 +314,46 @@ done:
 	SYSCTL_OUT(req, "", -1);
 	return retcode;
 }
+
+/*
+ * hw.dri.N.primary_devnum and hw.dri.N.render_devnum
+ *
+ * User-visible dev_t (unsigned int) of the primary and render DRM nodes
+ * respectively (e.g. /dev/drm/0 and /dev/drm/128).  Both are readable from
+ * within a jail.
+ *
+ * Compositors running under seatd inside a jail receive their primary DRM fd
+ * via privilege delegation but cannot resolve device node names through
+ * libdrm: drmGetDeviceNameFromFd2 and drmGetRenderDeviceNameFromFd both
+ * scan devfs, which may not expose the primary node inside the jail.
+ *
+ * The intended lookup sequence in userspace is:
+ *   1. fstat(primary_fd) -> st_rdev
+ *   2. Iterate hw.dri.0..N.primary_devnum until st_rdev matches -> slot N
+ *   3. Read hw.dri.N.render_devnum -> render dev_t
+ *   4. devname_r(render_dev_t, S_IFCHR, buf, sizeof(buf)) -> "drm/128"
+ *   5. Open "/dev/" + buf as the render node
+ *
+ * devname_r(3) resolves dev_t via the kern.devname sysctl (a kernel
+ * name-table lookup), not by scanning devfs, so step 4 works identically
+ * inside and outside a jail regardless of which nodes are visible in devfs.
+ */
+static int drm_primary_devnum_handler DRM_SYSCTL_HANDLER_ARGS
+{
+	struct drm_device *dev = arg1;
+	unsigned int v = 0;
+
+	if (dev->primary != NULL && dev->primary->bsd_device != NULL)
+		v = (unsigned int)dev2udev(dev->primary->bsd_device);
+	return (SYSCTL_OUT(req, &v, sizeof(v)));
+}
+
+static int drm_render_devnum_handler DRM_SYSCTL_HANDLER_ARGS
+{
+	struct drm_device *dev = arg1;
+	unsigned int v = 0;
+
+	if (dev->render != NULL && dev->render->bsd_device != NULL)
+		v = (unsigned int)dev2udev(dev->render->bsd_device);
+	return (SYSCTL_OUT(req, &v, sizeof(v)));
+}
