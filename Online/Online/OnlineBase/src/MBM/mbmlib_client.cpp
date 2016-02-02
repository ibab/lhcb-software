/*-----------------------------------------------------------------------*/
/*
 *   OS-9 BUFFER MANAGER
 *   TRAP MODULE  (C ROUTINES)
 *
 * Edition History
 *
 *  #   Date     Comments                                              By
 * -- -------- ------------------------------------------------------ ---
 *  0  28/09/88  Initial version                                       PM
 *  1  11/11/88  Released version 1.0                                  PM
 *  2  29/11/88  Multibuffer and spy introduced                        PM
 *  3  15/12/88  Released version 2.0                                  PM
 *  4  14/03/89  Minor corrections                                     PM
 *  5  24/09/91  Allow waiting space and event at the same time        PM
 *  6  06/10/92  Add update request for reformatting tasks          PM/BJ
 * ---
 *  7  ??/12/92  Multi-Buffer Manager calls                            BJ
 *  8  25/03/93  Basic clean-up                                      AMi8
 *  9  10/10/06  Major cleanup, move to C++ and implementation
 *               on linux and WIN32                                   MSF
 * 10  12/12/12  Major reimplementation using a client server         MSF
 *               approach.
 *
 *-----------------------------------------------------------------------*/

#define MBM_IMPLEMENTATION
#include <memory>
#include <cerrno>
#include "MBM/bmmessage.h"
#include "MBM/bmstruct.h"
#include "MBM/bmdef.h"
#include <mutex>

#define MBM_CHECK_CONS(bm) if(bm==0 || bm==MBM_INV_DESC || bm->user==0) {return MBM_ILL_CONS;}
#define MBM_CHECK_BMID(bm) if(bm==0 || bm==MBM_INV_DESC) {return MBM_ERROR;}

#if 0
static inline int mbm_error(const char* fn, int line)  {  
  ::lib_rtl_output(LIB_RTL_ERROR,"MBM Error at %s Line:%d.\n",fn,line);
  return MBM_ERROR;  
}
//#undef  MBM_ERROR
//#define MBM_ERROR mbm_error(__FILE__,__LINE__);
#endif

static qentry_t *desc_head = 0;
static int reference_count = 0;
typedef MBMMessage MSG;
typedef  std::lock_guard<std::mutex> LOCK;

struct BMDESCRIPT : public qentry_t  {
  int              pid;
  int              partID;
  int**            evt_ptr;
  int*             evt_size;
  int*             evt_type;
  unsigned int*    trmask;
  RTL_ast_t        ast_addr;
  void*            ast_param;
  lib_rtl_gbl_t    buff_add;
  char*            buffer_add;
  int              cancelled;
  MBMCommunication communication;
  MBMConnection    connection;
  std::mutex       lock;
  char             connectionName[BM_USER_NAME_LEN+BM_BUFF_NAME_LEN+16];
  char             name[BM_USER_NAME_LEN];     // user name
  char             bm_name[BM_BUFF_NAME_LEN];  // buffer name
  USER*            user;
  BMDESCRIPT() : qentry_t(0,0) {
    pid               = -1;
    partID            = -1;
    user              = 0;
    evt_ptr           = 0;
    evt_size          = 0;
    evt_type          = 0;
    trmask            = 0;
    ast_addr          = 0;
    ast_param         = 0;
    buff_add          = 0;
    buffer_add        = 0;
    name[0]           = 0;
    bm_name[0]        = 0;
    cancelled         = false;
    connection.init();
  }
  ~BMDESCRIPT() {}
};

void _mbm_start_debugger() {
  char text[256];
  ::snprintf(text,sizeof(text),"/usr/bin/gdb --pid %d",::lib_rtl_pid());
  ::system(text);
}

int _mbm_comm_message(BMID bm, int code) {
  MBM_CHECK_BMID(bm);
  MSG msg(code,bm->user);
  LOCK lck (bm->lock);
  return bm->communication.communicate(bm->connection,msg,0);
}

