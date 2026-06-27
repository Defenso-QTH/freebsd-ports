--- xf86drm.c.orig
+++ xf86drm.c
@@ -3950,6 +3950,36 @@ static int drmParsePciDeviceInfo(int maj, int min,
     struct pci_conf results[1];
     int fd, error;
 
+    /*
+     * Read the PCI vendor:device from the dev.drm.<minor>.PCI_ID sysctl.
+     * This works inside a jail, where /dev/pci is typically not exposed and
+     * the PCIOCGETCONF path below fails.  Fall back to PCIOCGETCONF when the
+     * sysctl is unavailable (e.g. older drm-kmod).
+     */
+    {
+        char dname[SPECNAMELEN];
+        char pci_id[64];
+        char mib[32];
+        size_t sz;
+        int unit;
+        unsigned int vid, did;
+
+        if (devname_r(makedev(maj, min), S_IFCHR, dname, sizeof(dname)) &&
+                sscanf(dname, "drm/%d", &unit) == 1) {
+            snprintf(mib, sizeof(mib), "dev.drm.%d.PCI_ID", unit);
+            sz = sizeof(pci_id);
+            if (sysctlbyname(mib, pci_id, &sz, NULL, 0) == 0 &&
+                    sscanf(pci_id, "%x:%x", &vid, &did) == 2) {
+                device->vendor_id = vid;
+                device->device_id = did;
+                device->subvendor_id = 0;
+                device->subdevice_id = 0;
+                device->revision_id = 0;
+                return 0;
+            }
+        }
+    }
+
     if (get_sysctl_pci_bus_info(maj, min, &info) != 0)
         return -EINVAL;
 
