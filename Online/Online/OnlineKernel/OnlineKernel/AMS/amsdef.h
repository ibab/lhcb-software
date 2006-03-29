/*
** AMS-message error codes
*/

#ifndef __AMSDEF_H__
#define __AMSDEF_H__

#define AMS_FAC 108

#define AMS_SUCCESS      1
#define AMS_ERROR        0

#define AMS_NONAME              (AMS_FAC*256 + 2)
#define AMS_NONODE              (AMS_FAC*256 + 4)
#define AMS_NONETWORK           (AMS_FAC*256 + 6)
#define AMS_TIMEOUT             (AMS_FAC*256 + 8)
#define AMS_NOMEMORY            (AMS_FAC*256 + 10)
#define AMS_NOPROCESS           (AMS_FAC*256 + 12)
#define AMS_TOOMANY_PROTOCOL    (AMS_FAC*256 + 14)
#define AMS_TOOMANY_CONNECT     (AMS_FAC*256 + 16)
#define AMS_DUPLICATE_NAME      (AMS_FAC*256 + 18)
#define AMS_WRONGACK            (AMS_FAC*256 + 20)
#define AMS_BADSEQUENCE         (AMS_FAC*256 + 22)
#define AMS_TRUNCATED           (AMS_FAC*256 + 24)
#define AMS_DATABASEFULL        (AMS_FAC*256 + 26)
#define AMS_TASKNOTFOUND        (AMS_FAC*256 + 28)
#define AMS_HOSTNOTFOUND        (AMS_FAC*256 + 30)
#define AMS_UNKNOWNMODE         (AMS_FAC*256 + 32)
#define AMS_STRBUFFOVFL         (AMS_FAC*256 + 34)
#define AMS_ALRINITED            AMS_SUCCESS
#define AMS_NOTINITED           (AMS_FAC*256 + 36)
#define AMS_CONNCLOSED          (AMS_FAC*256 + 38)
#define AMS_NOPEND              (AMS_FAC*256 + 40)
#define AMS_TASKDIED            (AMS_FAC*256 + 42)
#define AMS_NODATA              (AMS_FAC*256 + 44)
#define AMS_DAMAGED_QUEUE       (AMS_FAC*256 + 46)
#define AMS_DAMAGED_DB          (AMS_FAC*256 + 48)
#define AMS_TERRIBLE            (AMS_FAC*256 + 50)
#define AMS_NOTARMED            (AMS_FAC*256 + 52)
#define AMS_ODDRESPONSE         (AMS_FAC*256 + 54)
#define AMS_TANCLOSED           (AMS_FAC*256 + 56)
#define AMS_TANOPEN             (AMS_FAC*256 + 58)
#define AMS_ILLEGAL_LENGTH      (AMS_FAC*256 + 60)

#ifdef __cplusplus
#include <cstdlib>
#define __CXX_CONSTANT const
extern "C" {
#else
#include <stdlib.h>
#define __CXX_CONSTANT 
#endif

struct amsuc_info  {
  char message[80];
  char source[64];
  unsigned int  facility;
  size_t length;
  int  status;
};

enum AddressStyles   {
    DECNET_STYLE,
    INTERNET_STYLE
};
  void amsc_full_name (char *dest, const char *src, size_t length, int style);
  int amsc_init             ( __CXX_CONSTANT char* );
  int amsc_close            ( void );
  int amsc_send_message     ( __CXX_CONSTANT void*, size_t, __CXX_CONSTANT char*, int, __CXX_CONSTANT char* );
  int amsc_spy_next_message ( void*, size_t*, char*, unsigned int*, size_t*);
  int amsc_spy_next_msg_ptr ( void**, size_t*, char*, unsigned int*, unsigned int*);
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

  typedef int (*amsuc_callback_t)(__CXX_CONSTANT amsuc_info*, void*);
  int amsuc_init      		( void );  int amsuc_subscribe 		( unsigned int, amsuc_callback_t action, amsuc_callback_t broadcast, void* param);  int amsuc_dispatch  		( unsigned int, void* );  int amsuc_remove    		( unsigned int );  int amsuc_subscribe_death 	( __CXX_CONSTANT char*, unsigned int, void* param );  int amsuc_remove_death    	( __CXX_CONSTANT char* );
#ifdef __cplusplus
}
#endif
#undef __CXX_CONSTANT

#define FACILITIES__USER 1	    /* User messages (default) */
#define FACILITIES__SERVER 2	  /* No insertion into WT facility */
#define FACILITIES__UPI 3	      /* Private remote UPI protocol */
#define FACILITIES__FSM 4	      /* Protocol used by fsm messages */
#define FACILITIES__SL_IMMED 2	/* Slow control immed. answer */
#define FACILITIES__SL_DEFER 5	/* Slow control deferred answer */
#define FACILITIES__SL_WARN 6	  /* Slow control warning messages */
#define FACILITIES__SL_RESERV 7	/* Reserved for slow control */
#define FACILITIES__EXPERT 8	  /* Expert system protocol */
#define FACILITIES__USER1 10	  /*  */
#define FACILITIES__USER2 11	  /*  Reserved for user use */
#define FACILITIES__USER3 12	  /*  */
#define FACILITIES__USER4 13	  /*  */
#define FACILITIES__SC_REQ 14	  /*  new slow control request msg */
#define FACILITIES__SC_ALARM 15	/*  new slow control alarm msg */
#define FACILITIES__ERRLOG 20	  /*  Error Logger Handshake */
#define FACILITIES__INCSERV 21	/*  Incident server */
#define FACILITIES__ZBD 12	    /* ZBD code coincides with USER3 */
#define FACILITIES__FSM2 22	    /* FSM 2 facility */

#endif
