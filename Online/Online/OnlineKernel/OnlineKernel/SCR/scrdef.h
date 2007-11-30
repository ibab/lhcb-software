#ifndef __SCRDEF_H
#define __SCRDEF_H

#ifdef __cplusplus
namespace SCR {
#endif
  typedef enum {
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
          MOVE_RIGHT,  // 31   12F

          F6,          // 32
          F7,
          F8,
          F9,
          F10,
          F11,
          F12,
          F13,
          F14,         // 40
          F15,
          F16,
          F17,
          F18,
          F19,
          F20,
          KPD_COMMA,
          KPD_MINUS,
          KPD_PERIOD,

          KPD_0,       // 50
          KPD_1,
          KPD_2,
          KPD_3,
          KPD_4,
          KPD_5,
          KPD_6,
          KPD_7,
          KPD_8,
          KPD_9,

          KPD_ENTER,  // 60
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

  typedef enum {
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
          NORMAL              = 0x00,
          BOLD                = 0x01,
          UNDERLINE           = 0x02,
          INVERSE             = 0x04,
          FLASH               = 0x08,
          ATTRIBUTES          = 0x0F,
          ASCII               = 0x00,
          GRAPHIC             = 0x10,
          FONT_SUPP           = 0x20,
          FONT_USER           = 0x30,
          FONTS               = 0x30,
          MODIFIED            = 0x40,

          SEQUENTIAL_WINDOW   = 0,
          DETACHED_WINDOW     = 1,
          PULLDOWN_WINDOW     = 2
  } scr_constants;

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
#ifdef __cplusplus
}
#endif
#endif /* __SCRDEF_H */
