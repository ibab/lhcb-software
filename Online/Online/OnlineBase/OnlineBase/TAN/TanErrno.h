#ifndef ONLINEKERNEL_TAN_TANERRNO_H
#define ONLINEKERNEL_TAN_TANERRNO_H
enum TanErrorcodes  {
  TAN_SS_ERROR         = 0 
  ,TAN_SS_SUCCESS      = 1
  ,TAN_SS_MADSRV       = 2         /* Invalid server answer                         */
  ,TAN_SS_ODDRESPONSE  = 4         /* wrong message size from server                */
  ,TAN_SS_HOSTNOTFOUND = 6         /* can't open path to host                       */
  ,TAN_SS_OPEN         = 8         /* can't allocate port (socket already open)     */
  ,TAN_SS_NOTOPEN      = 10        /* can't deallocate port (socket already closed) */
  ,TAN_SS_UNKNOWNMODE  = 12        /* invalid function code                         */
  ,TAN_SS_NOMEM        = 14        /* memory eater                                  */
  ,TAN_SS_DATABASEFULL = 16        /* port table is full                            */
  ,TAN_SS_DUPLNAM      = 18        /* server did not close the socket               */
  ,TAN_SS_ENTNOTALLOC  = 20        /* port number is not allocated                  */
  ,TAN_SS_TASKNOTFOUND = 22        /* name not found                                */
  ,TAN_SS_ODDREQUEST   = 24        /* Bad request: Free alias as port...            */
  ,TAN_SS_RECV_TMO     = 26        /* Timeout while trying to receive a message     */
};
#endif  // ONLINEKERNEL_TAN_TANERRNO_H
