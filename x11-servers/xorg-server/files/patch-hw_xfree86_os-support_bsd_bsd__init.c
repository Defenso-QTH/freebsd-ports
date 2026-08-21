--- hw/xfree86/os-support/bsd/bsd_init.c.orig	2023-10-25 01:40:28 UTC
+++ hw/xfree86/os-support/bsd/bsd_init.c
@@ -39,6 +39,7 @@
 #include <sys/ioctl.h>
 #include <stdlib.h>
 #include <errno.h>
+#include <signal.h>
 
 static Bool KeepTty = FALSE;
 
@@ -48,6 +49,8 @@ static int initialVT = -1;
 #if defined (SYSCONS_SUPPORT) || defined (PCVT_SUPPORT)
 static int VTnum = -1;
 static int initialVT = -1;
+static struct termios tty_attr;	/* tty state to restore */
+static int tty_mode;		/* kbd mode to restore */
 #endif
 
 #ifdef PCCONS_SUPPORT
@@ -168,7 +171,81 @@ xf86OpenConsole()
         /* check if we are run with euid==0 */
         if (geteuid() != 0) {
             FatalError("xf86OpenConsole: Server must be suid root");
+        }
+
+#ifdef VT_GETINDEX
+        /*
+         * Taking over the VT that is already our controlling terminal is the
+         * one case where job control can reach the server.  The tcsetattr()
+         * further down is TIOCSETA, one of the ioctls tty(4) refuses to
+         * perform for a background process group, sending SIGTTOU instead --
+         * and its default action stops the server in the middle of
+         * initialisation, with the VT left in graphics mode and the keyboard
+         * in K_RAW, which leaves no way back to the console.
+         *
+         * A display manager that forks and runs the server in the child (Ly,
+         * lemurs) starts it in the launcher's foreground process group, where
+         * the call is allowed.  What breaks that is detaching: setpgrp() below
+         * makes the server its own process group leader, moving it out of the
+         * foreground group, and the TIOCNOTTY that follows cannot then drop
+         * the terminal, because FreeBSD only lets a session leader do that.
+         * Keep the terminal in that case, as the Linux implementation does
+         * (hw/xfree86/os-support/linux/lnx_init.c auto-enables KeepTty when
+         * the server is started on the VT it was launched from).
+         *
+         * From a background process group there is nothing to be done: say so
+         * and exit while the console is still untouched.
+         */
+        if (!KeepTty && VTnum != -1) {
+            int ttyfd = open("/dev/tty", O_RDONLY);
+
+            if (ttyfd >= 0) {
+                int ctty_vt = -1;
+
+                if (ioctl(ttyfd, VT_GETINDEX, &ctty_vt) < 0)
+                    ctty_vt = -1;
+
+                if (ctty_vt == VTnum) {
+                    pid_t fg = tcgetpgrp(ttyfd);
+                    struct sigaction sa;
+                    sigset_t mask;
+                    Bool blocked = FALSE;
+
+                    /*
+                     * Being in a background process group is not by itself a
+                     * problem: tty(4) performs the operation anyway when the
+                     * caller ignores or blocks SIGTTOU, and that is how xinit
+                     * runs the server -- it puts it in its own process group
+                     * and sets SIGTTOU to SIG_IGN, which survives the exec.
+                     * Refusing those would break every startx-style launch.
+                     */
+                    if (sigaction(SIGTTOU, NULL, &sa) == 0 &&
+                        sa.sa_handler == SIG_IGN)
+                        blocked = TRUE;
+                    if (!blocked && sigprocmask(SIG_BLOCK, NULL, &mask) == 0 &&
+                        sigismember(&mask, SIGTTOU))
+                        blocked = TRUE;
+
+                    if ((fg != -1 && fg == getpgrp()) || blocked) {
+                        xf86Msg(X_PROBED, "controlling tty is VT number %d, "
+                                "auto-enabling KeepTty\n", VTnum);
+                        KeepTty = TRUE;
+                        close(ttyfd);
+                    } else {
+                        close(ttyfd);
+                        FatalError("xf86OpenConsole: cannot take over VT %d, "
+                                   "the controlling terminal, from a "
+                                   "background process group: the terminal "
+                                   "settings this needs would raise SIGTTOU "
+                                   "and stop the server.  Start it in the "
+                                   "foreground, or on a different VT.\n",
+                                   VTnum);
+                    }
+                } else
+                    close(ttyfd);
+            }
         }
