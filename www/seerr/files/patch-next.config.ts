--- next.config.ts.orig	2026-09-28 00:06:55 UTC
+++ next.config.ts
@@ -22,6 +22,22 @@ const nextConfig: NextConfig = {
       },
     },
   },
+  // FreeBSD: there is no native SWC binding, so the build uses webpack, not
+  // turbopack; mirror the turbopack rule that turns *.svg into components.
+  webpack(config) {
+    const imageRule = config.module.rules.find(
+      (rule: { test?: RegExp }) =>
+        rule.test instanceof RegExp && rule.test.test('.svg')
+    );
+    if (imageRule) {
+      imageRule.exclude = /\.svg$/i;
+    }
+    config.module.rules.push({
+      test: /\.svg$/i,
+      use: ['@svgr/webpack'],
+    });
+    return config;
+  },
   experimental: {
     scrollRestoration: true,
     largePageDataBytes: 512 * 1000,
