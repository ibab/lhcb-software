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
#ifndef _MBM_MBM_STRUCTS_H
#define _MBM_MBM_STRUCTS_H

#define MBM_IMPLEMENTATION

#include "RTL/bits.h"
#include "RTL/que.h"
#include "RTL/rtl.h"
#include "MBM/bmdef.h"
#ifndef _WIN32
#include "semaphore.h"
#endif

#include "RTL/Pack.h"

typedef Bits::BitMask<BM_MASK_SIZE> TriggerMask;
typedef Bits::BitMask<BM_MASK_SIZE> UserMask;
typedef Bits::BitMask<BM_MASK_SIZE> VetoMask;

namespace MBM  {
  enum BufferStates   {
    S_nil    = -1,
    S_active = 11,
    S_pause  = 10,
    S_wspace =  9,
    S_wevent =  8,
    S_weslot =  7,
    S_wspace_ast_ready   = 12,
    S_wspace_ast_queued  =  6,
    S_wevent_ast_queued  =  5,
    S_weslot_ast_queued  =  4,
    S_wspace_ast_handled =  3,
    S_wevent_ast_handled =  2,
    S_weslot_ast_handled =  1
  };

  enum EVTIDs {
    EVTID_NONE = -1
  };

  enum BIDs {
    BID_USER  = 0xfeed0001,
    BID_EVENT = 0xfeed0002
  };

}

//#define Shift_p_Bit  12  // = (20-Bits_p_MegaByte+1)
// Bytes_p_Bit = 8191 [Shift_p_Bit=13]
//               4095 [Shift_p_Bit=12]
//               2047 [Shift_p_Bit=11]
//               1023 [Shift_p_Bit=10]
//                511 [Shift_p_Bit=9]
//#define Bytes_p_Bit  ((1<<Shift_p_Bit)-1)

struct REQ  {     //requirements structures xx bytes each   
  TriggerMask   tr_mask PACKED_DATA;     // event Mask
  TriggerMask   vt_mask;                 // veto mask
  int           ev_type;                 // event type
  int           masktype;                // requirement type for trigger mask   
  int           user_type;               // Requirement Type (VIP/normal User)   
  int           user_type_one;           // Requirement ONE Type
  int           freqmode;                // requirement mode       
  float         freq;                    // Frequency limit       
};

template <class T> struct QUEUE {
  T* next;
  T* prev;
  QUEUE() : next(0), prev(0) {}
};

template <class T> struct bm_iterator  {
  T  *m_ent, *m_head;
  int m_curr, m_max;
  bm_iterator(T* h, int mx) : m_ent(h),m_head(h),m_curr(0),m_max(mx) {
    if ( !(*this) ) ++(*this);
  }
  operator bool()  const {
    if ( m_ent == ((T*)~0x0) || m_ent == 0 ) return false;
    if ( m_ent->block_id != T::BID ) return false;
    if ( m_ent->busy == 0 ) return false;
    return true;
  }
  T& operator*()   const {  return *m_ent;     }
  void operator++()  {
    for (m_ent=&m_head[++m_curr];m_curr<m_max;++m_curr,++m_ent) {
      if ( m_ent == ((T*)~0x0) || m_ent == 0 ) break;
      if ( m_ent->block_id != T::BID ) continue;
      if ( m_ent->busy == 0 ) continue;
      return;
    }
    m_ent=0;
  }
};

union MBMConnection  {
  char           name[128];       // Name of answer connection
  struct ServerConnection {
    char         name[128];       // Name of answer connection
    int          request;
    int          poll;
  } master;
  struct FifoServerConnection {
    char         name[128];       // Name of answer connection
    int          request;
    int          response;
  } client;
  struct FifoClientConnection {
    char         name[128];       // Name of answer connection
    int          request;
    int          response;
  } server;
  struct AnyConnection {
    char         name[128];       // Name of answer connection
    void*        channel;
  } any;
  void init()  {
    name[0] = 0;
    client.request  = -1;
    client.response = -1;
  }
  bool hasResponse()  {  return client.response > 0; }
};

