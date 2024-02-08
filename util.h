#include <inttypes.h> // IWYU pragma: export
#include <stdio.h>    // IWYU pragma: export
#include <stdlib.h>   // IWYU pragma: export
#include <string.h>   // IWYU pragma: export
#include <unistd.h>

#include <xcb/xcb.h>
#include <xcb/xcb_ewmh.h>
#include <xcb/xproto.h>

#define WIDTH 160
#define HEIGHT 40

#define XPOS 0
#define YPOS 40

#define BORDERWIDTH 0

#define TIMEFORMAT "%a %b %d %I:%M %p"

#define XFONT "7x14bold"

typedef struct MotifHints {
  uint32_t flags;
  uint32_t functions;
  uint32_t decorations;
  int32_t input_mode;
  uint32_t status;
} Hints;

void testCookie(xcb_void_cookie_t cookie, xcb_connection_t *connection,
                char *errMessage);

xcb_intern_atom_reply_t *getAtom(xcb_connection_t *connection, char *name,
                                 uint8_t only_if_exists);

xcb_gc_t getFontGC(xcb_connection_t *connection, xcb_screen_t *screen,
                   xcb_window_t window, const char *font_name);

void drawText(xcb_connection_t *connection, xcb_screen_t *screen,
              xcb_window_t window, int16_t x1, int16_t y1, const char *label);

void drawTime(xcb_connection_t *connection, xcb_screen_t *screen,
              xcb_window_t window, int16_t x1, int16_t y1);