int _mbm_cons_message(BMID bm, int code) {
  MBM_CHECK_CONS(bm);
  MSG msg(code,bm->user);
  LOCK lck (bm->lock);
  return bm->communication.communicate(bm->connection,msg,0);
}

// clean-up this user in all buffers
int _mbm_shutdown (void* /* param */) {
  qentry_t *q, *bmq = desc_head;
  BMID bm, ids[32];
  MSG msg(MSG::EXCLUDE);
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
    for(sc=::remqhi(bmq,&q),cnt=0; cnt<len && lib_rtl_queue_success(sc); sc=::remqhi(bmq,&q)) {
      bm = (BMID)q;
      if ( !(bm == 0 || bm == MBM_INV_DESC) ) {      
        ids[cnt++] = bm;
      }
    }
    for(int i=0; i<cnt; ++i)  {
      bm = ids[i];
      if ( bm->connection.server.request>0 ) {    // Send EXCLUDE message to server 
        msg.user = bm->user;                      // if this is still possible.
        bm->communication.send_request(bm->connection,msg,false); // Otherwise it could also not become worse.
      }
    }
    ::lib_rtl_sleep(200);
    for(int i=0; i<cnt; ++i)  { // Now close communication channels and
      bm = ids[i];              // unmape the global sections.
      bm->communication.close(bm->connection);
      if ( bm->buff_add )  {
        ::lib_rtl_unmap_section(bm->buff_add);
      }
      bm->buff_add = 0;
      ::lib_rtl_output(LIB_RTL_INFO,
		       "[INFO] mbm client %s CRASHED - Shutdown of buffer %s [pid:%d, part:%d]\n",
		       bm->name, bm->bm_name, bm->pid, bm->partID);
    }
  }
  desc_head = 0;
  return MBM_NORMAL;
}

int mbm_clear(BMID bm)  {
  MBM_CHECK_CONS(bm);
  return bm->communication.clear(bm->connection);
}

int mbm_unmap_memory(BMID bm)  {
  std::auto_ptr<BMDESCRIPT> mbm(bm);
  if ( mbm.get() && bm->buffer_add )  {
    ::lib_rtl_unmap_section(bm->buff_add);
    bm->buffer_add = 0;
  }
  return MBM_ERROR;
}

/// Map monitoring shared memory section
BufferMemory* mbm_map_mon_memory(const char* bm_name) {
  std::auto_ptr<BufferMemory> bmid(new BufferMemory);
  if ( mbmsrv_map_mon_memory(bm_name,bmid.get()) == MBM_NORMAL ) {
    return bmid.release();
  }
  return MBM_INV_MEMORY;
}

/// Unmap monitoring shared memory section
int mbm_unmap_mon_memory(BufferMemory* bmid) {
  if ( bmid && bmid != MBM_INV_MEMORY ) {
    std::auto_ptr<BufferMemory> ptr(bmid);
    return mbmsrv_unmap_memory(ptr.get());
  }
  return MBM_ERROR;
}

/// Access control table in monitoring section
CONTROL* mbm_get_control_table(BufferMemory* bmid) {
  if ( bmid && bmid != MBM_INV_MEMORY ) {
    return bmid->ctrl;
  }
  return 0;
}

/// Access user table in monitoring section
USER* mbm_get_user_table(BufferMemory* bmid) {
  if ( bmid && bmid != MBM_INV_MEMORY ) {
    return bmid->user;
  }
  return 0;
}

/// Access event table in monitoring section
EVENT* mbm_get_event_table(BufferMemory* bmid) {
  if ( bmid && bmid != MBM_INV_MEMORY ) {
    return bmid->event;
  }
  return 0;
}

/// Map global buffer information on this machine
int mbm_map_global_buffer_info(lib_rtl_gbl_t* handle, bool create)  {
  return mbmsrv_map_global_buffer_info(handle,create);
}