struct USER : public qentry_t  {
  qentry_t wsnext;
  qentry_t wenext;
  qentry_t wesnext;
  unsigned int block_id;           // Slot magic word  
  int   busy;                      // slot busy flag       
  int   uid;                       // BM user id       
  unsigned int magic;              // Magic word to check consistency
  int   serverid;                  // Server thread this clients communicates with
  int   state;                     // consumer/producer state (Active,Pause) - (Active,Wspace)
  int   partid;                    // user partition ID
  int   pid;                       // process id
  MBMConnection connection;        // File descriptor of the answer connection
  char  name[BM_USER_NAME_LEN];    // user name
  TriggerMask ev_trmask;           // trmask of waiting exent
  char  ev_dest[BM_USER_NAME_LEN]; // If event was sent with destination, this is the target user
  int   ev_ptr;                    // pointer of waiting space/event
  int   ev_size;                   // size of waiting exent
  int   ev_type;                   // evtype of waiting exent
  int   space_add;                 // address of allocated space    
  int   space_size;                // size in bytes
  int   held_eid;                  // held event index
  int   ev_produced;               // events produced counter
  int   ev_actual;                 // events matching req
  int   ev_seen;                   // # of events seen
  int   ev_freed;                  // # of events freed
  int   n_req;                     // number of requierements
  int   get_ev_calls;
  int   get_sp_calls;
  int   free_calls;
  int   alloc_calls;
  REQ   req[BM_MAX_REQS];          // BM_MAX_REQS requirement maximum     
  //char  __spare[8];
  enum { BID = MBM::BID_USER };
  typedef bm_iterator<USER> iterator;
};

struct USERDesc : public qentry_t  { // active consumers
  qentry_t wev_head;      // consumers waiting events
  qentry_t wsp_head;      // producers waiting space
  qentry_t wes_head;      // producers waiting event slots
  USER   users[1];
};

struct EVENT : public qentry_t {
  unsigned int block_id;  //  0: Block identifier
  int   busy;             //  4: event busy flag
  int   eid;              //  8: event ID
  int partid;             // 12: Partition ID of the event
  int ev_type;            // 16: Event type
  int ev_add;             // 20: Event pointer
  int ev_size;            // 24: Event size
  int count;              // 28: Event identifier
  UserMask umask0;        // Mask of privilidged consumers
  UserMask umask1;        // Mask of non-privilidged consumers
  UserMask held_mask;     // Mask of users holding the event
  UserMask one_mask[BM_MAX_REQONE];  // Masks of privilidged one-only consumers
  TriggerMask tr_mask;    // Trigger Mask
  char spare[16];         // Padding to get to next 32 byte boundary
  enum { BID = MBM::BID_EVENT };
  typedef bm_iterator<EVENT> iterator;
};

struct EVENTDesc : public qentry_t  {    // general event queue
  char     _pad[8];
  EVENT    events[1];
};

struct CONTROL    {
  typedef long long int i_huge;
  int    version;            // Buffer manager version
  int    p_umax;             // maximum users
  int    p_emax;             // maximum events
  int    p_tmax;             // maximum threads
  int    p_base;             // Memory base address
  int    last_alloc;         // Byte Offset of last allocation
  int    shift_p_Bit;        // Shifts per Bit to obtain size in bytes
  int    bytes_p_Bit;        // Number of bytes per bit in bitmap.
  int    last_bit;           // last bit on the bipmap
  long   buff_size;          // Event Buffer size
  long   bm_size;            // size of bit map in bytes
  long   i_events;           // instantaneous event number
  long   i_space;            // instantaneous free  space
  long   i_users;            // instantaneous sctive users
  i_huge tot_produced;       // events produced counter
  i_huge tot_actual;         // events matching req
  i_huge tot_seen;           // events seen
  int    reqone_val[BM_MAX_REQONE];
  int    reqone_count[BM_MAX_REQONE];

} PACKED_DATA;

struct BUFFERS  {
  struct BUFF {
    char name[BM_BUFF_NAME_LEN];
    long size;
    int  used;
    int  spare;
  };
  int   p_bmax;           // Maximum number of buffers
  int   nbuffer;          // Current number of buffers
  BUFF  buffers[1];       // Buffer descriptors
} PACKED_DATA;

