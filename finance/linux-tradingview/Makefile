PORTNAME=	tradingview
DISTVERSION=	2.9.6
DISTVERSIONSUFFIX=	-1_amd64
CATEGORIES=	finance linux
MASTER_SITES=	https://tvd-packages.tradingview.com/ubuntu/stable/pool/multiverse/t/tradingview/jammy/
PKGNAMEPREFIX=	linux-
EXTRACT_SUFX=	.deb

MAINTAINER=	voxnod@gmail.com
COMMENT=	Desktop trading application
WWW=		https://www.tradingview.com

LICENSE=	PROPRIETARY
LICENSE_NAME=	PROPRIETARY
LICENSE_TEXT=	https://www.tradingview.com/desktop/
LICENSE_PERMS=	none

ONLY_FOR_ARCHS=	amd64

USES=		desktop-file-utils linux:rl9

USE_LINUX=	alsalib at-spi2-core cups-libs gtk3 libdrm libsecret nss

SUB_FILES=	tradingview
NO_BUILD=	yes

post-extract:
	cd ${WRKDIR} && ${EXTRACT_CMD} ${EXTRACT_BEFORE_ARGS} data.tar.xz ${EXTRACT_AFTER_ARGS}

do-install:
	cd ${WRKDIR}/opt/TradingView && ${COPYTREE_SHARE} \* ${STAGEDIR}${DATADIR}
	${CHMOD} +x ${STAGEDIR}${DATADIR}/tradingview
	${INSTALL_SCRIPT} ${WRKDIR}/tradingview ${STAGEDIR}${PREFIX}/bin
	${SED} 's|/opt/TradingView/tradingview|${PREFIX}/bin/tradingview|g' ${WRKDIR}/usr/share/applications/tradingview.desktop > ${STAGEDIR}${DESKTOPDIR}/tradingview.desktop
	@${MKDIR} ${STAGEDIR}${PREFIX}/share/icons/hicolor/512x512/apps
	${INSTALL_DATA} ${WRKDIR}/usr/share/icons/hicolor/512x512/apps/tradingview.png ${STAGEDIR}${PREFIX}/share/icons/hicolor/512x512/apps
	@${MKDIR} ${STAGEDIR}${PREFIX}/share/metainfo
	${INSTALL_DATA} ${FILESDIR}/com.tradingview.tradingview.appdata.xml ${STAGEDIR}${PREFIX}/share/metainfo/
	${STRIP_CMD} ${STAGEDIR}${DATADIR}/libvulkan.so.1
	${STRIP_CMD} ${STAGEDIR}${DATADIR}/chrome-sandbox

.include <bsd.port.mk>
