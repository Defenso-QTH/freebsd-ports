When a process without full /dev/input access enumerates devices via libudev-devd,
the udev_device structs do not get udev properties that mark them as inputs, keyboards, etc,
and get rejected as not being input devices.

libinput reopens devices just to check path equality.
The udev_devices from reopening do have the right properties,
so we just use them instead of the original (enumerated) ones.

--- src/evdev.c.orig	2025-04-01 02:46:07 UTC
+++ src/evdev.c
@@ -2193,8 +2193,10 @@ evdev_device_have_same_syspath(struct udev_device *ude
 		goto out;
 
 	udev_device_new = udev_device_new_from_devnum(udev, 'c', st.st_rdev);
-	if (!udev_device_new)
+	if (!udev_device_new) {
+        rc = true;
 		goto out;
+    }
 
 	rc = streq(udev_device_get_syspath(udev_device_new),
 		   udev_device_get_syspath(udev_device));
