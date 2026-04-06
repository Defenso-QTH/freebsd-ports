--- seatd/seat.c.orig	2026-02-28 21:04:33 UTC
+++ seatd/seat.c
@@ -94,7 +94,7 @@ static int vt_close(int vt) {
 		log_errorf("Could not open terminal to clean up VT %d: %s", vt, strerror(errno));
 		return -1;
 	}
-	terminal_set_process_switching(ttyfd, true);
+	terminal_set_process_switching(ttyfd, false);
 	terminal_set_keyboard(ttyfd, true);
 	terminal_set_graphics(ttyfd, false);
 	close(ttyfd);
@@ -281,8 +281,11 @@ void seat_remove_client(struct client *client) {
 		if (was_current && seat->active_client == NULL) {
 			// This client was current, but there were no clients
 			// waiting to take this VT, so clean it up.
 			log_debug("Closing active VT");
-			vt_close(seat->cur_vt);
+			vt_close(client->session);
 		} else if (!was_current && client->state != CLIENT_CLOSED) {
 			// This client was not current, but as the client was
 			// running, we need to clean up the VT.
