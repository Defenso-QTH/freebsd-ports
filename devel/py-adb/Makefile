PORTNAME=	adb
DISTVERSION=	1.3.0-9
PORTREVISION=	1
DISTVERSIONSUFFIX=	-gd0be33c
CATEGORIES=	devel comms python
PKGNAMEPREFIX=	${PYTHON_PKGNAMEPREFIX}

MAINTAINER=	ports@FreeBSD.org
COMMENT=	Pure-python implementation of ADB and Fastboot protocols
WWW=		https://github.com/google/python-adb

LICENSE=	APACHE20

RUN_DEPENDS=	${PYTHON_PKGNAMEPREFIX}libusb1>=1.0.16:devel/py-libusb1@${PY_FLAVOR}
TEST_DEPENDS=	${PYTHON_PKGNAMEPREFIX}mock>=0:devel/py-mock@${PY_FLAVOR}

USE_GITHUB=	yes
GH_ACCOUNT=	google
GH_PROJECT=	python-${PORTNAME}

USES=		python
USE_PYTHON=	autoplist cryptography distutils pytest

NO_ARCH=	yes

.include <bsd.port.mk>
