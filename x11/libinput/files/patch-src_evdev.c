When a process without full /dev/input access enumerates devices via libudev-devd,
the udev_device structs do not get udev properties that mark them as inputs, keyboards, etc,
and get rejected as not being input devices.

libinput reopens devices just to check path equality.
The udev_devices from reopening do have the right properties,
so we just use them instead of the original (enumerated) ones.

--- src/evdev.c.orig	2026-04-02 01:04:12 UTC
+++ src/evdev.c
@@ -1007,7 +1007,7 @@ evdev_sync_device(struct libinput *libinput, struct ev
 
 	evdev_device_dispatch_frame(libinput, device, frame);
 
-	return rc == -EAGAIN ? 0 : rc;
+	return (rc == -EAGAIN || rc == -EINVAL)? 0 : rc;
 }
 
 static inline void
@@ -1105,6 +1105,17 @@ evdev_device_dispatch(void *data)
 
 	if (rc != -EAGAIN && rc != -EINTR) {
 		libinput_remove_source(libinput, device->source);
+		/*
+		 * Dirty hack to allow cuse-based evdev backends to release
+		 * character device file when device has been detached
+		 * but still have it descriptor opened.
+		 * Issuing evdev_device_suspend() here leads to SIGSEGV
+		 */
+		int dummy_fd = open("/dev/null", O_RDONLY | O_CLOEXEC);
+		if (dummy_fd >= 0) {
+			dup2(dummy_fd, device->fd);
+			close(dummy_fd);
+		}
 		device->source = NULL;
 	}
 }

@@ -2116,24 +2116,34 @@ evdev_notify_added_device(struct evdev_device *device)
 static bool
 evdev_device_have_same_syspath(struct udev_device *udev_device, int fd)
 {
-	struct udev *udev = udev_device_get_udev(udev_device);
-	struct udev_device *udev_device_new = NULL;
 	struct stat st;
-	bool rc = false;
 
 	if (fstat(fd, &st) < 0)
-		goto out;
+		return false;
 
-	udev_device_new = udev_device_new_from_devnum(udev, 'c', st.st_rdev);
-	if (!udev_device_new)
-		goto out;
+#ifdef __FreeBSD__
+	/* On FreeBSD the evdev syspath (/dev/input/eventN) is determined by
+	 * the device's minor number, while the cached devnum carries both
+	 * that minor and the evdev major.  Comparing the full devnum against
+	 * the fd's st_rdev therefore verifies the same identity as the
+	 * syspath comparison -- in fact slightly more strongly, since it
+	 * also confirms the major -- without needing the device node to be
+	 * visible to the caller.  libudev-devd populates that devnum from
+	 * the kern.evdev.input.N.devnum sysctl, which is readable even in a
+	 * jail whose devfs does not expose the input node.
+	 */
+	return udev_device_get_devnum(udev_device) == st.st_rdev;
+#else
+	struct udev *udev = udev_device_get_udev(udev_device);
+	_unref_(udev_device) *udev_device_new =
+		udev_device_new_from_devnum(udev, 'c', st.st_rdev);
+	bool rc = false;
 
-	rc = streq(udev_device_get_syspath(udev_device_new),
-		   udev_device_get_syspath(udev_device));
-out:
 	if (udev_device_new)
-		udev_device_unref(udev_device_new);
+		rc = streq(udev_device_get_syspath(udev_device_new),
+			   udev_device_get_syspath(udev_device));
 	return rc;
+#endif
 }
 
 static bool
