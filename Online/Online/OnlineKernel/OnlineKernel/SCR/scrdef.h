#ifndef __SCRDEF_H
#define __SCRDEF_H

#ifdef IBM
#include "scr_ibm.h"
#define BELL           47
#define CARRIAGERETURN 13
#define LINEFEED       37
#define ESCAPE         39
#else
#define BELL            7
#define CARRIAGERETURN 13
#define LINEFEED       10
#define ESCAPE         27
#endif

typedef enum {
        INVALID = 0x100, 
        CTRL_A,
        CTRL_B,
        CTRL_C,
        CTRL_D,
        CTRL_E,
        CTRL_F,
        CTRL_G,
        CTRL_H,
        CTRL_I,
        CTRL_J,
        CTRL_K,
        CTRL_L,
        CTRL_M,
        CTRL_N,
        CTRL_O,
        CTRL_P,
        CTRL_Q,
        CTRL_R,
        CTRL_S,
        CTRL_T,
        CTRL_U,
        CTRL_V,
        CTRL_W,
        CTRL_X,
        CTRL_Y,
        CTRL_Z,

        DELETE,

        MOVE_UP,
        MOVE_DOWN,
        MOVE_LEFT,
        MOVE_RIGHT,

        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,
        F13,
        F14,
        F15,
        F16,
        F17,
        F18,
        F19,
        F20,
        KPD_COMMA,
        KPD_MINUS,
        KPD_PERIOD,

        KPD_0,
        KPD_1,
        KPD_2,
        KPD_3,
        KPD_4,
        KPD_5,
        KPD_6,
        KPD_7,
        KPD_8,
        KPD_9,

        KPD_ENTER,
        KPD_PF1,
        KPD_PF2,
        KPD_PF3,
        KPD_PF4,
        
        KPD_FIND,
        KPD_INSERT,
        KPD_REMOVE,
        KPD_SELECT,
        KPD_PREV,
        KPD_NEXT,
        
        KEY_CTRL,
        KEY_SHIFT,
        KEY_LOCK,
        KEY_COMPOSE,
        
        KEY_MESSAGE
} ansi_val;

#define PAGE_UP    KPD_MINUS
#define PAGE_DOWN  KPD_COMMA
#define ENTER      KPD_ENTER

#define BEEP       CTRL_G
#define BACK_SPACE CTRL_H
#define TAB        CTRL_I
#define LINE_FEED  CTRL_J
#define RETURN     CTRL_M
#define UPDATE     CTRL_W

        
#define TOP_LEFT_CORNER 	'l'
#define TOP_RIGHT_CORNER	'k'
#define BOTTOM_LEFT_CORNER	'm'
#define BOTTOM_RIGHT_CORNER	'j'
#define HORIZONTAL_BAR		'q'
#define VERTICAL_BAR		'x'
#define SPACE			' '

#define NORMAL     0x00
#define BOLD       0x01
#define UNDERLINE  0x02
#define INVERSE    0x04
#define FLASH      0x08
#define ATTRIBUTES 0x0F

#define ASCII     0x00
#define GRAPHIC   0x10
#define FONT_SUPP 0x20
#define FONT_USER 0x30

#define FONTS     0x30

#define MODIFIED  0x40

#define SEQUENTIAL_WINDOW 0
#define DETACHED_WINDOW   1
#define PULLDOWN_WINDOW   2

typedef enum {
  ON_SCREEN,
  ON_CONTENTS,
  ON_BORDER,
  ON_DRAG,
  ON_ICONIFY,
  ON_RESIZE,
  ON_PAGE_UP,
  ON_PAGE_DOWN,
  ON_POP_UP
} scr_on_enums;

#endif /* __SCRDEF_H */
