/*
** AMS-message error codes
*/

#ifndef __AMSDEF_H__
#define __AMSDEF_H__

#define AMS_FAC 108

#define AMS_SUCCESS      0
#define AMS_ERROR      (-1)

#define AMS_NONAME              (AMS_FAC*256 + 1)
#define AMS_NONODE              (AMS_FAC*256 + 2)
#define AMS_NONETWORK           (AMS_FAC*256 + 3)
#define AMS_TIMEOUT             (AMS_FAC*256 + 4)
#define AMS_NOMEMORY            (AMS_FAC*256 + 5)
#define AMS_NOPROCESS           (AMS_FAC*256 + 7)
#define AMS_TOOMANY_PROTOCOL    (AMS_FAC*256 + 9)
#define AMS_TOOMANY_CONNECT     (AMS_FAC*256 + 10)
#define AMS_DUPLICATE_NAME      (AMS_FAC*256 + 11)
#define AMS_WRONGACK            (AMS_FAC*256 + 12)
#define AMS_BADSEQUENCE         (AMS_FAC*256 + 13)
#define AMS_TRUNCATED           (AMS_FAC*256 + 14)
#define AMS_DATABASEFULL        (AMS_FAC*256 + 15)
#define AMS_TASKNOTFOUND        (AMS_FAC*256 + 16)
#define AMS_HOSTNOTFOUND        (AMS_FAC*256 + 17)
#define AMS_UNKNOWNMODE         (AMS_FAC*256 + 18)
#define AMS_STRBUFFOVFL         (AMS_FAC*256 + 19)
#define AMS_ALRINITED            0
#define AMS_NOTINITED           (AMS_FAC*256 + 21)
#define AMS_CONNCLOSED          (AMS_FAC*256 + 22)
#define AMS_NOPEND              (AMS_FAC*256 + 23)
#define AMS_TASKDIED            (AMS_FAC*256 + 24)
#define AMS_NODATA              (AMS_FAC*256 + 25)
#define AMS_DAMAGED_QUEUE       (AMS_FAC*256 + 64)
#define AMS_DAMAGED_DB          (AMS_FAC*256 + 65)
#define AMS_TERRIBLE            (AMS_FAC*256 + 66)
#define AMS_NOTARMED            (AMS_FAC*256 + 67)
#define AMS_ODDRESPONSE         (AMS_FAC*256 + 68)
#define AMS_TANCLOSED           (AMS_FAC*256 + 69)
#define AMS_TANOPEN             (AMS_FAC*256 + 70)
#define AMS_ILLEGAL_LENGTH      (AMS_FAC*256 + 71)

#ifdef __cplusplus
#define __CXX_CONSTANT const
extern "C" {
#else
#define __CXX_CONSTANT 
#endif
  int amsc_init             ( __CXX_CONSTANT char* );
  int amsc_close            ( void );
  int amsc_send_message     ( __CXX_CONSTANT void*, size_t, __CXX_CONSTANT char*, int, __CXX_CONSTANT char* );
  int amsc_spy_next_message ( void*, size_t*, char*, unsigned int*, size_t*);
  int amsc_spy_next_msg_ptr ( void**, size_t*, char*, unsigned int*, int*);
  int amsc_get_message (void* buffer, size_t* size, char* from, char* r_source_in,
                        int timeout, unsigned int* facility, unsigned int r_facility, char* dest);
  int amsc_read_message     ( void*, size_t*, char*, unsigned int*, char* );
  int amsc_get_node         ( char[40] );
  int amsc_test_input       ( void );
  int amsc_test_message     ( void );
  int amsc_spy_last_message ( void*, size_t*, char*, unsigned int*, size_t*);
  int amsc_declare_user_ast ( void (*astadd)(), int );
  int amsc_declare_alias    ( char* );
  int amsc_stack_next_message();
  int amsc_restore_stack(int *cnt);
  void amsc_flush_message_queue (void);
  int amsc_disconnect_task  ( __CXX_CONSTANT char* );

  int amsuc_init      		( void );  int amsuc_subscribe 		( unsigned int, int (*action)(unsigned int, void*), int (*broad)(unsigned int, void*), void* param);  int amsuc_dispatch  		( unsigned int, void* );  int amsuc_remove    		( int );  int amsuc_subscribe_death 	( __CXX_CONSTANT char*, unsigned int, void* param );  int amsuc_remove_death    	( __CXX_CONSTANT char* );
#ifdef __cplusplus
}
#endif
#undef __CXX_CONSTANT

#endif
