PORTNAME=	fbdamage-repro
DISTVERSION=	0.20260916
CATEGORIES=	sysutils

MAINTAINER=	quentin.thebault@defenso.fr
COMMENT=	Reproducer kmod for the drm-kmod fbdev damage overrun panic
WWW=		https://github.com/Defenso-QTH/freebsd-ports

LICENSE=	BSD2CLAUSE

# A diagnostic kernel module, not something to run in production: loading it
# and setting debug.fbdamage_repro.go=1 draws a console rectangle past the
# bottom edge of the screen, which panics an UNPATCHED drm-kmod (the
# drm_fbdev_ttm_helper_fb_dirty / drm_fb_helper_damage_work overrun) and is a
# no-op with the clamp fix in place.  Built here only so it does not have to be
# compiled by hand on the target.  See the module source for the full writeup.

# No distfile: the whole thing is two files carried in files/.
DISTFILES=	# empty
EXTRACT_ONLY=	# empty

# USES=kmod requires ${SRC_BASE}/sys (default /usr/src); it IGNOREs itself and
# poudriere skips the port when the kernel sources are absent, so this can
# never fail a bulk run -- it just will not appear in the repo.
USES=		kmod uidfix

WRKSRC=		${WRKDIR}/${PORTNAME}

# refuse to be mistaken for a released version
PLIST_FILES=	# kmod.mk adds the @kld entry

do-extract:
	@${MKDIR} ${WRKSRC}
	${CP} ${FILESDIR}/fbdamage_repro.c ${WRKSRC}/fbdamage_repro.c
	${CP} ${FILESDIR}/Makefile.module ${WRKSRC}/Makefile

.include <bsd.port.mk>
