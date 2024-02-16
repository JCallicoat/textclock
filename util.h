#ifndef UTIL_H_
#define UTIL_H_

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
/*
#define XFTFONT "CascadiaMono:style=bold:pixelsize=14,monospace:pixelsize=14\n"
*/

#define BGCOLOR ((uint32_t)0x00000000)
#define FONTCOLOR ((uint32_t)0xFF3F51B5)

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

void drawText(xcb_connection_t *connection, xcb_screen_t *screen,
              xcb_window_t window, int16_t x1, int16_t y1, char *label);

void drawTime(xcb_connection_t *connection, xcb_screen_t *screen,
              xcb_window_t window, int16_t x1, int16_t y1);

#endif // UTIL_H_