/// Unmap global buffer information on this machine
int mbm_unmap_global_buffer_info(lib_rtl_gbl_t handle, bool remove) {
  return mbmsrv_unmap_global_buffer_info(handle,remove);
}

/// Include as client in buffer manager
BMID mbm_include(const char* bm_name, const char* name, int partid, int com) {
  int status;
  char text[256];
  std::auto_ptr<BMDESCRIPT> bm(new BMDESCRIPT());
  if ( com == BM_COM_ASIO || 0 != ::getenv("BM_COM_ASIO") )
    _mbm_connections_use_asio(bm->communication);
  else if ( com == BM_COM_UNIX || 0 != ::getenv("BM_COM_UNIX") )
    _mbm_connections_use_unix(bm->communication);
  else
    _mbm_connections_use_fifos(bm->communication);
  ::strncpy(bm->bm_name,bm_name,sizeof(bm->bm_name)-1);
  bm->bm_name[sizeof(bm->bm_name)-1] = 0;
  ::strncpy(bm->name,name,sizeof(bm->name)-1);
  bm->name[sizeof(bm->name)-1] = 0;
  bm->pid    = ::lib_rtl_pid();
  bm->partID = partid;

  if ( bm->communication.open_server(bm->connection, bm->bm_name, bm->name) != MBM_NORMAL )  {
    bm->communication.close(bm->connection);
    return MBM_INV_DESC;
  }

  ::snprintf(text,sizeof(text),"bm_buff_%s",bm_name);  
  status  = ::lib_rtl_map_section(text,0,&bm->buff_add);
  if (!lib_rtl_is_success(status))  {
    ::lib_rtl_output(LIB_RTL_OS,
		     "%s: Error mapping buffer section for MBM buffer %s. Status=%d\n",
		     bm->name,bm->bm_name,status);
    bm->communication.close(bm->connection);
    return MBM_INV_DESC;
  }
  bm->buffer_add = (char*)bm->buff_add->address;

  // Exchange inclusion message with server instance
  MSG msg(MSG::INCLUDE);
  MSG::include_t& inc = msg.data.include;
  ::strncpy(msg.data.include.name,bm->name,sizeof(msg.data.include.name)-1);
  inc.name[sizeof(inc.name)-1] = 0;
  inc.serverid = 0;
  inc.pid = bm->pid;
  inc.partid = bm->partID;
  if ( bm->communication.communicate(bm->connection,msg,0) != MBM_NORMAL ) {
    ::lib_rtl_output(LIB_RTL_OS,
		     "%s: INCLUDE message to server '%s': Failed to communicate\n",
		     bm->name, bm->bm_name);
    bm->communication.close(bm->connection);
    ::lib_rtl_unmap_section(bm->buff_add);
    bm->buffer_add = 0;
    return MBM_INV_DESC;
  }

  // Communication now OK. Let's check if the actual request failed or not:
  if ( msg.status != MBM_NORMAL ) {
    ::lib_rtl_output(LIB_RTL_OS,
		     "%s: Failed to include into MBM buffer %s. Status=%d\n",
		     bm->name,bm->bm_name,msg.status);
    bm->communication.close(bm->connection);
    ::lib_rtl_unmap_section(bm->buff_add);
    bm->buffer_add = 0;
    return MBM_INV_DESC;
  }

  // Switch to "our" server's input connection
  if ( inc.serverid != 0 ) {
    status = bm->communication.move_server(bm->connection,bm->bm_name,msg.user,inc.serverid);
    if ( status != MBM_NORMAL )  {
      bm->communication.close(bm->connection);
      ::lib_rtl_unmap_section(bm->buff_add);
      bm->buffer_add = 0;
      return MBM_INV_DESC;
    }
  }

  // All fine: Store the server cookie for further requests
  bm->user = msg.user;

  // Now setup exit and rundown handler
  if ( reference_count == 0 )  {
    desc_head  = new qentry_t(0,0);
    ::lib_rtl_declare_exit(_mbm_shutdown, 0);
    ::lib_rtl_declare_rundown(_mbm_shutdown,0);
  }
  ::insqhi(bm.get(), desc_head);
  reference_count++;
  return bm.release();
}

