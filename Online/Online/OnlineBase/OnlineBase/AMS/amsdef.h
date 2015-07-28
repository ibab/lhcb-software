/*
** AMS-message error codes
*/

#ifndef __AMSDEF_H__
#define __AMSDEF_H__

#define AMS_FAC 108

#define AMS_SUCCESS      1
#define AMS_ERROR        0
// Possible errno mappings
#define AMS_NONAME              (AMS_FAC*256 + 2)  // ESRCH (3)
                                                   // ESRCH
#define AMS_NONODE              (AMS_FAC*256 + 4)  // ENONET (64)
                                                   // ENOENT             WSAHOST_NOT_FOUND
#define AMS_TIMEOUT             (AMS_FAC*256 + 8)  // ETIME (62)         ETIMEDOUT (110)
                                                   //                    WSAETIMEDOUT
#define AMS_NOMEMORY            (AMS_FAC*256 + 10) // ENOMEM  (12)
                                                   // ENOMEM
#define AMS_DUPLICATE_NAME      (AMS_FAC*256 + 18) // EADDINUSE (98)
                                                   // WSAEADDRINUSE      ERROR_DUP_NAME (52)
#define AMS_DATABASEFULL        (AMS_FAC*256 + 26) // EOVERFLOW (75)     EUSERS (87)
                                                   //                    WSAEUSERS
#define AMS_TASKNOTFOUND        (AMS_FAC*256 + 28) // ESRCH (3)
                                                   // ESRCH
#define AMS_HOSTNOTFOUND        (AMS_FAC*256 + 30) // EHOSTDOWN (112)    EHOSTUNREACH (113)
                                                   // WSAEHOSTDOWN       WSAEHOSTUNREACH
#define AMS_UNKNOWNMODE         (AMS_FAC*256 + 32) // EINVAL (22)
                                                   // EINVAL
#define AMS_STRBUFFOVFL         (AMS_FAC*256 + 34) // ENAMETOOLONG 936)  EOVERFLOW (75)
                                                   // WSAENAMETOOLONG
#define AMS_ALRINITED            AMS_SUCCESS
#define AMS_NOTINITED           (AMS_FAC*256 + 36) 
// WSASYSNOTREADY     WSANOTINITIALISED
#define AMS_CONNCLOSED          (AMS_FAC*256 + 38) // ECONNRESET (104)   ENOTCONN (106)
                                                   // WSAECONNRESET      WSAENOTCONN
#define AMS_NOPEND              (AMS_FAC*256 + 40) // ENODATA (61)
                                                   // WSANO_DATA
#define AMS_TASKDIED            (AMS_FAC*256 + 42) // ECONNABORTED (103) ENOTCONN (106)
                                                   // WSAECONNABORTED    WSAENOTCONN
#define AMS_NODATA              (AMS_FAC*256 + 44) // ENODATA (61)
                                                   // WSANO_DATA
#define AMS_TERRIBLE            (AMS_FAC*256 + 50)
#define AMS_ODDRESPONSE         (AMS_FAC*256 + 54) // EBADMSG (74)
#define AMS_TANCLOSED           (AMS_FAC*256 + 56) // ECONNRESET (104)
                                                   // WSAECONNRESET
#define AMS_TANOPEN             (AMS_FAC*256 + 58) // EALREADY (114)     EISCONN(106)
                                                   // WSAEALREADY        WSAEISCONN
#define AMS_ILLEGAL_LENGTH      (AMS_FAC*256 + 60) // EINVAL (22)        EMSGSIZE (90)
                                                   // WSAEINVAL          WSAEMSGSIZE

