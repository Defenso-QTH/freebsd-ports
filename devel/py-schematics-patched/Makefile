PORTNAME=	schematics-patched
DISTVERSIONPREFIX=	v
DISTVERSION=	1.1.2-patch
PORTREVISION=	1
CATEGORIES=	devel python
PKGNAMEPREFIX=	${PYTHON_PKGNAMEPREFIX}

MAINTAINER=	kai@FreeBSD.org
COMMENT=	Patched version of schematics that can pickle with multiprocessing
WWW=		https://github.com/pombredanne/schematics

LICENSE=	BSD3CLAUSE
LICENSE_FILE=	${WRKSRC}/LICENSE

RUN_DEPENDS=	${PYTHON_PKGNAMEPREFIX}six>=1.7.3:devel/py-six@${PY_FLAVOR}

USES=		python
USE_PYTHON=	distutils autoplist
USE_GITHUB=	yes
GH_ACCOUNT=	pombredanne
GH_PROJECT=	schematics

NO_ARCH=	yes

.include <bsd.port.mk>
