#ifndef __UPI_PROTO_H
#define __UPI_PROTO_H

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

int upic_dlhead (char*,int,int);
int upic_dlout (char*,int,int,int);
int upic_dlouto (char*,int,int,int);
int upic_dloutx (char*,int,int,int);
int upic_dlkey (void);
int upic_dltxt (char*,char*,char*,int,int*);
int upic_dldec (char*,int,int*,int,int);
int upic_dloct (char*,int,int*,int,int);
int upic_dlhex (char*,int,int*,int,int);
int upic_dlmask (char*,int,int*);
int upic_dlm32 (char*,int,int*);
int upic_dlyeno (char*,int,int*);
int upic_dlend (void);

/*- From UPI_F.C ------------------------------------------------------*/

void upi_ast (void);
void upi_enable_ast (void);
void upi_disable_ast (void);
void upi_enable_input (void);
void upi_disable_input (void);
int upi_test_input (void);
int upi_wait (int*);
int upi_get_input (int*,int*,int*);
int upi_get_input_with_index (int* , int* , int* ,int* );
void upi_start_recorder (int* , int*);
int upi_stop_recorder ( void );
void upi_record ( void );
void upi_start_replay (int*, int*);
void upi_stop_replay (void);
int upi_replay (void);
int upi_enable_command (int*,int*);
int upi_disable_command (int*,int*);
int upi_replace_command (int*,int*,int*,int*);
int upi_replace_comment (int*,int*,int*,int*);
int upi_replace_param_line (int*,int*,int*,int*);
int upi_delete_command (int*,int*);
int upi_delete_comment (int*,int*);
int upi_add_command (int*,int*,int*);
int upi_add_comment (int*,int*,int*);
int upi_add_param_line (int*,int*,int*);
int upi_insert_command (int*,int*,int*,int*,int*);
int upi_insert_comment (int*,int*,int*,int*,int*);
int upi_insert_param_line (int*,int*,int*,int*,int*);
int upi_enable_action_routine (int*,int*,Routine);
int upi_disable_action_routine (int*,int*);
int upi_delete_menu (int*);
int upi_erase_menu (int*);
int upi_open_menu (int*,int*,int*,int*,int*,int*);
int upi_open_pulldown_menu (int*,int*,int*,int*,int*,int*);
int upi_open_detached_menu (int*,int*,int*,int*,int*,int*);
int upi_open_param (int*,int*,int*,int*,int*,int*);
int upi_close_menu (void);
int upi_close_param (void);
int upi_change_titles (int*,int*,int*,int*);
int upi_set_message_window (int*,int*,int*,int*);
int upi_get_message_window (int*,int*,int*,int*);
int upi_write_message (int*,int*);
int upi_set_items_per_page (int*);
int upi_get_items_per_page (void);
int upi_refresh_param_page (int*);
int upi_refresh_param_line (int*,int*);
int upi_set_param (int*,int*,int*,Unsigned*,Unsigned*,Unsigned*,int*,int*,int*);
int upi_set_cursor (int*,int*,int*);
int upi_save_screen (int*,int*);
int upi_restore_screen (void);
int upi_save_setup (void);
int upi_restore_setup (void);
int upi_show_memory (void);
int upi_begin_update (void);
int upi_end_update (void);
int upi_hide_menu (int*);
int upi_attach_terminal (void);
int upi_declare_exit_handler (Routine);
int upi_declare_ast_handler (Routine);
int upi_attach_pf1 (int*);
int upi_register_on_keypad (int*,int*,int*);
int upi_set_mode (int*);
int upi_get_mode (void);
int upi_get_system (void);
int upi_get_info (int*);
int upi_quit (void);
int upi_open_window (void);
int upi_open_detached_window (void);
int upi_set_window_position (int*,int*,int*);
int upi_get_window_position (int*,int*,int*);
int upi_signal_error (int*,int*);
int upi_configure_menu (void);
int upi_get_message (void);
int upi_send_message (void);
int upi_modify_param (int*,int*,int*,Unsigned*,Unsigned*,Unsigned*,int*,int*,int*);
int upi_show_notice (int*,int*,int*);
int upi_cancel_notice (void);
int upi_show_warning (int*,int*);
int upi_open_old_window (int*);
int upi_store_vars (int*,int*,int*,int*);
int upi_retreive_vars (int*,int*,int*);
int upi_connect_process (int*);
int upi_disconnect_process (int*);
int upi_dlhead (int*,int*,int*);
int upi_dlout (int*,int*,int*);
int upi_dlouto (int*,int*,int*);
int upi_dloutx (int*,int*,int*);
int upi_dlkey (void);
int upi_dltxt (int*,int*,int*,int*,int*);
int upi_dldec (int*,int*,int*,int*,int*);
int upi_dloct (int*,int*,int*,int*,int*);
int upi_dlhex (int*,int*,int*,int*,int*);
int upi_dlmask (int*,int*,int*);
int upi_dlm32 (int*,int*,int*);
int upi_dlyeno (int*,int*,int*);
void upi_dlend (void);
void upi_lock_cursor (void);
void upi_unlock_cursor (void);
void upi_back_space (int*);
int upi_declare_callback (int*,int*,Routine,int*);