/// Exclude client from buffer manager
int mbm_exclude (BMID bm)  {
  MBM_CHECK_CONS(bm);
  MSG msg(MSG::EXCLUDE,bm->user);
  if ( bm->communication.communicate(bm->connection,msg,0) != MBM_NORMAL ) {
    _mbm_printf("communication problem while unlinking the connection");
  }
  if ( bm->communication.close(bm->connection) != MBM_NORMAL ) {
    _mbm_printf("Problem while closing the connections");
  }
  ::lib_rtl_unmap_section(bm->buff_add);
  qentry_t *dummy = ::remqent(bm);
  delete dummy;
  reference_count--;
  if (reference_count == 0)  {
    ::lib_rtl_remove_rundown(_mbm_shutdown,0);
    ::lib_rtl_remove_exit(_mbm_shutdown,0);
    delete desc_head;
    desc_head = 0;
  }
  return MBM_NORMAL;
}

int mbm_cancel_request (BMID bm)   {
  MBM_CHECK_BMID(bm);
  MSG msg(MSG::CANCEL_REQUEST,bm->user);
  bm->cancelled = true;
  return bm->communication.send_request(bm->connection,msg,false);
}

// Consumer routines
int mbm_add_req (BMID bm, int evtype, 
                 const unsigned int* trg_mask, 
                 const unsigned int* veto_mask, 
                 int masktype, int usertype, int freqmode, float freq)
{
  MBM_CHECK_CONS(bm);
  MSG msg(MSG::ADD_REQUEST,bm->user);
  MSG::requirement_t& r = msg.data.requirement;
  LOCK lck (bm->lock);
  r.evtype    = evtype;
  r.masktype  = masktype;
  r.freqmode  = freqmode;
  r.usertype  = usertype;
  r.frequency = freq;
  ::memcpy(r.mask,trg_mask,sizeof(r.mask));
  ::memcpy(r.veto,veto_mask,sizeof(r.veto));
  return bm->communication.communicate(bm->connection,msg,&bm->cancelled);
}

int mbm_del_req (BMID bm, int evtype, 
                 const unsigned int* trmask,
                 const unsigned int* veto, 
                 int masktype, int usertype)
{
  MBM_CHECK_CONS(bm);
  MSG msg(MSG::DEL_REQUEST,bm->user);
  MSG::requirement_t& r = msg.data.requirement;
  r.evtype    = evtype;
  r.masktype  = masktype;
  r.usertype  = usertype;
  r.freqmode  = 0;
  r.frequency = 0;
  ::memcpy(r.mask,trmask,sizeof(r.mask));
  ::memcpy(r.veto,veto,sizeof(r.veto));
  LOCK lck (bm->lock);
  int status = bm->communication.communicate(bm->connection,msg,&bm->cancelled);
  if ( MBM_REQ_CANCEL == status ) {
    bm->cancelled = false;
    return MBM_NORMAL;
  }
  return status;
}

int mbm_get_event_a (BMID bm, int** ptr, int* size, int* evtype, unsigned int* trmask,
                     int /* part_id */, RTL_ast_t astadd, void* astpar) {
  MBM_CHECK_CONS(bm);
  MSG msg(MSG::GET_EVENT,bm->user);
  bm->evt_ptr   = ptr;
  bm->evt_size  = size;
  bm->evt_type  = evtype;
  bm->trmask    = trmask;
  bm->ast_addr  = astadd;
  bm->ast_param = astpar;
  bm->cancelled = false;
  ::memset(trmask,0x0,sizeof(TriggerMask));
  return bm->communication.send_request(bm->connection,msg,true);
}

