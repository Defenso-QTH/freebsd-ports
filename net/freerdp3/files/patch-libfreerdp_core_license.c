--- libfreerdp/core/license.c.orig	2026-09-02 05:48:06 UTC
+++ libfreerdp/core/license.c
@@ -508,20 +508,15 @@ state_run_t license_recv(rdpLicense* license, wStream* s)
 }
 
 WINPR_ATTR_NODISCARD
-static BOOL license_check_stream_length(wLog* log, wStream* s, SSIZE_T expect, const char* where)
+static BOOL license_check_stream_length(wLog* log, wStream* s, UINT64 expect, const char* where)
 {
 	const size_t remain = Stream_GetRemainingLength(s);
 
 	WINPR_ASSERT(where);
 
-	if (expect < 0)
+	if (remain < expect)
 	{
-		WLog_Print(log, WLOG_WARN, "invalid %s, expected value %" PRIdz " invalid", where, expect);
-		return FALSE;
-	}
-	if (remain < (size_t)expect)
-	{
-		WLog_Print(log, WLOG_WARN, "short %s, expected %" PRIdz " bytes, got %" PRIuz, where,
+		WLog_Print(log, WLOG_WARN, "short %s, expected %" PRIu64 " bytes, got %" PRIuz, where,
 		           expect, remain);
 		return FALSE;
 	}
@@ -760,7 +755,14 @@ static BOOL license_read_preamble(wLog* log, wStream* s, BYTE* bMsgType, BYTE* f
 	Stream_Read_UINT8(s, *bMsgType);  /* bMsgType (1 byte) */
 	Stream_Read_UINT8(s, *flags);     /* flags (1 byte) */
 	Stream_Read_UINT16(s, *wMsgSize); /* wMsgSize (2 bytes) */
-	return license_check_stream_length(log, s, *wMsgSize - 4ll, "license preamble::wMsgSize");
+	if (*wMsgSize < 4)
+	{
+		WLog_Print(log, WLOG_WARN,
+		           "invalid license preamble::wMsgSize, expected value >= 4, got %" PRIu32,
+		           *wMsgSize);
+		return FALSE;
+	}
+	return license_check_stream_length(log, s, *wMsgSize - 4ull, "license preamble::wMsgSize");
 }
 
 /**
@@ -1732,7 +1734,7 @@ BOOL license_read_scope_list(wLog* log, wStream* s, SCOPE_LIST* scopeList)
 
 	Stream_Read_UINT32(s, scopeCount); /* ScopeCount (4 bytes) */
 
-	if (!license_check_stream_length(log, s, 4ll * scopeCount, "license scope list::count"))
+	if (!license_check_stream_length(log, s, 4ull * scopeCount, "license scope list::count"))
 		return FALSE;
 
 	if (!license_scope_list_resize(scopeList, scopeCount))
