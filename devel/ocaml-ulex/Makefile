PORTNAME=	ulex
PORTVERSION=	1.1
PORTREVISION=	4
CATEGORIES=	devel
MASTER_SITES=	http://www.cduce.org/download/
PKGNAMEPREFIX=	ocaml-

MAINTAINER=	ports@FreeBSD.org
COMMENT=	Lexer generator for Unicode and OCaml

BUILD_DEPENDS=	ocamlbuild:devel/ocaml-ocamlbuild

USES=		gmake ocaml:camlp4,findplist,ldconfig

ALL_TARGET=	all all.opt
MAKE_JOBS_UNSAFE=	yes

DOCSDIR=	${OCAML_DOCSDIR}/${PORTNAME}
PORTDOCS=	*

OPTIONS_DEFINE=	DOCS

DOCS_ALL_TARGET=	doc

.include <bsd.port.options.mk>

.if ${ARCH} == powerpc
USE_BINUTILS=	yes
.endif

post-patch:
	@${REINPLACE_CMD} -e '/^let latin1_sub_lexeme/,+3 \
		s,^  s,  Bytes.to_string s,' ${WRKSRC}/ulexing.ml

post-install-DOCS-on:
	@${MKDIR} ${STAGEDIR}${DOCSDIR}
	${INSTALL_DATA} ${WRKSRC}/*.html ${WRKSRC}/*.css ${STAGEDIR}${DOCSDIR}

.include <bsd.port.mk>
