PORTNAME=	pulseaudio
PORTVERSION=	10.0
DISTVERSIONSUFFIX=	-6.el7_9
PORTREVISION=	2
CATEGORIES=	audio linux
PKGNAMESUFFIX=	-utils

MAINTAINER=	emulation@FreeBSD.org
COMMENT=	Utils for PulseAudio clients (Linux CentOS ${LINUX_DIST_VER})
WWW=		https://pulseaudio.org/

USES=		cpe linux:c7
USE_LDCONFIG=	yes
NO_SHLIB_REQUIRES_GLOB=	libpulsecommon-10.0.so:Linux*
USE_LINUX=	xorglibs pulseaudio-libs
USE_LINUX_RPM=	nolib

BIN_DISTNAMES=	${PORTNAME}${PKGNAMESUFFIX}-${DISTVERSIONFULL}

CONFLICTS=	linux-c6-${PORTNAME}${PKGNAMESUFFIX}
DESCR=		${.CURDIR}/../${PORTNAME}/pkg-descr

.include <bsd.port.mk>
