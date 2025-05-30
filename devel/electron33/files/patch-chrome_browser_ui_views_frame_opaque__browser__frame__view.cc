--- chrome/browser/ui/views/frame/opaque_browser_frame_view.cc.orig	2024-10-16 21:31:02 UTC
+++ chrome/browser/ui/views/frame/opaque_browser_frame_view.cc
@@ -54,7 +54,7 @@
 #include "ui/views/window/vector_icons/vector_icons.h"
 #include "ui/views/window/window_shape.h"
 
-#if BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_CHROMEOS)
+#if BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_CHROMEOS) || BUILDFLAG(IS_BSD)
 #include "ui/views/controls/menu/menu_runner.h"
 #endif
 
@@ -559,7 +559,7 @@ OpaqueBrowserFrameView::GetFrameButtonStyle() const {
 OpaqueBrowserFrameView::GetFrameButtonStyle() const {
 // TODO(crbug.com/40118868): Revisit the macro expression once build flag switch
 // of lacros-chrome is complete.
-#if BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_CHROMEOS_LACROS)
+#if BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_CHROMEOS_LACROS) || BUILDFLAG(IS_BSD)
   return FrameButtonStyle::kMdButton;
 #else
   return FrameButtonStyle::kImageButton;
@@ -578,7 +578,7 @@ bool OpaqueBrowserFrameView::ShouldDrawRestoredFrameSh
   return false;
 }
 
-#if BUILDFLAG(IS_LINUX)
+#if BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_BSD)
 bool OpaqueBrowserFrameView::IsTiled() const {
   return frame()->tiled();
 }
@@ -777,7 +777,7 @@ void OpaqueBrowserFrameView::WindowIconPressed() {
 }
 
 void OpaqueBrowserFrameView::WindowIconPressed() {
-#if BUILDFLAG(IS_LINUX)
+#if BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_BSD)
   // Chrome OS doesn't show the window icon, and Windows handles this on its own
   // due to the hit test being HTSYSMENU.
   menu_runner_ = std::make_unique<views::MenuRunner>(
