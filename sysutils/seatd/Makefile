PORTNAME=	seatd
DISTVERSION=	0.9.3.20260420
CATEGORIES=	sysutils
MASTER_SITES=	https://git.sr.ht/~kennylevinsen/${PORTNAME}/archive/${SEATD_COMMIT}${EXTRACT_SUFX}?dummy=/

MAINTAINER=	jbeich@FreeBSD.org
COMMENT=	Minimal seat management daemon and universal library
WWW=		https://git.sr.ht/~kennylevinsen/seatd

LICENSE=	MIT
LICENSE_FILE=	${WRKSRC}/LICENSE

# Post-0.9.3 snapshot.  sr.ht serves the archive under the commit hash and
# unpacks it as ${PORTNAME}-${SEATD_COMMIT}, so pin both explicitly.
SEATD_COMMIT=	4bcdc81972157be5ac0265e8fab29617f0caa5ab
WRKSRC=		${WRKDIR}/${PORTNAME}-${SEATD_COMMIT}

USES=		compiler:c11 cpe meson pkgconfig
USE_LDCONFIG=	yes
USE_RC_SUBR=	${PORTNAME}
CPE_VENDOR=	${PORTNAME}_project
MESON_DISABLED=	libseat-logind
SUB_FILES=	pkg-message

# Socket path is compiled into seatd/libseat (SEATD_DEFAULTPATH).  Expose it
# as a build-time knob; override with `make SEATD_SOCKET=...` or make.conf.
SEATD_SOCKET?=	/var/run/seatd.sock
MESON_ARGS+=	-Ddefaultpath=${SEATD_SOCKET}
SUB_LIST+=	SEATD_SOCKET=${SEATD_SOCKET}
PLIST_FILES=	bin/${PORTNAME} \
		"@(,video,4750) bin/${PORTNAME}-launch" \
		include/libseat.h \
		lib/libseat.so \
		lib/libseat.so.1 \
		libdata/pkgconfig/libseat.pc

OPTIONS_DEFINE=	MANPAGES
OPTIONS_DEFAULT=MANPAGES
OPTIONS_SUB=	yes

MANPAGES_BUILD_DEPENDS=	scdoc:textproc/scdoc
MANPAGES_MESON_ENABLED=	man-pages
MANPAGES_PLIST_FILES=	share/man/man1/${PORTNAME}.1.gz \
			share/man/man1/${PORTNAME}-launch.1.gz

.include <bsd.port.mk>