struct BufferMemory : public qentry_t   {
  unsigned int     magic;
  char*            gbl;
  CONTROL*         ctrl;
  USER*            user;
  USERDesc*        usDesc;
  EVENT*           event;
  EVENTDesc*       evDesc;
  char*            bitmap;
  unsigned int     bitmap_size;
  char*            buffer_add;
  unsigned int     buffer_size;
  char             bm_name[BM_BUFF_NAME_LEN];
  lib_rtl_gbl_t    gbl_add;
  lib_rtl_gbl_t    buff_add;
  BufferMemory();
};

struct MBMMessage;
struct MBMCommunication  {
  // MBM Server communication functions
  int (*accept)(MBMConnection& connection, const char* bm_name, const char* name);
  int (*close)(MBMConnection& connection);
  int (*bind)(MBMConnection& connection, const char* bm_name, int id);
  int (*open)(MBMConnection& connection, const char* bm_name, const char* name);
  int (*send_response)(MBMConnection& connection, const void* buffer, size_t length);

  /// Create new poll pool
  int (*poll_create)(MBMConnection& connection, int max_count);
  /// Add connection to poll pool
  int (*poll_add)(MBMConnection& connection, MBMConnection& client);
  /// Remove connection from poll pool
  int (*poll_del)(MBMConnection& connection, MBMConnection& client);
  /// Poll on connections delivering data
  int (*poll)(MBMConnection& connection, int& events, int tmo);

  int (*dispatch)(ServerBMID bm, int which);

  /** MBM Client communication functions   */
  /// Open connection to server process
  int (*open_server)(MBMConnection& connection, const char* bm_name, const char* name);
  /// Move server connection to worker thread
  int (*move_server)(MBMConnection& connection, const char* bm_name, USER* u, int serverid);
  /// Exchange MBM message with server
  int (*communicate)(MBMConnection& connection, MBMMessage& msg, int* cancelation_flag);
  /// Clear possibly pending data from communication channel
  int (*clear)(MBMConnection& connection);
  /// Send request to server
  int (*send_request)(MBMConnection& connection, MBMMessage& msg, bool clear_before);
  /// Read server response
  int (*read_response)(MBMConnection& connection, MBMMessage& msg, int* cancelled);

  int type;
  MBMCommunication();
};

struct ServerBMID_t;
typedef ServerBMID_t* ServerBMID;

#include "RTL/Unpack.h"

extern "C" {
  /// Map global buffer information on this machine
  int mbmsrv_map_global_buffer_info(lib_rtl_gbl_t* handle, bool create=true);
  /// Unmap global buffer information on this machine
  int mbmsrv_unmap_global_buffer_info(lib_rtl_gbl_t handle, bool remove=false);

  int mbmsrv_map_memory(const char* buffer_name, BufferMemory* bmid);
  int mbmsrv_map_mon_memory(const char* buffer_name, BufferMemory* bmid);
  int mbmsrv_remap_mon_memory(BufferMemory* bmid);
  int mbmsrv_unmap_memory(BufferMemory* bmid);
  int mbmsrv_dispatch_blocking(ServerBMID bmid);
  int mbmsrv_dispatch_nonblocking(ServerBMID bmid);
  int mbmsrv_stop_dispatch(ServerBMID bmid);
  int mbmsrv_wait_dispatch(ServerBMID bmid);

  int  mbmsrv_handle_request(ServerBMID bm, void* connection, MBMMessage& msg);
  int  mbmsrv_check_pending_tasks(ServerBMID bm);
  void mbmsrv_check_clients(ServerBMID bm);
}

#ifndef MBM_PRINT
inline int _mbm_printf(const char* , ...)  {  return 1;   }
#else
inline int _mbm_printf(const char* fmt, ...)  {
  char buff[1024];
  va_list args;
  va_start(args, fmt);
  int len = vsnprintf(buff, sizeof(buff), fmt, args);
  ::lib_rtl_output(LIB_RTL_ERROR,buff);
  return len;
}
#endif

#endif // _MBM_MBM_STRUCTS_H
