PORTNAME=	mctc-lib
DISTVERSIONPREFIX=	v
DISTVERSION=	0.4.0
CATEGORIES=	science # chemistry

MAINTAINER=	yuri@FreeBSD.org
COMMENT=	Computation tool chain library to work with molecular structure data
WWW=		https://grimme-lab.github.io/mctc-lib/

LICENSE=	APACHE20
LICENSE_FILE=	${WRKSRC}/LICENSE

USES=		cmake:testing fortran
USE_LDCONFIG=	yes

USE_GITHUB=	yes
GH_ACCOUNT=	grimme-lab

CMAKE_ON=	BUILD_SHARED_LIBS

post-patch: # same as https://github.com/grimme-lab/mctc-lib/pull/55
	@${REINPLACE_CMD} -e 's|$${CMAKE_Fortran_COMPILER_ID}-$${CMAKE_Fortran_COMPILER_VERSION}||' ${WRKSRC}/config/CMakeLists.txt

# tests as of 0.4.0: 100% tests passed, 0 tests failed out of 28

.include <bsd.port.mk>
