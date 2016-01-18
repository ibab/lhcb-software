// $Id: $
//==========================================================================
//  LHCb Online software suite
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see OnlineSys/LICENSE.
//
// Author     : M.Frank
//
//==========================================================================
//
//    T A N   database structure
//==========================================================================

#define _TanDB_C_

#include    <cstdio>
#include    <cstdlib>
#include    <cstring>
#include    <vector>

#include    "NET/defs.h"
#include    "CPP/PubArea.h"
#include    "CPP/XMLStream.h"
#include    "TAN/TanDB.h"
#include    "TAN/TanErrno.h"
#include    "RTL/rtl.h"
#include    "RTL/Lock.h"

#define NAMESRV_PUBAREA_NAME     "TAN_PUBAREA"
#define NAMESRV_PUBAREA_SIZE     ((int)(sizeof(TanPaSlot)+sizeof(TanPaSlot)/2))

#define _PreviousEntry(x) (qentry_t*)((char*)(x) + (long)(x)->prev)
#define _NextEntry(x)     (qentry_t*)((char*)(x) + (long)(x)->next)
#define _INSQHI(x,y)      insqhi(x,y)
#define _INSQTI(x,y)      insqti(x,y)
#define _REMQHI(x,y)      remqhi(x,y)
#define _REMQTI(x,y)      remqti(x,y)
#define _TheEntry(x) ((qentry_t*)&e.al)

class TanPaSlot  {
public:
  enum {  NumEntries = 1024  };
  qentry_t     _name_head;       /* hash linked list */
  int          _allocated;
  int          _ports;
#ifndef WIN32
  sem_t        _semaphore;
#endif
  TanDataBase::Entry* _pEntry[NumEntries];
  TanDataBase::Entry  _Entry[NumEntries];
};

