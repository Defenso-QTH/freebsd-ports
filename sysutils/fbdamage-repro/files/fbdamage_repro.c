/*-
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Defenso
 *
 * Reproducer for the drm-kmod fbdev damage overrun.
 *
 * Asks the vt(4) console driver to fill a rectangle that ends a few scanlines
 * below the bottom of the screen.  On a drmfb console that goes:
 *
 *   vt_drmfb_drawrect()            no clipping at all
 *     -> cfb_fillrect()            no clipping either; its only bound is a
 *                                  KASSERT in fb_mem_wr*(), compiled out on a
 *                                  kernel without INVARIANTS.  The pixels land
 *                                  past the end of the shadow buffer.
 *     -> drm_fb_helper_damage_area()  records the rectangle unclipped, so
 *                                  clip->y2 ends up below the last scanline
 *     -> drm_fb_helper_damage_work() (taskqueue, a moment later)
 *        -> drm_fbdev_ttm_damage_blit_real() reads screen_buffer up to
 *           clip->y2 and runs off the end of the vzalloc()'d buffer:
 *
 *   panic: vm_fault_lookup: fault on nofault entry, addr: 0x...
 *   #7  drm_fbdev_ttm_helper_fb_dirty+0x16e
 *   #8  drm_fb_helper_damage_work+0x96
 *   #9  linux_work_fn+0xe4
 *
 * THIS PANICS AN UNPATCHED KERNEL ON PURPOSE, and before it panics it writes
 * over whatever kernel memory follows the shadow buffer.  Run it on a machine
 * you are willing to crash, from a console you can lose, with nothing
 * important unsaved.
 *
 * Usage:
 *   kldload ./fbdamage_repro.ko
 *   sysctl debug.fbdamage_repro.rows=5     # scanlines past the bottom edge
 *   sysctl debug.fbdamage_repro.go=1       # fire
 *
 * With the clamps in drm_fb_helper_damage_area() and cfb_fillrect() this
 * prints the rectangle, clips it, and survives.
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/module.h>
#include <sys/sysctl.h>
#include <sys/lock.h>
#include <sys/mutex.h>

#include <dev/vt/vt.h>

/* Not declared in vt.h, but it is a global kernel symbol. */
extern struct vt_device *main_vd;

static int fbdamage_rows = 5;

static SYSCTL_NODE(_debug, OID_AUTO, fbdamage_repro,
    CTLFLAG_RW | CTLFLAG_MPSAFE, 0,
    "drm-kmod fbdev damage overrun reproducer");

SYSCTL_INT(_debug_fbdamage_repro, OID_AUTO, rows, CTLFLAG_RW,
    &fbdamage_rows, 0,
    "How many scanlines past the bottom edge the rectangle should reach");

static int
fbdamage_go(SYSCTL_HANDLER_ARGS)
{
	struct vt_device *vd;
	int error, val, x2, y1, y2;

	val = 0;
	error = sysctl_handle_int(oidp, &val, 0, req);
	if (error != 0 || req->newptr == NULL)
		return (error);
	if (val == 0)
		return (0);

	vd = main_vd;
	if (vd == NULL || vd->vd_driver == NULL ||
	    vd->vd_driver->vd_drawrect == NULL)
		return (ENXIO);

	if (strcmp(vd->vd_driver->vd_name, "drmfb") != 0) {
		printf("fbdamage_repro: console driver is \"%s\", not "
		    "\"drmfb\" -- nothing to reproduce\n",
		    vd->vd_driver->vd_name);
		return (ENXIO);
	}

	if (fbdamage_rows < 1)
		return (EINVAL);

	/*
	 * One scanline tall, starting on the last valid row, reaching
	 * fbdamage_rows past it.  Full width, so the blit reads far enough
	 * into the first unmapped page to fault rather than land in whatever
	 * slack the allocator left behind.
	 */
	x2 = vd->vd_width - 1;
	y1 = vd->vd_height - 1;
	y2 = y1 + fbdamage_rows;

	printf("fbdamage_repro: console is %ux%u (%s)\n",
	    vd->vd_width, vd->vd_height, vd->vd_driver->vd_name);
	printf("fbdamage_repro: drawrect (0,%d)-(%d,%d), %d row%s past the "
	    "bottom edge\n", y1, x2, y2, fbdamage_rows,
	    fbdamage_rows == 1 ? "" : "s");
	printf("fbdamage_repro: if this kernel is unpatched, the damage "
	    "worker should panic shortly\n");

	vd->vd_driver->vd_drawrect(vd, 0, y1, x2, y2, 1, TC_BLACK);

	printf("fbdamage_repro: drawrect returned; waiting on the damage "
	    "worker\n");

	return (0);
}

SYSCTL_PROC(_debug_fbdamage_repro, OID_AUTO, go,
    CTLTYPE_INT | CTLFLAG_WR | CTLFLAG_MPSAFE, NULL, 0,
    fbdamage_go, "I",
    "Write 1 to draw the out-of-bounds rectangle");

static int
fbdamage_modevent(module_t mod __unused, int type, void *data __unused)
{

	switch (type) {
	case MOD_LOAD:
		printf("fbdamage_repro: loaded; "
		    "sysctl debug.fbdamage_repro.go=1 to fire\n");
		return (0);
	case MOD_UNLOAD:
		return (0);
	default:
		return (EOPNOTSUPP);
	}
}

static moduledata_t fbdamage_repro_mod = {
	"fbdamage_repro",
	fbdamage_modevent,
	NULL
};

DECLARE_MODULE(fbdamage_repro, fbdamage_repro_mod, SI_SUB_DRIVERS,
    SI_ORDER_ANY);
MODULE_VERSION(fbdamage_repro, 1);
