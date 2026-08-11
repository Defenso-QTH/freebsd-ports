PORTNAME=	bhyve-viewer
DISTVERSION=	0.1.0.20260810
CATEGORIES=	sysutils

MAINTAINER=	quentin.thebault@defenso.fr
COMMENT=	Present a bhyve guest GPU output on the host Wayland compositor
WWW=		https://github.com/Defenso-QTH/bhyve-viewer

LICENSE=	BSD2CLAUSE

# Companion to the Defenso bhyve virtio-gpu device: bhyve exports the guest's
# scanout as a dma_buf and passes the fd over a unix socket, and this imports
# it with zwp_linux_dmabuf_v1 so the host compositor presents it directly.
# Nothing is read back or encoded, which is the point -- the guest otherwise
# spends its CPUs capturing and encoding a framebuffer that already lives in
# host GPU memory.
#
# Deliberately not linked into bhyve: the VMM keeps no Wayland or EGL
# dependency, and this can be jailed with nothing but the socket.

# Pinned to a commit rather than a tag: there is no release yet, and the
# protocol header here must match the one bhyve was built with.  Bump both
# together and rerun `make makesum`.
USE_GITHUB=	yes
GH_ACCOUNT=	Defenso-QTH
GH_TAGNAME=	c10f71656d08ad1a3bdddd0735a2c1a80d2cb6f9

BUILD_DEPENDS=	wayland-protocols>=1.32:graphics/wayland-protocols
LIB_DEPENDS=	libwayland-client.so:graphics/wayland

# EGL and GLESv2: the scanout dma_buf is imported with EGL_LINUX_DMA_BUF_EXT
# and blitted, rather than handed to the compositor as a linux-dmabuf buffer,
# because virglrenderer does not report a usable format modifier.
USES=		gl pkgconfig uidfix
USE_GL=		egl glesv2
MAKE_ARGS=	BINDIR=${PREFIX}/bin

PLIST_FILES=	bin/bhyve-viewer

.include <bsd.port.mk>
