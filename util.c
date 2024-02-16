#include <inttypes.h>
#include <stdio.h>

#include "util.h"
#include "xcbft.h"

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

#ifdef XFONT
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
  uint32_t value_list[3] = {FONTCOLOR, BGCOLOR, font};

  xcb_void_cookie_t gcCookie =
      xcb_create_gc_checked(connection, gc, window, mask, value_list);

  testCookie(gcCookie, connection, "can't create gc");

  /* close font */
  fontCookie = xcb_close_font_checked(connection, font);

  testCookie(fontCookie, connection, "can't close font");

  return gc;
}

void drawText(xcb_connection_t *connection, xcb_screen_t *screen,
              xcb_window_t window, int16_t x1, int16_t y1, char *label) {
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
#endif

#ifdef XFTFONT
void drawText(xcb_connection_t *connection, xcb_screen_t *screen,
              xcb_window_t window, int16_t x1, int16_t y1, char *label) {

  FcStrSet *fontsearch;
  struct xcbft_patterns_holder font_patterns;
  struct utf_holder text;
  struct xcbft_face_holder faces;
  xcb_render_color_t text_color;

  // The fonts to use and the text in unicode
  char *searchlist = XFTFONT;
  text = char_to_uint32(label);

  // extract the fonts in a list
  fontsearch = xcbft_extract_fontsearch_list(searchlist);
  // do the search and it returns all the matching fonts
  font_patterns = xcbft_query_fontsearch_all(fontsearch);
  // no need for the fonts list anymore
  FcStrSetDestroy(fontsearch);
  // get the dpi from the resources or the screen if not available
  long dpi = xcbft_get_dpi(connection);
  // load the faces related to the matching fonts patterns
  faces = xcbft_load_faces(font_patterns, dpi);
  // no need for the matching fonts patterns
  xcbft_patterns_holder_destroy(font_patterns);

  // FIXME: this is pretty ugly is there a better way to do this?
  // FONTCOLOR is a uint32_t with pattern 0xAARRGGBB
  text_color.alpha = ((FONTCOLOR) >> 16) & 0xFF00; // top 8 bits as uint16_t
  text_color.red = ((FONTCOLOR) >> 8) & 0xFF00;    // next 8 bits as unit16_t
  text_color.green = (((FONTCOLOR) >> 8) & 0x00FF) << 8; // etc
  text_color.blue = ((FONTCOLOR) << 8) & 0xFF00;         // etc

  // printf("r:0x%X, g:0x%X, b:0x%X, a:0x%X\n", text_color.red,
  // text_color.green, text_color.blue, text_color.alpha);

  // draw on the drawable (pixmap here) pmap at position (50,60) the text
  // with the color we chose and the faces we chose
  xcbft_draw_text(connection, // X connection
                  window,     // win or pixmap
                  x1, y1,     // x, y
                  text,       // text
                  text_color, faces, dpi);

  // no need for the text and the faces
  utf_holder_destroy(text);
  xcbft_face_holder_destroy(faces);
}
#endif

void drawTime(xcb_connection_t *connection, xcb_screen_t *screen,
              xcb_window_t window, int16_t x1, int16_t y1) {
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  char fmt_time[255];
  strftime(fmt_time, 255, TIMEFORMAT, &tm);

  drawText(connection, screen, window, x1, y1, fmt_time);
}
