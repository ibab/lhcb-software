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

#define MBM_IMPLEMENTATION
#define MBM_SERVER_IMPLEMENTATION

// Framework include files
#include "RTL/Lock.h"
#include "RTL/strdef.h"
#include "RTL/DoubleLinkedQueue.h"
#include "RTL/DoubleLinkedQueueScan.h"
#include "MBM/bmstruct.h"
#include "MBM/bmserver.h"
#include "MBM/bmmessage.h"
#include "bm_internals.h"

// C/C++ include files
#include <cerrno>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <signal.h>
#include <fcntl.h>
#include <stdexcept>

//#include <google/profiler.h>

using namespace MBM;
static int EVENT_next_off;
static int USER_we_off;
static int USER_ws_off;
static int USER_wes_off;
static int USER_next_off;

static qentry_t *desc_head = 0;
static int reference_count = 0;

static inline int mbm_error_code()  {  
  return MBM_ERROR;  
}

static inline int mbm_error(const char* fn, int line)  {  
  int e = errno;
  ::lib_rtl_output(LIB_RTL_ERROR,"MBM inconsistency in '%s' Line:%d errno:%d\n",
		   fn, line, e, RTL::errorString(e));
  return MBM_ERROR;
}

#define MBM_MAX_BUFF  32
#define MBM_USER_MAGIC 0xFEEDBABE
#define  MBM_ERROR_CODE mbm_error_code()
#undef   MBM_ERROR
#define  MBM_ERROR  mbm_error(__FILE__,__LINE__);
#define CHECKED_CONSUMER(u)  (u); if ( !(u) || (u)->magic != MBM_USER_MAGIC || (u)->busy == 0 || (u)->uid == -1   ) { mbm_error(__FILE__,__LINE__); return MBM_ILL_CONS; }
#define CHECKED_CLIENT(u)    (u); if ( !(u) || (u)->magic != MBM_USER_MAGIC || (u)->busy == 0 || (u)->uid == -1   ) return MBM_ERROR;

#ifndef MBM_HAVE_STD_MUTEX
#define CHECK_BMID(b)        (b); if ( !(b) || (b)->magic != MBM_USER_MAGIC || 0 == (b)->lockid ) return MBM_ERROR;
typedef  RTL::Lock  LOCK;
#else
#define CHECK_BMID(b)        (b); if ( !(b) || (b)->magic != MBM_USER_MAGIC ) return MBM_ERROR;
typedef  std::lock_guard<std::mutex> LOCK;
#endif

#define  MBMQueue   RTL::DoubleLinkedQueue
#define  MBMScanner RTL::DoubleLinkedQueueScan
typedef  MBMMessage MSG;

/// Interface routine: Map all memory sections of the server process
int mbmsrv_map_memory(const char* bm_name, BufferMemory* bm);
/// Interface routine: map memory necessary for monitoring
int mbmsrv_map_mon_memory(const char* bm_name, BufferMemory* bm);
/// Interface routine: map memory necessary for monitoring
int mbmsrv_remap_mon_memory(BufferMemory* bm);
/// Interface routine: unmape memory
int mbmsrv_unmap_memory(BufferMemory* bmid);
/// Include into buffer manager
int mbmsrv_include (ServerBMID bm, void* connection, MSG& msg);
/// Reconnect after moving to different worker
int mbmsrv_reconnect (ServerBMID bm, void* connection, MSG& msg);
/// Exclude client from buffer manager
int mbmsrv_exclude (ServerBMID bm, MSG& msg);
/// Consumer interface: Free event after processing
int mbmsrv_free_event(ServerBMID bm, MSG& msg);
/// Consumer interface: Pause event access
int mbmsrv_pause(ServerBMID bm, MSG& msg);
/// Consumer interface: Get event pending / subscribe to event eventually pending
int mbmsrv_get_event(ServerBMID bm, MSG & msg);
/// Consumer interface: Consumer add request
int mbmsrv_add_req(ServerBMID bm, MSG & msg);
/// Consumer interface: Delete consumer request
int mbmsrv_del_req(ServerBMID bm, MSG& msg);
/// Producer interface: Try to get space from the buffer manager
int mbmsrv_get_space_try(ServerBMID bm, MSG& msg);
/// Producer interface: Get space from the buffer manager
int mbmsrv_get_space (ServerBMID bm, MSG& msg);
/// Producer interface: Server side implementation of mbm_declare_event(_try)
int mbmsrv_declare_event(ServerBMID bm, MSG& msg);
/// Producer interface: Server implementation of mbm_free_space call
int mbmsrv_free_space(ServerBMID bm, MSG& msg);
/// Producer interface: Notify consumers about the presence of an event
int mbmsrv_send_space(ServerBMID bm, MSG& msg);
/// Consumer/Producer interface: cancel pending request(s)
int mbmsrv_cancel_request(ServerBMID bm, MSG& msg);
/// Consumer interface: stop consumer
int mbmsrv_stop_consumer(ServerBMID bm, MSG& msg);
/// Add consumer requirement to server
int mbmsrv_req_consumer(ServerBMID bm, MSG& msg);
/// Deregister consumer requirement from server
int mbmsrv_unreq_consumer(ServerBMID bm, MSG& msg);

/// Evaluate consumer rules
int _mbmsrv_evaluate_rules(ServerBMID bm);
/// Match single consumer requirement
int _mbmsrv_match_cons_req(const MSG::cons_requirement_t& rq, 
                           const ServerBMID_t::ConsumerREQ& cr);

/// Server interface: Connect server process
int _mbmsrv_connect(ServerBMID bm);
/// Server interface: Server dispatch function
int _mbmsrv_call(ServerBMID bm, MSG& msg);


/// MAP buffer manager section
int _mbmsrv_map_buff_section(BufferMemory* bm);
/// MAP buffer manager sections
int _mbmsrv_map_ctrl_section(BufferMemory* bm);
/// MAP buffer manager sections
int _mbmsrv_map_ctrl_section(BufferMemory* bm);
/// Unmap server memory sections
int _mbmsrv_unmap_sections(BufferMemory* bm);
/// Check existance of a user with a given name 
int _mbmsrv_findnam (ServerBMID bm, const char* name);
/// Allocate user slot
USER* _mbmsrv_ualloc(ServerBMID bm);
/// Free user slot
int _mbmsrv_ufree(USER* u);
/// Clean-up this user slot
int _mbmsrv_uclean(ServerBMID bm, USER* u);
/// Deallocate buffer space
int _mbmsrv_sfree(ServerBMID bm, int add, int size);
// Alloc event slot
EVENT* _mbmsrv_ealloc (ServerBMID bm);
/// Free event slot
int _mbmsrv_efree (ServerBMID bm, EVENT* e);
/// Delete user from the wait_event queue
int _mbmsrv_del_wev (USER* u);
/// Delete user from the wait_space queue
int _mbmsrv_del_wsp (USER* us);
/// Try to get space ...
int _mbmsrv_get_sp (ServerBMID bm, USER* us, int size);
/// Delete user from the wait_event_slot queue
int _mbmsrv_del_wes(USER* us);
/// Check wait space queue
int _mbmsrv_check_wsp (ServerBMID bm);
/// check wait event queue
int _mbmsrv_check_wev (ServerBMID bm, EVENT* e);
/// Check consumer if (some) clients are waiting for events
int _mbmsrv_check_cons(ServerBMID bm, USER* u);

/// Alloc a special REQ_ONE slot and increase it's reference count
int _mbmsrv_alloc_reqone_id(ServerBMID bm,int user_type,int desired_type, int* user_type_one);
/// Dealloc a special REQ_ONE slot and decrease it's reference count
int _mbmsrv_free_reqone_id(ServerBMID bm,int user_type,int user_type_one);
/// Find matching req
int _mbmsrv_match_req (ServerBMID bm, int partid, int evtype, TriggerMask& trmask, 
                       UserMask& mask0, UserMask& mask1, UserMask* one_masks, int* match_or);
/// Remove event from active event queue
int _mbmsrv_del_event(ServerBMID bm, EVENT* e);
/// clear events with freqmode = notall
int _mbmsrv_check_freqmode (ServerBMID bm);
// Release event held by this user
int _mbmsrv_rel_event (ServerBMID bm, USER* u);
/// Get event pending / subscribe to event eventually pending
int _mbmsrv_get_ev(ServerBMID bm, USER* u);

MBMCommunication::MBMCommunication()
  :
  /* Server functions */
  accept(0), close(0), bind(0), open(0), send_response(0),
  poll_create(0), poll_add(0), poll_del(0), poll(0), dispatch(0), 
  /* Client functions  */
  open_server(0), move_server(0), communicate(0), clear(0), 
  send_request(0), read_response(0),
  /* Misc. */
  type(BM_COM_NONE)
{
}

BufferMemory::BufferMemory() : qentry_t(0,0), gbl_add(0), buff_add(0) {
  magic             = MBM_USER_MAGIC;
  gbl               = 0;
  ctrl              = 0;
  user              = 0;
  usDesc            = 0;
  event             = 0;
  evDesc            = 0;
  bitmap            = 0;
  bitmap_size       = 0;
  buffer_add        = 0;
  buffer_size       = 0;
  bm_name[0]        = 0;
}

