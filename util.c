#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "xcbft.h"

#define KGFLAGS_IMPLEMENTATION
#include "kgflags.h"

Options options = {
    .show_help = false,
    .fg_color = 0xFF3F51B5,
    .bg_color = 0x00000000,
    .width = 160,
    .height = 40,
    .xpos = 0,
    .ypos = 0,
    .border_width = 0,
    .time_format = "%a %b %d %I:%M %p",
    .use_freetype = false,
    .xfont = "7x14bold",
    .xftfont = "CascadiaMono:style=bold:pixelsize=13,monospace:pixelsize=13"};

bool parse_args(int argc, char **argv) {

  kgflags_bool("help", options.show_help, "Print this help", false,
               &options.show_help);

  const char *fg_color_string = NULL;
  kgflags_string("fg-color", "0xFF3F51B5",
                 "32-bit color string of the form 0xAARRGGBB",
                 false, // required
                 &fg_color_string);

  const char *bg_color_string = NULL;
  kgflags_string("bg-color", "0x00000000",
                 "32-bit color string of the form 0xAARRGGBB",
                 false, // required
                 &bg_color_string);

  int width = options.width;
  kgflags_int("width", width, "Width of the window",
              false, // required
              &width);

  int height = options.height;
  kgflags_int("height", height, "Height of the window",
              false, // required
              &height);

  int xpos = options.xpos;
  kgflags_int("xpos", xpos, "X position of the window",
              false, // required
              &xpos);

  int ypos = options.ypos;
  kgflags_int("ypos", ypos, "Y position of the window",
              false, // required
              &ypos);

  int border_width = options.border_width;
  kgflags_int("border_width", border_width, "Border width of window",
              false, // required
              &border_width);

  const char *time_format_string = malloc(255 * sizeof(char));
  kgflags_string("time-format", options.time_format, "Time format for clock",
                 false, // required
                 &time_format_string);

  const char *xfont_string = malloc(512 * sizeof(char));
  kgflags_string("xfont", options.xfont, "X font to use (see xlsfonts)",
                 false, // required
                 &xfont_string);

  const char *xftfont_string = malloc(512 * sizeof(char));
  kgflags_string("xftfont", options.xftfont, "Freetype font search string",
                 false, // required
                 &xftfont_string);

  kgflags_bool("freetype", options.use_freetype,
               "Use FreeType font (uses xftfont value)", false,
               &options.use_freetype);

  if (!kgflags_parse(argc, argv)) {
    kgflags_print_errors();
    kgflags_print_usage();
    return false;
  }

  if (options.show_help) {
    kgflags_print_usage();
    return false;
  }

  uint32_t fg_color = strtol(fg_color_string, NULL, 16);
  options.fg_color = (uint32_t)fg_color;

  uint32_t bg_color = strtol(bg_color_string, NULL, 16);
  options.bg_color = (uint32_t)bg_color;

  options.width = (uint16_t)width;
  options.height = (uint16_t)height;

  options.xpos = (uint16_t)xpos;
  options.ypos = (uint16_t)ypos;

  options.border_width = (uint16_t)border_width;

  options.time_format = time_format_string;

  return true;
}

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
  uint32_t value_list[3] = {options.fg_color, options.bg_color, font};

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

  if (!options.use_freetype) {
    /* get graphics context */
    xcb_gcontext_t gc = getFontGC(connection, screen, window, options.xfont);

    /* draw the text */
    xcb_void_cookie_t textCookie = xcb_image_text_8_checked(
        connection, strlen(label), window, gc, x1, y1, label);

    testCookie(textCookie, connection, "can't write text");

    /* free the gc */
    xcb_void_cookie_t gcCookie = xcb_free_gc(connection, gc);

    testCookie(gcCookie, connection, "can't free gc");
  } else {

    FcStrSet *fontsearch;
    struct xcbft_patterns_holder font_patterns;
    struct utf_holder text;
    struct xcbft_face_holder faces;
    xcb_render_color_t text_color;

    // The fonts to use and the text in unicode
    char *searchlist = (char *)options.xftfont;
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

    // FONTCOLOR is a uint32_t with pattern 0xAARRGGBB
    text_color.alpha =
        ((options.fg_color >> 24) & 0xFF) * 257; // top 8 bits as uint16_t
    text_color.red = ((options.fg_color >> 16) & 0xFF) * 257;  // next 8 bits
    text_color.green = ((options.fg_color >> 8) & 0xFF) * 257; // etc
    text_color.blue = (options.fg_color & 0xFF) * 257;         // etc

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
}

void drawTime(xcb_connection_t *connection, xcb_screen_t *screen,
              xcb_window_t window, int16_t x1, int16_t y1) {
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  char fmt_time[255];
  strftime(fmt_time, 255, options.time_format, &tm);

  drawText(connection, screen, window, x1, y1, fmt_time);
}