int mbm_wait_event (BMID bm) {
  MBM_CHECK_CONS(bm);
  MSG msg(0);
  LOCK lck (bm->lock);
  int status = bm->communication.read_response(bm->connection,msg,&bm->cancelled);
  if ( MBM_NORMAL == status ) {
    MSG::get_event_t& evt = msg.data.get_event;
    char* ptr = bm->buffer_add + evt.offset;
    *bm->evt_ptr  = (int*)ptr;
    *bm->evt_size = evt.size;
    *bm->evt_type = evt.type;
    ::memcpy(bm->trmask,evt.trmask,sizeof(evt.trmask));
    if ( bm->ast_addr != 0 ) {
      ::lib_rtl_run_ast(bm->ast_addr, bm->ast_param, 3);
    }
    bm->ast_addr = 0;
    bm->ast_param = 0;
  }
  else if ( MBM_REQ_CANCEL == status ) {
    *bm->evt_ptr  = 0;
    *bm->evt_size = 0;
    *bm->evt_type = 0;
    bm->cancelled = false;
  }
  return status;
}

int mbm_wait_event_a (BMID bm)    {
  return _mbm_cons_message(bm,MSG::WAIT_EVENT);
}

int mbm_get_event (BMID bm,int** ptr,int* size,int* evtype,unsigned int* trmask,int part_id) {
  int sc = mbm_get_event_a(bm,ptr,size,evtype,trmask,part_id,NULL,bm);
  if ( sc == MBM_NORMAL || sc == MBM_NO_EVENT ) {
    return mbm_wait_event(bm);
  }
  return sc;
}

const char* mbm_buffer_address(BMID bm) {
  if ( bm != MBM_INV_DESC && bm->buff_add ) {
    return bm->buffer_add;
  }
  return 0;
}

int mbm_stop_consumer(BMID bm)   {
  return _mbm_comm_message(bm, MSG::STOP_CONSUMER);
}

int mbm_free_event (BMID bm) {
  return _mbm_cons_message(bm,MSG::FREE_EVENT);
}

int mbm_pause (BMID bm)  {
  return _mbm_cons_message(bm,MSG::PAUSE);
}

/*
 * Producer Routines
 */
int mbm_get_space_try(BMID bm, int size, int** ptr, RTL_ast_t astadd, void* astpar)  {
  MBM_CHECK_BMID(bm);
  MSG msg(MSG::GET_SPACE_TRY,bm->user);
  MSG::get_space_t& sp = msg.data.get_space;
  sp.size = size;
  int status = bm->communication.communicate(bm->connection,msg,0);
  if ( status == MBM_NORMAL ) {
    bm->evt_ptr   = ptr;
    bm->ast_addr  = astadd;
    bm->ast_param = astpar;
    *ptr          = (int*)(bm->buffer_add + sp.offset);
  }
  return status;
}

int mbm_get_space_a (BMID bm, int size, int** ptr, RTL_ast_t astadd, void* astpar)  {
  MBM_CHECK_BMID(bm);
  MSG msg(MSG::GET_SPACE,bm->user);
  MSG::get_space_t& sp = msg.data.get_space;
  bm->evt_ptr   = ptr;
  bm->ast_addr  = astadd;
  bm->ast_param = astpar;
  bm->cancelled = false;
  sp.size    = size;
  return bm->communication.send_request(bm->connection, msg, true);
}

int mbm_wait_space(BMID bm)    {
  MBM_CHECK_BMID(bm);
  MSG msg(MBM_ERROR,bm->user);
  int status = bm->communication.read_response(bm->connection,msg,&bm->cancelled);
  if ( status == MBM_NORMAL )  {
    if ( msg.type != MSG::GET_SPACE ) {
      ::lib_rtl_output(LIB_RTL_FATAL,"MBM Error: Got message of type:%s instead of expected GET_SPACE\n",
                       msg.typeStr(msg.type));
    }
    MSG::get_space_t& sp = msg.data.get_space;
    char* ptr = bm->buffer_add + sp.offset;
    *bm->evt_ptr  = (int*)ptr;
    ::lib_rtl_run_ast(bm->ast_addr, bm->ast_param, 3);
    bm->ast_addr  = 0;
    bm->ast_param = 0;
  }
  else if ( MBM_REQ_CANCEL == status )  {
    *bm->evt_ptr  = 0;
    bm->cancelled = false;
  }
  else {
    ::lib_rtl_output(LIB_RTL_FATAL,"MBM Error: Got message with bad status:%d.\n",msg.status);
  }
  return status;
}

