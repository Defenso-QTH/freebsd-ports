--- channels/rdpecam/client/camera_device_main.c.orig	2025-09-23 06:40:57 UTC
+++ channels/rdpecam/client/camera_device_main.c
@@ -28,6 +28,7 @@ static const CAM_MEDIA_FORMAT_INFO supportedFormats[] 
  * H264, MJPG, I420 (used as input for H264 encoder), other YUV based, RGB based
  */
 static const CAM_MEDIA_FORMAT_INFO supportedFormats[] = {
+	{ CAM_MEDIA_FORMAT_MJPG, CAM_MEDIA_FORMAT_MJPG },
 /* inputFormat, outputFormat */
 #if defined(WITH_INPUT_FORMAT_H264)
 	{ CAM_MEDIA_FORMAT_H264, CAM_MEDIA_FORMAT_H264 }, /* passthrough */