ServerBMID_t::ServerBMID_t() : BufferMemory(), client_thread(0) {
  ::memset(server,  0, sizeof(server));
  ::memset(cons_req,0, sizeof(cons_req));
  free_event        = 0;
  free_event_param  = 0;
  alloc_event       = 0;
  alloc_event_param = 0;
  stop              = 0;
#ifndef MBM_HAVE_STD_MUTEX
  lockid            = 0;
#endif
  num_threads       = 0;
  allow_declare     = 1;
  clients.init();
}

ServerBMID_t::~ServerBMID_t()  {
}

/// Map global buffer information on this machine
int mbmsrv_map_global_buffer_info(lib_rtl_gbl_t* handle, bool create)  {
  lib_rtl_gbl_t h;
  size_t len = sizeof(BUFFERS)+(MBM_MAX_BUFF-1)*sizeof(BUFFERS::BUFF);
  int status = ::lib_rtl_map_section("bm_buffers",len,&h);
  if( !lib_rtl_is_success(status) && create )    {
    status = ::lib_rtl_create_section("bm_buffers",len,&h,true);
    if(!lib_rtl_is_success(status))   {   
      return MBM_ERROR_CODE;
    }
    BUFFERS* buffs = (BUFFERS*)h->address;
    ::memset(buffs,0,len);
    buffs->nbuffer = 0;
  }
  else if (!lib_rtl_is_success(status)) {
    *handle = 0;
    return MBM_ERROR_CODE;
  }
  BUFFERS* b = (BUFFERS*)h->address;
  b->p_bmax = MBM_MAX_BUFF;
  *handle = h;
  return MBM_NORMAL;
}

/// Unmap global buffer information on this machine
int mbmsrv_unmap_global_buffer_info(lib_rtl_gbl_t handle, bool remove)  {
  if ( handle )  {
    BUFFERS* buffs = (BUFFERS*)handle->address;
    int status = (buffs->nbuffer == 0 && remove) 
      ? ::lib_rtl_delete_section(handle)
      : ::lib_rtl_unmap_section(handle);
    return lib_rtl_is_success(status) ? MBM_NORMAL : MBM_ERROR;
  }
  return MBM_ERROR;
}

/// MAP buffer manager section
int _mbmsrv_map_buff_section(BufferMemory* bm)  {
  char text[128];
  const char* bm_name = bm->bm_name;
  ::snprintf(text,sizeof(text),"bm_buff_%s",bm_name);
  int status = ::lib_rtl_map_section(text,0,&bm->buff_add);
  if (!lib_rtl_is_success(status))    {
    ::lib_rtl_output(LIB_RTL_OS,"++bm_serevr++ Error mapping buffer section for MBM buffer %s\n",bm_name);
    return MBM_ERROR;
  }
  bm->buffer_add  = (char*)bm->buff_add->address;
  return MBM_NORMAL;
}

/// MAP buffer manager sections
int _mbmsrv_map_ctrl_section(BufferMemory* bm)  {
  char text[128];
  size_t ctrl_len, evnt_len, user_len;
  const char* bm_name = bm->bm_name;
  ::snprintf(text,sizeof(text),"bm_ctrl_%s",bm_name);
  int status = ::lib_rtl_map_section(text,0,&bm->gbl_add);
  if (!lib_rtl_is_success(status))    {
    ::lib_rtl_output(LIB_RTL_OS,"++bm_server++ Error mapping control section for MBM buffer %s\n",bm_name);
    return MBM_ERROR;
  }
  bm->gbl         = (char*)bm->gbl_add->address;
  bm->ctrl        = (CONTROL*)bm->gbl;
  ctrl_len        = mbm_section_length(sizeof(CONTROL));
  evnt_len        = mbm_section_length(sizeof(EVENTDesc)+(bm->ctrl->p_emax-1)*sizeof(EVENT));
  user_len        = mbm_section_length(sizeof(USERDesc)+(bm->ctrl->p_umax-1)*sizeof(USER));
  bm->usDesc      = (USERDesc*)(bm->gbl+ctrl_len);
  bm->user        = &bm->usDesc->users[0];
  bm->evDesc      = (EVENTDesc*)(bm->gbl+ctrl_len+user_len);
  bm->event       = &bm->evDesc->events[0];
  bm->bitmap      = bm->gbl+ctrl_len+evnt_len+user_len+evnt_len;
  bm->bitmap_size = bm->ctrl->bm_size;
  bm->buffer_size = bm->ctrl->buff_size;
  return MBM_NORMAL;
}

/// Unmap server memory sections
int _mbmsrv_unmap_sections(BufferMemory* bm)  {
  if ( bm->gbl        ) ::lib_rtl_unmap_section(bm->gbl_add);
  if ( bm->buffer_add ) ::lib_rtl_unmap_section(bm->buff_add);
  bm->gbl        = 0;
  bm->buff_add   = 0;
  bm->buffer_add = 0;
  bm->bitmap     = 0;
  bm->user       = 0;
  bm->event      = 0;
  bm->ctrl       = 0;
  bm->user       = 0;
  bm->usDesc     = 0;
  bm->event      = 0;
  bm->evDesc     = 0;
  bm->bitmap     = 0;
  return MBM_NORMAL;
}

int _mbmsrv_close_connections(ServerBMID bm) {
  for(size_t i=0; i < BM_MAX_THREAD; ++i) {
    ServerBMID_t::Server& s = bm->server[i];
    bm->communication.close(s.connection);
  }
  bm->communication.close(bm->clients);
  // Return for convenience here error, since this is called in initialize
  return MBM_NORMAL;
}

int _mbmsrv_shutdown (void* /* param */) {
  qentry_t *q, *bmq = desc_head;
  ServerBMID ids[32];  // I have to simply assume
  int cnt = 0, sc = 1, len=sizeof(ids)/sizeof(ids[0]);

  if ( 0 == reference_count )   {
    return MBM_NORMAL;
  }
  else if (bmq == 0)  {
    return MBM_NORMAL;
  }
  reference_count = 0;
  while( lib_rtl_queue_success(sc) ) {
    for(cnt=0; cnt<len; ++cnt) ids[cnt]=0;
    for(sc=::remqhi(bmq,&q),cnt=0; cnt<len && lib_rtl_queue_success(sc); sc=::remqhi(bmq,&q))
      ids[cnt++] = ServerBMID(q);
    ::lib_rtl_sleep(500);
    for(int i=0; i<cnt; ++i)  {
      ServerBMID bm = ids[i];
      if ( bm != 0 )   {
        _mbmsrv_close_connections(bm);
        for (USER* u=bm->user, *last=u+bm->ctrl->p_umax; u != last; ++u)    {
          if ( u->busy )   {
	    bm->communication.close(u->connection);
          }
        }
        if ( bm->gbl        ) ::lib_rtl_delete_section(bm->gbl_add);
        if ( bm->buffer_add ) ::lib_rtl_delete_section(bm->buff_add);
      }
    }
  }
  return MBM_NORMAL;
}

/// Check existance of a user with a given name 
int _mbmsrv_findnam (ServerBMID bm, const char* name) {
  MBMScanner<USER> que(bm->usDesc,-USER_next_off);
  for(USER* u=que.get(); u; u=que.get())  {
    if ( ::strncmp(u->name, name, sizeof(u->name)) == 0 )  {
      return u->uid;
    }
  }
  return -1;
}

/// Check if a given event is held by any user. Returns true if at least one consumer needs to see this event.
bool _mbmsrv_evt_held(EVENT* e) {
  unsigned int held = 0;
  for(int i=0, words=e->umask0.words(); 0==held && i<words; ++i) {
    held += e->umask0.bits()[i];
    held += e->umask1.bits()[i];
    held += e->held_mask.bits()[i];
    for(int j=0; 0==held && j<BM_MAX_REQONE;++j) {
      held += e->one_mask[j].bits()[i];
    }
  }
  return held != 0;
}

/// Check if a given event is held by a user or must be processed by a user. 
/// Returns true if at least one consumer needs to see this event.
bool _mbmsrv_evt_held_vip(EVENT* e) {
  unsigned int held  = 0;
  for(int i=0, words=e->umask0.words(); 0==held && i<words; ++i) {
    held += e->umask0.bits()[i];
    held += e->held_mask.bits()[i];
    for(int j=0; 0==held && j<BM_MAX_REQONE;++j) {
      held += e->one_mask[j].bits()[i];
    }
  }
  return held != 0;
}

/// Clear user identified by uid from the access to thie event e
void _mbmsrv_evt_clear(EVENT* e, USER* u) {
  int uid = u->uid;
  e->umask0.clear(uid);
  e->umask1.clear(uid);
  for(size_t i=0; i<BM_MAX_REQONE; ++i)
    e->one_mask[i].clear(uid);
  e->held_mask.clear(uid);
}

