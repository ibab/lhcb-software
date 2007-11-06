#ifndef __UPI_PROTO_H
#define __UPI_PROTO_H
#include <cstdio>
/* 
        upi_proto.h
         Created           :  2-JAN-1992 by Christian Arnault
*/

#ifdef __cplusplus
extern "C"  {
#define __CXX_CONST const
#else
#define __CXX_CONST 
#endif

/*- From UPI_AST.C ----------------------------------------------------*/

void upic_ast(void);
void upic_enable_ast(void);
void upic_enable_input(void);
void upic_disable_input(void);
int  upic_test_input(void);
void upic_disable_ast(void);

/*- From UPI_DLPACK.C -------------------------------------------------*/

int upic_dlhead (const char*,int,int);
int upic_dlout (const char*,int,int,int);
int upic_dlouto (const char*,int,int,int);
int upic_dloutx (const char*,int,int,int);
int upic_dlkey (void);
int upic_dltxt (const char*,char*,char*,int,int*);
int upic_dldec (const char*,int,int*,int,int);
int upic_dloct (const char*,int,int*,int,int);
int upic_dlhex (const char*,int,int*,int,int);
int upic_dlmask (const char*,int,int*);
int upic_dlm32 (const char*,int,int*);
int upic_dlyeno (const char*,int,int*);
int upic_dlend (void);

/*- From UPI_HELP.C ---------------------------------------------------*/

#ifdef SCREEN
void upic_open_help (Menu*);
void upic_close_help (void);
#endif

/*- From UPI_HISTO.C --------------------------------------------------*/

int upi_book_histo (void);
Histo* upic_book_histo (const char*,int,int,double,double);
int upic_unbook_histo (Histo*);
void upic_scale_histo (Histo*,double);
void upic_unscale_histo (Histo*);
int upic_fill_histo (Histo*,double,double);
Histo* upic_find_histo (const char*);
void upic_save_histo (Histo*, const char*);
Histo* upic_restore_histo (const char*,const char*);
Histo* upic_copy_histo (Histo*);
int upic_bins_of_histo (Histo*);
double upic_min_of_histo (Histo*);
double upic_max_of_histo (Histo*);
void upic_display_histo (Histo*, int, int);
void upic_hide_histo (Histo*);
void upic_set_drag_histo_action (Routine);
int  upic_drag_histo (Display*,int,int);
void upic_move_histo (Histo*,int,int);
void upic_pos_of_histo (Histo*,int*,int*);
void upic_moving_histo (Histo*);

/*- From UPI_INPUT.C --------------------------------------------------*/
int upic_wait (int*);
int upic_get_input_with_index (int*,int*,int*,int*);
int upic_get_input (int*,int*,int*);
void upic_start_recorder (int*, int);
int upic_stop_recorder (void);
void upic_start_replay (int*, int);
void upic_stop_replay (void);
void upic_record (int);
int upic_replay (void);

#ifdef SCREEN
int upic_key_rearm (unsigned int, void*);
int upic_key_action (unsigned int, void*);
#endif

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
Item* upic_find_item (Item*,int);
Item* upic_find_item_row (Item*,int,int*);
Item* upic_find_next_item (Item*,int*);
Item* upic_find_prev_item (Item*,int*);
void upic_drop_item (Item*);
void upic_init_item_strings (Item* i, const char* text_0, const char* text_1);

#ifdef SCREEN
void upic_draw_item (Item*,int);
#endif

/*- From UPI_MENU.C ---------------------------------------------------*/

int upic_open_menu (int,int,int,const char*,const char*,const char*);
int upic_open_pulldown_menu (int,int,int,const char*,const char*,const char*);
int upic_open_detached_menu (int,int,int,const char*, const char*,const char*);
int upic_open_param (int,int,int,const char*,const char*,const char*);
int upic_close_param (void);
int upic_close_menu (void);
int upic_delete_menu (int);
int upic_erase_menu (int);
Menu* upic_find_menu (int);
void upic_drop_menu (Menu*);
int upic_open_menus (int,int,int,const char*,const char*,const char*,int);
int upic_connect_menu (Menu*, int, int);

#ifdef SCREEN
Menu* upic_find_menu_on_window (Window*);
void upic_draw_menu (Menu*);
#endif

/*- From UPI_MESSAGE.C ------------------------------------------------*/
int upic_write_message(const char*,const char*);
int upic_write_message2(const char* fmt, ...);
int upic_write_rendered_message (const char*, const char*, int render);
int upic_write_rendered_message_sync (const char* text1, const char* text2, int render);
int upic_set_message_window (int,int,int,int);
int upic_get_message_window (int*,int*,int*,int*);
void upic_drop_async (Async_line*);

#ifdef SCREEN
int upic_scroll_message (int);
int upic_resize_message (Display*,int,int);
int upic_drag_message (Display*,int,int);
void upic_draw_message (void);
void upic_paste_message (void);
void upic_refresh_message (Async*);
#endif

/*- From UPI_PAGE.C ---------------------------------------------------*/

int upic_set_items_per_page (int);
int upic_get_items_per_page (void);
int upic_change_titles (int,const char*,const char*,const char*);
Page* upic_open_page (void*);
void upic_drop_page (Page*);

int upic_change_pasteboard(int width,int height);
#ifdef SCREEN
void upic_init_page (Menu*,Page*,int);
void upic_change_page (Menu*,Page*,int);
void upic_draw_title_bar (Display*,const char*,int,int,const char*);
#endif

/*- From UPI_PARAM.C --------------------------------------------------*/

int upic_set_param ( const void*, int, const char *, ... );
int upic_modify_param ( int, int, int, ... );
int upic_get_param (int,int,int,Menu**,Page**,Item**,Param**);
int upic_refresh_param_page (int);
int upic_refresh_param_line (int,int);
Param* upic_find_param (Param*,int);
void upic_drop_params (Param*);
void upic_drop_param (Param*);
void upic_refresh_params_in_line (Param*);
void upic_refresh_param (Param*);
int upic_build_format (const char*,Param*);
void upic_install_params (Param*,char*);
int upic_check_int (int,int,int);
int upic_check_double (double val, double min, double max);
int upic_valid_numeric (int,char);
void upic_update_var_of_param (Param*);
void upic_update_vars_of_line (Item*);
void upic_update_vars_of_page (Menu*);
int upic_store_vars (int,int,char**,int*);
int upic_retreive_vars (int,int,char*);
void upic_restore_params_in_line (Item*);
void upic_restore_params_in_page (Menu*);
int upic_itom (char*,int,Unsigned);
Unsigned upic_mtoi (char*,int);
int upic_itol (char*,int,Unsigned);
int upic_ltoi (char*,int);
void upic_print_param (Param*,char*,Convert);
int upic_find_list_elem (int,int,int);
int upic_set_value_from_list (int,int,int, int);

#ifdef SCREEN
void upic_draw_param (Page*,Param*,int,int,int);
#endif

/*- From UPI_POPUP.C --------------------------------------------------*/

#ifdef SCREEN
void upic_open_pop_up (Menu*,Param*,int,int);
void upic_close_pop_up (void);
void upic_set_pop_up_cursor (void);
void upic_act_on_pop_up (int*,int,int);
#endif

/*- From UPI_REMOTE.C -------------------------------------------------*/

void upic_net_close ();
int  upic_net_init (const char* name, char** server, WtRoutine handler, WtRoutine broadcast);
void upic_net_discard_server ();
int  upic_net_server_name (char* name);
int  upic_net_set_server_name (const char* name);
int  upic_net_read (char** buffer, size_t* bytes, char** source);
int  upic_net_spy (char** source);
int  upic_net_send_to_name (const char* buffer, int bytes, const char* dest);
int  upic_net_who_am_i (char* name);
void upic_net_start_log ();
void upic_net_stop_log ();
void upic_net_flush_log();
void upic_net_log_spy (int status, int bytes);
void upic_net_log_last_message (int status, int bytes, const char* buffer);

#ifdef REMOTE
struct _UpiConnectRec;
typedef _UpiConnectRec* UpiConnect;
void upir_init_remote (void);
int upir_input (int*,int*,int*,int*);
void upir_delete_command (int,int);
void upir_enable_command (int,int);
void upir_disable_command (int,int);
void upir_replace_item (Menu*,Item*);
void upir_insert_item (Menu*,Item*);
void upir_close_menu (Menu*,UpiConnect);
void upir_delete_menu (int);
void upir_erase_menu (int);
void upir_write_message (const char*, const char*);
void upir_write_rendered_message (const char*, const char*, int render);
void upir_set_message_window (int,int,int,int);
int upir_get_message_window (int*,int*,int*,int*);
void upir_change_titles (Menu*);
void upir_modify_param (Menu*,Item*,Param*);
void upir_refresh_param_page (Menu*);
void upir_refresh_param_line (Menu*,Item*);
void upir_set_cursor (int,int,int);
void upir_save_screen (void);
void upir_restore_screen (void);
void upir_begin_update (void);
void upir_end_update (void);
void upir_hide_menu (int);
void upir_quit (void);
void upir_open_window (void);
void upir_open_old_window (int);
void upir_set_window_position (int,int,int);
int  upir_get_window_position (int,int*,int*);
int  upir_get_window_size (int menu_id, int* col, int* row);
int  upir_get_screen_size(int* rows, int* cols);
void upir_attach_pf1 (int);
void upir_cancel_notice (void);
int  upir_get_items_per_page (int*);
void upir_set_items_per_page (int);
int  upir_get_mode (int*);
void upir_set_mode (int);
void upir_register_on_keypad (int,int,int);

void upir_show_notice (const char*,int,const char**);

int  upir_show_warning (int,const char**);
void upir_connect_process (const char*);
void upir_disconnect_process (const char*);
int  upir_dldec (const char*,int,int*,int,int);
void upir_dlend (void);
int  upir_dlhex (const char*,int,int*,int,int);
void upir_dlhead (const char*,int,int);
int  upir_dlkey (void);
int  upir_dlm32 (const char*,int,int*);
int  upir_dlmask (const char*,int,int*);
int  upir_dloct (const char*,int,int*,int,int);
int  upir_dltxt (const char*,const char*,char*,int,int*);
void upir_dlout (const char*,int,int,int);
void upir_dlouto (const char*,int,int,int);
void upir_dloutx (const char*,int,int,int);
int  upir_dlyeno (const char*,int,int*);
void upir_lock_cursor (void);
void upir_unlock_cursor (void);
void upir_back_space (int);
void upir_book_histo (Histo*);
void upir_unbook_histo (Histo*);
void upir_scale_histo (Histo*,double);
void upir_unscale_histo (Histo*);
void upir_fill_histo (Histo*,double,double);
void upir_display_histo (Histo*,int,int);
void upir_hide_histo (Histo*);
void upir_move_histo (Histo*,int,int);
void upir_moving_histo (Histo*);
void fill_histo (Histo*);
void upir_copy_histo (Histo*);
void upir_declare_callback (int,int);

#endif

/*- From UPI_SCREEN.C -------------------------------------------------*/

int upic_set_cursor (int,int,int);
int upic_set_cursor_and_mark (int,int,int,int);
int upic_save_screen (FILE**,FILE**);
int upic_restore_screen (void);
int upic_begin_update (void);
int upic_end_update (void);
int upic_hide_menu (int);

#ifdef SCREEN
void upic_refresh_screen (void);
void upic_broadcast_handler (const char*);
void upic_init_screen (void);
void upic_erase_screen (void);
int upic_move_cursor (Menu*,Page*,Item*,int);
int upic_draw_cursor (FLAG);
int upic_move_up (Menu*);
int upic_move_down (Menu*);
int upic_move_left (Menu*);
int upic_move_right (Menu*);
int upic_page_up (Menu*);
int upic_page_down (Menu*);
int upic_go_backward (Menu*);
#endif


/*- From UPI_SYSTEM.C -------------------------------------------------*/

int upic_attach_terminal (void);
int upic_declare_exit_handler (Routine);
int upic_declare_ast_handler (Routine);
int upic_quit (void);
int upic_set_mode (int);
int upic_get_mode (void);
System* upic_get_system (void);
int upic_get_info (Pasteboard**);
void upic_set_message_callback (Routine);
unsigned long upic_non_blanks (const char*);
int upic_save_setup (void);
int upic_restore_setup (void);
int upic_signal_error (int,const char*);
int upic_connect_process (const char*);
int upic_disconnect_process (const char*);
int upic_show_memory (void);

/*- From UPI_TOOLS.C --------------------------------------------------*/

int upic_show_notice (const char*,int,const char**);
int upic_cancel_notice (void);
int upic_show_warning (int,const char**);
int upic_lock_cursor (void);
int upic_unlock_cursor (void);
int upic_back_space (int);
int upic_register_on_keypad (int,int,int);
int upic_attach_pf1 (int);
int upic_declare_callback (int,int,Routine,void*);

#ifdef SCREEN
int upic_valid_keypad (int);
int upic_branch_on_keypad (int);
#endif


/*- From UPI_WAKE.C ---------------------------------------------------*/

void upic_wakeup (void);
void upic_set_wakeup (void);
int upic_test_wakeup (void);
void upic_reset_wakeup (void);

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

/*- From --------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif
#undef __CXX_CONST

#endif /* __UPI_PROTO_H */
