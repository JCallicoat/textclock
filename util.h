#ifndef UTIL_H_
#define UTIL_H_

#include <stdbool.h>
#include <xcb/xcb.h>
#include <xcb/xcb_ewmh.h>
#include <xcb/xproto.h>

typedef struct Options {
  bool show_help;
  uint32_t fg_color;
  uint32_t bg_color;
  uint16_t width;
  uint16_t height;
  uint16_t xpos;
  uint16_t ypos;
  uint8_t border_width;
  const char *time_format;
  bool use_freetype;
  const char *xfont;
  const char *xftfont;
} Options;

extern Options options;

typedef struct MotifHints {
  uint32_t flags;
  uint32_t functions;
  uint32_t decorations;
  int32_t input_mode;
  uint32_t status;
} Hints;

bool parse_args(int argc, char **argv);

void testCookie(xcb_void_cookie_t cookie, xcb_connection_t *connection,
                char *errMessage);

xcb_intern_atom_reply_t *getAtom(xcb_connection_t *connection, char *name,
                                 uint8_t only_if_exists);

void drawText(xcb_connection_t *connection, xcb_screen_t *screen,
              xcb_window_t window, int16_t x1, int16_t y1, char *label);

void drawTime(xcb_connection_t *connection, xcb_screen_t *screen,
              xcb_window_t window, int16_t x1, int16_t y1);

#endif // UTIL_H_