/*- From UPI_HELP.C ---------------------------------------------------*/

#ifdef SCREEN
void upic_open_help (Menu*);
void upic_close_help (void);
#endif

/*- From UPI_HISTO.C --------------------------------------------------*/

int upi_book_histo (void);
Histo* upic_book_histo (char*,int,int,double,double);
int upi_unbook_histo (Histo*);
int upic_unbook_histo (Histo*);
void upi_scale_histo (void);
void upic_scale_histo (Histo*,double);
void upi_unscale_histo (Histo*);
void upic_unscale_histo (Histo*);
int upi_fill_histo (Histo*,double,double);
int upic_fill_histo (Histo*,double,double);
int upi_find_histo (void);
Histo* upic_find_histo (const char*);
void upi_save_histo (void);
void upic_save_histo (Histo*, char*);
int upi_restore_histo (void);
Histo* upic_restore_histo (char*,char*);
int upi_copy_histo (void);
Histo* upic_copy_histo (Histo*);
int upi_bins_of_histo (void);
int upic_bins_of_histo (Histo*);
float upi_min_of_histo (void);
double upic_min_of_histo (Histo*);
float upi_max_of_histo (void);
double upic_max_of_histo (Histo*);
void upi_display_histo (void);
void upic_display_histo (Histo*, int, int);
void upi_hide_histo (void);
void upic_hide_histo (Histo*);
void upi_set_drag_histo_action (void);
void upic_set_drag_histo_action (Routine);
int  upic_drag_histo (Display*,int,int);
void upi_move_histo (void);
void upic_move_histo (Histo*,int,int);
void upi_pos_of_histo (void);
void upic_pos_of_histo (Histo*,int*,int*);
void upi_moving_histo (void);
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

int upic_write_message (const char*,const char*);
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

#ifdef SCREEN
void upic_init_page (Menu*,Page*,int);
void upic_change_page (Menu*,Page*,int);
void upic_draw_title_bar (Display*,const char*,int,int,const char*);
#endif

/*- From UPI_PARAM.C --------------------------------------------------*/

int upic_set_param ( void*, int, char *, ... );
int upic_modify_param ( int, int, int, ... );
int upic_get_param (int,int,int,Menu**,Page**,Item**,Param**);
int upic_refresh_param_page (int);
int upic_refresh_param_line (int,int);
Param* upic_find_param (Param*,int);
void upic_drop_params (Param*);
void upic_drop_param (Param*);
void upic_refresh_params_in_line (Param*);
void upic_refresh_param (Param*);
int upic_build_format (char*,Param*);
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

#ifdef REMOTE

void upir_init_remote (void);
int upir_input (int*,int*,int*,int*);
void upir_delete_command (int,int);
void upir_enable_command (int,int);
void upir_disable_command (int,int);
void upir_replace_item (Menu*,Item*);
void upir_insert_item (Menu*,Item*);
void upir_close_menu (Menu*,Connect*);
void upir_delete_menu (int);
void upir_erase_menu (int);
void upir_write_message (char*,char*);
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
int upir_get_window_position (int,int*,int*);
void upir_attach_pf1 (int);
void upir_cancel_notice (void);
int upir_get_items_per_page (int*);
void upir_set_items_per_page (int);
int upir_get_mode (int*);
void upir_set_mode (int);
void upir_register_on_keypad (int,int,int);
void upir_show_notice (char*,int,char**);
int upir_show_warning (int,char**);
void upir_connect_process (char*);
void upir_disconnect_process (char*);
int upir_dldec (char*,int,int*,int,int);
void upir_dlend (void);
int upir_dlhex (char*,int,int*,int,int);
void upir_dlhead (char*,int,int);
int upir_dlkey (void);
int upir_dlm32 (char*,int,int*);
int upir_dlmask (char*,int,int*);
int upir_dloct (char*,int,int*,int,int);
int upir_dltxt (char*,char*,char*,int,int*);
void upir_dlout (char*,int,int,int);
void upir_dlouto (char*,int,int,int);
void upir_dloutx (char*,int,int,int);
int upir_dlyeno (char*,int,int*);
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
int upic_save_screen (int*,int*);
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
int upic_signal_error (int,char*);
int upic_connect_process (char*);
int upic_disconnect_process (char*);
int upic_show_memory (void);

/*- From UPI_TOOLS.C --------------------------------------------------*/

int upic_show_notice (char*,int,char**);
int upic_cancel_notice (void);
int upic_show_warning (int,char**);
int upic_lock_cursor (void);
int upic_unlock_cursor (void);
int upic_back_space (int);
int upic_register_on_keypad (int,int,int);
int upic_attach_pf1 (int);
int upic_declare_callback (int,int,Routine,int);

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
int upic_get_window_position (int,int*,int*);
int upic_open_detached_window (void);

/*- From --------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif
#undef __CXX_CONST

#endif /* __UPI_PROTO_H */
