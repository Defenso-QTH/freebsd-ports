--- node_modules/.pnpm/sqlite3@5.1.7/node_modules/sqlite3/src/database.cc.orig	2026-10-09 03:15:37 UTC
+++ node_modules/.pnpm/sqlite3@5.1.7/node_modules/sqlite3/src/database.cc
@@ -173,6 +173,12 @@ void Database::Work_Open(napi_env e, void* data) {
     else {
         // Set default database handle values.
         sqlite3_busy_timeout(db->_handle, 1000);
+        // FreeBSD: databases/sqlite3 builds with SQLITE_DQS=0, and node
+        // links it, so its symbols take precedence over a bundled SQLite.
+        // Accept double-quoted string literals as upstream SQLite does:
+        // the database migrations of applications rely on them.
+        sqlite3_db_config(db->_handle, SQLITE_DBCONFIG_DQS_DML, 1, (int *)NULL);
+        sqlite3_db_config(db->_handle, SQLITE_DBCONFIG_DQS_DDL, 1, (int *)NULL);
     }
 }
 
