PORTNAME=	sphinxcontrib-actdiag
PORTVERSION=	3.0.0
PORTREVISION=	1
CATEGORIES=	textproc
MASTER_SITES=	PYPI
PKGNAMEPREFIX=	${PYTHON_PKGNAMEPREFIX}

MAINTAINER=	python@FreeBSD.org
COMMENT=	Sphinx extension for embedding activity diagram
WWW=		https://github.com/blockdiag/${PORTNAME}

LICENSE=	BSD2CLAUSE
LICENSE_FILE=	${WRKSRC}/LICENSE

RUN_DEPENDS=	${PYTHON_PKGNAMEPREFIX}actdiag>=0.5.3:graphics/py-actdiag@${PY_FLAVOR} \
		${PYTHON_PKGNAMEPREFIX}sphinx>=2.0:textproc/py-sphinx@${PY_FLAVOR}

USES=		python
USE_PYTHON=	autoplist distutils

NO_ARCH=	yes

.include <bsd.port.mk>