static void strup (char* s1, const char* s2)   {
  for(; *s2; s2++) *s1++ = ::toupper(*s2);
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
  return TanDataBase::Instance().allocatePort(ce);
}
// ----------------------------------------------------------------------------
// C INTERFACE: insert alias entry
//                                      M.Frank
// ----------------------------------------------------------------------------
extern "C" int tandb_insert_alias (TanDataBase::Entry *ce)   {
  return TanDataBase::Instance().insertAlias ( ce );
}
// ----------------------------------------------------------------------------
// C INTERFACE: convert Dbase entry into its information
//                                      M.Frank
// ----------------------------------------------------------------------------
extern "C" void tandb_get_info (TanDataBase::Entry *ce, NetworkChannel::Channel *chan, TanMessage **msg)  {
  *chan = ce->m_chan;
  *msg  = &ce->m_msg;
}
// ----------------------------------------------------------------------------
// C INTERFACE: find port by entry
//                                      M.Frank
// ----------------------------------------------------------------------------
extern "C" u_short tandb_find_port (TanDataBase::Entry *ce)   {
  return TanDataBase::Instance().findPort ( ce );
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
  return TanDataBase::Instance().freePort(ce);
}
// ----------------------------------------------------------------------------
// Database Constructor
//                                      M.Frank
// ----------------------------------------------------------------------------
TanDataBase::TanDataBase() : m_data(0), m_lock(0), m_error(0)  {
  PubArea Pa(NAMESRV_PUBAREA_NAME);
  int status = PA_FAILURE;
  void* slot = 0;
  while ( status != PA_SUCCESS )  {
    status = Pa.LinkPubArea(NAMESRV_PUBAREA_SIZE);  
    if ( status != PA_SUCCESS )       {
      ::lib_rtl_output(LIB_RTL_INFO,"--1-- Waiting for Publishing area beeing valid!\n");
      ::lib_rtl_sleep(1000);
    }
  }
  status = PA_FAILURE;
  while ( status != PA_SUCCESS )  {
    int context = (-1);
    status = Pa.GetSlotofType(123,&context,slot);
    if ( status != PA_SUCCESS )       {
      ::lib_rtl_output(LIB_RTL_INFO,"--2-- Waiting for Publishing area beeing valid!\n");
      ::lib_rtl_sleep(1000);
    }
  }
  m_error = TAN_SS_SUCCESS;
  m_data = (TanPaSlot*)slot;
#ifdef WIN32
  status = ::lib_rtl_create_lock ("TANDB", &m_lock);
#else
  status = ::lib_rtl_create_lock2(&m_data->_semaphore, &m_lock, true);
#endif
  if ( !::lib_rtl_is_success(status) )  {
    ::lib_rtl_output(LIB_RTL_ERROR,"Error creating TANDB lock!\n");
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
int TanDataBase::initialize()  {
  PubArea Pa(NAMESRV_PUBAREA_NAME);
  int status = Pa.CreatePubArea(NAMESRV_PUBAREA_SIZE);
  int context = (-1);
  void *slot;

  if ( status != PA_SUCCESS )  {
    status = Pa.LinkPubArea(NAMESRV_PUBAREA_SIZE);  
    if ( status != PA_SUCCESS )  {
      ::lib_rtl_output(LIB_RTL_ERROR,"Nameserver failed to link publishing area:%s.\n",NAMESRV_PUBAREA_NAME);
      return TAN_SS_NOMEM;
    }
    ::lib_rtl_output(LIB_RTL_ALWAYS,"Nameserver linked to existing publishing area:%s.\n",NAMESRV_PUBAREA_NAME);
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
    e->next = e->prev = 0;
    for ( int i = 0; i < TanPaSlot::NumEntries; i++ )
      data->_pEntry[i] = 0;
    ::lib_rtl_output(LIB_RTL_ALWAYS,"Nameserver Publishing area successfully initialized!\n");
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
  static TanDataBase s_instance;
  return s_instance;
}
// ----------------------------------------------------------------------------
//  Given a message, find the approprate port number in the database. 
//  If not existant, retrun 0.
//                                      M.Frank
// ----------------------------------------------------------------------------
NetworkChannel::Port TanDataBase::findPort ( const TanMessage& msg )  {
  RTL::Lock lock(m_lock);
  Entry *e = _findEntry ( msg._Name() );  
  return (e && e->m_dead == 0) ? e->m_port : 0;
}
// ----------------------------------------------------------------------------
// Allocate database port from a given already alloctaed entry
//                                      M.Frank
// ----------------------------------------------------------------------------
NetworkChannel::Port TanDataBase::allocatePort ( Entry* ce)   {
  RTL::Lock lock(m_lock);
  return _allocatePort(ce);
}
NetworkChannel::Port TanDataBase::_allocatePort ( Entry* ce)   {
  int index;
  if(m_data->_ports >= (TanPaSlot::NumEntries-1)) {  // JUST TO AVOID FILLING THE TABLE ...
    m_error = TAN_SS_DATABASEFULL;
    return 0;
  }
  m_data->_ports++;
  strup(ce->m_name, ce->m_msg.m_name);              // FIND INDEX
  Entry* entry = _findEntry(ce->m_name);
  if ( entry != 0 )  {                              // CHECK IF ENTRY ALREADY EXISTS
    m_error = TAN_SS_DUPLNAM;
    return 0;
  }
  _INSQTI(&ce->hl, &m_data->_name_head);
  for ( index = 0; index < TanPaSlot::NumEntries; index++ ) {
    if ( ce == m_data->_pEntry[index] ) break;
  }
  ce->mark_dead = 0;
  ce->m_dead   = 0;
  ce->m_port   = NAMESERVICE_BASE_PORT+index+1;   // THAT'S MY PORT NUMBER
  //fprintf(stdout, "%s [%s] Got port:%d %X -> current: %d remaining: %d \n", 
  //        ce->name, ce->m_msg.m_name, ce->port,ce->port, current_port, ports_availible-current_port);
  //fflush(stdout);
  ce->m_port_flag = true;
  return ce->m_port;
}
// ----------------------------------------------------------------------------
// Free database port from a given alloctaed entry
//                                      M.Frank
// ----------------------------------------------------------------------------
int TanDataBase::freePort ( Entry* e)   {
  RTL::Lock lock(m_lock);
  return _freePort(e);
}
int TanDataBase::_freePort (Entry* e)   {
  if ( e )  {
    qentry* dummy = 0;
    while (_REMQHI (&e->al, &dummy) != QUE_QUEWASEMPTY)  {
      // REMOVE ALIAS ENTRIES
      // ADJUST OFFSET OF THE REMOVED ENTRY
      Entry* re = (Entry*)(2 * (u_long)dummy - (u_long)&((Entry*)dummy)->al);
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
int TanDataBase::insertAlias ( Entry *ce )  {
  RTL::Lock lock(m_lock);
  Entry *e = _findEntry ( ce->m_msg.m_name );
  if ( e != 0 )  {                           // CHECK IF ENTRY ALREADY EXISTS 
    m_error = TAN_SS_DUPLNAM;
    return TAN_SS_ERROR;
  }
  e = _allocateEntry(ce->m_chan);            // NO, ONE EXTRA ENTRY IS NEEDED 
  if ( e == 0 )    {                         // CHECK ALLOCATION STATUS 
    m_error = TAN_SS_NOMEM;
    return TAN_SS_ERROR;
  }
  e->m_msg = ce->m_msg;                      // COPY REQUEST MESSAGE 
  strup(e->m_name,      ce->m_msg.m_name);   // COPY REMOTE ALIAS NAME 
  strup(e->m_msg.m_name,ce->m_msg.m_name);   // COPY REMOTE ALIAS NAME 
  _INSQTI (&e->hl, &m_data->_name_head);     // INSERT ENTRY IN NAME TABLE 
  _INSQTI (&e->al, &ce->al);                 // INSERT ENTRY IN ALIAS TABLE 
  e->m_port       = ce->m_port;              // PORT IS MAIN ENTRIES PORT 
  e->m_alias_flag = true;
  e->mark_dead = 0;
  return TAN_SS_SUCCESS;
}
// ----------------------------------------------------------------------------
//  Remove ALIAS entry, to a given natural entry
//                                      M.Frank
// ----------------------------------------------------------------------------
int TanDataBase::removeAlias ( Entry *e )  {
  RTL::Lock lock(m_lock);
  Entry *db = _findEntry ( e->m_msg.m_name );
  if ( db == 0 )  {                        // CHECK IF ENTRY ALREADY EXISTS
    m_error = TAN_SS_ENTNOTALLOC;
    return TAN_SS_ERROR;
  }
  if ( db->m_port_flag )  {             // CHECK IF THIS IS REALLY A PORT ENTRY
    m_error = TAN_SS_ODDREQUEST;
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
    if ( m_data->_pEntry[i] == 0 )  {
      e = &m_data->_Entry[i];
      e->m_port_flag = e->m_alias_flag = false;
      e->m_port = 0;
      e->m_dead = 0;
      e->m_chan = chan;
      e->mark_dead = 0;
      e->hl.next = e->hl.prev = e->al.next = e->al.prev = 0;
      m_data->_pEntry[i] =  e;
      m_data->_allocated++;
      return e;
    }
  }
  m_error = TAN_SS_NOMEM;
  return 0;
}
// ----------------------------------------------------------------------------
// Free raw database entry
//                                      M.Frank
// ----------------------------------------------------------------------------
int TanDataBase::_freeEntry (TanDataBase::Entry* e)  {
  bool found = false;
  for ( int i = 0; i < TanPaSlot::NumEntries; i++ )   {
    Entry* db = m_data->_pEntry[i];
    if ( db == e )  {
      m_data->_pEntry[i] = 0;
      e->hl.next = e->hl.prev = e->al.next = e->al.prev = 0;
      e->m_dead = 1;
      e->m_name[0] = 0;
      e->m_chan = 0;
      e->mark_dead = 0;
      if ( e->m_port_flag ) m_data->_ports--;
      m_data->_allocated--;
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
  strup(s, proc_name);
  qentry_t *e = (qentry_t*)&m_data->_name_head;
  for ( Entry* a  = (Entry*)_NextEntry(e), *last = 0; 
        a != (Entry*) e && a != 0 && a != last;
        last = a, a  = (Entry*)((char*)a + (long)a->hl.next) )
    {
      if ( (a->m_port_flag || a->m_alias_flag) && a->mark_dead == 0 )  {
	if (::strlen(a->m_name) > 0 && ::strncmp(s,a->m_name,sizeof(a->m_name)) == 0)  {
	  return a;
	}
      }
    }
  m_error = TAN_SS_TASKNOTFOUND;
  return 0;
}

void TanDataBase::entriesMarkedDead(std::vector<Entry*>& ents)   {
  RTL::Lock lock(m_lock);
  qentry_t *e = (qentry_t*)&m_data->_name_head;
  for ( Entry* a  = (Entry*)_NextEntry(e), *last = 0; 
        a != (Entry*) e && a != 0 && a != last;
        last = a, a  = (Entry*)((char*)a + (long)a->hl.next) )
    {
      if ( a->mark_dead )  { //&& (a->m_port_flag || a->m_alias_flag) )  {
	ents.push_back(a);
      }
    }
}

void TanDataBase::entriesByFD(int chan, std::vector<Entry*>& ents)   {
  RTL::Lock lock(m_lock);
  qentry_t *e = (qentry_t*)&m_data->_name_head;
  for ( Entry* a  = (Entry*)_NextEntry(e), *last = 0; 
        a != (Entry*) e && a != 0 && a != last;
        last = a, a  = (Entry*)((char*)a + (long)a->hl.next) )
    {
      if ( chan > 0 && chan == a->m_chan )  {
	if ( a->m_port_flag || a->m_alias_flag )  {
	  ents.push_back(a);
	}
      }
    }
}

// ----------------------------------------------------------------------------
// Find entry identified by its channel
//                                      M.Frank
// ----------------------------------------------------------------------------
TanDataBase::Entry* TanDataBase::FindEntry( NetworkChannel::Channel chan )  {
  RTL::Lock lock(m_lock);
  return _findEntry(chan);
}
TanDataBase::Entry* TanDataBase::_findEntry( NetworkChannel::Channel chan )  {
  qentry_t *e = (qentry_t*)&m_data->_name_head;
  for ( Entry* a  = (Entry*)_NextEntry(e), *last = 0; 
        a != (Entry*) e && a != 0 && a != last;
        last = a, a  = (Entry*)((char*)a + (long)a->hl.next) )
    {
      if ( chan > 0 && chan == a->m_chan )  {
	if ( a->m_port_flag || a->m_alias_flag )  {
	  return a;
	}
      }
    }
  m_error = TAN_SS_TASKNOTFOUND;
  return 0;
}
// ----------------------------------------------------------------------------
// Find port associated to e valid entry
//                                      M.Frank
// ----------------------------------------------------------------------------
NetworkChannel::Port TanDataBase::findPort(Entry *e)  {
  RTL::Lock lock(m_lock);
  return _findPort(e);
}
NetworkChannel::Port TanDataBase::_findPort(Entry *e)  {
  Entry *db = _findEntry (e->m_msg.m_name);
  if ( db != 0 )  return db->m_port;
  m_error = TAN_SS_TASKNOTFOUND;
  return 0;
}
// ----------------------------------------------------------------------------
// Remove complete entry with allocated port from the database
// including all aliases.
//                                      M.Frank
// ----------------------------------------------------------------------------
int TanDataBase::Close (TanDataBase::Entry *ce)   {
  RTL::Lock lock(m_lock);
  return (ce->m_port_flag)  ? _freePort (ce) : _freeEntry(ce);
}
// ----------------------------------------------------------------------------
// Dump database content to a file
//                                      M.Frank
// ----------------------------------------------------------------------------
int TanDataBase::Dump( std::ostream& os )  {
  char text[1024];
  const char *func;
  ::snprintf(text,sizeof(text),"NameServer Database entry dump: #Allocated %d With port:%d",
             m_data->_allocated,m_data->_ports);
  os << text << std::endl;
  ::snprintf(text,sizeof(text),"%-32s %-4s(%-3s) %-4s Msg:%-6s %-3s %-32s %s",
             "Name","Port","Flg","Chan","Reqst","Len","Name","Address");
  os << text << std::endl;
  for ( int i = 0; i < TanPaSlot::NumEntries; i++ )     {
    if ( m_data->_pEntry[i] != 0 )  {
      Entry& e = m_data->_Entry[i];
      if ( e.m_port_flag )  {
        switch ( e.m_msg.m_function ) {
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
        ::snprintf(text,sizeof(text),"%-32s %04X Prt  %-4d %-3s %-7s%-4d%-32s %s",
                   e._Name(), e.port(), e.channel(), e.m_dead==1 ? "***" : "",
                   func, e.m_msg._Length(), e.m_msg._Name(),
                   inet_ntoa(e.m_msg.address()));
        os << text << std::endl;
        for ( qentry_t* a  = _NextEntry(&e.al), *last = 0; 
              a != _TheEntry(&e.al) && a != 0 && a != last;
              last = a, a  = _NextEntry(a) )  
	  {
	    Entry* ee  = (Entry*) ((char*)a - sizeof(qentry_t));
	    a = (qentry_t*)&ee->al;
	    switch ( ee->m_msg.m_function ) {
	    case TanMessage::ALLOCATE:       func = "ALLOC";       break;
	    case TanMessage::DEALLOCATE:     func = "DEALLOC";     break;
	    case TanMessage::INQUIRE:        func = "INQUIRE";     break;
	    case TanMessage::ALIAS:          func = "ALIAS";       break;
	    case TanMessage::DEALIAS:        func = "DEALIAS";     break;
	    case TanMessage::DUMP:           func = "DUMPDB";      break;
	    default:                         func = "-----";       break;
	    }
	    ::snprintf(text,sizeof(text),"%-32s %04X Als  %-4d %-3s %-7s%-4d%-32s %s",
		       ee->_Name(), ee->port(), ee->channel(), ee->m_dead==1 ? "***" : "",
		       func, ee->m_msg._Length(), ee->m_msg._Name(), inet_ntoa(ee->m_msg.address()));
	    os << text << std::endl;
	  }
      }
    }
  }
  return TAN_SS_SUCCESS;
}
// ----------------------------------------------------------------------------
// Dump database content to a file
//                                      M.Frank
// ----------------------------------------------------------------------------
int TanDataBase::DumpXML(std::ostream& s)  {
  const char *func;
  XML::Stream os(s.rdbuf());
  XML::Guard top(os,"TAN_SUMMARY");
  os << XML::item("Time",      XML::text(::lib_rtl_timestr("%a %d %b %Y  %H:%M:%S",0)))
     << XML::item("Node",      XML::text(RTL::nodeName()))
     << XML::item("Allocated", m_data->_allocated)
     << XML::item("Ports",     m_data->_ports);
  for ( int j, i = 0; i < TanPaSlot::NumEntries; i++ )     {
    if ( m_data->_pEntry[i] != 0 )  {
      Entry& e = m_data->_Entry[i];
      if ( e.m_port_flag )  {
        switch ( e.m_msg.m_function ) {
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
        XML::Guard entry(os, "Entry", i);
        os << XML::item("Name", XML::text(e._Name()))
           << XML::item("Port", e.port())
           << XML::item("Channel", e.channel())
           << XML::item("Dead", e.m_dead)
           << XML::item("Function", func)
           << XML::item("RemName", XML::text(e.m_msg._Name()))
           << XML::item("Address", XML::text(inet_ntoa(e.m_msg.address())));
        j = 0;
        for ( qentry_t* a  = _NextEntry(&e.al), *last = 0; 
              a != _TheEntry(&e.al) && a != 0 && a != last;
              last = a, a  = _NextEntry(a), ++j )  
	  {
	    Entry* ee  = (Entry*) ((char*)a - sizeof(qentry_t));
	    a = (qentry_t*)&ee->al;
	    switch ( ee->m_msg.m_function ) {
	    case TanMessage::ALLOCATE:       func = "ALLOC";       break;
	    case TanMessage::DEALLOCATE:     func = "DEALLOC";     break;
	    case TanMessage::INQUIRE:        func = "INQUIRE";     break;
	    case TanMessage::ALIAS:          func = "ALIAS";       break;
	    case TanMessage::DEALIAS:        func = "DEALIAS";     break;
	    case TanMessage::DUMP:           func = "DUMPDB";      break;
	    default:                         func = "-----";       break;
	    }
	    XML::Guard aliases(os, "Aliases", i);
	    os << XML::item("Name", XML::text(ee->_Name()))
	       << XML::item("Port", ee->port())
	       << XML::item("Channel", ee->channel())
	       << XML::item("Dead", ee->m_dead)
	       << XML::item("RemName", XML::text(ee->m_msg._Name()))
	       << XML::item("Address", XML::text(inet_ntoa(ee->m_msg.address())));
	  }
      }
    }
  }
  return TAN_SS_SUCCESS;
}
