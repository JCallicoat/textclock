#include "util.h"

void testCookie(xcb_void_cookie_t cookie, xcb_connection_t *connection,
                char *errMessage) {
  xcb_generic_error_t *error = xcb_request_check(connection, cookie);
  if (error) {
    fprintf(stderr, "ERROR: %s : %" PRIu8 "\n", errMessage, error->error_code);
    xcb_disconnect(connection);
    exit(-1);
  }
}

xcb_intern_atom_reply_t *getAtom(xcb_connection_t *connection, char *name,
                                 uint8_t only_if_exists) {
  xcb_intern_atom_cookie_t cookie =
      xcb_intern_atom(connection, 0, strlen(name), name);
  return xcb_intern_atom_reply(connection, cookie, NULL);
}

xcb_gc_t getFontGC(xcb_connection_t *connection, xcb_screen_t *screen,
                   xcb_window_t window, const char *font_name) {
  /* get font */
  xcb_font_t font = xcb_generate_id(connection);
  xcb_void_cookie_t fontCookie =
      xcb_open_font_checked(connection, font, strlen(font_name), font_name);

  testCookie(fontCookie, connection, "can't open font");

  /* create graphics context */
  xcb_gcontext_t gc = xcb_generate_id(connection);
  uint32_t mask = XCB_GC_FOREGROUND | XCB_GC_BACKGROUND | XCB_GC_FONT;
  uint32_t value_list[3] = {0xAA3F51B5, screen->black_pixel, font};

  xcb_void_cookie_t gcCookie =
      xcb_create_gc_checked(connection, gc, window, mask, value_list);

  testCookie(gcCookie, connection, "can't create gc");

  /* close font */
  fontCookie = xcb_close_font_checked(connection, font);

  testCookie(fontCookie, connection, "can't close font");

  return gc;
}

void drawText(xcb_connection_t *connection, xcb_screen_t *screen,
              xcb_window_t window, int16_t x1, int16_t y1, const char *label) {
  /* get graphics context */
  xcb_gcontext_t gc = getFontGC(connection, screen, window, XFONT);

  /* draw the text */
  xcb_void_cookie_t textCookie = xcb_image_text_8_checked(
      connection, strlen(label), window, gc, x1, y1, label);

  testCookie(textCookie, connection, "can't write text");

  /* free the gc */
  xcb_void_cookie_t gcCookie = xcb_free_gc(connection, gc);

  testCookie(gcCookie, connection, "can't free gc");
}

void drawTime(xcb_connection_t *connection, xcb_screen_t *screen,
              xcb_window_t window, int16_t x1, int16_t y1) {
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  char fmt_time[255];
  strftime(fmt_time, 255, TIMEFORMAT, &tm);

  drawText(connection, screen, window, x1, y1, fmt_time);
}