/// Allocate user slot
USER* _mbmsrv_ualloc(ServerBMID bm)  {
  USER*    u = 0;
  CONTROL* c = bm->ctrl;
  for (int i = 0; i < c->p_umax; ++i, ++u)  {
    u = bm->user + i;
    if (u->busy == 0)    {
      // Reset queue entries
      u->wsnext.next  = 0;
      u->wsnext.prev  = 0;
      u->wesnext.next = 0;
      u->wesnext.prev = 0;
      u->wenext.next  = 0;
      u->wenext.prev  = 0;
      // Reset basic data
      u->busy     =  1;
      u->uid      =  i;
      u->pid      = -1;
      u->partid   = -1;
      u->name[0]  =  0;
      u->magic    =  MBM_USER_MAGIC;
      u->held_eid =  EVTID_NONE;
      u->ev_size  =  0;
      u->ev_ptr   =  0;
      u->ev_size  =  0;
      // Producer variables
      u->space_size = 0;
      u->connection.init();
      ::insqti(u,bm->usDesc);
      return u;
    }
  }
  return 0;
}

/// Delete user from the wait_event queue
int _mbmsrv_del_wev (USER* u) {
  if ( u->state != S_wevent )  {
    ::lib_rtl_output(LIB_RTL_ERROR,"INCONSISTENCY: Delete user from WEV queue without state S_wevent\n");
  }
  if ( u->wenext.next == 0 && u->wenext.prev != 0 )   {
    ::lib_rtl_output(LIB_RTL_ERROR,"INCONSISTENCY: Remove user from WEV queue without valid QUE entry\n");
  }
  ::remqent(&u->wenext);
  return MBM_NORMAL;
}

/// Delete user from the wait_space queue
int _mbmsrv_del_wsp (USER* us) {
  if (us->state != S_wspace)  {
    _mbm_printf("INCONSISTENCY: Delete user from WSP queue without state S_wspace\n");
  }
  ::remqent(&us->wsnext);
  us->state = S_wspace_ast_queued;
  return MBM_NORMAL;
}

/// Try to get space ...
int _mbmsrv_get_sp (ServerBMID bm, USER* us, int size)  {
  CONTROL *ctrl = bm->ctrl;
  char *bitmap  = bm->bitmap;
  int shift     = ctrl->shift_p_Bit;
  int bit, nbit = (size + ctrl->bytes_p_Bit) >> shift;  // round size to block
  ctrl->last_alloc = 0;
  int status = BF_alloc(bitmap,ctrl->bm_size,nbit,&bit);
  if (lib_rtl_is_success(status))  {
    bit += ctrl->last_alloc<<3 ;
    ctrl->last_alloc  = (bit+nbit)>>3;
    ctrl->last_bit = bit;
    ctrl->i_space -= nbit;
    us->ev_ptr     = bit  << shift;
    us->space_add  = bit  << shift;    // keep space info
    us->space_size = nbit << shift;
    return MBM_NORMAL;
  }
  return MBM_NO_ROOM;
}

/// Delete user from the wait_event_slot queue
int _mbmsrv_del_wes(USER* us)   {
  us->state = S_weslot_ast_queued;
  ::remqent(&us->wesnext);
  return MBM_NORMAL;
}

/// Alloc a special REQ_ONE slot and increase it's reference count
int _mbmsrv_alloc_reqone_id(ServerBMID bm,int user_type,int desired_type, int* user_type_one) {
  *user_type_one = 0;
  if ( user_type == BM_REQ_ONE )   {
    CONTROL *c = bm->ctrl;
    int free_id = -1;
    for(size_t ir=0; ir<BM_MAX_REQONE;++ir) {
      int val = c->reqone_val[ir];
      if ( free_id < 0 && val == 0 ) free_id=ir;
      if ( val == desired_type ) {
        ++c->reqone_count[ir];
        *user_type_one = ir;
        return MBM_NORMAL;
      }
    }
    if ( free_id >= 0 ) {
      c->reqone_val[free_id] = desired_type;
      ++c->reqone_count[free_id];
      *user_type_one = free_id;
      return MBM_NORMAL;
    }
    return MBM_ERROR;
  }
  return MBM_NORMAL;
}

/// Dealloc a special REQ_ONE slot and decrease it's reference count
int _mbmsrv_free_reqone_id(ServerBMID bm,int user_type,int user_type_one) {
  int id = user_type_one;
  if ( user_type == BM_REQ_ONE && id>=0 && id<BM_MAX_REQONE )    {
    CONTROL *c = bm->ctrl;
    if ( --c->reqone_count[id] == 0 ) {
      c->reqone_val[id] = 0;
    }
  }
  return MBM_NORMAL;
}

/// Free user slot
int _mbmsrv_ufree(USER* u)  {
  if ( u->busy == 1 )  {
    u->space_size   =  0;
    u->space_add    =  0;
    u->busy         =  0;
    u->uid          = -1;
    u->name[0]      =  0;
    u->state        =  S_nil;
    u->wsnext.next  =  0;
    u->wsnext.prev  =  0;
    u->wesnext.next =  0;
    u->wesnext.prev =  0;
    u->wenext.next  =  0;
    u->wenext.prev  =  0;
    u->connection.init();
    ::remqent(u);
    return MBM_NORMAL;
  }
  return MBM_INTERNAL;
}

/// Check wait space queue
int _mbmsrv_check_wsp(ServerBMID bm)  {
  CONTROL *ctrl   = bm->ctrl;
  char    *bitmap = bm->bitmap;
  int      shift  = ctrl->shift_p_Bit;
  MBMScanner<USER> que(&bm->usDesc->wsp_head, -USER_ws_off);
  for (USER* u=que.get(); u; u = que.get() )  {
    if ( u->state == S_wspace )      {
      int ubit = (u->ev_size + ctrl->bytes_p_Bit) >> shift;
      int bit = 0, nbit = ubit;
      if( BF_count(bitmap,ctrl->bm_size,&bit,&nbit) == 1) {    // find largest block 
        ctrl->last_alloc = 0;
        int status = BF_alloc(bitmap,ctrl->bm_size,ubit,&bit);
        if ( lib_rtl_is_success(status) )   {
          MSG msg(MSG::GET_SPACE, u, MBM_NORMAL);
          MSG::get_space_t& sp = msg.data.get_space;
          bit             += ctrl->last_alloc<<3;
          ctrl->last_alloc = (bit+ubit)>>3;
          ctrl->last_bit   = bit;
          ctrl->i_space   -= ubit;
          u->ev_ptr        = bit  << shift;
          u->space_size    = ubit << shift;
          u->space_add     = u->ev_ptr;
          _mbmsrv_del_wsp(u);
          u->state         = S_active;
          sp.size          = u->space_size;
          sp.offset        = u->space_add;
	  return msg.status=bm->communication.send_response(u->connection,&msg,sizeof(msg));
        }
      }
    }
  }
  return MBM_NORMAL;
}

/// Find matching req
int _mbmsrv_match_req(ServerBMID bm, int partid, int evtype, TriggerMask& trmask, 
                      UserMask& mask0, UserMask& mask1, UserMask* one_masks, int* match_or)  
{
  int i;
  REQ *rq;
  UserMask dummy;
  MBMScanner<USER> que(bm->usDesc, -USER_next_off);
  *match_or = 0;
  for(USER* u=que.get(); u; u=que.get() )  {
    if (((u->partid != 0) && (partid != 0) && (u->partid != partid)) || (u->state == S_pause))  {
      continue;
    }
    for (i = 0, rq = u->req; i < u->n_req; i++, rq++)  {
      if (evtype != rq->ev_type)    {
        continue;
      }
      else if (rq->masktype == BM_MASK_ALL )  {
        if ( trmask != rq->tr_mask )
          continue;
      }
      else if ( !dummy.mask_and(trmask, rq->tr_mask) )
        continue;
      else if (  dummy.mask_and(trmask, rq->vt_mask) )  
        continue;
      // Somehow what comes out is twice the rate. Need to divide rate by 2
      else if ( rq->freq < 100.0 &&
                (float(::rand())/float(RAND_MAX)*100.0) > rq->freq/2.0 )
        continue;

      if ( rq->user_type == BM_REQ_ONE )
        one_masks[rq->user_type_one].set(u->uid);
      else if ( rq->user_type == BM_REQ_VIP )
        mask0.set(u->uid);
      else
        mask1.set(u->uid);

      u->ev_actual++;
      bm->ctrl->tot_actual++;
      *match_or = 1;
      break;
    }
  }
  return MBM_NORMAL;
}

/// Deallocate buffer space
int _mbmsrv_sfree(ServerBMID bm, int add, int size)  {
  CONTROL *ctrl = bm->ctrl;
  int bit   =  add >> ctrl->shift_p_Bit;
  int nbit  = (size + ctrl->bytes_p_Bit) >> ctrl->shift_p_Bit;
  BF_free(bm->bitmap,bit,nbit);
  ctrl->last_alloc = 0;
  ctrl->i_space += nbit;
  if ( bm->usDesc->next )  {
    _mbmsrv_check_wsp(bm);   // check the space wait queue 
  }
  return MBM_NORMAL;
}

