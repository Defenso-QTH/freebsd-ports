--- client/X11/xf_gfx.c.orig	2026-09-02 05:48:06 UTC
+++ client/X11/xf_gfx.c
@@ -348,6 +348,8 @@ static UINT xf_CreateSurface(RdpgfxClientContext* context,
 	surface->gdi.outputTargetWidth = createSurface->width;
 	surface->gdi.outputTargetHeight = createSurface->height;
 
+	const BOOL rails =
+	    freerdp_settings_get_bool(gdi->context->settings, FreeRDP_RemoteApplicationMode);
 	switch (createSurface->pixelFormat)
 	{
 		case GFX_PIXEL_FORMAT_ARGB_8888:
@@ -355,7 +357,7 @@ static UINT xf_CreateSurface(RdpgfxClientContext* context,
 			break;
 
 		case GFX_PIXEL_FORMAT_XRGB_8888:
-			surface->gdi.format = PIXEL_FORMAT_BGRA32;
+			surface->gdi.format = rails ? PIXEL_FORMAT_BGRA32 : PIXEL_FORMAT_BGRX32;
 			break;
 
 		default:
