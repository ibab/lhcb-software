#ifndef ONLINEKERNEL_TAN_TANDB_H
#define ONLINEKERNEL_TAN_TANDB_H 1

#include <cstdio>
#include <ctype.h>
#include <cstring>

#define NAMESERVICE_BASE_PORT    0x5300

#include "NET/NetworkChannel.h"
#include "TAN/TanMessage.h"
#include "TAN/TanErrno.h"
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

#include <ostream>
#include "CPP/SmartObject.h"

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
  IOSB    m_iosb;
  /// Conected channel number
  NetworkChannel::Channel m_chan;
  /// Port number of the connection
  NetworkChannel::Port    m_port;
  /// Boolean flag to store if the connection is a port
  u_short m_port_flag;       /* this entry is used for allocating a port */
  /// Boolean flag to store if the connection is an alias
  u_short m_alias_flag;       /* this entry is an alias entry */
  /// Store the name of the network partner
  char    m_name [128];
  /// last message sent
  TanMessage m_msg;
#ifdef _VMS
  /// On VMS only 3 bytes padding, since the socket size is short
  char    _pad[3];
#else
  /// Only 1 bytes padding, since the socket size is int
  char    _pad[1];
#endif
  /// Dead entry flag (used mainly for OS9)
  char    m_dead;
  //@Man: Public access method
  /// Return channel number
  NetworkChannel::Channel channel() const {
    return m_chan;
  }
  /// Return Port number
  NetworkChannel::Port port()  const {
    return m_port;
  }
  /// return Name of the network partner
  const char* _Name() const {
    return m_name;
  }
  /// Return last message received
  TanMessage& _Message() {
    return m_msg;
  }
}; 

/// Database class storing all connections the nameserver knows about
/** @class TanDataBase TanDB.h TAN/TanDB.h

    M.Frank
*/
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
  static int initialize();

  /// Construction method
  static TanDataBase& Instance();

  /// Standard destructor
  virtual ~TanDataBase ();

  /// Retrieve error
  int Error() const {
    return _iError;
  }

  /// Find port entry
  NetworkChannel::Port findPort     ( Entry *ce );

  /// Insert alias entry
  int insertAlias                   ( Entry *ce );

  /// Remove alias entry
  int removeAlias                   ( Entry *ce );

  /// Allocate entry
  Entry* AllocateEntry              ( NetworkChannel::Channel chan );

  /// Allocate Port
  NetworkChannel::Port allocatePort ( Entry *ce );

  /// Free port
  int freePort                      ( Entry *ce );

  /// Close single connection
  int Close                         ( Entry *ce );

  /// Find entry by name
  Entry* FindEntry                  ( const char* s);

  /// Dump dbase content to FILE
  int Dump                          ( std::ostream& os );

  /// Dump dbase content to FILE
  int DumpXML                       ( std::ostream& os );

  /// Given a message, find the approprate port number in the database. If not existant, retrun 0.
  NetworkChannel::Port findPort     ( TanMessage& msg );
};

#endif /*  C++ ONLY!                */
#endif /* ONLINEKERNEL_TAN_TANDB_H  */
