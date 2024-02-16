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

Configuration
=============

Config is just harcoded in util.h so edit that if you want to change anything:

```c
#define WIDTH 160
#define HEIGHT 40
#define XPOS 0
#define YPOS 40

#define BORDERWIDTH 0
#define TIMEFORMAT "%a %b %d %I:%M %p"

#define XFONT "7x14bold"
// #define XFTFONT \
// "CascadiaMono:style=bold:pixelsize=14,monospace:pixelsize=14\n"

#define BGCOLOR ((uint32_t)0x00000000)
#define FONTCOLOR ((uint32_t)0xFF3F51B5)
```

`BGCOLOR` and `FONTCOLOR` are in 0xAARRGGBB

**NOTE:** Only define **one** of `XFONT` or `XFTFONT` or it won't compile.