#ifdef __cplusplus
#include <cstdlib>
#define __CXX_CONSTANT const
extern "C" {
#else
#include <stdlib.h>
#define __CXX_CONSTANT 
#endif
  enum { AMS_NAME_LENGTH = 128 };
  struct amsuc_info  {
    char message[80];
    char source[AMS_NAME_LENGTH];
    unsigned int  facility;
    size_t length;
    int  status;
  };

  enum AddressStyles   {
    DECNET_STYLE,
    INTERNET_STYLE
  };
  /** Initialize AMS with the given task name                     */
  int amsc_init             ( __CXX_CONSTANT char* );
  /** Shutdown AMS                                                */
  int amsc_close            ( void );
  /** Send AMS message to target                                  */
  int amsc_send_message     ( __CXX_CONSTANT void*, size_t, __CXX_CONSTANT char*, int, __CXX_CONSTANT char* );
  /** Spy on next message entry without removing from the queue   */
  int amsc_spy_next_message ( void*, size_t*, char*, unsigned int*, size_t*);
  /** Spy on next message entry without copying the data and without removing from the queue   */
  //Not implemented: int amsc_spy_next_msg_ptr ( void**, size_t*, char*, unsigned int*, unsigned int*);
  /** Blocking reading of the next message entry                  */
  int amsc_get_message (void* buffer, size_t* size, char* from, char* r_source_in,
                        int timeout, unsigned int* facility, unsigned int r_facility, char* dest);
  /** Read message and move message buffer to user. It's the users responsability to delete the buffer! */
  int amsc_read_message_long (void** buff, size_t* size, char* from, unsigned int* facility, char* dest);
  /** release long message pointer                                 */
  int amsc_release_message_long(void* buff);

  /** Read message and copy data to target buffer                  */
  int amsc_read_message     ( void*, size_t*, char*, unsigned int*, char* );
  /** Retrieve AMS node name                                       */
  int amsc_get_node         ( char[40] );
  /** Test if message is pending or beeing assembled               */
  int amsc_test_input       ( void );
  /** Test if message queue is not empty                           */
  int amsc_test_message     ( void );
  /** Spy on the last message received (not necessarily the one to be read next)  */
  int amsc_spy_last_message ( void*, size_t*, char*, unsigned int*, size_t*);
  /** Optional user mode AST on message reception                  */
  int amsc_declare_user_ast ( int (*astadd)(void*), void* astpar );
  int amsc_declare_alias( __CXX_CONSTANT char* );
  int amsc_stack_next_message();
  int amsc_restore_stack(int *cnt);
  void amsc_flush_message_queue (void);
  int amsc_disconnect_task  ( __CXX_CONSTANT char* );
  void amsc_full_name (char *dest, const char *src, size_t length, int style);

  typedef int (*amsuc_callback_t)(__CXX_CONSTANT amsuc_info*, void*);
  /** Initialize AMSU                                                */
  int amsuc_init          ();
  /** Subscribe to AMSU by facility code to receive callbacks on message receival     */
  int amsuc_subscribe     ( unsigned int facility, amsuc_callback_t action, amsuc_callback_t broadcast, void* param);
  /** Internal message dispatcher                                    */
  int amsuc_dispatch      ( unsigned int, void* );
  /** Unsubscribe specified facility from AMSU                       */
  int amsuc_remove        ( unsigned int facility );
  /** Subscribe to IAMDEAD messages from a given source              */
  int amsuc_subscribe_death   ( __CXX_CONSTANT char*, unsigned int, void* param );
  /** Unsubscribe from IAMDEAD messages from a given source          */
  int amsuc_remove_death      ( __CXX_CONSTANT char* );
#ifdef __cplusplus
}
#endif
#undef __CXX_CONSTANT

#define FACILITIES__USER       1      /* User messages (default) */
#define FACILITIES__SERVER     2      /* No insertion into WT facility */
#define FACILITIES__UPI        3      /* Private remote UPI protocol */
#define FACILITIES__FSM        4        /* Protocol used by fsm messages */
#define FACILITIES__SL_IMMED   2  /* Slow control immed. answer */
#define FACILITIES__SL_DEFER   5  /* Slow control deferred answer */
#define FACILITIES__SL_WARN    6    /* Slow control warning messages */
#define FACILITIES__SL_RESERV  7  /* Reserved for slow control */
#define FACILITIES__EXPERT     8    /* Expert system protocol */
#define FACILITIES__USER1 10    /*  */
#define FACILITIES__USER2 11    /*  Reserved for user use */
#define FACILITIES__USER3 12    /*  */
#define FACILITIES__USER4 13    /*  */
#define FACILITIES__SC_REQ 14    /*  new slow control request msg */
#define FACILITIES__SC_ALARM 15  /*  new slow control alarm msg */
#define FACILITIES__ERRLOG 20    /*  Error Logger Handshake */
#define FACILITIES__INCSERV 21  /*  Incident server */
#define FACILITIES__ZBD 12      /* ZBD code coincides with USER3 */
#define FACILITIES__FSM2 22      /* FSM 2 facility */

#endif