/// Free event slot
int _mbmsrv_efree (ServerBMID bm, EVENT* e)  {
  if (e == 0)  {
    return MBM_INTERNAL;
  }
  else if (e->busy == 0)  {
    return MBM_INTERNAL;
  }
  CONTROL* c = bm->ctrl;
  if ( bm->free_event )  {
    void* pars[4];
    pars[0] = bm;
    pars[1] = bm->free_event_param;
    pars[2] = (int*)(e->ev_add+bm->buffer_add);
    pars[3] = e;
    (*bm->free_event)(pars);
  }
  e->busy = 0;
  ::remqent(e);
  c->i_events--;
  /// Check wait event slot queue
  MBMScanner<USER> que(&bm->usDesc->wes_head,-USER_wes_off);
  for(USER* u=que.get(); u; u=que.get() )  {
    if ( u->state == S_weslot )    {
      _mbmsrv_del_wes(u);
      e = _mbmsrv_ealloc(bm);
      // Important: first change producer state!
      u->state = S_active;
      if ( u->ev_dest[0] )  {  // find all destinations 
        int uid = _mbmsrv_findnam(bm,u->ev_dest);
        if ( uid != -1 )    {
          e->umask0.set(uid);
        }
      }
      int match_or = 0;
      _mbmsrv_match_req(bm,u->partid,u->ev_type,u->ev_trmask,e->umask0,e->umask1,e->one_mask,&match_or);
      int rlen   = ((u->ev_size + c->bytes_p_Bit) >> c->shift_p_Bit) << c->shift_p_Bit;
      e->ev_add  = u->space_add;
      e->ev_size = rlen;
      if ( match_or )   {
        e->busy    = 2;            // exclusive use of event
        e->partid  = u->partid;
        e->ev_type = u->ev_type;
        e->tr_mask = u->ev_trmask;
        e->ev_size = u->ev_size;
        e->held_mask.set(u->uid);
      }
      else  {
	++u->free_calls;
        _mbmsrv_del_event(bm,e);             // de-allocate event slot/space
      }
      u->space_add  += rlen;
      u->space_size -= rlen;
      if (u->space_size <= 0)  {
        u->space_add = 0;                    // if size zero, address zero
      }
      c->tot_produced++;
      u->ev_produced++;
      // Send answer to the client
      MSG msg(MSG::DECLARE_EVENT,u,MBM_NORMAL);
      MSG::declare_event_t& d = msg.data.declare_event;
      d.freeAddr = u->space_add;
      d.freeSize = u->space_size;
      return msg.status=bm->communication.send_response(u->connection,&msg,sizeof(msg));
    }
  }
  return MBM_NORMAL;
}

/// Remove event from active event queue
int _mbmsrv_del_event(ServerBMID bm, EVENT* e)  {
  int add = e->ev_add;
  int len = e->ev_size;
  _mbmsrv_efree(bm, e);        // de-allocate event 
  memset(bm->buffer_add+add,0xAB,len);
  _mbmsrv_sfree(bm, add, len);    // de-allocate event slot/space
  return MBM_NORMAL;
}

/// Release event held by this user
int _mbmsrv_rel_event (ServerBMID bm, USER* u)  {
  // Release event if held by user
  if ( u->held_eid != EVTID_NONE )  {
    EVENT *e = bm->event + u->held_eid;
    u->held_eid = EVTID_NONE;
    _mbmsrv_evt_clear(e,u);
    if ( _mbmsrv_evt_held(e) )  {
      return MBM_NORMAL;
    }
    ++u->free_calls;
    return _mbmsrv_del_event(bm, e);
  }
  return MBM_NORMAL;
}

/// Clean-up this user slot
int _mbmsrv_uclean(ServerBMID bm, USER* u)  {
  if (u->state == S_wspace)
    _mbmsrv_del_wsp (u);
  if ( u->state == S_weslot )
    _mbmsrv_del_wes (u);
  if ( u->space_size )   {              // free the held space
    _mbmsrv_sfree(bm, u->space_add, u->space_size);
    u->space_add = 0;
    u->space_size = 0;
  }

  _mbmsrv_rel_event(bm, u);           // free the held event
  if (u->state == S_wevent )
    _mbmsrv_del_wev (u);

  MBMScanner<EVENT> que(bm->evDesc,-EVENT_next_off);
  for(EVENT* e=que.get(); e; e=que.get() )  {
    _mbmsrv_evt_clear(e,u);
    if ( !_mbmsrv_evt_held(e) )   {
      ++u->free_calls;
      _mbmsrv_del_event(bm, e);      // de-allocate event slot/space
    }
  }
  _mbmsrv_ufree(u);                     // de-allocate user slot
  bm->ctrl->i_users--;
  _mbmsrv_evaluate_rules(bm);
  return MBM_NORMAL;
}

// Alloc event slot
EVENT* _mbmsrv_ealloc (ServerBMID bm)  {
  int i = 0;
  static int cnt = 0;
  EVENT *e = bm->event;
  for (CONTROL *c = bm->ctrl; i < c->p_emax; ++i, ++e)  {
    if ( e->busy == 0 )    {
      e->busy = 2;
      e->eid  = i;
      e->count = cnt++;
      e->held_mask.clear();
      e->umask0.clear();
      e->umask1.clear();
      for(size_t i=0; i<BM_MAX_REQONE;++i)
        e->one_mask[i].clear();
      c->i_events++;
      if ( bm->alloc_event )   {
        void* pars[4];
        pars[0] = bm;
        pars[1] = bm->alloc_event_param;
	pars[2] = (int*)(e->ev_add+bm->buffer_add);
        pars[3] = e;
        (*bm->alloc_event)(pars);
      }
      ::insqti(e,bm->evDesc);
      return e;
    }
  }
  return 0;
}

/// Clear events with freqmode = notall
int _mbmsrv_check_freqmode (ServerBMID bm)  {
  int ret = 0;
  MBMScanner<EVENT> que(bm->evDesc, -EVENT_next_off);
  for(EVENT* e=que.get(); e; e = que.get() )  {
    if ( !_mbmsrv_evt_held(e) ) {
      _mbmsrv_del_event(bm,e);   // de-allocate event slot/space
      ++ret;
    }
    else if ( !_mbmsrv_evt_held_vip(e) ) {
      _mbmsrv_del_event(bm,e);   // de-allocate event slot/space
      ++ret;
    }
  }
  return ret;
}

/*
 * Misc. routines
 */

/// Interface routine: unmap memory
int mbmsrv_unmap_memory(BufferMemory* bmid) {
  return _mbmsrv_unmap_sections(bmid);
}

/// Interface routine: map memory necessary for monitoring
int mbmsrv_map_mon_memory(const char* bm_name, BufferMemory* bm)  {
  if ( bm && bm->magic == MBM_USER_MAGIC ) {
    ::strncpy(bm->bm_name,bm_name,sizeof(bm->bm_name));
    bm->bm_name[sizeof(bm->bm_name)-1] = 0;
    int sc = _mbmsrv_map_ctrl_section(bm);
    if ( sc == MBM_NORMAL )  {
      return MBM_NORMAL;
    }
  }
  return MBM_ERROR;
}

/// Interface routine: map memory necessary for monitoring
int mbmsrv_remap_mon_memory(BufferMemory* bm) {
  return _mbmsrv_map_ctrl_section(bm);
}

/// Interface routine: Map all memory sections of the server process
int mbmsrv_map_memory(const char* bm_name, BufferMemory* bm)  {
  if ( bm && bm->magic == MBM_USER_MAGIC ) {
    ::strncpy(bm->bm_name,bm_name,sizeof(bm->bm_name));
    bm->bm_name[sizeof(bm->bm_name)-1] = 0;
    int sc = _mbmsrv_map_ctrl_section(bm);
    if ( sc == MBM_NORMAL )  {
      sc = _mbmsrv_map_buff_section(bm);
      if ( sc == MBM_NORMAL )  {
        return MBM_NORMAL;
      }
    }
  }
  return MBM_ERROR;
}

/// Evaluate consumer rules
int _mbmsrv_evaluate_rules(ServerBMID bm)   {
  CONTROL* c = bm->ctrl;
  bool match = false;
  // Check all rules
  for(size_t j=0; j<BM_MAX_REQS; ++j)  {
    const ServerBMID_t::ConsumerREQ& cr = bm->cons_req[j];
    if ( 0 != cr.name[0] )  {
      USER* u = bm->user;
      const REQ& req = cr.requirement;
      for (int i = 0; i < c->p_umax; ++i, ++u)  {
        if(u->magic == MBM_USER_MAGIC && u->busy == 1 )  {
          match = ::str_match_wild(u->name,cr.name);
          if ( match )  {
            for(int k=0; match && k<u->n_req; ++k)  {
              if ( req.ev_type == u->req[k].ev_type )  {
                if ( u->req[k].tr_mask.mask_or(req.tr_mask) )  {
                  goto Match_Next_requirement;
                }
              }
            }
          }
        }
      }
      // If we get here at least one active requirement was not matched.
      // This means event declaration must be forbidden.
      bm->allow_declare = false;
      return MBM_NORMAL;
    }
  Match_Next_requirement:
    continue;
  }
  bm->allow_declare = true;
  return MBM_NORMAL;
}

