PORTNAME=	seerr
DISTVERSIONPREFIX=	v
DISTVERSION=	3.5.0
CATEGORIES=	www

MAINTAINER=	quentin.thebault@defenso.fr
COMMENT=	Media request and discovery manager for Jellyfin, Plex, and Emby
WWW=		https://seerr.dev/

LICENSE=	MIT
LICENSE_FILE=	${WRKSRC}/LICENSE

ONLY_FOR_ARCHS=	aarch64 amd64
ONLY_FOR_ARCHS_REASON=	native Node.js modules are only tested on \
			64-bit platforms

LIB_DEPENDS=	libvips.so:graphics/vips

USES=		gettext-runtime gmake gnome nodejs:22,build,run npm:pnpm \
		pkgconfig python:build sqlite:3
USE_GNOME=	glib20
USE_GITHUB=	yes
GH_ACCOUNT=	seerr-team

USE_RC_SUBR=	seerr

# Next.js ships no native SWC binding for FreeBSD, so files/packagejsons
# adds @next/swc-wasm-nodejs as a dependency: the prefetched store then
# carries the WebAssembly binding, which next would otherwise try to
# download.  Turbopack cannot use it, hence next build --webpack.
MAKE_ENV=	CI=1 \
		CYPRESS_INSTALL_BINARY=0 \
		NEXT_TELEMETRY_DISABLED=1 \
		NODE_OPTIONS=--max-old-space-size=4096 \
		PYTHON=${PYTHON_CMD} \
		SHARP_FORCE_GLOBAL_LIBVIPS=1 \
		npm_config_build_from_source=true \
		npm_config_nodedir=${LOCALBASE} \
		npm_config_python=${PYTHON_CMD} \
		npm_config_sqlite=${LOCALBASE}

USERS=		seerr
GROUPS=		seerr

do-build:
	cd ${WRKSRC} && ${SETENV} ${MAKE_ENV} \
		${NPM_CMDNAME} rebuild bcrypt
# sqlite3 pins node-gyp 8, which needs distutils, gone since Python 3.12;
# build it with the node-gyp the project itself depends on, against
# databases/sqlite3: node links that one, so a bundled SQLite would be
# shadowed at runtime anyway.  The patch to database.cc re-enables the
# double-quoted string literals the database migrations rely on.
	cd ${WRKSRC}/node_modules/sqlite3/ && ${SETENV} ${MAKE_ENV} \
		${WRKSRC}/node_modules/.bin/node-gyp rebuild
# sharp has no prebuilt binary for FreeBSD and its install script falls
# back to "npm run build"; run the build script directly instead.
	cd ${WRKSRC}/node_modules/sharp/ && ${SETENV} ${MAKE_ENV} \
		PATH=${WRKSRC}/node_modules/.bin:${PATH} \
		${LOCALBASE}/bin/node install/build.js
# next imports the binding from node_modules by a file URL made of the bare
# package name, which never resolves, and then falls back to the directory
# it downloads into.  Put the prefetched binding there.
	@${MKDIR} ${WRKSRC}/node_modules/next/wasm/@next
	${CP} -R ${WRKSRC}/node_modules/@next/swc-wasm-nodejs/ \
		${WRKSRC}/node_modules/next/wasm/@next/swc-wasm-nodejs
	cd ${WRKSRC} && ${SETENV} ${MAKE_ENV} \
		${NPM_CMDNAME} exec next build --webpack
	cd ${WRKSRC} && ${SETENV} ${MAKE_ENV} \
		${NPM_CMDNAME} run build:server
	cd ${WRKSRC} && ${SETENV} ${MAKE_ENV} \
		${NPM_CMDNAME} prune --prod --ignore-scripts
	${RM} -r ${WRKSRC}/.next/cache

do-install:
	${MKDIR} ${STAGEDIR}${WWWDIR}
	cd ${WRKSRC} && ${PAX} -rw package.json next.config.ts \
		seerr-api.yml .next dist node_modules public ${STAGEDIR}${WWWDIR}
# next writes optimized images under .next/cache at runtime
	${MKDIR} ${STAGEDIR}/var/cache/seerr ${STAGEDIR}/var/db/seerr
	${RLN} ${STAGEDIR}/var/cache/seerr ${STAGEDIR}${WWWDIR}/.next/cache

post-install:
# node-gyp is a build tool and bcrypt's prebuilds are for other systems;
# drop them, and the links pnpm made to node-gyp.  The .bin wrappers only
# serve the command line and embed WRKDIR.
	@${RM} -r ${STAGEDIR}${WWWDIR}/node_modules/.pnpm/node-gyp@* \
		${STAGEDIR}${WWWDIR}/node_modules/.pnpm/bcrypt@*/node_modules/bcrypt/prebuilds \
		${STAGEDIR}${WWWDIR}/node_modules/.pnpm/bcrypt@*/node_modules/bcrypt/build-all.sh
	@${FIND} ${STAGEDIR}${WWWDIR}/node_modules -type l \
		! -exec ${TEST} -e {} \; -delete
	@${FIND} ${STAGEDIR}${WWWDIR}/node_modules -type d -name .bin -prune \
		-exec ${RM} -r {} +
# Leftovers of the native module builds: only build/Release/*.node is
# loaded at runtime.
	@${FIND} ${STAGEDIR}${WWWDIR}/node_modules -type d \
		\( -name obj.target -o -name .deps \) -prune \
		-exec ${RM} -r {} +
	@${FIND} ${STAGEDIR}${WWWDIR}/node_modules -path '*/build/*' -type f \
		\( -name Makefile -o -name '*.mk' -o -name config.gypi \
		-o -name '*.a' -o -name '*.stamp' \) -delete
	@${FIND} ${STAGEDIR}${WWWDIR} -name '*.node' -exec ${STRIP_CMD} {} +
# Build-time records that embed WRKSRC
	@${RM} ${STAGEDIR}${WWWDIR}/.next/trace \
		${STAGEDIR}${WWWDIR}/node_modules/.modules.yaml
	@${SED} -i '' -e 's|${WRKSRC}|${WWWDIR}|g' \
		${STAGEDIR}${WWWDIR}/.next/required-server-files.js \
		${STAGEDIR}${WWWDIR}/.next/required-server-files.json
	@${FIND} ${STAGEDIR}${WWWDIR} -type d -empty -delete
	@cd ${STAGEDIR}${PREFIX} && ${FIND} ${WWWDIR_REL} \
		\( -type f -o -type l \) | ${SORT} >> ${TMPPLIST}
	@${ECHO_CMD} "@dir(seerr,seerr,750) /var/cache/seerr" >> ${TMPPLIST}
	@${ECHO_CMD} "@dir(seerr,seerr,750) /var/db/seerr" >> ${TMPPLIST}

.include <bsd.port.mk>