static int _mbm_declare_event (BMID bm, int len, int evtype, const unsigned int* trmask,
                               const char* dst, void** free_add, int* free_size, int /* part_id */, int wait)
{
  MBM_CHECK_BMID(bm);
  MSG msg(MSG::DECLARE_EVENT,bm->user);
  MSG::declare_event_t& evt = msg.data.declare_event;
  evt.size = len;
  evt.wait = wait;
  evt.type = evtype;
  ::strncpy(evt.dest,dst ? dst : "", sizeof(evt.dest)-1);
  ::memcpy(evt.trmask,trmask,sizeof(evt.trmask));
  *free_add = 0;
  *free_size = 0;
  int status = bm->communication.send_request(bm->connection, msg, false);
  if ( status == MBM_NORMAL )   {
    if ( bm->cancelled )  {
      *free_add  = 0;
      *free_size = 0;
      bm->cancelled = false;
      return MBM_REQ_CANCEL;
    }
    status = bm->communication.read_response(bm->connection,msg,&bm->cancelled);
    if ( status == MBM_NORMAL ) {
      *free_add  = (bm->buffer_add + evt.freeAddr);
      *free_size = evt.freeSize;
      if ( msg.type != MSG::DECLARE_EVENT ) {
        ::lib_rtl_output(LIB_RTL_FATAL,
			 "MBM: Got message of type:%s instead of expected DECLARE_EVENT\n",
                         msg.typeStr(msg.type));
      }
    }
    else if ( MBM_REQ_CANCEL == status )  {
      *free_add  = 0;
      *free_size = 0;
      bm->cancelled = false;
    }
    else {
      ::lib_rtl_output(LIB_RTL_FATAL,
		       "MBM: _mbm_declare_event got message with bad status:%d.\n",
		       msg.status);
    }
  }
  return status;
}

int mbm_declare_event(BMID bm, int len, int evtype, const unsigned int* trmask,
                      const char* dst, void** free_add, int* free_size, int part_id)
{
  return _mbm_declare_event(bm,len,evtype,trmask,dst,free_add,free_size,part_id,1);
}

int mbm_declare_event_try(BMID bm, int len, int evtype, const unsigned int* trmask,
                          const char* dst, void** free_add, int* free_size, int part_id)
{
  return _mbm_declare_event(bm,len,evtype,trmask,dst,free_add,free_size,part_id,0);
}

int mbm_declare_event_and_send(BMID bm, int len, int evtype, const unsigned int* trmask,
                               const char* dst, void** free_add, int* free_size, int part_id)
{
  return _mbm_declare_event(bm,len,evtype,trmask,dst,free_add,free_size,part_id,-1);
#if 0
  int sc = _mbm_declare_event(bm,len,evtype,trmask,dst,free_add,free_size,part_id,1);
  if( MBM_NORMAL == sc )  {
    sc = mbm_send_space(bm);
    if (lib_rtl_is_success(sc))      {
      *free_add  = 0;
      *free_size = 0;
    }
  }
  return sc;
#endif
}

int mbm_free_space (BMID bm)   {
  return _mbm_comm_message(bm, MSG::FREE_SPACE);
}

int mbm_send_space (BMID bm)    {
  MBM_CHECK_BMID(bm);
  MSG msg(MSG::SEND_SPACE,bm->user);
  int sc = bm->communication.communicate(bm->connection,msg,0);
  if ( msg.type != MSG::SEND_SPACE ) {
    ::lib_rtl_output(LIB_RTL_FATAL,
		     "MBM: Got message of type:%s instead of expected SEND_SPACE\n",
                     msg.typeStr(msg.type));
  }
  return sc;
}

