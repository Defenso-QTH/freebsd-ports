--- src/path-seat.c.orig	2026-04-24 09:38:46 UTC
+++ src/path-seat.c
@@ -335,7 +335,7 @@ udev_device_from_devnode(struct libinput *libinput,
 	size_t count = 0;
 
 	if (stat(devnode, &st) < 0)
-		return NULL;
+		return udev_device_new_from_syspath(udev, devnode);
 
 	dev = udev_device_new_from_devnum(udev, 'c', st.st_rdev);
 
