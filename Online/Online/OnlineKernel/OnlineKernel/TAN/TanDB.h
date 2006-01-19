#ifndef __tandb_H__
#define __tandb_H__

#include <cstdio>
#include <ctype.h>
#include <cstring>

#define NAMESERVICE_BASE_PORT    0x5300
#include "NET/NetworkChannel.h"
#include "TAN/TanMessage.h"
#include "TAN/tanerrno.h"
#include "RTL/que.h"
#include "RTL/rtl.h"

#ifdef __cplusplus
class ReceiveHandler;
#endif

typedef struct {                       /* IOSB structure definition */
#ifdef _VMS
  unsigned short status;             /* I/O status code           */
  unsigned short count;              /* Number of Bytes I/O'ed    */
  unsigned int   dev_info;           /* Device specific info      */
#else
  unsigned short _pPort;
  unsigned short _lPort;
#ifdef __cplusplus
  ReceiveHandler* _pHandler;
#else
  void* _pHandler;
#endif
#endif
} __TAN_IOSB__;                        /*                           */

#ifndef __cplusplus                    /*  NO C++ !                 */
typedef void TANDB_ENTRY;              /*  cast TANDB_ENTRY         */
#else                                  /*  NO C++ !                 */
/*  External declarations    */
class  TANDB_ENTRY;                    /*  C++ Forward declarations */
class  TanDataBase;                    /*                           */
class  TanPaSlot;                      /*                           */
#endif                                 /*  C++ ONLY!                */
#ifdef __cplusplus                     /*  C++ ONLY!                */
extern "C"  {
#endif                                 /*  C++ ONLY!                */
  TANDB_ENTRY *tandb_allocate_entry  (__NetworkChannel__ chan);
  u_short      tandb_allocate_port   (TANDB_ENTRY *ce);
  u_short      tandb_allocate_port   (TANDB_ENTRY *ce);
  int          tandb_insert_alias    (TANDB_ENTRY *ce);
  u_short      tandb_find_port       (TANDB_ENTRY *ce);
  int          tandb_close_socket    (TANDB_ENTRY *ce);
  int          tandb_init            (void);
  void         tandb_get_info        (TANDB_ENTRY *ce, 
                                      __NetworkChannel__ *chan, 
                                      TanMessage **msg, 
                                      __TAN_IOSB__ **iosb);
  int          tandb_free_port       (TANDB_ENTRY *ce);
#ifdef __cplusplus                     /*  C++ ONLY!                */
}                                      /*  External declarations    */
#endif                                 /*  C++ ONLY!                */
/*                           */
#ifdef __cplusplus                     /*  C++ ONLY!                */


/// TAN Database entry definition
/** @class TANDB_ENTRY
  *
  * @author  M.Frank
  */
class TANDB_ENTRY  {
public:
  //@Man: public member variables
  typedef __TAN_IOSB__  IOSB;
  /// Linked list of entries
  qentry_t   hl;       /* hash linked list */
  /// Linked list of alias entries
  qentry_t   al;       /* alias linked list */
  /// IOSB structure
  IOSB    iosb;
  /// Conected channel number
  NetworkChannel::Channel chan;
  /// Port number of the connection
  NetworkChannel::Port    port;
  /// Boolean flag to store if the connection is a port
  u_short port_flag;       /* this entry is used for allocating a port */
  /// Boolean flag to store if the connection is an alias
  u_short alias_flag;       /* this entry is an alias entry */
  /// Store the name of the network partner
  char    name [32];
  /// last message sent
  TanMessage msg;
#ifdef _VMS
  /// On VMS only 3 bytes padding, since the socket size is short
  char    _pad[3];
#else
  /// Only 1 bytes padding, since the socket size is int
  char    _pad[1];
#endif
  /// Dead entry flag (used mainly for OS9)
  char    _IsDead;
  //@Man: Public access method
  /// Return channel number
  NetworkChannel::Channel _Channel() const {
    return chan;
  }
  /// Return Port number
  NetworkChannel::Port _Port()  const {
    return port;
  }
  /// return Name of the network partner
  const char* _Name() const {
    return name;
  }
  /// Return last message received
  TanMessage& _Message() {
    return msg;
  }
}; 

/// Database class storing all connections the nameserver knows about
/**

{\Large{\bfclass TanDataBase}}
\\
M.Frank                                   \\
*/
#include "CPP/SmartObject.h"

//class TanDataBase  { ! solved by $cc -qt=local on OS9 M.F
class TanDataBase : public SmartObject<TanDataBase>  {
public:
  //@Man: public data declarations
  /// Database entry definition
  typedef TANDB_ENTRY   Entry;
protected:
  //@Man: Protected member variables
  /// Pointer to data in Pubarea
  TanPaSlot* _pData;
  /// Number of currently allocated Ports
  int _iPort;
  /// Number of currently allocated slots
  int _iAlloc;
  /// Internal errno
  int _iError;
  /// Mutex to protect the database
  lib_rtl_lock_t m_lock;

  //@Man Protected member functions
  /// Standard constructor
  TanDataBase ();

  /// Free an allocated entry
  int _freeEntry (Entry* entry);
  /// Lookupo entry by name
  Entry* _findEntry( const char* proc_name);
  /// Find port entry
  NetworkChannel::Port _findPort( Entry *ce );
  /// Allocate entry
  Entry* _allocateEntry( NetworkChannel::Channel chan );
  /// Allocate Port
  NetworkChannel::Port _allocatePort( Entry *ce );

  /// Free port
  int _freePort( Entry *ce );

public:
  //@Man Public member functions
  /// Initialise/Create the database (Should ONLY be done by the allocator server)
  static int Initialize();

  /// Construction method
  static TanDataBase& Instance();

  /// Standard destructor
  virtual ~TanDataBase ();

  /// Retrieve error
  int Error() const {
    return _iError;
  }

  /// Find port entry
  NetworkChannel::Port FindPort     ( Entry *ce );

  /// Insert alias entry
  int InsertAlias                   ( Entry *ce );

  /// Remove alias entry
  int RemoveAlias                   ( Entry *ce );

  /// Allocate entry
  Entry* AllocateEntry              ( NetworkChannel::Channel chan );

  /// Allocate Port
  NetworkChannel::Port AllocatePort ( Entry *ce );

  /// Free port
  int FreePort                      ( Entry *ce );

  /// Close single connection
  int Close                         ( Entry *ce );

  /// Find entry by name
  Entry* FindEntry            ( const char* s);

  /// Dump dbase content to FILE
  int Dump                          ( FILE* fptr );

  /// Given a message, find the approprate port number in the database. If not existant, retrun 0.
  NetworkChannel::Port FindPort     ( TanMessage& msg );
};
#endif                                 /*  C++ ONLY!   */

#endif
