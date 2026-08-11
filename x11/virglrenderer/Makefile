PORTNAME=	virglrenderer
DISTVERSION=	1.3.0.20260720
PORTREVISION=	14
CATEGORIES=	x11

MAINTAINER=	quentin.thebault@defenso.fr
COMMENT=	VirGL virtual OpenGL and Vulkan (venus) renderer
WWW=		https://virgil3d.github.io/

LICENSE=	MIT
LICENSE_FILE=	${WRKSRC}/COPYING

# Pinned to a git main snapshot (2026-07-20, "Uprev Mesa"): the venus
# renderer/proxy must match the guest's bleeding-edge Mesa (26.1) for
# Vulkan-over-virtio to work; released 1.2.0/1.3.0 are too old.
BUILD_DEPENDS=	${PYTHON_PKGNAMEPREFIX}pyyaml>=0:devel/py-pyyaml@${PY_FLAVOR}
LIB_DEPENDS=	libdrm.so:graphics/libdrm \
		libepoxy.so:graphics/libepoxy

USES=		cmake:indirect compiler:c++11-lang cpe gl meson pkgconfig \
		python xorg
CPE_VENDOR=	${PORTNAME}_project
USE_GITLAB=	yes
GL_SITE=	https://gitlab.freedesktop.org
GL_ACCOUNT=	virgl
GL_COMMIT=	9ae1fb1cca8ec884f5e44d6ae09425288aeae4cd
USE_GL=		gbm
USE_XORG=	x11

BINARY_ALIAS=	python3=${PYTHON_CMD}

OPTIONS_DEFINE=		VENUS
OPTIONS_DEFAULT=	VENUS
OPTIONS_SUB=		yes

VENUS_DESC=		Support for vulkan
VENUS_BUILD_DEPENDS=	vulkan-headers>0:graphics/vulkan-headers
VENUS_LIB_DEPENDS=	libvulkan.so:graphics/vulkan-loader \
			libepoll-shim.so:devel/libepoll-shim
VENUS_MESON_TRUE=	venus

.include <bsd.port.mk>