/// Match single consumer requirement
int _mbmsrv_match_cons_req(const ServerBMID_t::ConsumerREQ& cr,
                           const char* nam, int partid, int evtype, const unsigned int mask[])   {
  if ( 0 != cr.name[0] )
    if ( partid == cr.requirement.user_type )
      if ( evtype == cr.requirement.ev_type )
        if ( cr.requirement.tr_mask.mask_or(mask) )
          if ( cr.requirement.masktype || ::str_match_wild(nam,cr.name) )
            return MBM_NORMAL;
  return MBM_ILL_REQ;
}

int _mbmsrv_add_req_consumer(ServerBMID bm, const char* name, int partid, int evtype, const unsigned int mask[])   {
  // No similar request found. Insert new one....
  for(size_t i=0; i<BM_MAX_REQS; ++i)  {
    ServerBMID_t::ConsumerREQ& cr = bm->cons_req[i];
    if ( 0 == cr.name[0] )  {
      ::memset(&cr,0,sizeof(ServerBMID_t::ConsumerREQ));
      ::strncpy(cr.name,name,sizeof(cr.name));
      cr.requirement.tr_mask = mask;
      cr.requirement.ev_type = evtype;
      cr.requirement.user_type = partid;
      cr.requirement.masktype = (0 == ::strcmp(name,"*"));
      cr.count = 1;
      return _mbmsrv_evaluate_rules(bm);
    }
  }
  return MBM_ERROR;
}

/// Add consumer requirement to server
int mbmsrv_req_consumer(ServerBMID bm, MSG& msg)   {
  const MSG::cons_requirement_t& rq = msg.data.cons_requirement;
  return mbmsrv_require_consumer(bm,rq.name,rq.partid,rq.evtype,rq.mask);
}

/// Deregister consumer requirement from server
int mbmsrv_unreq_consumer(ServerBMID bm, MSG& msg)   {
  const MSG::cons_requirement_t& rq = msg.data.cons_requirement;
  return mbmsrv_unrequire_consumer(bm,rq.name,rq.partid,rq.evtype,rq.mask);
}

/// Add consumer requirement to server
int mbmsrv_require_consumer(ServerBMID bmid, const char* name, int partid, int evtype, const unsigned int mask[])   {
  ServerBMID bm = CHECK_BMID(bmid);
  LOCK lock(bm->lockid);
  // Check if a similar request is already present.
  // If yes, just increase reference count.
  for(size_t i=0; i<BM_MAX_REQS; ++i)  {
    ServerBMID_t::ConsumerREQ& cr = bm->cons_req[i];
    if ( MBM_NORMAL == _mbmsrv_match_cons_req(cr,name,partid,evtype,mask) )  {
      cr.requirement.tr_mask.apply_or(mask);
      ++cr.count;
      return MBM_NORMAL;
    }
  }
  return _mbmsrv_add_req_consumer(bm,name,partid,evtype,mask);
}

/// Deregister consumer requirement from server
int mbmsrv_unrequire_consumer(ServerBMID bmid, const char* name, int partid, int evtype, const unsigned int mask[])   {
  ServerBMID bm = CHECK_BMID(bmid);
  LOCK lock(bm->lockid);
  // Check if a similar request is already present.
  // If yes, just increase reference count.
  for(size_t i=0; i<BM_MAX_REQS; ++i)  {
    ServerBMID_t::ConsumerREQ& cr = bm->cons_req[i];
    if ( MBM_NORMAL == _mbmsrv_match_cons_req(cr,name,partid,evtype,mask) )  {
      // Cannot change mask, because we do no longer know the value before
      // the last add. If refcount is NULL, reset the slot....
      if ( 0 == --cr.count )   {
        ::memset(&cr,0,sizeof(ServerBMID_t::ConsumerREQ));
      }
      return _mbmsrv_evaluate_rules(bm);
    }
  }
  return MBM_ILL_REQ;
}

int mbmsrv_send_include_error(ServerBMID bm, MSG& msg, int status)   {
  MSG::include_t& inc = msg.data.include;
  MBMConnection connection;
  msg.status = status;
  bm->communication.open(connection,bm->bm_name,inc.name);
  bm->communication.send_response(connection,&msg,sizeof(msg));
  bm->communication.close(connection);
  return MBM_NO_REPLY;
}

/**
 * \function int mbmsrv_include
 * \author Nicolas R.
 *
 * \brief This function is used by the client to fill in
 *   the BMDESCRIPT structure which will contains the data
 *   used by the server to add a new client
 * \return pointer to the new BMDESCRIPT structure
 */
int mbmsrv_include (ServerBMID bmid, void* connection, MSG& msg) {
  static int      tot_clients = 0;
  ServerBMID      bm  = CHECK_BMID(bmid);
  MSG::include_t& inc = msg.data.include;
  const char* bm_name = bm->bm_name;           // buffer manager name 
  const char* name    = inc.name;              // client name
  LOCK lock(bm->lockid);

  USER* u = _mbmsrv_ualloc(bm);                // find free user slot
  if (u == 0)  {
    ::lib_rtl_output(LIB_RTL_OS,"++bm_server++ Failed to allocate user slot of %s for %s.\n",
		     bm_name,name);
    return mbmsrv_send_include_error(bm,msg,MBM_NO_FREE_US); // Typical use case: Too many users
  }
  u->pid           = inc.pid;
  u->partid        = inc.partid;
  u->state         = S_active;
  u->space_add     = 0;
  u->space_size    = 0;
  u->ev_produced   = 0;
  u->ev_actual     = 0;
  u->ev_seen       = 0;
  u->ev_freed      = 0;
  u->held_eid      = EVTID_NONE;
  u->n_req         = 0;
  u->get_ev_calls  = 0;
  u->get_sp_calls  = 0;
  u->free_calls    = 0;
  u->alloc_calls   = 0;
  u->serverid      = (++tot_clients)%bm->ctrl->p_tmax;
  u->connection.init();
  u->connection.any.channel = connection;
  ::strncpy(u->name,name,sizeof(u->name));
  u->name[sizeof(u->name)-1] = 0;
  bm->ctrl->i_users++;
  msg.user     = u;
  inc.serverid = u->serverid;
  if (bm->communication.accept(u->connection,bm->bm_name,name) != MBM_NORMAL )  {
    return mbmsrv_send_include_error(bm,msg,MBM_INTERNAL); // We can try again, but no big hope....
  }
  // Add new client to the poll procedure
  if (bm->communication.poll_add(bm->clients,u->connection) != MBM_NORMAL )  {
    return mbmsrv_send_include_error(bm,msg,MBM_INTERNAL);
  }
  return _mbmsrv_evaluate_rules(bm);
}

/// Reconnect after moving to different worker
int mbmsrv_reconnect (ServerBMID bmid, void* connection, MSG& msg)   {
  ServerBMID bm  = CHECK_BMID(bmid);
  LOCK lock(bm->lockid);
  USER* u = msg.user;
  if ( u )   {
    u->connection.any.channel = connection;
  }
  return MBM_NORMAL;
}

/// Exclude client from buffer manager
int mbmsrv_exclude (ServerBMID bmid, MSG& msg)  {
  ServerBMID bm = CHECK_BMID(bmid);
  LOCK lock(bm->lockid);
  USER* u = msg.user;
  if ( u )   {
    bm->communication.poll_del(bm->clients,u->connection);
    msg.status = bm->communication.send_response(u->connection,&msg,sizeof(msg));
    bm->communication.close(u->connection);
  }
  if ( u->busy == 1 ) _mbmsrv_uclean(bm, u);
  return MBM_NO_REPLY;
}

/*
 * Consumer routines
 */
/// Consumer interface: Free event after processing
int mbmsrv_free_event(ServerBMID bmid, MSG& msg) {
  ServerBMID bm = CHECK_BMID(bmid);
  LOCK lock(bm->lockid);
  USER* u = CHECKED_CONSUMER(msg.user);
  // release event held by him (checked internally)
  return _mbmsrv_rel_event(bm,u);
}

/// Consumer interface: Pause event access
int mbmsrv_pause(ServerBMID bmid, MSG& msg) {
  ServerBMID bm = CHECK_BMID(bmid);
  LOCK lock(bm->lockid);
  USER* u = CHECKED_CONSUMER(msg.user);
  // release event held by him (checked internally)
  _mbmsrv_rel_event(bm, u);  
  u->state = S_pause;
  MBMQueue<EVENT> que(bm->evDesc,-EVENT_next_off);
  for(EVENT* e = que.get(); e; e = que.get())  {
    _mbmsrv_evt_clear(e,u);
    if ( _mbmsrv_evt_held(e) ) _mbmsrv_del_event(bm,e);
  }
  return MBM_NORMAL;
}

