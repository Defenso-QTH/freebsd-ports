PORTNAME=	libwebp
PORTVERSION=	1.2.0
DISTVERSIONSUFFIX=	-8.el9
PORTREVISION=	1
CATEGORIES=	graphics linux

MAINTAINER=	emulation@FreeBSD.org
COMMENT=	Library routines for working with TIFF images (Rocky Linux ${LINUX_DIST_VER})
WWW=		http://www.simplesystems.org/libtiff/

USES=		cpe linux:rl9
USE_LDCONFIG=	yes
USE_LINUX_RPM=	yes
WANT_LINUX32=	yes

CONFLICTS=	linux-c7-${PORTNAME}
DESCR=		${PORTSDIR}/${PKGCATEGORY}/webp/pkg-descr
DOCSDIR=	${PREFIX}/usr/share/doc/${PORTNAME}${PKGNAMESUFFIX}

OPTIONS_DEFINE=	DOCS

.include <bsd.port.mk>
