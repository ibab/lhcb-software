/*


*/
#define _TanDB_C_

#include    <cstdio>
#include    <cstdlib>
#include    <cstring>
#ifdef _OSK
#include <sg_codes.h>
#include <INET/sg_pcodes.h>
#include <INET/tcp.h>
#elif defined(_VMS)
#include    <vms.h>
#include <unistd.h>
#endif

#include    "NET/defs.h"
#include    "CPP/PubArea.h"
#include    "TAN/TanDB.h"
#include    "TAN/TanErrno.h"
#include    "RTL/rtl.h"
#include    "RTL/Lock.h"

#define NAMESRV_PUBAREA_NAME     "TAN_PUBAREA"
#define NAMESRV_PUBAREA_SIZE     ((int)(sizeof(TanPaSlot)+sizeof(TanPaSlot)/2))

#ifdef _OSK
extern "C" int _sockstt(int,int,void*,int,int,int);
typedef void IOSB;
#define _PreviousEntry(x) (qentry_t*)(x)->prev
#define _NextEntry(x)     (qentry_t*)(x)->next
#define _INSQHI(x,y)      insqhi ( (qentry_t*)x, (head_t*)y    )
#define _INSQTI(x,y)      insqti ( (qentry_t*)x, (head_t*)y    )
#define _REMQHI(x,y)      remqhi ( (head_t*)  x, (qentry_t**)y )
#define _REMQTI(x,y)      remqti ( (head_t*)  x, (qentry_t**)y )
#elif defined(_VMS)
#define _PreviousEntry(x) (qentry_t*)((char*)(x) + (int)(x)->prev)
#define _NextEntry(x)     (qentry_t*)((char*)(x) + (int)(x)->next)
#define _INSQHI(x,y)      lib$insqhi((void*)x,(void*)y)
#define _INSQTI(x,y)      lib$insqti((void*)x,(void*)y)
#define _REMQHI(x,y)      lib$remqhi((void*)x,(void**)y)
#define _REMQTI(x,y)      lib$remqti((void*)x,(void**)y)
#else
#define _PreviousEntry(x) (qentry_t*)((char*)(x) + (int)(x)->prev)
#define _NextEntry(x)     (qentry_t*)((char*)(x) + (int)(x)->next)
#define _INSQHI(x,y)      insqhi(x,y)
#define _INSQTI(x,y)      insqti(x,y)
#define _REMQHI(x,y)      remqhi(x,y)
#define _REMQTI(x,y)      remqti(x,y)
#endif
#define _TheEntry(x) ((qentry_t*)&e.al)

class TanPaSlot  {
public:
  enum {
#ifndef _OSK
    NumEntries = 512
#else // _OSK
    #define OS9_PORT_FLIP 512
    NumEntries = 32
#endif
  };
  qentry_t     _name_head;       /* hash linked list */
  int          _allocated;
  int          _ports;
  TanDataBase::Entry* _pEntry[NumEntries];
  TanDataBase::Entry  _Entry[NumEntries];
};

/*    
**  local variables
*/

static void strlow (char* s1, const char* s2)   {
  char c, diff = 'a' - 'A';
  while ( (c = *s2++) != 0 )    {
    *s1++ = c + ((c >= 'A' && c <= 'Z')  ? diff : 0);
  }
  *s1 = 0;
}
// ----------------------------------------------------------------------------
// C INTERFACE: Allocate raw database entry
//                                      M.Frank
// ----------------------------------------------------------------------------
extern "C" TanDataBase::Entry *tandb_allocate_entry (NetworkChannel::Channel chan)    {
  return TanDataBase::Instance().AllocateEntry(chan);
}
// ----------------------------------------------------------------------------
// C INTERFACE: allocate port
//                                      M.Frank
// ----------------------------------------------------------------------------
extern "C" u_short tandb_allocate_port (TanDataBase::Entry *ce)   {
  return TanDataBase::Instance().AllocatePort(ce);
}
// ----------------------------------------------------------------------------
// C INTERFACE: insert alias entry
//                                      M.Frank
// ----------------------------------------------------------------------------
extern "C" int tandb_insert_alias (TanDataBase::Entry *ce)   {
  return TanDataBase::Instance().InsertAlias ( ce );
}
// ----------------------------------------------------------------------------
// C INTERFACE: convert Dbase entry into its information
//                                      M.Frank
// ----------------------------------------------------------------------------
extern "C" void tandb_get_info (TanDataBase::Entry *ce, NetworkChannel::Channel *chan, TanMessage **msg, TanDataBase::Entry::IOSB **iosb)   {
  *chan = ce->chan;
  *msg  = &ce->msg;
  *iosb = &ce->iosb;
}
// ----------------------------------------------------------------------------
// C INTERFACE: find port by entry
//                                      M.Frank
// ----------------------------------------------------------------------------
extern "C" u_short tandb_find_port (TanDataBase::Entry *ce)   {
  return TanDataBase::Instance().FindPort ( ce );
}
// ----------------------------------------------------------------------------
// C INTERFACE: close connection on an allocated port
//                                      M.Frank
// ----------------------------------------------------------------------------
extern "C" int tandb_close_socket (TanDataBase::Entry *ce)   {
  return TanDataBase::Instance().Close(ce);
}
// ----------------------------------------------------------------------------
// C INTERFACE: Initialize database
//                                      M.Frank
// ----------------------------------------------------------------------------
extern "C" int tandb_init()  {
  TanDataBase::Instance();
  return 0;
}
// ----------------------------------------------------------------------------
// C INTERFACE: free port entry
//                                      M.Frank
// ----------------------------------------------------------------------------
extern "C" int tandb_free_port (TanDataBase::Entry* ce)   {
  return TanDataBase::Instance().FreePort(ce);
}
// ----------------------------------------------------------------------------
// Database Constructor
//                                      M.Frank
// ----------------------------------------------------------------------------
TanDataBase::TanDataBase() : m_lock(0) {
  PubArea Pa(NAMESRV_PUBAREA_NAME);
  int status = PA_FAILURE;
  void* slot = 0;
  while ( status != PA_SUCCESS )  {
    status = Pa.LinkPubArea(NAMESRV_PUBAREA_SIZE);  
    if ( status != PA_SUCCESS )       {
      printf("--1-- Waiting for Publishing area beeing valid!\n");
      lib_rtl_sleep(1000);
    }
  }
  status = PA_FAILURE;
  while ( status != PA_SUCCESS )  {
    int context = (-1);
    status = Pa.GetSlotofType(123,&context,slot);
    if ( status != PA_SUCCESS )       {
      printf("--2-- Waiting for Publishing area beeing valid!\n");
      lib_rtl_sleep(1000);
    }
  }
  _iError = TAN_SS_SUCCESS;
  _pData = (TanPaSlot*)slot;
  status = lib_rtl_create_lock ("TANDB", &m_lock);
  if ( !lib_rtl_is_success(status) )  {
    ::printf("Error creating TANDB lock!\n");
  }
}
// ----------------------------------------------------------------------------
// Database Destructor
//                                      M.Frank
// ----------------------------------------------------------------------------
TanDataBase::~TanDataBase() {
  if ( m_lock ) lib_rtl_delete_lock(m_lock);
}
// ----------------------------------------------------------------------------
// Create the database
//                                      M.Frank
// ----------------------------------------------------------------------------
int TanDataBase::Initialize()  {
  PubArea Pa(NAMESRV_PUBAREA_NAME);
  int status = Pa.CreatePubArea(NAMESRV_PUBAREA_SIZE);
  int context = (-1);
  void *slot;

  if ( status != PA_SUCCESS )  {
    status = Pa.LinkPubArea(NAMESRV_PUBAREA_SIZE);  
    if ( status != PA_SUCCESS ) return TAN_SS_NOMEM;
  }
  status = Pa.GetSlotofType(123,&context,slot);
  if ( status != PA_SUCCESS )  {
    status = Pa.AllocateSlot(123,sizeof(TanPaSlot),slot);
    if ( status != PA_SUCCESS )  {
      return TAN_SS_NOMEM;
    }
  }
  TanPaSlot* data = (TanPaSlot*)slot;  
  if ( data != 0 )  {
    data->_allocated = data->_ports = 0;
    qentry_t* e = (qentry_t*)&data->_name_head;
#ifdef _OSK
    e->next = e->prev = e;
#else
    e->next = e->prev = 0;
#endif
    for ( int i = 0; i < TanPaSlot::NumEntries; i++ )
      data->_pEntry[i] = 0;
    printf("Nameserver Publishing area successfully initialized!\n");
    return TAN_SS_SUCCESS;
  }
  return TAN_SS_NOMEM;
}
// ----------------------------------------------------------------------------
// Database Instanciator
//                                      M.Frank
// ----------------------------------------------------------------------------
TanDataBase& TanDataBase::Instance() {
  /// Pointer to object instance
  static TanDataBase __instance;
  return __instance;
}

// ----------------------------------------------------------------------------
//  Given a message, find the approprate port number in the database. 
//  If not existant, retrun 0.
//                                      M.Frank
// ----------------------------------------------------------------------------
NetworkChannel::Port TanDataBase::FindPort ( TanMessage& msg )  {
  RTL::Lock lock(m_lock);
  Entry *e = _findEntry ( msg._Name() );  
  return (e && e->_IsDead == 0) ? e->port : 0;
}
// ----------------------------------------------------------------------------
// Allocate database port from a given already alloctaed entry
//                                      M.Frank
// ----------------------------------------------------------------------------
NetworkChannel::Port TanDataBase::AllocatePort ( Entry* ce)   {
  RTL::Lock lock(m_lock);
  return _allocatePort(ce);
}
NetworkChannel::Port TanDataBase::_allocatePort ( Entry* ce)   {
  int index;
#ifdef _OSK
  static int current_port    = 0;
  static int ports_availible = OS9_PORT_FLIP-1;
#endif
  if(_pData->_ports >= (TanPaSlot::NumEntries-1)) {  /* JUST TO AVOID FILLING THE TABLE ... */
    _iError = TAN_SS_DATABASEFULL;
    return 0;
  }
  _pData->_ports++;
  strlow(ce->name, ce->msg.name);                  /* FIND INDEX             */
  Entry* entry = _findEntry(ce->name);
  if ( entry != 0 )  {                             /* CHECK IF ENTRY ALREADY EXISTS       */
    _iError = TAN_SS_DUPLNAM;
    return 0;
  }
  _INSQTI(&ce->hl, &_pData->_name_head);
#ifndef _OSK
  for ( index = 0; index < TanPaSlot::NumEntries; index++ ) {
    if ( ce == _pData->_pEntry[index] ) break;
  }
#else
  if ( ++current_port > ports_availible )    {
    //
    // Strategy:
    // =========
    // Starting from each slot: find the biggest range of free
    // port numbers.
    // 
    Entry* e, *ee;
    int count, next, used, numUsed = 0, first_free = 0;
    ports_availible = 0;
    for ( int j = 0; j < TanPaSlot::NumEntries; j++ )    {
      e = _pData->_pEntry[j];
      count = 0;
      if ( 0 != e && e->_IsDead == 0 && e->port != 0 )   {
        used = next = (e->port+1 > (NAMESERVICE_BASE_PORT+OS9_PORT_FLIP))
          ? NAMESERVICE_BASE_PORT : e->port+1;
        for ( int i = next-NAMESERVICE_BASE_PORT; i < OS9_PORT_FLIP; i++ )    {
          for ( int k = 0; k < TanPaSlot::NumEntries; k++ )    {
            ee = _pData->_pEntry[k];
            if ( 0 != ee && e != ee && ee->_IsDead == 0 && ee->port == used )    {
              goto Done;
            }
          }
          count++;
          used++;
        }
Done:
        if ( count > ports_availible )     {
          ports_availible = count;
          first_free     = next-NAMESERVICE_BASE_PORT;
        }
        numUsed++;
      }
    }
    if ( numUsed == 0 )  {
      first_free = 1;  // no port used: start at index 0
      ports_availible = OS9_PORT_FLIP;
    }
    if ( numUsed == TanPaSlot::NumEntries )    {
      printf("ALL Slots are used up....PROBLEM!!!!!\n");
      first_free = 8;  // a guess
    }
    if ( ports_availible == 0 )     {
      ::printf("NO free range availible.....PROBLEM!!!!!\n");
      first_free = 8;  // a guess
    }
    ::printf("New start port %d -> %X slots availible:%d\n",
      first_free, NAMESERVICE_BASE_PORT+first_free, 
      ports_availible);
    current_port     = first_free-1;
    ports_availible += first_free-2;
  }
  index = current_port;
#endif
  ce->_IsDead   = 0;
  ce->port      = NAMESERVICE_BASE_PORT+index+1;   /* THAT'S MY PORT NUMBER          */
  //fprintf(stdout, "%s [%s] Got port:%d %X -> current: %d remaining: %d \n", 
  //        ce->name, ce->msg.name, ce->port,ce->port, current_port, ports_availible-current_port);
  //fflush(stdout);
  ce->port_flag = true;
  return ce->port;
}
// ----------------------------------------------------------------------------
// Free database port from a given alloctaed entry
//                                      M.Frank
// ----------------------------------------------------------------------------
int TanDataBase::FreePort ( Entry* e)   {
  RTL::Lock lock(m_lock);
  return _freePort(e);
}
int TanDataBase::_freePort (Entry* e)   {
  if ( e )  {
    qentry* dummy = 0;
    while (_REMQHI (&e->al, &dummy) != QUE_QUEWASEMPTY)  {
      // REMOVE ALIAS ENTRIES
      // ADJUST OFFSET OF THE REMOVED ENTRY
      Entry* re = (Entry*)(2 * (u_int)dummy - (u_int)&((Entry*)dummy)->al);
      _REMQHI( _PreviousEntry(&re->hl), &dummy);
      _freeEntry(re);
    }
    _REMQHI ( _PreviousEntry(&e->hl), &dummy);  // REMOVE MAIN ENTRY
    _freeEntry(e); 
    return TAN_SS_SUCCESS;
  }
  return TAN_SS_ENTNOTALLOC;
}
// ----------------------------------------------------------------------------
//  Insert ALIAS entry, to a given natural entry
//                                      M.Frank
// ----------------------------------------------------------------------------
int TanDataBase::InsertAlias ( Entry *ce )  {
  RTL::Lock lock(m_lock);
  Entry *e = _findEntry ( ce->msg.name );
  if ( e != 0 )  {                           // CHECK IF ENTRY ALREADY EXISTS 
    _iError = TAN_SS_DUPLNAM;
    return TAN_SS_ERROR;
  }
  e = _allocateEntry(ce->chan);              // NO, ONE EXTRA ENTRY IS NEEDED 
  if ( e == 0 )    {                         // CHECK ALLOCATION STATUS 
    _iError = TAN_SS_NOMEM;
    return TAN_SS_ERROR;
  }
  e->msg = ce->msg;                          // COPY REQUEST MESSAGE 
  strlow  (e->name,     ce->msg.name);       // COPY REMOTE ALIAS NAME 
  strlow  (e->msg.name, ce->msg.name);       // COPY REMOTE ALIAS NAME 
  _INSQTI (&e->hl, &_pData->_name_head);     // INSERT ENTRY IN NAME TABLE 
  _INSQTI (&e->al, &ce->al);                 // INSERT ENTRY IN ALIAS TABLE 
  e->port       = ce->port;                  // PORT IS MAIN ENTRIES PORT 
  e->alias_flag = true;
  return TAN_SS_SUCCESS;
}
// ----------------------------------------------------------------------------
//  Remove ALIAS entry, to a given natural entry
//                                      M.Frank
// ----------------------------------------------------------------------------
int TanDataBase::RemoveAlias ( Entry *e )  {
  RTL::Lock lock(m_lock);
  Entry *db = _findEntry ( e->msg.name );
  if ( db == 0 )  {                        // CHECK IF ENTRY ALREADY EXISTS
    _iError = TAN_SS_ENTNOTALLOC;
    return TAN_SS_ERROR;
  }
  if ( db->port_flag )  {             // CHECK IF THIS IS REALLY A PORT ENTRY
    _iError = TAN_SS_ODDREQUEST;
    return TAN_SS_ERROR;
  }
  return _freeEntry(db);
}
// ----------------------------------------------------------------------------
//  Allocate raw database entry
//                                      M.Frank
// ----------------------------------------------------------------------------
TanDataBase::Entry* TanDataBase::AllocateEntry ( NetworkChannel::Channel chan )   {
  RTL::Lock lock(m_lock);
  return _allocateEntry(chan);
}

TanDataBase::Entry* TanDataBase::_allocateEntry ( NetworkChannel::Channel chan )   {
  Entry *e = 0;
  for ( int i = 0; i < TanPaSlot::NumEntries; i++ )   {
    if ( _pData->_pEntry[i] == 0 )  {
      e = &_pData->_Entry[i];
      e->port_flag = e->alias_flag = false;
      e->port = 0;
      e->_IsDead = 0;
      e->chan = chan;
#ifdef _OSK
      sockaddr_in peer;
      int peerlen = sizeof(peer);
      e->iosb._pHandler = 0;
      e->iosb._pPort = e->iosb._lPort = 0;
      e->hl.next = e->hl.prev = (int)&e->hl;
      e->al.next = e->al.prev = (int)&e->al;
      if ( getpeername( e->chan, (sockaddr *)&peer, &peerlen ) == 0 )
        e->iosb._pPort = peer.sin_port;
      else
        ::printf("Cannot determine peer of socket %d\n", e->chan);
      if ( getsockname( e->chan, (sockaddr *)&peer, &peerlen ) == 0 )
        e->iosb._lPort = peer.sin_port;
      else
        ::printf("Cannot determine sock of socket %d\n", e->chan);
#elif _VMS
      e->iosb.dev_info = e->iosb.status = e->iosb.count = 0;
      e->hl.next = e->hl.prev = e->al.next = e->al.prev = 0;
#else
      e->iosb._pHandler = 0;
      e->iosb._pPort = e->iosb._lPort = 0;
      e->hl.next = e->hl.prev = e->al.next = e->al.prev = 0;
#endif
      _pData->_pEntry[i] =  e;
      _pData->_allocated++;
      return e;
    }
  }
  _iError = TAN_SS_NOMEM;
  return 0;
}
// ----------------------------------------------------------------------------
// Free raw database entry
//                                      M.Frank
// ----------------------------------------------------------------------------
int TanDataBase::_freeEntry (TanDataBase::Entry* e)  {
  bool found = false;
  for ( int i = 0; i < TanPaSlot::NumEntries; i++ )   {
    Entry* db = _pData->_pEntry[i];
    if ( db == e )  {
      _pData->_pEntry[i] = 0;
#ifdef _OSK
      e->hl.next = e->hl.prev = (int)&e->hl;
      e->al.next = e->al.prev = (int)&e->al;
#else
      e->hl.next = e->hl.prev = e->al.next = e->al.prev = 0;
#endif
      e->_IsDead = 1;
      e->name[0] = 0;
      e->chan = 0;
      if ( e->port_flag ) _pData->_ports--;
      _pData->_allocated--;
      found = true;
    }
  }
  return found ? TAN_SS_SUCCESS : TAN_SS_ERROR;
}
// ----------------------------------------------------------------------------
// Find entry identified by its name
//                                      M.Frank
// ----------------------------------------------------------------------------
TanDataBase::Entry* TanDataBase::FindEntry( const char* proc_name)  {
  RTL::Lock lock(m_lock);
  return _findEntry(proc_name);
}
TanDataBase::Entry* TanDataBase::_findEntry( const char* proc_name)  {
  char s[64];
  strlow(s, proc_name);
  qentry_t *e = (qentry_t*)&_pData->_name_head;
  for ( Entry* a  = (Entry*)_NextEntry(e), *last = 0; 
    a != (Entry*) e && a != 0 && a != last;
#ifdef _OSK
    last = a, a = (Entry*) a->hl.next )
#else
    last = a, a  = (Entry*)((char*)a + (int)a->hl.next) )