/// Get event pending / subscribe to event eventually pending
int _mbmsrv_get_ev(ServerBMID bm, USER* u)  {
  int uid = u->uid;
  MBMScanner<EVENT> que(bm->evDesc, -EVENT_next_off);
  for(EVENT* e = que.get(); e != 0; e = que.get() )  {
    if ( (e->busy != 2) && (e->busy != 0) )  {
      bool req_pending = e->umask0.test(uid) || e->umask1.test(uid);
      for(size_t i=0; i<BM_MAX_REQONE;++i) {
        if ( e->one_mask[i].test(uid) ) {
          req_pending = true;
          break;
        }
      }
      if ( req_pending ) {
        u->ev_ptr    = e->ev_add;
        u->ev_size   = e->ev_size;
        u->ev_type   = e->ev_type;
        u->ev_trmask = e->tr_mask;
        u->held_eid  = e->eid;
        e->held_mask.set(uid);
        e->umask0.clear(uid);
        e->umask1.clear(uid);
        // BM_REQ_ONE consumer: No other from the same group may see this event
        //                      and of course: the same client should not see it twice!
        for(size_t i=0; i<BM_MAX_REQONE; ++i) {
          if ( e->one_mask[i].test(uid) ) {
            e->one_mask[i].clear();
          }
        }
        u->ev_seen++;
        bm->ctrl->tot_seen++;
        return MBM_NORMAL;
      }
    }
  }
  return MBM_NO_EVENT;
}

/// Consumer interface: Get event pending / subscribe to event eventually pending
int mbmsrv_get_event(ServerBMID bm, MSG& msg) {
  LOCK lock(bm->lockid);
  USER* u = CHECKED_CONSUMER(msg.user);
  if ( u->state == S_wevent )    {
    ::lib_rtl_output(LIB_RTL_ERROR,"++bm_server++ Too many calls to mbm_get_event\n");
    return MBM_NO_REPLY;
  }
  _mbmsrv_rel_event(bm, u);
  if (u->state == S_pause)    {
    u->state = S_active;
  }
  u->state    = S_wevent;
  u->held_eid = EVTID_NONE;
  u->get_ev_calls++;
  msg.status =_mbmsrv_get_ev(bm,u);   
  if (msg.status == MBM_NORMAL)    {
    // Now we prepare the correct answer indicating the client that the event is ready
    MSG::get_event_t& evt = msg.data.get_event;
    evt.size   = u->ev_size;
    evt.type   = u->ev_type;
    evt.offset = u->ev_ptr;
    ::memcpy(evt.trmask,&u->ev_trmask,sizeof(evt.trmask));
    u->state = S_active;
    return msg.status;
  }
  // Add user in wait_event queue
  ::insqti(&u->wenext, &bm->usDesc->wev_head);
  return MBM_NO_REPLY;
}

/// Consumer interface: Consumer add request
int mbmsrv_add_req(ServerBMID bm, MSG& msg)   {
  LOCK lock(bm->lockid);
  USER* u = CHECKED_CONSUMER(msg.user);
  MSG::requirement_t& args = msg.data.requirement;
  if (u->n_req == 8)  {
    return MBM_TOO_MANY;
  }
  REQ* rq = u->req + u->n_req;
  ::memset(rq,0,sizeof(REQ));
  rq->ev_type   = args.evtype;
  rq->tr_mask   = args.mask;
  rq->vt_mask   = args.veto;
  rq->masktype  = args.masktype;
  rq->freqmode  = args.freqmode;
  rq->freq      = args.frequency;
  rq->user_type = args.usertype&BM_MASK_REQ;
  _mbmsrv_alloc_reqone_id(bm,rq->user_type,args.usertype&~BM_MASK_REQ,&rq->user_type_one);
  ++u->n_req;
  return _mbmsrv_evaluate_rules(bm);
}

/// Consumer interface: Delete consumer request
int mbmsrv_del_req(ServerBMID bm, MSG& msg)   { 
  LOCK lock(bm->lockid);
  USER* u = CHECKED_CONSUMER(msg.user);
  MSG::requirement_t& args = msg.data.requirement;
  REQ *rq, *rqn;
  int i, j;
  for (i = 0, rq = u->req; i < u->n_req; i++, rq++)  {
    int user_type     = rq->user_type;
    int user_type_one = rq->user_type_one;
    if (args.evtype != rq->ev_type || rq->tr_mask != args.mask || rq->vt_mask != args.veto)
      continue;
    if (args.masktype != rq->masktype || args.usertype != rq->user_type)
      continue;
    for (j = i + 1, rqn = rq + 1; j < u->n_req; j++, rq++, rqn++)      {
      ::memcpy(rq,rqn,sizeof(REQ));
    }
    _mbmsrv_free_reqone_id(bm,user_type,user_type_one);
    --u->n_req;
    //return MBM_NORMAL;
    return _mbmsrv_evaluate_rules(bm);
  }
  //return MBM_NORMAL;
  return _mbmsrv_evaluate_rules(bm);
}

/*
 * Producer routines
 */
/// Producer interface: Try to get space from the buffer manager
int mbmsrv_get_space_try(ServerBMID bm, MSG& msg) {
  LOCK lock(bm->lockid);
  USER*     u    = CHECKED_CLIENT(msg.user);
  MSG::get_space_t& args = msg.data.get_space;
  CONTROL*  ctrl = bm->ctrl;
  if (args.size <= 0 || args.size > ctrl->buff_size)    {
    return MBM_ILL_LEN;
  }
  if ( u->state == S_wspace )    {
    _mbmsrv_del_wsp(u);
  }
  if ( u->space_size )    {
    _mbmsrv_sfree(bm,u->space_add,u->space_size);
    u->space_add  = 0;
    u->space_size = 0;
  }
  u->get_sp_calls++;
  int status = _mbmsrv_get_sp(bm,u,args.size);
  if (status == MBM_NO_ROOM)  {
    if (_mbmsrv_check_freqmode(bm) > 0) {
      status = _mbmsrv_get_sp(bm,u,args.size);
    }
  }
  if (status == MBM_NO_ROOM)  {
    return MBM_NO_ROOM;
  }
  u->ev_size = args.size;
  u->state   = S_wspace_ast_ready;
  return MBM_NORMAL;
}

/// Producer interface: Get space from the buffer manager
int mbmsrv_get_space (ServerBMID bm, MSG& msg) {
  USER* u = CHECKED_CLIENT(msg.user);
  MSG::get_space_t& sp = msg.data.get_space;
  CONTROL*  ctrl = bm->ctrl;
  LOCK lock(bm->lockid);
  if ( sp.size <= 0 || sp.size  > ctrl->buff_size )    {
    return MBM_ILL_LEN;
  }
  if ( u->state == S_wspace )    {
    _mbmsrv_del_wsp(u);
  }
  if ( u->space_size )    {
    _mbmsrv_sfree(bm, u->space_add, u->space_size);
    u->space_add  = 0;
    u->space_size = 0;
  }
  u->get_sp_calls++;
  int status =_mbmsrv_get_sp (bm, u, sp.size);
  if (status == MBM_NO_ROOM)  {
    if (_mbmsrv_check_freqmode(bm) > 0)  {
      status = _mbmsrv_get_sp(bm, u, sp.size);
    }
    if (status == MBM_NO_ROOM)  {
      /// Add user in the wait_space queue
      u->state   = S_wspace;
      u->ev_size = sp.size;
      ::insqti (&u->wsnext, &bm->usDesc->wsp_head);
      return MBM_NO_REPLY;//MBM_NO_ROOM;
    }
  }
  u->ev_size = sp.size;
  u->state   = S_wspace_ast_ready;
  sp.offset  = u->space_add;
  sp.size    = u->space_size;
  return MBM_NORMAL;
}

/// Producer interface: Server side implementation of mbm_declare_event(_try)
int mbmsrv_declare_event(ServerBMID bm, MSG& msg)    {
  USER* u      = CHECKED_CLIENT(msg.user);
  MSG::declare_event_t& evt = msg.data.declare_event;
  int len      = evt.size;
  int evtype   = evt.type;
  CONTROL* c   = bm->ctrl;
  LOCK lock(bm->lockid);

  if ( !bm->allow_declare )   {
    // Testing only: int res = _mbmsrv_evaluate_rules(bm);
    return MBM_NO_CONS;
  }
  else if ( !u->space_size )
    return MBM_NO_EVENT;
  else if ( len <= 0 )
    return MBM_ZERO_LEN;
  else if ( len > u->space_size )
    return MBM_EV_TOO_BIG;

  int dest_uid = -1;
  int  rlen    = ((len + c->bytes_p_Bit) >> c->shift_p_Bit) << c->shift_p_Bit;
  TriggerMask& trmask = *(TriggerMask*)evt.trmask;
  u->ev_dest[0] = 0;
  if ( evt.dest[0] )  {  // find all destinations 
    dest_uid = _mbmsrv_findnam(bm,evt.dest);
  }
  EVENT* e = _mbmsrv_ealloc(bm);
  if ( e == 0 )    {
    if (_mbmsrv_check_freqmode(bm) > 0)  {
      e = _mbmsrv_ealloc(bm);
    }
  }
  if ( e == 0 && !evt.wait )   { // directly return
    _mbmsrv_sfree(bm,u->space_add,u->space_size);
    u->space_add  = 0;
    u->space_size = 0;
    return MBM_NO_ROOM;
  }
  if ( e == 0 && evt.wait )    { // add on the wait event slot queue
    u->ev_type    = evtype;
    u->ev_trmask  = trmask;
    u->ev_size    = len;
    if ( evt.dest[0] )  {
      ::strncpy(u->ev_dest,evt.dest,sizeof(u->ev_dest));
      u->ev_dest[sizeof(u->ev_dest)-1] = 0;
    }
    /// Add user in the wait_event_slot queue
    u->state = S_weslot;
    ::insqti(&u->wesnext,&bm->usDesc->wes_head);
    return MBM_NO_REPLY; //MBM_NO_FREE_SL;
  }
  u->alloc_calls++;
  int match_or = 0;
  if ( dest_uid > 0 ) e->umask0.set(dest_uid);
  _mbmsrv_match_req(bm,u->partid,evtype,trmask,e->umask0,e->umask1,e->one_mask,&match_or);
  e->ev_add  = u->space_add;
  e->ev_size = len;
  if ( match_or )   {
    e->busy    = 2;              // exclusive use of event
    e->partid  = u->partid;
    e->ev_type = evtype;
    e->tr_mask = trmask;
    e->held_mask.set(u->uid);
  }
  else  {
    _mbmsrv_del_event(bm,e);   // de-allocate event slot/space
  }
  u->space_add  += rlen;
  u->space_size -= rlen;
  if (u->space_size <= 0)  {
    u->space_add = 0;                    // if size zero, address zero
  }
  evt.freeAddr   = u->space_add;
  evt.freeSize   = u->space_size;
  ++c->tot_produced;
  ++u->ev_produced;
  return MBM_NORMAL;
}

