--- libfreerdp/gdi/gfx.c.orig	2026-09-02 05:48:06 UTC
+++ libfreerdp/gdi/gfx.c
@@ -1288,6 +1288,8 @@ static UINT gdi_CreateSurface(RdpgfxClientContext* context,
 	surface->outputTargetWidth = createSurface->width;
 	surface->outputTargetHeight = createSurface->height;
 
+	const BOOL rails =
+	    freerdp_settings_get_bool(gdi->context->settings, FreeRDP_RemoteApplicationMode);
 	switch (createSurface->pixelFormat)
 	{
 		case GFX_PIXEL_FORMAT_ARGB_8888:
@@ -1295,7 +1297,7 @@ static UINT gdi_CreateSurface(RdpgfxClientContext* context,
 			break;
 
 		case GFX_PIXEL_FORMAT_XRGB_8888:
-			surface->format = PIXEL_FORMAT_BGRA32;
+			surface->format = rails ? PIXEL_FORMAT_BGRA32 : PIXEL_FORMAT_BGRX32;
 			break;
 
 		default:
