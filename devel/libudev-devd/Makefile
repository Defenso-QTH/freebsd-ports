PORTNAME=	libudev-devd
DISTVERSION=	0.6.0.20260614
CATEGORIES=	devel

MAINTAINER=	x11@FreeBSD.org
COMMENT=	libudev-compatible interface for devd
WWW=		https://github.com/wulf7/libudev-devd

LICENSE=	BSD2CLAUSE

BUILD_DEPENDS=	${LOCALBASE}/include/linux/input.h:devel/evdev-proto

# CI build: track the latest commit of the (patched) fork rather than the
# v0.6.0 release tag.  GH_TAGNAME pins the exact commit; bump it and rerun
# `make makesum` to advance.
USE_GITHUB=	yes
GH_ACCOUNT=	Defenso-QTH
GH_TAGNAME=	05ee8f544809a2cd9db0b3182a82843a0b5b0fc3

USES=		compiler:c11 localbase meson
USE_LDCONFIG=	yes

CFLAGS+=	-Wno-error

PLIST_FILES=	include/libudev.h \
		lib/libudev.so \
		lib/libudev.so.0 \
		lib/libudev.so.0.0.0 \
		libdata/pkgconfig/libudev.pc

OPTIONS_DEFINE=	GPL

GPL_DESC=	Enable GPL-licensed code
GPL_MESON_TRUE=	enable-gpl
GPL_VARS=	LICENSE+=LGPL21+ LICENSE_COMB=multi

.include <bsd.port.mk>
