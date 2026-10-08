PORTNAME=	pnpm
DISTVERSION=	10.34.6
CATEGORIES=	www
MASTER_SITES=	https://registry.npmjs.org/${PORTNAME}/-/
EXTRACT_SUFX=	.tgz

MAINTAINER=	quentin.thebault@defenso.fr
COMMENT=	Fast, disk space efficient package manager for Node.js
WWW=		https://pnpm.io/

LICENSE=	MIT
LICENSE_FILE=	${WRKSRC}/LICENSE

# pnpm 10 is the last pure-JavaScript major: bin/pnpm.cjs runs under node and
# requires the bundled dist/pnpm.cjs.  pnpm 11+ ship a platform-native binary
# (no FreeBSD build), so stay on the 10.x line.  The only native artefacts in
# this tarball are the optional reflink helpers for macOS/Windows, which we
# drop below; FreeBSD falls back to ordinary copies.
USES=		nodejs:22,run

NO_ARCH=	yes
NO_BUILD=	yes

# The npm tarball unpacks to package/.
WRKSRC=		${WRKDIR}/package

SUB_FILES=	pnpm

post-extract:
	@${RM} -r ${WRKSRC}/dist/reflink.*.node

do-install:
	@${MKDIR} ${STAGEDIR}${DATADIR}
	cd ${WRKSRC} && ${COPYTREE_SHARE} . ${STAGEDIR}${DATADIR}
	${INSTALL_SCRIPT} ${WRKDIR}/pnpm ${STAGEDIR}${PREFIX}/bin/pnpm

.include <bsd.port.mk>
