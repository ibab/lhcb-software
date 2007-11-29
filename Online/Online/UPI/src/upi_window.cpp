/*
  upic_window.c
  Created           : 23-OCT-1989 by Christian Arnault
*/

//---------------------------------------------------------------------------
#include "UPI/upidef.h"
#include "SCR/scr.h"
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
extern System Sys;
#ifdef REMOTE
static long Window_id = 0;
#endif

//---------------------------------------------------------------------------
int upic_open_window () {
#ifdef SCREEN
  Sys.window = scrc_open_window (SCR::SEQUENTIAL_WINDOW);
#else
  Sys.window = (Window*)++Window_id;
  /*  upir_open_window ();  */
#endif
  return (UPI_SS_NORMAL);
}

//---------------------------------------------------------------------------
int upic_open_pulldown_window ()  {
#ifdef SCREEN
  Sys.window = scrc_open_window (SCR::PULLDOWN_WINDOW);
#else
#endif
  return (UPI_SS_NORMAL);
}

//---------------------------------------------------------------------------
int upic_open_old_window (int menu_id)  {
  Menu* m;    
  if (!(m = upic_find_menu(menu_id))) return (UPI_SS_INVMENU);
  Sys.window = m->window;
#ifdef SCREEN
#else
  upir_open_old_window (menu_id);
#endif
  return (UPI_SS_NORMAL);
}

//---------------------------------------------------------------------------
int upic_set_window_position (int menu_id, int col, int row)    {
#ifdef SCREEN
  Menu* m;
  if (!(m = upic_find_menu(menu_id))) return UPI_SS_INVMENU;
  scrc_move_window_to (m->window, row, col);
#else
  upir_set_window_position (menu_id, col, row);
#endif
  return UPI_SS_NORMAL;
}

//---------------------------------------------------------------------------
int upic_get_window_position (int menu_id, int* col, int* row)    {
#ifdef SCREEN
  Menu* m;
  if (!(m = upic_find_menu(menu_id))) return UPI_SS_INVMENU;
  scrc_get_window_position (m->window, row, col);
#else
  upir_get_window_position (menu_id, col, row);
#endif
  return UPI_SS_NORMAL;
}

//-------------------------------------------------------------------------
int upic_get_window_size (int menu_id, int* width, int* height) {
#ifdef SCREEN
  Menu* m;
  if (!(m = upic_find_menu(menu_id))) return UPI_SS_INVMENU;
  scrc_get_window_size (m->window, width, height);
#else
  upir_get_window_size (menu_id, width, height);
#endif
  return UPI_SS_NORMAL;
}

//---------------------------------------------------------------------------
int upic_open_detached_window ()    {
#ifdef SCREEN
  Sys.detached_window = scrc_open_window (SCR::DETACHED_WINDOW);
#else
#endif
  return (UPI_SS_NORMAL);
}