#endif
  {
    if (::strlen(a->name) > 0 && ::strncmp (s, a->name, sizeof (a->name)) == 0)  {
      if ( a->port_flag || a->alias_flag )  {
        return a;
      }
    }
  }
  _iError = TAN_SS_TASKNOTFOUND;
  return 0;
}
// ----------------------------------------------------------------------------
// Find port associated to e valid entry
//                                      M.Frank
// ----------------------------------------------------------------------------
NetworkChannel::Port TanDataBase::FindPort(Entry *e)  {
  RTL::Lock lock(m_lock);
  return _findPort(e);
}
NetworkChannel::Port TanDataBase::_findPort(Entry *e)  {
  Entry *db = _findEntry (e->msg.name);
  if ( db != 0 )  return db->port;
  _iError = TAN_SS_TASKNOTFOUND;
  return 0;
}
// ----------------------------------------------------------------------------
//
// Remove complete entry with allocated port from the database
// including all aliases.
//
//                                      M.Frank
// ----------------------------------------------------------------------------
int TanDataBase::Close (TanDataBase::Entry *ce)   {
  RTL::Lock lock(m_lock);
  return (ce->port_flag)  ? _freePort (ce) : _freeEntry(ce);
}
// ----------------------------------------------------------------------------
// Dump database content to a file
//                                      M.Frank
// ----------------------------------------------------------------------------
int TanDataBase::Dump( FILE* fptr)  {
  const char *func;
  ::fprintf(fptr,"NameServer Database entry dump: #Allocated %d With port:%d\n",
    _pData->_allocated,_pData->_ports);
  ::fprintf(fptr,"%-16s %-4s(%-3s) %-4s Msg:%-6s %-3s %-16s %s\n",
    "Name","Port","Flg","Chan","Reqst","Len","Name","Address");
  for ( int i = 0; i < TanPaSlot::NumEntries; i++ )     {
    if ( _pData->_pEntry[i] != 0 )  {
      Entry& e = _pData->_Entry[i];
      if ( e.port_flag )  {
        switch ( htonl(e.msg.function) ) {
          case TanMessage::ALLOCATE:
            func = "ALLOC";  
            break;
          case TanMessage::DEALLOCATE:
            func = "DEALLOC"; 
            break;
          case TanMessage::INQUIRE:
            func = "INQUIRE";
            break;
          case TanMessage::ALIAS:
            func = "ALIAS"; 
            break;
          case TanMessage::DEALIAS:
            func = "DEALIAS";
            break;
          case TanMessage::DUMP:
            func = "DUMPDB";
            break;
          default:
            func = "-----";
            break;
        }
        ::fprintf(fptr,"%-16s %04X Prt  %-4d %-3s %-7s%-4d%-16s %s\n",
          e._Name(), e._Port(), e._Channel(), e._IsDead==1 ? "***" : "",
          func, htonl(e.msg._Length()), e.msg._Name(),
          inet_ntoa(e.msg._Address()));
        for ( qentry_t* a  = _NextEntry(&e.al), *last = 0; 
          a != _TheEntry(&e.al) && a != 0 && a != last;
          last = a, a  = _NextEntry(a) )  
        {
          Entry* ee  = (Entry*) ((char*)a - sizeof(qentry_t));
          a = (qentry_t*)&ee->al;
          switch ( htonl(ee->msg.function) ) {
            case TanMessage::ALLOCATE:       func = "ALLOC";       break;
            case TanMessage::DEALLOCATE:     func = "DEALLOC";     break;
            case TanMessage::INQUIRE:        func = "INQUIRE";     break;
            case TanMessage::ALIAS:          func = "ALIAS";       break;
            case TanMessage::DEALIAS:        func = "DEALIAS";     break;
            case TanMessage::DUMP:           func = "DUMPDB";      break;
            default:                         func = "-----";       break;
          }
          ::fprintf(fptr,"%-16s %04X Als  %-4d %-3s %-7s%-4d%-16s %s\n",
            ee->_Name(), ee->_Port(), ee->_Channel(), ee->_IsDead==1 ? "***" : "",
            func, ee->msg._Length(), ee->msg._Name(), inet_ntoa(ee->msg._Address()));
        }
      }
    }
  }
  return TAN_SS_SUCCESS;
}
