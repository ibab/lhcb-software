#ifndef __TanErrno_H__
#define __TanErrno_H__
enum TanErrorcodes  {
  TAN_SS_ERROR   = -1
  ,TAN_SS_SUCCESS = 0
  ,TAN_SS_MADSRV         /* Invalid server answer                         */
  ,TAN_SS_ODDRESPONSE         /* wrong message size from server                */
  ,TAN_SS_HOSTNOTFOUND      /* can't open path to host                       */
  ,TAN_SS_OPEN         /* can't allocate port (socket already open)     */
  ,TAN_SS_NOTOPEN         /* can't deallocate port (socket already closed) */
  ,TAN_SS_UNKNOWNMODE         /* invalid function code                         */
  ,TAN_SS_NOMEM         /* memory eater                                  */
  ,TAN_SS_DATABASEFULL      /* port table is full                            */
  ,TAN_SS_DUPLNAM         /* server did not close the socket               */
  ,TAN_SS_ENTNOTALLOC         /* port number is not allocated                  */
  ,TAN_SS_TASKNOTFOUND      /* name not found                                */
  ,TAN_SS_ODDREQUEST         /* Bad request: Free alias as port...           */
  ,TAN_SS_RECV_TMO                    /* Timeout while trying to receive a message     */
};
#endif