+#endif                          /* VT_GETINDEX */
 
         if (!KeepTty) {
             /*
@@ -253,6 +330,7 @@ xf86OpenConsole()
 #endif
  acquire_vt:
             if (!xf86Info.ShareVTs) {
+                struct termios nTty;
                 /*
                  * now get the VT
                  */
@@ -287,6 +365,26 @@ xf86OpenConsole()
                 if (ioctl(xf86Info.consoleFd, KDSETMODE, KD_GRAPHICS) < 0) {
                     FatalError("xf86OpenConsole: KDSETMODE KD_GRAPHICS failed");
                 }
+
+                tcgetattr(xf86Info.consoleFd, &tty_attr);
+                ioctl(xf86Info.consoleFd, KDGKBMODE, &tty_mode);
+
+                /* disable special keys */
+                if (ioctl(xf86Info.consoleFd, KDSKBMODE, K_RAW) < 0) {
+                    FatalError("xf86OpenConsole: KDSKBMODE K_RAW failed (%s)",
+                               strerror(errno));
+                }
+
+                nTty = tty_attr;
+                nTty.c_iflag = IGNPAR | IGNBRK;
+                nTty.c_oflag = 0;
+                nTty.c_cflag = CREAD | CS8;
+                nTty.c_lflag = 0;
+                nTty.c_cc[VTIME] = 0;
+                nTty.c_cc[VMIN] = 1;
+                cfsetispeed(&nTty, 9600);
+                cfsetospeed(&nTty, 9600);
+                tcsetattr(xf86Info.consoleFd, TCSANOW, &nTty);
             }
             else {              /* xf86Info.ShareVTs */
                 close(xf86Info.consoleFd);
@@ -303,7 +401,7 @@ xf86OpenConsole()
     else {
         /* serverGeneration != 1 */
 #if defined (SYSCONS_SUPPORT) || defined (PCVT_SUPPORT)
-        if (!xf86Info.ShareVTs &&
+        if (!xf86Info.ShareVTs && xf86Info.autoVTSwitch &&
             (xf86Info.consType == SYSCONS || xf86Info.consType == PCVT)) {
             if (ioctl(xf86Info.consoleFd, VT_ACTIVATE, xf86Info.vtno) != 0) {
                 xf86Msg(X_WARNING, "xf86OpenConsole: VT_ACTIVATE failed\n");
@@ -594,6 +692,8 @@ xf86CloseConsole()
     case SYSCONS:
     case PCVT:
         ioctl(xf86Info.consoleFd, KDSETMODE, KD_TEXT);  /* Back to text mode */
+        ioctl(xf86Info.consoleFd, KDSKBMODE, tty_mode);
+        tcsetattr(xf86Info.consoleFd, TCSANOW, &tty_attr);
         if (ioctl(xf86Info.consoleFd, VT_GETMODE, &VT) != -1) {
             VT.mode = VT_AUTO;
             ioctl(xf86Info.consoleFd, VT_SETMODE, &VT); /* dflt vt handling */
@@ -604,7 +704,7 @@ xf86CloseConsole()
                            strerror(errno));
         }
 #endif
-        if (initialVT != -1)
+        if (xf86Info.autoVTSwitch && initialVT != -1)
             ioctl(xf86Info.consoleFd, VT_ACTIVATE, initialVT);
         break;
 #endif                          /* SYSCONS_SUPPORT || PCVT_SUPPORT */
