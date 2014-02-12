#ifndef __UPI_MENU_H
#define __UPI_MENU_H
#include <cstdio>

#ifdef __cplusplus
extern "C"  {
#define __CXX_CONST const
#else
#define __CXX_CONST 
#endif
typedef void (*Routine)(int,int,...);

/*- From UPI_ITEM.C ---------------------------------------------------*/
int upic_add_command (int id,  const char* text_0, const char* text_1);
int upic_add_comment ( int id, const char* text_0, const char* text_1);
int upic_add_param_line ( int id, const char* text_0, const char* text_1);
int upic_replace_command ( int menu_id, int id, const char* text_0, const char* text_1);
int upic_replace_comment (int menu_id, int id, const char* text_0, const char* text_1);
int upic_replace_param_line (int menu_id, int id, const char* text_0, const char* text_1);
int upic_delete_comment (int,int);
int upic_delete_command (int,int);
int upic_insert_command (int menu_id, int pos, int id, const char* text_0, const char* text_1);
int upic_insert_comment (int menu_id, int pos, int id, const char* text_0, const char* text_1);
int upic_insert_param_line (int menu_id, int pos, int id, const char* text_0, const char* text_1);
int upic_enable_command (int,int);
int upic_disable_command (int,int);
int upic_enable_commands(int m, int n, ...);
int upic_disable_commands(int m, int n, ...);
int upic_enable_action_routine (int,int,Routine);
int upic_disable_action_routine (int,int);
int upic_add_item (int,const char*,const char*,int);
int upic_replace_item (int,int,const char*,const char*,int);
int upic_insert_item (int,int,int,const char*,const char*,int);

/*- From UPI_MENU.C ---------------------------------------------------*/
int upic_open_menu (int,int,int,const char*,const char*,const char*);
int upic_open_pulldown_menu (int,int,int,const char*,const char*,const char*);
int upic_open_detached_menu (int,int,int,const char*, const char*,const char*);
int upic_open_param (int,int,int,const char*,const char*,const char*);
int upic_close_param (void);
int upic_close_menu (void);
int upic_delete_menu (int);
int upic_erase_menu (int);
int upic_open_menus (int,int,int,const char*,const char*,const char*,int);

/*- From UPI_MESSAGE.C ------------------------------------------------*/
int upic_write_message(const char*,const char*);
int upic_write_message2(const char* fmt, ...);
int upic_write_rendered_message (const char*, const char*, int render);
int upic_write_rendered_message_sync (const char* text1, const char* text2, int render);
int upic_set_message_window (int,int,int,int);
int upic_get_message_window (int*,int*,int*,int*);
/*- From UPI_PAGE.C ---------------------------------------------------*/
int upic_change_titles (int,const char*,const char*,const char*);
/*- From UPI_PARAM.C --------------------------------------------------*/
int upic_set_param ( const void*, int, const char *, ... );
int upic_modify_param ( int, int, int, ... );
/*- From UPI_WINDOW.C -------------------------------------------------*/
int upic_open_window (void);
int upic_open_pulldown_window (void);
int upic_open_old_window (int);
int upic_set_window_position (int,int,int);
int upic_get_window_position (int,int* rols,int* cols);
int upic_has_screen();
int upic_get_screen_size(int* rows, int* cols);
int upic_open_detached_window (void);
int upic_get_window_size(int menu_id, int* col, int* row);
int upic_get_screen_size(int* rows, int* cols);
#ifdef __cplusplus
}
#endif
#undef __CXX_CONST

#endif /* __UPI_MENU_H */
