#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "util.h"

int main() {
  /* get the connection */
  int screenNum;
  xcb_connection_t *connection = xcb_connect(NULL, &screenNum);
  if (!connection) {
    fprintf(stderr, "ERROR: can't connect to an X server\n");
    return -1;
  }

  /* get the current screen */
  xcb_screen_iterator_t iter =
      xcb_setup_roots_iterator(xcb_get_setup(connection));

  // we want the screen at index screenNum of the iterator
  for (int i = 0; i < screenNum; ++i) {
    xcb_screen_next(&iter);
  }

  xcb_screen_t *screen = iter.data;

  if (!screen) {
    fprintf(stderr, "ERROR: can't get the current screen\n");
    xcb_disconnect(connection);
    return -1;
  }

  /* create the window */
  xcb_window_t window = xcb_generate_id(connection);

  uint32_t mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
  uint32_t values[2];
  values[0] = BGCOLOR;
  values[1] = XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_BUTTON_PRESS |
              XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_POINTER_MOTION;

  xcb_void_cookie_t windowCookie = xcb_create_window_checked(
      connection, screen->root_depth, window, screen->root, XPOS, YPOS, WIDTH,
      HEIGHT, BORDERWIDTH, XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual,
      mask, values);

  testCookie(windowCookie, connection, "can't create window");

  /* set the title of the window */
  char *title = "textclock";
  xcb_void_cookie_t titleCookie = xcb_change_property_checked(
      connection, XCB_PROP_MODE_REPLACE, window, XCB_ATOM_WM_NAME,
      XCB_ATOM_STRING, 8, strlen(title), title);

  testCookie(titleCookie, connection, "can't set window title");

  /* set the icon name */
  char *iconName = "clock";
  xcb_void_cookie_t iconNameCookie = xcb_change_property_checked(
      connection, XCB_PROP_MODE_REPLACE, window, XCB_ATOM_WM_ICON_NAME,
      XCB_ATOM_STRING, 8, strlen(iconName), iconName);

  testCookie(iconNameCookie, connection, "can't set window icon name");

  /* always on top ewmh state */
  xcb_ewmh_connection_t ewmh;
  xcb_intern_atom_cookie_t *ewmhCookie = xcb_ewmh_init_atoms(connection, &ewmh);
  xcb_ewmh_init_atoms_replies(&ewmh, ewmhCookie, NULL);

  xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window,
                      ewmh._NET_WM_WINDOW_TYPE, XCB_ATOM_ATOM, 32, 1,
                      &ewmh._NET_WM_WINDOW_TYPE_NORMAL);

  xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window,
                      ewmh._NET_WM_STATE, XCB_ATOM_ATOM, 32, 1,
                      &ewmh._NET_WM_STATE_ABOVE);

  /*
  // allegedly we should tell the wm this through a message...
  // but doesn't seem needed?
  xcb_client_message_event_t payload = {
      .response_type = XCB_CLIENT_MESSAGE,
      .type = EWMH._NET_WM_STATE,
      .format = 32,
      .window = window,
      .data = {.data32[0] = 1, // _NET_WM_STATE_ADD
               .data32[1] = EWMH._NET_WM_STATE_ABOVE,
               .data32[2] = XCB_ATOM_NONE}};

  xcb_send_event(connection, 1, window,
                 XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT |
                     XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY,
                 (const char *)&payload);
   */

  xcb_ewmh_connection_wipe(&ewmh);

  /* no window decorations */
  Hints hints;
  hints.flags = 2;
  hints.functions = 0;
  hints.decorations = 0;
  hints.input_mode = 0;
  hints.status = 0;

  xcb_intern_atom_reply_t *motifHintsReply =
      getAtom(connection, "_MOTIF_WM_HINTS", 0);
  xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window,
                      motifHintsReply->atom, motifHintsReply->atom, 32, 5,
                      &hints);
  free(motifHintsReply);

  /* handle wm window close */
  xcb_intern_atom_reply_t *wmProtoReply =
      getAtom(connection, "WM_PROTOCOLS", 1);
  xcb_intern_atom_reply_t *wmDeleteReply =
      getAtom(connection, "WM_DELETE_WINDOW", 0);
  xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window,
                      wmProtoReply->atom, XCB_ATOM_ATOM, 32, 1,
                      &wmDeleteReply->atom);

  xcb_atom_t deleteAtom = wmDeleteReply->atom;
  free(wmProtoReply);
  free(wmDeleteReply);

  /* show the window */
  xcb_void_cookie_t mapCookie = xcb_map_window_checked(connection, window);

  testCookie(mapCookie, connection, "can't map window");

  xcb_flush(connection); // make sure window is drawn

  int slept = 0;

  /* event loop */
  xcb_generic_event_t *event;
  while (1) {
    if ((event = xcb_poll_for_event(connection))) {
      switch (event->response_type & ~0x80) {
      default:
      case XCB_EXPOSE: {
        drawTime(connection, screen, window, 10, HEIGHT - 14);
        break;
      }
      case XCB_KEY_RELEASE: {
        xcb_key_release_event_t *kr = (xcb_key_release_event_t *)event;
        switch (kr->detail) {
        // ESC
        case 9:
        // Q
        case 24: {
          goto quit;
        }
        }
      }
      case XCB_CLIENT_MESSAGE: {
        xcb_client_message_event_t *cm = (xcb_client_message_event_t *)event;
        if (cm->data.data32[0] == deleteAtom) {
          // puts("Goodbye");
          goto quit;
        }
        break;
      }
      }
      free(event);
    }
    slept += 50 + usleep(50 * 1000); // 50ms
    if (slept >= 1000) {
      drawTime(connection, screen, window, 10, HEIGHT - 14);
      slept = 0;
    }
  }

quit:
  free(event);
  xcb_disconnect(connection);
  return 0;
}