int mbm_wait_space_a(BMID bm)    {
  return _mbm_comm_message(bm, MSG::WAIT_SPACE_A);
}

// Statistics routines
int mbm_events_actual (BMID bm, int *events)   {
  MBM_CHECK_BMID(bm);
  MSG msg(MSG::STAT_EVENTS_ACTUAL,bm->user);
  if ( bm->communication.communicate(bm->connection,msg,0) == MBM_NORMAL ) {
    *events = msg.data.statistics;
    return MBM_NORMAL;
  }
  return MBM_ERROR;
}

int mbm_events_produced (BMID bm, int *events)   {
  MBM_CHECK_BMID(bm);
  MSG msg(MSG::STAT_EVENTS_PRODUCED,bm->user);
  if ( bm->communication.communicate(bm->connection,msg,0) == MBM_NORMAL ) {
    *events = msg.data.statistics;
    return MBM_NORMAL;
  }
  return MBM_ERROR;
}

int mbm_events_seen (BMID bm, int *events)   {
  MBM_CHECK_BMID(bm);
  MSG msg(MSG::STAT_EVENTS_SEEN,bm->user);
  if ( bm->communication.communicate(bm->connection,msg,0) == MBM_NORMAL ) {
    *events = msg.data.statistics;
    return MBM_NORMAL;
  }
  return MBM_ERROR;
}

int mbm_stop_producer(BMID bm)   {
  return _mbm_comm_message(bm, MSG::STOP_PRODUCER);
}

int mbm_reset_statistics (BMID bm)   {
  return _mbm_comm_message(bm, MSG::STAT_RESET);
}

int mbm_grant_update (BMID bm)   {
  return _mbm_comm_message(bm, MSG::GRANT_UPDATE);
}

int mbm_min_alloc(BMID bm, int* size) {
  MBM_CHECK_BMID(bm);
  MSG msg(MSG::STAT_MIN_ALLOC,bm->user);
  if( bm->communication.communicate(bm->connection,msg,0) == MBM_NORMAL ) {
    *size = msg.data.statistics;
    return MBM_NORMAL;
  }
  return MBM_ERROR;
}

int mbm_buffer_size(BMID bm, int* size) {
  MBM_CHECK_BMID(bm);
  MSG msg(MSG::STAT_BUFFER_SIZE,bm->user);
  if( bm->communication.communicate(bm->connection,msg,0) ) {
    *size = msg.data.statistics;
    return MBM_NORMAL;
  }
  return MBM_ERROR;
}

int mbm_events_in_buffer (BMID bm, int* events)  {
  MBM_CHECK_BMID(bm);
  MSG msg(MSG::STAT_EVENTS_IN_BUFFER,bm->user);
  if( bm->communication.communicate(bm->connection,msg,0) == MBM_NORMAL ) {
    *events = msg.data.statistics;
    return MBM_NORMAL;
  }
  return MBM_ERROR;
}

int mbm_space_in_buffer (BMID bm, int* total, int* large)  {
  MBM_CHECK_BMID(bm);
  MSG msg(MSG::STAT_SPACE_IN_BUFFER,bm->user);
  if( bm->communication.communicate(bm->connection,msg,0) == MBM_NORMAL ) {
    *total = msg.data.space_in_buffer.total;
    *large = msg.data.space_in_buffer.large;
    return MBM_NORMAL;
  }
  return MBM_ERROR;
}

int mbm_process_exists(BMID bm, const char* name, int* exists)  {
  MBM_CHECK_BMID(bm);
  MSG msg(MSG::PROCESS_EXISTS,bm->user);
  ::strncpy(msg.data.process_exists.name,name,sizeof(msg.data.process_exists.name)-1);
  msg.data.process_exists.name[sizeof(msg.data.process_exists.name)-1] = 0;
  if( bm->communication.communicate(bm->connection,msg,0) == MBM_NORMAL ) {
    *exists = msg.data.statistics;
    return MBM_NORMAL;
  }
  return MBM_ERROR;
}
