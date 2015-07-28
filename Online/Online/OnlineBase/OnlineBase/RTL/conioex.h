// created by renkoe@tbkoenig.de

#ifndef   CONIOEX_H
#define CONIOEX_H

#define TBK_CONIOEXAPI  __cdecl

#ifdef __BORLANDC__

// We don't need conioex.h since Borland-Compilers support
// their own prototypes trough conio.h
#include <conio.h>

#else

#if (!defined(_WIN32))
#error Only Win32-Console-Targets supported
#endif

#ifndef __COLORS
#define __COLORS

enum COLORS
  {
    BLACK,          /* dark colors */
    BLUE,
    GREEN,
    CYAN,
    RED,
    MAGENTA,
    BROWN,
    LIGHTGRAY,
    DARKGRAY,       /* light colors */
    LIGHTBLUE,
    LIGHTGREEN,
    LIGHTCYAN,
    LIGHTRED,
    LIGHTMAGENTA,
    YELLOW,
    WHITE
  };
#endif

// cursortypes used with _setcursortype()
#define _NOCURSOR      0
#define _SOLIDCURSOR   1
#define _NORMALCURSOR  2

// use this macro to build the attribute used with textattr()
// usage: BUILD_TEXTATTR(textcolor, backgroundcolor);
#ifndef BUILD_TEXTATTR
#define BUILD_TEXTATTR(f, b)    (int)((((b) & 15) << 4) | ((f) & 15))
#endif

#ifdef __cplusplus
extern "C" {
#endif

  // clears the screen and moves the cursor to position x = 1, y = 1
  void  TBK_CONIOEXAPI  clrscr(void);

  // clears the line from the actual cursor position to the end,
  // the cursor position remains unchanged
  void  TBK_CONIOEXAPI  clreol(void);

  // moves the cursor to position x, y
  // to move the cursor to the top-left corner of the screen use
  // the coordinates x = 1, y = 1
  void  TBK_CONIOEXAPI  gotoxy(int x, int y);

  // returns the x-coordinate of the current cursor-position
  int   TBK_CONIOEXAPI  wherex(void);

  // returns the y-coordinate of the current cursor-position
  int   TBK_CONIOEXAPI  wherey(void);

  // inserts a line at the current cursor position and moves all
  // lines from the current cursor-position one line down. the
  // bottom line is discarded
  void  TBK_CONIOEXAPI  insline(void);

  // deletes the line at the current cursor-position and moves all
  // lines below the cursor one line up
  void  TBK_CONIOEXAPI  delline(void);

  // sets the current textcolor. possible values are in the range
  // from 0-15 and are seen in the COLORS-enumeration
  void  TBK_CONIOEXAPI  textcolor(int newcolor);

  // sets the current color for the textbackground. possible values
  // are in the range from 0-15 and are seen in the COLORS-enumeration
  void  TBK_CONIOEXAPI  textbackground(int newcolor);

  // sets the textcolor and the textbackground in one step. build the
  // attribute using the BUILD_TEXTATTR() macro
  void  TBK_CONIOEXAPI  textattr(int newattr);

  // turns on high intensity for following text outputs
  void  TBK_CONIOEXAPI  highvideo(void);

  // turns off high intensity for following text outputs
  void  TBK_CONIOEXAPI  lowvideo(void);

  // turns on underline mode
  void TBK_CONIOEXAPI underlinevideo(void);

  // turns off underline mode
  void TBK_CONIOEXAPI nounderlinevideo(void);

  // turns on reverse video mode
  void TBK_CONIOEXAPI inversevideo(void);

  // turns off reverse video mode
  void TBK_CONIOEXAPI noinversevideo(void);

  // turn on blinking
  void TBK_CONIOEXAPI blinkvideo(void);

  // turn off blinking
  void TBK_CONIOEXAPI noblinkvideo(void);

  // turn off underline and inverse mode
  void TBK_CONIOEXAPI normalvideo(void);

  // set the cursor shape to one of the following shapes:
  // _NOCURSOR     - the cursor is invisible
  // _NORMALCURSOR - default underscore-cursor
  // _SOLIDCURSOR  - boxed cursor
  void  TBK_CONIOEXAPI  _setcursortype(int cur_t);

  // returns the console size
  void  TBK_CONIOEXAPI  consolesize(int* rows, int* cols);

  // Scroll console up or down
  bool TBK_CONIOEXAPI consolescroll(bool Up);

#ifdef __cplusplus
}
#endif

#endif  // __BORLANDC__
#endif  // CONIOEX_H
