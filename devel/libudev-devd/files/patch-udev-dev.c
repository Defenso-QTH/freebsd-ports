--- udev-dev.c.orig
+++ udev-dev.c
@@ -108,6 +108,107 @@ udev_dev_enumerate_cb(const char *path, mode_t type, void *arg)
 	return (0);
 }

+#ifdef HAVE_SYSCTLBYNAME
+/*
+ * Enumerate evdev input devices via the kern.evdev.input sysctl subtree.
+ * This is a fallback for jails where /dev/input/ is not exposed via devfs
+ * but the kern.evdev.input.* sysctls remain readable.
+ *
+ * We use the FreeBSD sysctl tree-walk protocol (OID 0.2 = CTL_SYSCTL_NEXT)
+ * to discover exactly the children that exist under kern.evdev.input, with
+ * no fixed upper bound.  CTL_SYSCTL_NEXT skips CTLTYPE_NODE containers, so
+ * the walk lands on leaves of depth parent_len+2 such as
+ * kern.evdev.input.N.name rather than on kern.evdev.input.N itself.
+ * Each group of consecutive leaves sharing the same parent OID component at
+ * index parent_len corresponds to one evdev unit.  When the component at
+ * next[parent_len] changes, we call CTL_SYSCTL_NAME on the depth-(parent_len+1)
+ * prefix to resolve the unit number N as a string, then add /dev/input/eventN.
+ * This yields each unit exactly once with no fixed upper bound.
+ */
+static int
+udev_dev_enumerate_evdev_sysctl(struct udev_enumerate *ue)
+{
+	int parent_mib[CTL_MAXNAME];
+	size_t parent_len;
+	int oid[CTL_MAXNAME + 2];
+	size_t oidlen;
+	int next[CTL_MAXNAME];
+	size_t nextlen;
+	size_t next_count;
+	int last_unit_oid;
+	int name_oid[CTL_MAXNAME + 2];
+	char name[128];
+	size_t namelen;
+	char syspath[DEV_PATH_MAX];
+	char *endp;
+	int unit;
+
+	parent_len = CTL_MAXNAME;
+	last_unit_oid = -1;
+	if (sysctlnametomib("kern.evdev.input", parent_mib, &parent_len) < 0)
+		return (0);	/* no evdev support; nothing to enumerate */
+
+	/* Seed the walk: oid[0..1] = { 0, 2 } (CTL_SYSCTL_NEXT), then parent */
+	oid[0] = 0;
+	oid[1] = 2;	/* CTL_SYSCTL_NEXT */
+	memcpy(&oid[2], parent_mib, parent_len * sizeof(int));
+	oidlen = parent_len + 2;
+
+	for (;;) {
+		nextlen = sizeof(next);
+		if (sysctl(oid, (u_int)oidlen, next, &nextlen, NULL, 0) < 0)
+			break;	/* end of tree or error — done */
+
+		next_count = nextlen / sizeof(int);
+
+		/* Stop when we have walked out of the kern.evdev.input subtree */
+		if (next_count <= parent_len ||
+		    memcmp(next, parent_mib, parent_len * sizeof(int)) != 0)
+			break;
+
+		/*
+		 * next[parent_len] is the internal OID number of the unit
+		 * node.  When it changes we have entered a new unit's subtree.
+		 * Resolve the unit number by asking CTL_SYSCTL_NAME for the
+		 * depth-(parent_len+1) prefix, which yields the string
+		 * "kern.evdev.input.N"; parse N from the last token.
+		 */
+		if (next_count >= parent_len + 2 &&
+		    next[parent_len] != last_unit_oid) {
+			last_unit_oid = next[parent_len];
+
+			name_oid[0] = 0;
+			name_oid[1] = 1;	/* CTL_SYSCTL_NAME */
+			memcpy(&name_oid[2], next,
+			    (parent_len + 1) * sizeof(int));
+			namelen = sizeof(name) - 1;
+			if (sysctl(name_oid, (u_int)(parent_len + 3),
+			    name, &namelen, NULL, 0) < 0)
+				goto advance;
+			name[namelen] = '\0';
+
+			endp = strrchr(name, '.');
+			if (endp == NULL)
+				goto advance;
+			unit = (int)strtol(endp + 1, &endp, 10);
+			if (*endp != '\0')
+				goto advance;
+
+			snprintf(syspath, sizeof(syspath),
+			    DEV_PATH_ROOT "/input/event%d", unit);
+			if (udev_enumerate_add_device(ue, syspath) == -1)
+				return (-1);
+		}
+
+advance:
+		/* Advance: ask for the next OID after `next` */
+		memcpy(&oid[2], next, nextlen);
+		oidlen = next_count + 2;
+	}
+	return (0);
+}
+#endif	/* HAVE_SYSCTLBYNAME */
+
 int
 udev_dev_enumerate(struct udev_enumerate *ue)
 {
@@ -117,8 +218,15 @@ udev_dev_enumerate(struct udev_enumerate *ue)
 		.cb = udev_dev_enumerate_cb,
 		.args = ue,
 	};
+	int ret;

-	return (scandir_recursive(path, sizeof(path), &ctx));
+	ret = scandir_recursive(path, sizeof(path), &ctx);
+	if (ret != 0)
+		return (ret);
+#ifdef HAVE_SYSCTLBYNAME
+	ret = udev_dev_enumerate_evdev_sysctl(ue);
+#endif
+	return (ret);
 }

 int