/// Producer interface: Server implementation of mbm_free_space call
int mbmsrv_free_space(ServerBMID bm, MSG& msg) {
  USER* u = CHECKED_CLIENT(msg.user);
  LOCK lock(bm->lockid);
  if ( u->space_add )    {
    _mbmsrv_sfree(bm, u->space_add, u->space_size);
    u->space_add  = 0;
    u->space_size = 0;
    return MBM_NORMAL;
  }
  return MBM_NO_EVENT;
}

/// Check wait consumer event queue
int _mbmsrv_check_wev (ServerBMID bm, EVENT* e)  {
  int count = 0;
  MBMScanner<USER> que(&bm->usDesc->wev_head, -USER_we_off);
  for(USER* u = que.get(); u != 0; u = que.get(), ++count )  {
    if ( u->state == S_wevent ) {
      int uid = u->uid;
      bool req_pending = e->umask0.test(uid) || e->umask1.test(uid);
      for(size_t i=0; !req_pending && i<BM_MAX_REQONE; ++i) {
        if ( e->one_mask[i].test(uid) ) {
          req_pending = true;
          break;
        }
      }
      if ( req_pending )  {
        u->ev_ptr    = e->ev_add;
        u->ev_size   = e->ev_size;
        u->ev_type = e->ev_type;
        u->ev_trmask = e->tr_mask;
        u->held_eid  = e->eid;
        e->umask0.clear(uid);
        e->umask1.clear(uid);
        e->held_mask.set(uid);
        // BM_REQ_ONE consumer: No other from the same group may see this event
        //                      and of course: the same client should not see it twice!
        for(size_t i=0; i<BM_MAX_REQONE; ++i) {
          if ( e->one_mask[i].test(uid) ) {
            e->one_mask[i].clear();
          }
        }
        u->ev_seen++;
        bm->ctrl->tot_seen++;
        _mbmsrv_del_wev(u);
        // Event is ready: inform the client with the information where to find it.
        MSG msg(MSG::GET_EVENT, u, MBM_NORMAL);
        MSG::get_event_t& evt = msg.data.get_event;
        evt.size   = u->ev_size;
        evt.type   = u->ev_type;
        evt.offset = u->ev_ptr;
        ::memcpy(evt.trmask,&u->ev_trmask,sizeof(evt.trmask));
        u->state = S_active;
	return msg.status=bm->communication.send_response(u->connection,&msg,sizeof(msg));
      }
    }
    if ( count == (bm->ctrl->p_umax+1) )    {
      // Something is really odd here.
      // The BM internal structure looks corrupted....
      ::lib_rtl_output(LIB_RTL_ERROR,
		       "++bm_server++ structures of '%s' look corrupted. Something ugly happened!\n",
		       bm->bm_name);
      return MBM_NORMAL;
    }
  }
  return MBM_NORMAL;
}

/// Check consumer if (some) clients are waiting for events
int _mbmsrv_check_cons(ServerBMID bm, USER* u)  {
  int owner = u->uid;
  if ( owner == -1 )  {
    return MBM_INTERNAL;
  }
  MBMScanner<EVENT> que(bm->evDesc, -EVENT_next_off);
  for(EVENT* e=que.get(); e; e=que.get() )  {
    if (e->busy != 2)     {
      continue;
    }
    if ( e->held_mask.test(owner) )  {
      e->held_mask.clear(owner);
      e->busy = 1;
      _mbmsrv_check_wev(bm, e);  // check wev queue
    }
  }
  return MBM_NORMAL;
}

/// Producer interface: Notify consumers about the presence of an event
int mbmsrv_send_space(ServerBMID bm, MSG& msg) {
  USER* u = CHECKED_CLIENT(msg.user);
  LOCK lock(bm->lockid);
  int sc = _mbmsrv_check_cons(bm,u);
  if ( sc == MBM_NORMAL )   {
    if ( u->space_size )  {
      _mbmsrv_sfree(bm, u->space_add, u->space_size);  /* free space */
    }
    u->space_add = 0;
    u->space_size = 0;
  }
  return sc;
}

/// Consumer/Producer interface: cancel pending request(s)
int mbmsrv_cancel_request(ServerBMID bmid, MSG& msg) {
  ServerBMID bm = CHECK_BMID(bmid);
  LOCK lock(bm->lockid);
  USER* u = CHECKED_CLIENT(msg.user);
  if (u->state == S_wevent || u->state == S_active)    {
    u->state = S_wevent;
    _mbmsrv_del_wev(u);
    u->state = S_active;
  }
  if (u->state == S_wspace)    {
    _mbmsrv_del_wsp(u);
    u->state = S_active;
  }
  if (u->state == S_weslot)    {
    _mbmsrv_del_wes(u);
    u->state = S_active;
  }
  return MBM_NO_REPLY;
}

/// Consumer interface: stop consumer
int mbmsrv_stop_consumer(ServerBMID bm, MSG& msg)   {
  LOCK lock(bm->lockid);
  USER* u = CHECKED_CONSUMER(msg.user);
  u->state = S_wevent;
  _mbmsrv_del_wev(u);
  u->state = S_active;
  return MBM_NORMAL;
}

/// Server interface: Connect server process
int _mbmsrv_connect(ServerBMID bm)    {
  byte_offset(USER,next,   USER_next_off);
  byte_offset(USER,wsnext, USER_ws_off);
  byte_offset(USER,wenext, USER_we_off);
  byte_offset(USER,wesnext,USER_wes_off);
  byte_offset(EVENT,next,  EVENT_next_off);

  bm->num_threads = bm->ctrl->p_tmax;
  bm->clients.init();
  ::memset(bm->server,0,sizeof(bm->server));

  /// Bind all server
  for(int i=0; i<bm->num_threads; ++i) {
    ServerBMID_t::Server& s = bm->server[i];
    if ( MBM_NORMAL != bm->communication.bind(s.connection,bm->bm_name,i) )  {
      _mbmsrv_close_connections(bm);
      return MBM_ERROR;
    }
  }
  /// Create client poll. Add, when clients get accepted
  if ( bm->communication.poll_create(bm->clients,bm->ctrl->p_umax) != MBM_NORMAL )   {
    _mbmsrv_close_connections(bm);
    return MBM_ERROR;
  }
#ifndef MBM_HAVE_STD_MUTEX
  /// Create table lock
  if ( !lib_rtl_is_success(::lib_rtl_create_lock(0, &bm->lockid)) ) {
    ::lib_rtl_output(LIB_RTL_OS,"++bm_server++ Failed create server lock for buffer %s.\n",
		     bm->bm_name);
    _mbmsrv_close_connections(bm);
    return MBM_ERROR;
  }
#endif

  // Now setup exit and rundown handler
  if ( reference_count == 0 )  {
    desc_head  = new qentry_t(0,0);
    ::lib_rtl_declare_exit(_mbmsrv_shutdown, 0);
    ::lib_rtl_declare_rundown(_mbmsrv_shutdown,0);
  }
  ::insqhi(bm, desc_head);
  reference_count++;

  return MBM_NORMAL;
}

