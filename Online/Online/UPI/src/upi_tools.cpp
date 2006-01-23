/*
        upic_TOOLS.C
         Created           :  6-NOV-1989 by 
*/
 
#include <cstdio>                   /* I/O definitions                       */
#include "UPI/upidef.h"
#include "SCR/scr.h"

extern System Sys;

//---------------------------------------------------------------------------
int upic_show_notice (const char* title, int lines, const char** text)    {
#ifdef SCREEN
  Menu* m = upic_find_menu (NOTICE);
  if (!m)  {
    upic_open_detached_menu (NOTICE, 0, 0, "", "", "");
    upic_add_command (1, "", "");
    upic_close_menu ();
    m = upic_find_menu (NOTICE);
  }
  upic_change_titles (NOTICE, title, "Notice --- Notice --- Notice", 0);  
  int i, n = m->items;
  for (i=lines+1; i<=n; i++) upic_delete_command (NOTICE, i);
  for (i=1; i<=n; i++,text++)
    upic_replace_command (NOTICE, i, *text, "");
  for (; i<=lines; i++,text++)
    upic_insert_command (NOTICE, 1000, i, *text, "");
  upic_set_cursor (NOTICE, 1, 0);
#else
  upir_show_notice (title, lines, text);
#endif
  return (UPI_SS_NORMAL);
}

//---------------------------------------------------------------------------
int upic_cancel_notice ()   {
#ifdef SCREEN
  Menu* m = upic_find_menu (NOTICE);
  if (m) upic_go_backward (m);
#else
  upir_cancel_notice ();
#endif
  return (UPI_SS_NORMAL);
}

//---------------------------------------------------------------------------
int upic_show_warning (int lines, char** text)    {
#ifdef SCREEN
  int menu, command, param;  
  Menu* m = upic_find_menu (WARNING);
  if (!m)  {
    upic_open_detached_menu (WARNING, 0, 0,
      "Warning --- Warning --- Warning", "", "");
    upic_add_command (1000, "          OK ? ", "");
    upic_close_menu ();
    m = upic_find_menu (WARNING);
  }
  
  int i, n = m->items - 1;
  for (i=lines+1; i<=n; i++) upic_delete_command (WARNING, i);
  for (i=1; i<=n; i++, text++)
    upic_replace_command (WARNING, i, *text, "");
  for (; i<=lines; i++, text++)
    upic_insert_command (WARNING, 1000, i, *text, "");
  do
  {
    upic_set_cursor (WARNING, 1000, 0);
    upic_get_input(&menu, &command, &param);
  } while (menu != WARNING);
  upic_go_backward (m);
#else
  upir_show_warning (lines, text);
#endif
  return (UPI_SS_NORMAL);
}


//---------------------------------------------------------------------------
int upic_lock_cursor ()   {
#ifdef SCREEN
#else
  upir_lock_cursor ();
#endif
  return (UPI_SS_NORMAL);
}

//---------------------------------------------------------------------------
int upic_unlock_cursor ()   {
#ifdef SCREEN
#else
  upir_unlock_cursor ();
#endif
  return (UPI_SS_NORMAL);
}

//---------------------------------------------------------------------------
int upic_back_space (int menu_id)   {
#ifdef SCREEN
  Menu* m;
  
  m = upic_find_menu (menu_id);
  if (!m) return (UPI_SS_INVMENU);
  upic_go_backward (m);
#else
  upir_back_space (menu_id);
#endif
  return (UPI_SS_NORMAL);
}

//---------------------------------------------------------------------------
int upic_register_on_keypad (int menu, int item, int key)   {
#ifdef SCREEN
  Menu* m = upic_find_menu (menu);
  if (!m) return UPI_SS_INVMENU;
  Page* d= m->page.first;
  if (!d || !upic_find_item (d->item.first, item)) return UPI_SS_INVCOMMAND;
  if (key < 0 || key > 9) return UPI_SS_INVPARAM;
  Sys.keypad[key].menu = menu;
  Sys.keypad[key].item = item;
#else
  upir_register_on_keypad (menu, item, key);
#endif
  return UPI_SS_NORMAL;
}

//---------------------------------------------------------------------------
int upic_attach_pf1 (int menu)  {
#ifdef SCREEN
  if (!upic_find_menu (menu)) return UPI_SS_INVMENU;
  Sys.pf1.menu = menu;
  Sys.pf1.item = 0;
#else
  upir_attach_pf1 (menu);
#endif
  return UPI_SS_NORMAL;
}

//---------------------------------------------------------------------------
int upic_declare_callback (int menu_id,int condition, Routine user_routine, int user_arg) {
#define CONDITIONS (CALL_ON_BACK_SPACE | CALL_ON_ENTER | CALL_ON_DRAG)
  Menu* m = upic_find_menu (menu_id);
  if (!m) return (UPI_SS_INVMENU);
  /* Check validity of the Condition argument. */
  if (!(condition &= CONDITIONS)) return (UPI_SS_INVPARAM);
  m->condition = condition;
  m->callback  = user_routine;
  m->arg       = user_arg;  
#ifdef REMOTE
  upir_declare_callback (menu_id, condition);
#endif
  return (UPI_SS_NORMAL);
}



#ifdef SCREEN
//---------------------------------------------------------------------------
int upic_valid_keypad (int key) {
  int menu, item;
  switch (key)  {
    case KPD_PF1 :
      menu = Sys.pf1.menu;
      item = 0;
      break;
    case KPD_0 :
    case KPD_1 :
    case KPD_2 :
    case KPD_3 :
    case KPD_4 :
    case KPD_5 :
    case KPD_6 :
    case KPD_7 :
    case KPD_8 :
    case KPD_9 :
      key -= KPD_0;
      menu = Sys.keypad[key].menu;
      item = Sys.keypad[key].item;
      break;
    default :
      return (0);
  }
  Menu* m = upic_find_menu (menu);
  if (!m) return (0);
  if (!item) return (1);
  Page* d= m->page.first;
  if (!d || !upic_find_item (d->item.first, item)) return (0);
  return (1);
}

//---------------------------------------------------------------------------
int upic_branch_on_keypad (int key)  {
  switch (key)  {
    case KPD_PF1 :
      upic_set_cursor (Sys.pf1.menu, 0, 0);
      break;
    case KPD_0 :
    case KPD_1 :
    case KPD_2 :
    case KPD_3 :
    case KPD_4 :
    case KPD_5 :
    case KPD_6 :
    case KPD_7 :
    case KPD_8 :
    case KPD_9 :
      key -= KPD_0;
      upic_set_cursor (Sys.keypad[key].menu, Sys.keypad[key].item, 0);
      break;
  }
  return (1);
}
#endif
