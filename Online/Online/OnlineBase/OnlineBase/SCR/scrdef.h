#ifndef __SCRDEF_H
#define __SCRDEF_H

#ifdef __cplusplus
namespace SCR {
#endif
  enum ansi_val {
    INVALID = 0x100, 
    CTRL_A, //      101
    CTRL_B, //      102
    CTRL_C, //      103
    CTRL_D,
    CTRL_E,
    CTRL_F,
    CTRL_G,
    CTRL_H,
    CTRL_I,
    CTRL_J, // 10   10A
    CTRL_K,
    CTRL_L,
    CTRL_M,
    CTRL_N,
    CTRL_O,
    CTRL_P,
    CTRL_Q,
    CTRL_R,
    CTRL_S,
    CTRL_T, // 20   114
    CTRL_U,
    CTRL_V,
    CTRL_W,
    CTRL_X,
    CTRL_Y,
    CTRL_Z, // 26   11A

    DELETE_KEY, // 27   11B

    MOVE_UP,     // 28   11C
    MOVE_DOWN,   // 29   11D
    MOVE_LEFT,   // 30   11E
    MOVE_RIGHT,  // 31   11F

    F6,          // 32   120
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    F13,
    F14,         // 40  128
    F15,
    F16,         // 42  12A
    F17,
    F18,
    F19,
    F20,
    KPD_COMMA,   //     12F
    KPD_MINUS,   //     130
    KPD_PERIOD,  //     131

    KPD_0,       // 50
    KPD_1,
    KPD_2,
    KPD_3,
    KPD_4,
    KPD_5,
    KPD_6,
    KPD_7,      //     139
    KPD_8,
    KPD_9,

    KPD_ENTER,  // 60
    KPD_PF1,
    KPD_PF2,
    KPD_PF3,    //    13F
    KPD_PF4,    // 64 140
          
    KPD_FIND,   // 65 141
    KPD_INSERT, // 66 142
    KPD_REMOVE, // 67 143
    KPD_SELECT,
    KPD_PREV,
    KPD_NEXT,
          
    KEY_CTRL,
    KEY_SHIFT,
    KEY_LOCK,
    KEY_COMPOSE,
          
    KEY_MESSAGE
  };

  enum scr_constants {
    BELL                = 7,
    CARRIAGERETURN      = 13,
    LINEFEED            = 10,
    ESCAPE              = 27,

    PAGE_UP             = KPD_MINUS,
    PAGE_DOWN           = KPD_COMMA,
    ENTER               = KPD_ENTER,
    BEEP                = CTRL_G,
    BACK_SPACE          = CTRL_H,
    TAB                 = CTRL_I,
    LINE_FEED           = CTRL_J,
    //#define RETURN     CTRL_M
    RETURN_KEY          = CTRL_J,
    UPDATE_KEY          = CTRL_W,

    SPACE               = ' ',

	  /* Rendering */
    NORMAL              = 0x0000,
    BOLD                = 0x0001,
    UNDERLINE           = 0x0002,
    INVERSE             = 0x0004,
    FLASH               = 0x0008,

    ASCII               = 0x0000,
    GRAPHIC             = 0x0010,
    FONT_SUPP           = 0x0020,
    FONT_USER           = 0x0030,
    FONTS               = 0x0030,
    MODIFIED            = 0x0040,

	  /*  Colors                */
	  BLACK               = 0x0100,
	  RED                 = 0x0200,
	  GREEN               = 0x0400,
	  YELLOW              = 0x0800,
	  BLUE                = 0x1000,
	  MAGENTA             = 0x2000,
	  CYAN                = 0x4000,
	  WHITE               = 0x8000,

	  FG_BLACK            = BLACK,
	  FG_RED              = RED,
	  FG_GREEN            = GREEN,
	  FG_YELLOW           = YELLOW,
	  FG_BLUE             = BLUE,
	  FG_MAGENTA          = MAGENTA,
	  FG_CYAN             = CYAN,
	  FG_WHITE            = WHITE,

	  BG_BLACK            = 0x010000,
	  BG_RED              = 0x020000,
	  BG_GREEN            = 0x040000,
	  BG_YELLOW           = 0x080000,
	  BG_BLUE             = 0x100000,
	  BG_MAGENTA          = 0x200000,
	  BG_CYAN             = 0x400000,
	  BG_WHITE            = 0x800000,

    COLORS              = 0xFfFF00,
    ATTRIBUTES          = 0xFFffFF,

    SEQUENTIAL_WINDOW   = 0,
    DETACHED_WINDOW     = 1,
    PULLDOWN_WINDOW     = 2
  };

  enum scr_on_enums {
    ON_SCREEN,
    ON_CONTENTS,
    ON_BORDER,
    ON_DRAG,
    ON_ICONIFY,
    ON_RESIZE,
    ON_PAGE_UP,
    ON_PAGE_DOWN,
    ON_POP_UP
  };
#ifdef __cplusplus
}
#endif
#endif /* __SCRDEF_H */