/// Server interface: Disconnect server process from clients and shut it down
int mbmsrv_disconnect(ServerBMID bm)    {
#ifndef MBM_HAVE_STD_MUTEX
  // Delete table lock
  if ( bm->lockid ) {
    ::lib_rtl_delete_lock(bm->lockid);
    bm->lockid = 0;
  }
#endif
  // Close server connections
  _mbmsrv_close_connections(bm);
  for (USER* u=bm->user, *last=u+bm->ctrl->p_umax; u != last; ++u)    {
    if ( u->busy )   {
      bm->communication.close(u->connection);
      _mbmsrv_uclean(bm,u);
    }
  }

  // to be seen if this should stay here, since the global sections are created externally
  // in the installer
  if ( bm->gbl        ) ::lib_rtl_delete_section(bm->gbl_add);
  if ( bm->buffer_add ) ::lib_rtl_delete_section(bm->buff_add);

  // Remove entry from global list:
  qentry_t *dummy = ::remqent(bm);
  delete dummy;
  reference_count--;
  if (reference_count == 0)  {
    ::lib_rtl_remove_rundown(_mbmsrv_shutdown,0);
    ::lib_rtl_remove_exit(_mbmsrv_shutdown,0);
    delete desc_head;
    desc_head = 0;
  }
  return MBM_NORMAL;
}

/// Check consumer if (some) clients are waiting for events
int mbmsrv_check_pending_tasks(ServerBMID bm)  {
  int free_slots = 0;
  LOCK lock(bm->lockid);
  MBMScanner<EVENT> que(bm->evDesc, -EVENT_next_off);
  for(EVENT* e=que.get(); e; e=que.get() )  {
    /// Check if any consumer still waiting for an event....
    _mbmsrv_check_wev(bm,e);  // check wev queue
    /// Check if any producer still waiting for a slot....
    if (e->busy == 0)  {
      ++free_slots;
    }
  }
  _mbmsrv_check_wsp(bm);
  return free_slots;
}

/// Generic fix to find dead clients
void mbmsrv_check_clients(ServerBMID bm)   {
  bool client_removed = false;
  //LOCK lock(bm->lockid);
  for (USER* u=bm->user, *last=u+bm->ctrl->p_umax; u != last; ++u)    {
    if ( u->magic == MBM_USER_MAGIC && u->busy && u->uid > 0 && u->connection.hasResponse() ) {
      int ret = ::kill(u->pid,0);
      if ( -1 == ret ) {
	LOCK lock(bm->lockid);
	ret = bm->communication.poll_del(bm->clients,u->connection);
	bm->communication.close(u->connection);
	if ( ret == MBM_NORMAL )  {
	  ::fprintf(stdout,"[ERROR] MBM server removing dead client '%s'.pid:%d\n",u->name,u->pid);
	}
	_mbmsrv_uclean(bm,u);
	client_removed = true;
      }
    }
  }
  if ( client_removed ) {
    LOCK lock(bm->lockid);
    MBMScanner<EVENT> que(bm->evDesc, -EVENT_next_off);
    for(EVENT* e=que.get(); e; e=que.get() )  {
      _mbmsrv_check_wev(bm,e);  // check wev queue
    }
  }
}

/// Generic MBM server action handler routine
int mbmsrv_handle_request(ServerBMID bm, void* connection, MBMMessage& msg)   {
  switch(msg.type) {
  case MSG::INCLUDE:
    mbmsrv_check_clients(bm);
    return msg.status = mbmsrv_include(bm,connection,msg);
  case MSG::EXCLUDE:
    return msg.status = mbmsrv_exclude(bm,msg);
  case MSG::RECONNECT:
    return msg.status = mbmsrv_reconnect(bm,connection,msg);

    // Server command handlers
  case MSG::REQUIRE_CONS:
    return msg.status = mbmsrv_req_consumer(bm,msg);
  case MSG::UNREQUIRE_CONS:
    return msg.status = mbmsrv_unreq_consumer(bm,msg);

    // Consumer routines
  case MSG::ADD_REQUEST:
    return msg.status = mbmsrv_add_req(bm,msg);
  case MSG::DEL_REQUEST:
    return msg.status = mbmsrv_del_req(bm,msg);
  case MSG::GET_EVENT: 
    return msg.status = mbmsrv_get_event(bm,msg);
  case MSG::STOP_CONSUMER:
    return msg.status = mbmsrv_stop_consumer(bm,msg);
  case MSG::PAUSE:
    return msg.status = mbmsrv_pause(bm,msg);
  case MSG::FREE_EVENT:
    return msg.status = mbmsrv_free_event(bm,msg);

    // Producer routines
  case MSG::GET_SPACE_TRY:
    return msg.status = mbmsrv_get_space_try(bm,msg);
  case MSG::GET_SPACE:
    return msg.status = mbmsrv_get_space(bm,msg);
  case MSG::FREE_SPACE:
    return msg.status = mbmsrv_free_space(bm,msg);
  case MSG::SEND_SPACE:
    return msg.status = mbmsrv_send_space(bm,msg);
  case MSG::DECLARE_EVENT:
    return msg.status = mbmsrv_declare_event(bm,msg);
  case MSG::CANCEL_REQUEST:
    return msg.status = mbmsrv_cancel_request(bm,msg);
  default:
    return msg.status = MBM_ERROR;
  }
  return msg.status;
}
 
static int _mbmsrv_client_watch(void* param) {  
  ServerBMID bm = (ServerBMID)param;
  try {
    int events = 0;
    while ( !bm->stop )   {
      int fd = bm->communication.poll(bm->clients, events, 250);
      if ( bm->stop )
	return 0x1;
      else if ( fd > 0 && (events&(EPOLLERR | EPOLLHUP)) != 0 )  {
	::lib_rtl_output(LIB_RTL_ERROR,"++bm_server++ MBM epoll error/hup signal for fd:%d\n",fd);
	mbmsrv_check_clients( bm );
      }
      else  {
	mbmsrv_check_clients( bm );
      }
      //if ( bm->communication.type != BM_COM_FIFO )
      mbmsrv_check_pending_tasks( bm );
    }
    return 0x1;
  }
  catch(const std::exception& e) {
    ::lib_rtl_output(LIB_RTL_ERROR,"++bm_server++ MBM server thread exited with exception: %s\n",e.what());
  }
  catch(...) {
    ::lib_rtl_output(LIB_RTL_ERROR,"++bm_server++ MBM server thread exited with UNKNOWN exception.\n");
  }
  return 0x0;
}

int _mbmsrv_watch_clients(ServerBMID bm)   {
  if ( !bm->client_thread ) {
    int sc = ::lib_rtl_start_thread(_mbmsrv_client_watch,bm,&bm->client_thread);
    if ( lib_rtl_is_success(sc) ) return MBM_NORMAL;
    return MBM_ERROR;
  }
  return MBM_NORMAL;
}

static int mbmsrv_thread_routine(void* param) {  
  std::pair<int,ServerBMID> *p = (std::pair<int,ServerBMID>*)param;
  int        id = p->first;
  ServerBMID bm = p->second;
  delete p;
  try {
    bm->stop = false;
    return bm->communication.dispatch(bm,id);
  }
  catch(const std::exception& e) {
    ::lib_rtl_output(LIB_RTL_ERROR,"++bm_server++ Thread exited with exception: %s\n",e.what());
  }
  catch(...) {
    ::lib_rtl_output(LIB_RTL_ERROR,"++bm_server++ Thread exited with UNKNOWN exception.\n");
  }
  return 0;
}

extern "C" int mbmsrv_dispatch_blocking(ServerBMID bm)  {
  if ( bm && bm->magic == MBM_USER_MAGIC ) {
    bm->stop = false;
    int sc = _mbmsrv_connect(bm);
    if ( sc == MBM_NORMAL )  {
      sc = bm->communication.dispatch(bm,0);
      if ( sc == MBM_NORMAL )  {
        _mbmsrv_watch_clients(bm);
        return MBM_NORMAL;
      }
    }
  }
  return MBM_ERROR;
}

extern "C" int mbmsrv_dispatch_nonblocking(ServerBMID bm)  {
  if ( bm && bm->magic == MBM_USER_MAGIC ) {
    bm->stop = false;
    int sc = _mbmsrv_connect(bm);
    if ( sc == MBM_NORMAL )  {
      for(int i=0; i<bm->num_threads; ++i) {
	void* param = new std::pair<int,ServerBMID>(i,bm);
        sc = ::lib_rtl_start_thread(mbmsrv_thread_routine,param,&bm->server[i].dispatcher);
        if ( !lib_rtl_is_success(sc) ) {
          return MBM_ERROR;
        }
      }
      _mbmsrv_watch_clients(bm);
      return MBM_NORMAL;
    }
  }
  return MBM_ERROR;
}

extern "C" int mbmsrv_wait_dispatch(ServerBMID bm) {
  if ( bm && bm->magic == MBM_USER_MAGIC )    {
    for(int i=0; i<bm->num_threads; ++i) {
      if ( 0 != bm->server[i].dispatcher )  {
        ::lib_rtl_join_thread(bm->server[i].dispatcher);
        bm->server[i].dispatcher = 0;
      }
    }
    if ( 0 != bm->client_thread ) {
      ::lib_rtl_join_thread(bm->client_thread);
      bm->client_thread = 0;
    }
    return MBM_NORMAL;
  }
  return MBM_ERROR;
}

extern "C" int mbmsrv_stop_dispatch(ServerBMID bm) {
  if ( bm && bm->magic == MBM_USER_MAGIC )    {
    bm->stop = true;
    return mbmsrv_wait_dispatch(bm);
  }
  return MBM_ERROR;
}
