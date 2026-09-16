PORTNAME=	drm-612-kmod
DISTVERSION=	${DRM_KMOD_DISTVERSION}
PORTREVISION=	7
CATEGORIES=	graphics kld

.include "Makefile.version"

MAINTAINER=	x11@FreeBSD.org
COMMENT=	Direct Rendering Manager (DRM) GPU drivers
WWW=		https://github.com/freebsd/drm-kmod/

LICENSE=	BSD2CLAUSE MIT GPLv2
LICENSE_COMB=	multi

ONLY_FOR_ARCHS=	amd64
ONLY_FOR_ARCHS_REASON=	the new KMS components are only supported on amd64

CONFLICTS_INSTALL=	drm-latest-kmod \
			drm-515-kmod \
			drm-61-kmod \
			drm-66-kmod

USES=		kmod uidfix compiler:c++11-lang
PATCH_STRIP=	-p1

# Diagnostic build: ship the modules UNSTRIPPED so a crash inside drm.ko /
# amdgpu.ko is analyzable from a vmcore (kgdb reads the symbols straight out of
# /boot/modules/*.ko; the stripped _6 package left "No debugging symbols found
# in /boot/modules/drm.ko" in the dump).  DEBUG_FLAGS is left at the default -g
# so codegen matches the non-debug build -- only symbols are retained, no source
# change and no -O0.  This is NOT the damage-clamp fix; that is deliberately not
# applied here.  Expect a noticeably larger package.  Drop WITH_DEBUG once the
# fbdev damage overrun is diagnosed.
WITH_DEBUG=	yes

USE_GITHUB=	yes
GH_ACCOUNT=	Defenso-QTH
GH_PROJECT=	drm-kmod
GH_TAGNAME=	28fae4bc26b5100e9a87cf141a3533ce19162ddf

.include <bsd.port.options.mk>

SUB_FILES=	pkg-message
SUB_LIST=	PORTNAME=${PORTNAME} OPSYS=${OPSYS} OSREL=${OSREL}

.if ${OPSYS} != FreeBSD
IGNORE=         not supported on anything but FreeBSD (missing linuxkpi functionality)
.elif ${OSVERSION} < 1500509
IGNORE=         not supported on FreeBSD < 1500509, no kernel support
.elif 1600000 <= ${OSVERSION} && ${OSVERSION} < 1600018
IGNORE=         not supported on FreeBSD 16 before 1600018, no kernel support
.endif

MAKE_ENV+=	MAKEOBJDIRPREFIX=${WRKSRC}/obj

pre-build:
		${MKDIR} ${WRKSRC}/obj
		(cd ${WRKSRC} ; ${SETENV} ${MAKE_ENV} ${MAKE_CMD} obj)

.include <bsd.port.mk>
