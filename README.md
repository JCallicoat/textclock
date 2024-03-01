Textclock
=========

It's a text clock kinda like `xclock -d`.

Draws an undecorated (borderless) window with a black brackground using libxcb, sets it to be always on top, and writes the current date and time to it every 50ms in a purple font.

The background color and font color can be configured, see below.

Useful when you want to keep track of time while doing some other activity in another window.

![obligatry pic](textclock.png)

When the window has focus the `q` or `ESC` keys will exit.

To see what it looks like run `make run`

To install to ~/.local/bin run `make install`

A desktop file will also be added to ~/.local/share/applications/textclock.desktop

Configuration
=============

Options are configurable via cli flags. See `-h` / `--help`:

```shell
Usage of ./textclock:
Flags:
        --fg-color      (string, optional)
                Default: 0xFF3F51B5
                32-bit color string of the form 0xAARRGGBB

        --bg-color      (string, optional)
                Default: 0x00000000
                32-bit color string of the form 0xAARRGGBB

        --width (integer, optional)
                Default: 160
                Width of the window

        --height        (integer, optional)
                Default: 40
                Height of the window

        --xpos  (integer, optional)
                Default: 0
                X position of the window

        --ypos  (integer, optional)
                Default: 0
                Y position of the window

        --time-format   (string, optional)
                Default: %a %b %d %I:%M %p
                Time format for clock

        --xfont (string, optional)
                Default: 7x14bold
                X font to use (see xlsfonts)

        --xftfont       (string, optional)
                Default: CascadiaMono:style=bold:pixelsize=13,monospace:pixelsize=13
                Freetype font search string

        --freetype, --no-freetype       (boolean, optional)
                Default: False
                Use FreeType font (uses xftfont value)

        --on-top, --no-on-top   (boolean, optional)
                Default: True
                Adds hints to stay on top (_NET_WM_STATE_ABOVE)

        --skip-taskbar, --no-skip-taskbar       (boolean, optional)
                Default: False
                Adds hints to skip taskbar and pager (_NET_WM_STATE_SKIP_TASKBAR | _NET_WM_STATE_SKIP_PAGER)

        --is-dock, --no-is-dock (boolean, optional)
                Default: False
                Sets the window to be a dock (_NET_WM_WINOW_TYPE_DOCK)

        --help, --no-help       (boolean, optional)
                Default: False
                Print this help

```

Credits
=======

Uses utf8.h and xcbft.h from https://github.com/venam/fonts-for-xcb for freetype fonts

Uses kgflags from https://github.com/kgabis/kgflags for cli argument parsing
