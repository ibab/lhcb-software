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
*/
/*-----------------------------------------------------------------------*/
#define MBM_IMPLEMENTATION
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <cerrno>
#include <memory>
#include "bm_struct.h"
#include "bm_internals.h"
#include "RTL/DoubleLinkedQueue.h"
#define MBM_MAX_BUFF  32

#define _mbm_return_err(err_code)  {  errno = err_code; return err_code; }

static inline int mbm_error(const char* fn, int line)  {  
  ::lib_rtl_printf("MBM Error:Bad!!!! %s Line:%d\n",fn,line);
  return MBM_ERROR;  
}
#undef MBM_ERROR
#define  MBM_ERROR mbm_error(__FILE__,__LINE__);
#define  MBMQueue RTL::DoubleLinkedQueue
using namespace MBM;

static qentry_t *desc_head;
static int reference_count = 0;

static int USER_next_off;
static int USER_ws_off;
static int USER_we_off;
static int USER_wes_off;
static int EVENT_next_off;
static int USER_active_off;
static int CONTROL_cwe_off;
static int CONTROL_pws_off;
static int CONTROL_wes_off;
static int CONTROL_ev_off;
static RTL_ast_t _mbm_wes_ast_add = 0;
static int disable_rundown=0;

#ifndef MBM_PRINT
inline int _mbm_printf(const char* , ...)  {  return 1;   }
#else
inline int _mbm_printf(const char* fmt, ...)  {
  char buff[1024];
  va_list args;
  va_start(args, fmt);
  int len = vsnprintf(buff, sizeof(buff), fmt, args);
  ::lib_rtl_printf(buff);
  return len;
}
#endif
#ifndef MBM_CHECK_MEMORY
#define _mbm_reset_memory(ctrl,q,c,s)
#define _mbm_check_memory(q,c,s)
#else
static void _mbm_reset_memory(CONTROL* ctrl, void* q,int c, size_t s)  {
  size_t i;
  int *p = (int*)q;
  size_t siz = ((s + ctrl->bytes_p_Bit) >> ctrl->shift_p_Bit) << ctrl->shift_p_Bit;
  _mbm_printf("Reset %d bytes from %p to %p to %X\n", siz, p, ((char*)p)+siz, c);
  for(i=0; i<siz/sizeof(int); ++i)  {
    p[i] = c;
  }
}
void _mbm_check_memory(void* q,int c, size_t s)  {
  size_t i;
  int bad = 0, *p = (int*)q;
  for(i=0; i<s/sizeof(int); ++i)  {
    if ( p[i] != c )  {
      ::lib_rtl_printf("Bad memory:%08X[%d]=%08X %08X\n",p,i,&p[i],p[i]);
      bad++;
    }
  }
  if ( bad )  {
    lib_rtl_printf("Checked %d Bytes from %p to %p found %d bad words.\n",s, q, &p[i-1], bad);
  }
  else  {
    _mbm_printf("Checked %d Bytes from %p to %p found %d bad words.\n",s, q, &p[i-1], bad);
  }
}
#endif
#ifndef MBM_CHECK_LOCKS
#define _mbm_instrument_lock(x)
#define _mbm_deinstrument_lock(x)
#else
void _mbm_instrument_lock(BMID bm)  {
#ifdef __linux
    int val;
    lib_rtl_lock_value(bm->lockid,&val);
    if ( val != 0 ) {
      ::lib_rtl_printf("%5d LOCK: Seamphore value:%d\n",lib_rtl_pid(), val);
\    }
#endif
    if ( m_bm->_control()->spare1 != 0 )  {
      lib_rtl_signal_message(LIB_RTL_OS,"%5d: LOCK: Lock error on BM tables:%d",lib_rtl_pid(),
           bm->_control()->spare1);
    }
    bm->_control()->spare1 = lib_rtl_pid();
}
void _mbm_deinstrument_lock(BMID bm)  {
    if ( bm->_control()->spare1 != lib_rtl_pid() )  {
      lib_rtl_signal_message(LIB_RTL_OS,"%5d: UNLOCK: Lock error on BM tables:%d",lib_rtl_pid(),
           bm->_control()->spare1);
    }
    bm->_control()->spare1 = 0;
#ifdef __linux
    int val;
    lib_rtl_lock_value(bm->lockid,&val);
    if ( val != 0 ) {
      ::lib_rtl_printf("%5d UNLOCK: Seamphore value:%d\n",lib_rtl_pid(), val);
    }
#endif
}
#endif

#ifdef _WIN32
void _mbm_update_rusage(USER* us) {
}
#else
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
void _mbm_update_rusage(USER* us) {
  rusage ru;
  getrusage(RUSAGE_SELF, &ru);
  us->utime = float(ru.ru_utime.tv_sec) + float(ru.ru_utime.tv_usec)/1e6;
  us->stime = float(ru.ru_stime.tv_sec) + float(ru.ru_stime.tv_usec)/1e6;
}
#endif

template <class T> void print_queue(const char* format,const T* ptr, int type)  {
  switch (type)  {
  case 0:
    lib_rtl_printf(format,*ptr,*ptr,add_ptr(ptr,*ptr));
    break;
  case 1:
    lib_rtl_printf(format,*ptr,*ptr,add_ptr(ptr,(*ptr-4)));
    break;
  }
}

class Lock  {
  BMID m_bm;
  int m_status;
public:
  Lock(BMID bm) : m_bm(bm) {
    m_status = _mbm_lock_tables(m_bm);
    if ( !(m_status&1) ) {
      lib_rtl_signal_message(LIB_RTL_OS,"%5d: LOCK: System Lock error on BM tables.",lib_rtl_pid());
    }
    _mbm_instrument_lock(m_bm);
  }
  virtual ~Lock() {
    _mbm_deinstrument_lock(m_bm);
    m_status = _mbm_unlock_tables(m_bm);
    if ( !(m_status&1) )  {
      lib_rtl_signal_message(LIB_RTL_OS,"%5d: UNLOCK: System Lock error on BM tables\n",lib_rtl_pid());
    }
  }
  operator int ()   {
    return lib_rtl_is_success(m_status);
  }
  int status()      const   {
    return m_status;
  }
};

class UserLock  {
  Lock  m_lock;
  USER* m_user;
  int m_status;
public:
  UserLock(BMID bm, int def=MBM_ILL_CONS) : m_lock(bm), m_user(0) {
    if ( m_lock ) {
      m_user = bm->_user();
      m_status = m_user ? MBM_NORMAL : (errno = def);
    }
    m_status = m_lock.status();
  }
  virtual ~UserLock() {
  }
  USER* user()      const   {
    return m_user;
  }
  int status()  const  {
    return m_status;
  }
};

BMDESCRIPT::BMDESCRIPT() : qentry_t(0,0) {
  ctrl = 0;
  user = 0;
  usDesc = 0;
  event = 0;
  evDesc = 0;
  bitmap = 0;
  bitmap_size = 0;
  buffer_add = 0;
  buffer_size = 0;
  owner = 0;
  lockid = 0;
  bm_name[0] = 0;
  mutexName[0] = 0;
  free_event = 0;
  free_event_param = 0;
  alloc_event = 0;
  alloc_event_param = 0;
  ctrl_add = 0;
  event_add = 0;
  user_add = 0;
  bitm_add = 0;
  buff_add = 0;
  WES_event_flag = 0;
  WEV_event_flag = 0;
  WSP_event_flag = 0;
  WEVA_event_flag = 0;
  WSPA_event_flag = 0;
  pThread = 0;
  cThread = 0;
  pThreadState = 0;
  cThreadState = 0;
  lastVar = 0;
}

BMID mbm_map_memory(const char* bm_name)  {
  std::auto_ptr<BMDESCRIPT> bm(new BMDESCRIPT());
  ::strncpy(bm->bm_name,bm_name,sizeof(bm->bm_name));
  bm->bm_name[sizeof(bm->bm_name)-1] = 0;
  bm->owner = -1;
  int sc = _mbm_map_sections(bm.get());
  if ( sc == MBM_NORMAL )  {
    return bm.release();
  }
  return 0;
}

int mbm_unmap_memory(BMID bm)  {
  std::auto_ptr<BMDESCRIPT> mbm(bm);
  if ( mbm.get() )  {
    return _mbm_unmap_sections(mbm.get());
  }
  return MBM_ERROR;
}

BMID mbm_include (const char* bm_name, const char* name, int partid) {
  int status;
  _mbm_fill_offsets();
  _mbm_wes_ast_add  = _mbm_wes_ast;
  std::auto_ptr<BMDESCRIPT> bm(mbm_map_memory(bm_name));
  if ( !bm.get() )  {
    lib_rtl_signal_message(LIB_RTL_OS,"Cannot map memory sections for %s.",
            bm_name);
    return MBM_INV_DESC;
  }

  if ( !bm->lockid )  {
    ::strcpy(bm->mutexName,"BM_");
    ::strcat(bm->mutexName,bm->bm_name);
    status = lib_rtl_create_lock(bm->mutexName, &bm->lockid);
    if (!lib_rtl_is_success(status))    {
      lib_rtl_signal_message(LIB_RTL_OS,
              "Failed to create lock %s for %s.",
              bm->mutexName,bm_name);
      _mbm_unmap_sections(bm.get());
      return MBM_INV_DESC;
    }
  }
  status = _mbm_lock_tables(bm.get());
  if (!lib_rtl_is_success(status))  {
    lib_rtl_signal_message(LIB_RTL_OS,"Failed to lock tables for %s.",bm_name);
    _mbm_unmap_sections(bm.get());
    _mbm_unlock_tables(bm.get());
    return MBM_INV_DESC;
  }
  USER* us = _mbm_ualloc (bm.get());  // find free user slot
  if (us == 0)  {
    _mbm_unlock_tables(bm.get());
    errno = MBM_NO_FREE_US;
    return MBM_INV_DESC;
  }
  bm->pThread       = 0;
  bm->cThread       = 0;
  bm->pThreadState  = 0;
  bm->cThreadState  = 0;
  bm->owner = us->uid;
  us->c_state = S_active;
  us->p_state = S_active;
  us->partid  = partid;
  ::strncpy (us->name, name, sizeof(us->name));
  us->name[sizeof(us->name)-1] = 0;
  us->pid = ::lib_rtl_pid ();
  us->space_add     = 0;
  us->space_size    = 0;
  us->ev_produced   = 0;
  us->ev_actual     = 0;
  us->ev_seen       = 0;
  us->ev_freed      = 0;
  us->held_eid      = EVTID_NONE;
  us->n_req         = 0;
  us->get_ev_calls  = 0;
  us->get_sp_calls  = 0;
  us->get_wakeups   = 0;
  us->sleep_calls   = 0;
  us->get_asts_run  = 0;  
  us->free_calls    = 0;
  us->alloc_calls   = 0;
  bm->ctrl->i_users++;

  // Activate this user
#ifdef _WIN32
  ::sprintf(us->wes_flag, "bm_%s_WES_%d_%d", bm_name, us->uid, us->pid);
  ::sprintf(us->wev_flag, "bm_%s_WEV_%d_%d", bm_name, us->uid, us->pid);
  ::sprintf(us->wsp_flag, "bm_%s_WSP_%d_%d", bm_name, us->uid, us->pid);
  lib_rtl_create_event(us->wes_flag, &bm->WES_event_flag);
  lib_rtl_create_event(us->wev_flag, &bm->WEV_event_flag);
  lib_rtl_create_event(us->wsp_flag, &bm->WSP_event_flag);
#else
  lib_rtl_create_event2(&us->wes_handle, &bm->WES_event_flag);
  lib_rtl_create_event2(&us->wev_handle, &bm->WEV_event_flag);
  lib_rtl_create_event2(&us->wsp_handle, &bm->WSP_event_flag);
#endif
  lib_rtl_create_event(0, &bm->WSPA_event_flag);
  lib_rtl_create_event(0, &bm->WEVA_event_flag);

  if ( reference_count == 0 )  {
    desc_head  = new qentry_t(0,0);
    lib_rtl_declare_exit (_mbm_shutdown, 0);
    lib_rtl_declare_rundown(_mbm_shutdown,0);
  }
  insqhi (bm.get(), desc_head);
  reference_count++;
  errno = 0;
  _mbm_unlock_tables(bm.get());
  _mbm_printf("MBM: %s is now included in %s\n",name, bm_name);
  return bm.release();
}

// Exclude from buffer manager
int mbm_exclude (BMID bm)  {
  typedef std::map<long long int,lib_rtl_event_t> event_map;
  event_map::iterator i;
  int owner = bm->owner;
  if (owner == -1)    {
    _mbm_return_err (MBM_ILL_CONS);
  }
  for(i=bm->us_wev_flags.begin();i!=bm->us_wev_flags.end();++i)
    ::lib_rtl_delete_event((*i).second);
  bm->us_wev_flags.clear();
  for(i=bm->us_wes_flags.begin();i!=bm->us_wes_flags.end();++i)
    ::lib_rtl_delete_event((*i).second);
  bm->us_wes_flags.clear();
  for(i=bm->us_wsp_flags.begin();i!=bm->us_wsp_flags.end();++i)
    ::lib_rtl_delete_event((*i).second);
  bm->us_wsp_flags.clear();
  int sc = _mbm_lock_tables(bm);
  if ( 1==sc )  {
    lib_rtl_delete_event(bm->WES_event_flag);
    lib_rtl_delete_event(bm->WEV_event_flag);
    lib_rtl_delete_event(bm->WSP_event_flag);
    lib_rtl_delete_event(bm->WSPA_event_flag);
    lib_rtl_delete_event(bm->WEVA_event_flag);
    _mbm_uclean(bm);
    _mbm_unmap_sections(bm);
    _mbm_unlock_tables(bm);
  }
  else {
    return sc;
  }
  qentry_t *dummy = remqent(bm);
  //_mbm_delete_lock(bm);

  delete dummy;
  reference_count--;
  if (reference_count == 0)  {
    lib_rtl_remove_rundown(_mbm_shutdown,0);
    lib_rtl_remove_exit(_mbm_shutdown,0);
    delete desc_head;
    desc_head = 0;
  }
  return MBM_NORMAL;
}

// Register optional callback on mbm_free_event
int mbm_register_free_event(BMID bm, RTL_ast_t astadd, void* astparam)   {
  USER* user = bm->_user();
  if ( user )  {
    bm->free_event       = astadd;
    bm->free_event_param = astparam;
    return MBM_NORMAL;
  }
  return MBM_ILL_CONS;
}

// Register optional callback on _mbm_ealloc
int mbm_register_alloc_event(BMID bm, RTL_ast_t astadd, void* astparam)   {
  USER* user = bm->_user();
  if ( user )  {
    bm->alloc_event       = astadd;
    bm->alloc_event_param = astparam;
    return MBM_NORMAL;
  }
  return MBM_ILL_CONS;
}

// Consumer routines
int mbm_add_req (BMID bm, int evtype, 
                 const unsigned int trg_mask[BM_MASK_SIZE], 
                 const unsigned int veto_mask[BM_MASK_SIZE], 
                 int masktype, int usertype, int freqmode, float freq)
{
  UserLock user(bm);
  USER* us = user.user();
  if ( us )  {
    TriggerMask* trmask = (TriggerMask*)trg_mask;
    VetoMask*    veto   = (VetoMask*)veto_mask;
    if (us->n_req == 8)  {
      _mbm_return_err (MBM_TOO_MANY);
    }
    REQ* rq = us->req + us->n_req;
    ::memset(rq,0,sizeof(REQ));
    rq->ev_type   = evtype;
    rq->tr_mask   = *trmask;
    rq->vt_mask   = *veto;
    rq->masktype  = masktype;
    rq->freqmode  = freqmode;
    rq->user_type = usertype;
    rq->freq      = freq;
    ++us->n_req;
  }
  return user.status();
}

int mbm_del_req (BMID bm, int evtype, 
                 const unsigned int trmask[BM_MASK_SIZE],
                 const unsigned int veto[BM_MASK_SIZE], 
                 int masktype, int usertype)
{
  UserLock user(bm);
  USER* us = user.user();
  if ( us )  {
    REQ *rq, *rqn;
    int i, j;
    for (i = 0, rq = us->req; i < us->n_req; i++, rq++)  {
      if ( evtype != rq->ev_type || rq->tr_mask != trmask || rq->vt_mask != veto )
        continue;
      if ( masktype != rq->masktype || usertype != rq->user_type )
        continue;
      for (j = i + 1, rqn = rq + 1; j < us->n_req; j++, rq++, rqn++)      {
        ::memcpy(rq,rqn,sizeof(REQ));
      }
      --us->n_req;
      return MBM_NORMAL;
    }
  }
  return user.status();
}

int mbm_get_event_a (BMID bm, int** ptr, int* size, int* evtype, 
                     unsigned int trmask[BM_MASK_SIZE], 
                     int part_id, RTL_ast_t astadd, void* astpar) {
  UserLock user(bm);
  USER* us = user.user();
  if ( us )  {
    TriggerMask* mask = (TriggerMask*)trmask;
    if (us->c_state == S_wevent)    {
      lib_rtl_signal_message(0,"Too many calls to mbm_get_event_a");
      return MBM_NORMAL;
    }
    if (us->held_eid != EVTID_NONE)    {
      _mbm_rel_event(bm, us);
    }
    if (us->c_state == S_pause)    {
      us->c_state = S_active;
    }
    _mbm_update_rusage(us);
    us->get_ev_calls++;
    int status = _mbm_get_ev (bm, us);
    if (status == MBM_NORMAL)    {
      us->c_partid      = part_id;
      us->we_ptr_add    = ptr;
      us->we_size_add   = size;
      us->we_evtype_add = evtype;
      us->we_trmask_add = mask;
      us->c_state       = S_wevent_ast_queued;
      us->c_astadd      = astadd;
      us->c_astpar      = astpar;
      us->reason        = BM_K_INT_EVENT;
      us->get_wakeups++;
      lib_rtl_set_event(bm->WEV_event_flag);
      return MBM_NORMAL;
    }
    // add wait event queue
    _mbm_add_wev (bm, us, ptr, size, evtype, mask, part_id, astadd, astpar);
    return MBM_NO_EVENT;
  }
  return user.status();
}

int mbm_get_event(BMID bm, int** ptr, int* size, int* evtype, 
                  unsigned int trmask[BM_MASK_SIZE], int part_id) {
  int sc = mbm_get_event_a(bm, ptr,size,evtype,trmask,part_id,mbm_get_event_ast,bm);
  if ( sc == MBM_NORMAL || sc == MBM_NO_EVENT ) {
    return mbm_wait_event(bm);
  }
  return sc;
}

int mbm_free_event (BMID bm) {
  UserLock user(bm, MBM_INTERNAL);
  USER* us = user.user();
  if ( us )  {
    if (us->held_eid == EVTID_NONE)    {
      _mbm_return_err (MBM_NO_EVENT);
    }
    _mbm_rel_event (bm, us);  /* release event held by him  */
  }
  return user.status();
}

int mbm_pause (BMID bm)  {
  UserLock user(bm);
  USER* us = user.user();
  if ( us )  {
    if (us->held_eid != EVTID_NONE)  {
      _mbm_rel_event (bm, us);
    }
    us->c_state = S_pause;
    MBMQueue<EVENT> que(bm->evDesc, -EVENT_next_off);
    for(EVENT* e = que.get(); e; e = que.get())  {
      e->isValid();
      if (e->umask0.test(us->uid) || e->umask1.test(us->uid) || e->umask2.test(us->uid) )    {
        e->umask0.clear(us->uid);
        e->umask1.clear(us->uid);
        e->umask2.clear(us->uid);
        if ( !e->umask0.mask_or(e->umask1,e->umask2) )    {  // no more consumers
          _mbm_del_event(bm, e, e->ev_size);
        }
      }
    }
  }
  return user.status();
}

/*
Producer Routines
*/
int mbm_get_space_a (BMID bm, int size, int** ptr, RTL_ast_t astadd, void* astpar)  {
  UserLock user(bm);
  USER* us = user.user();
  if ( us )  {
    CONTROL* ctrl = bm->_control();
    if (size <= 0 || size > ctrl->buff_size)    {
      _mbm_return_err (MBM_ILL_LEN);
    }
    if ( us->p_state == S_wspace )    {
      _mbm_del_wsp (bm, us);
    }
    if ( us->space_size )    {
      _mbm_printf("mbm_get_space_a> Free event space:%08X\n",us->space_add);
      _mbm_sfree (bm, us->space_add, us->space_size);
      us->space_add = 0;
      us->space_size = 0;
    }
    _mbm_update_rusage(us);
    us->get_sp_calls++;
    int status = _mbm_get_sp (bm, us, size, ptr);
    if (status == MBM_NO_ROOM)  {
      if (_mbm_check_freqmode(bm) > 0)  {
        status = _mbm_get_sp(bm, us, size, ptr);
      }
      if (status == MBM_NO_ROOM)  {
        _mbm_add_wsp (bm, us, size, ptr, astadd, astpar);
        return user.status();
      }
    }
    us->reason      = BM_K_INT_SPACE;
    us->ws_size     = size;
    us->ws_ptr_add  = ptr;
    us->p_state     = S_wspace_ast_ready;
    us->p_astadd    = astadd;
    us->p_astpar    = astpar;
    if (!us->space_size)  {
      _mbm_return_err(MBM_ZERO_LEN);
    }
  }
  return user.status();
}

int mbm_declare_event (BMID bm, int len, int evtype, const unsigned int* trmask,
                       const char* dst, void** free_add, int* free_size, int part_id)
{
  Lock lock(bm);
  if ( lock )  {
    return _mbm_declare_event (bm,len,evtype,*(TriggerMask*)trmask,dst,free_add,free_size,part_id);
  }
  return lock.status();
}

int mbm_declare_event_and_send (BMID bm, int len, int evtype, const unsigned int* trmask,
                                const char* dest, void** free_add, int* free_size, int part_id)
{
  Lock lock(bm);
  if ( lock )  {
    int sc = _mbm_declare_event (bm, len, evtype, *(TriggerMask*)trmask, dest, free_add, free_size, part_id);
    if (lib_rtl_is_success(sc))    {
      sc = _mbm_send_space(bm);
      if (lib_rtl_is_success(sc))      {
        *free_add = 0;
        *free_size  = 0;
      }
    }
    return sc;
  }
  return lock.status();
}

int mbm_free_space (BMID bm) {
  UserLock user(bm, MBM_ILL_CONS);
  USER* us = user.user();
  if ( us )  {
    if ( us->space_size )    {
      _mbm_sfree (bm, us->space_add, us->space_size);
      us->space_add = 0;
      us->space_size = 0;
      return MBM_NORMAL;
    }
    _mbm_return_err (MBM_NO_EVENT);
  }
  return user.status();
}

int mbm_send_space (BMID bm) {
  Lock lock(bm);
  if ( lock )  {
    return _mbm_send_space(bm);
  }
  return lock.status();
}

#if 0
int mbm_send_event (BMID bm, int* array, int len, int evtype, const unsigned int* trmask, const char* dest, int partid)  {
  int *buff, dummy;
  int status = mbm_get_space (bm, len, &buff);
  if (status != MBM_NORMAL)  {
    return (status);
  }
  for (int *bfs = array, *bfd = buff, i = (len + 3) >> 2; i > 0; bfs++, bfd++, i--)  {
    *bfd = *bfs;
  }
  status = mbm_declare_event (bm, len, evtype, trmask, dest, &dummy, &dummy, partid);
  if (status != MBM_NORMAL)  {
    return (status);
  }
  status = mbm_send_space (bm);
  if (status != MBM_NORMAL)  {
    return (status);
  }
  return MBM_NORMAL;
}
#endif

int mbm_cancel_request (BMID bm)   {
  UserLock user(bm, MBM_ILL_CONS);
  USER* us = user.user();
  if ( us )  {
    if (us->c_state == S_wevent)    {
      _mbm_del_wev (bm, us);
      us->c_state = S_active;
      ::lib_rtl_set_event(bm->WEV_event_flag);
    }
    if (us->p_state == S_wspace)    {
      _mbm_del_wsp (bm, us);
      us->p_state = S_active;
      ::lib_rtl_set_event(bm->WSP_event_flag);
    }
    if (us->p_state == S_weslot)    {
      _mbm_del_wes (bm, us);
      us->p_state = S_active;
      ::lib_rtl_set_event(bm->WES_event_flag);
    }
  }
  return user.status();
}

// Map global buffer information on this machine
int mbm_map_global_buffer_info(lib_rtl_gbl_t* handle, bool create)  {
  lib_rtl_gbl_t h;
  size_t len = sizeof(BUFFERS)+(MBM_MAX_BUFF-1)*sizeof(BUFFERS::BUFF);
  int status = ::lib_rtl_map_section("bm_buffers", len, &h);
  if( !lib_rtl_is_success(status) && create )  {
    status = ::lib_rtl_create_section("bm_buffers", len, &h, true);
    if(!lib_rtl_is_success(status))   {   
      ::lib_rtl_signal_message(LIB_RTL_OS,"Cannot access section bm_buffers.");
      return MBM_ERROR;
    }
    BUFFERS* buffs = (BUFFERS*)h->address;
    ::memset(buffs,0,len);
    buffs->nbuffer = 0;
  }
  else if (!lib_rtl_is_success(status)) {
    *handle = 0;
    return MBM_ERROR;
  }
  BUFFERS* b = (BUFFERS*)h->address;
  b->p_bmax = MBM_MAX_BUFF;
  *handle = h;
  return MBM_NORMAL;
}

// Unmap global buffer information on this machine
int mbm_unmap_global_buffer_info(lib_rtl_gbl_t handle, bool remove)  {
  if ( handle )  {
    int status;
    BUFFERS* buffs = (BUFFERS*)handle->address;
    if ( buffs->nbuffer == 0 && remove )  {
      status = lib_rtl_delete_section(handle);
    }
    else {
      status = lib_rtl_unmap_section(handle);
    }
    if( lib_rtl_is_success(status) )   {   
      return MBM_NORMAL;
    }
  }
  return MBM_ERROR;
}

// Utility routines

// clear events with freqmode = notall
int _mbm_check_freqmode (BMID bm)  {
  int ret = 0;
  MBMQueue<EVENT> que(bm->evDesc, -EVENT_next_off);
  for(EVENT* ev=que.get(); ev; ev = que.get() )  {
    ev->isValid();
    //if (!ev->umask0.mask_summ() && ev->umask1.mask_summ() && !ev->umask2.mask_summ() && !ev->held_mask.mask_summ() )  {
    if ( 0 == ev->umask0.mask_or(ev->umask2,ev->held_mask) )    {
      _mbm_del_event(bm, ev, ev->ev_size);          // de-allocate event slot/space
      ++ret;
    }
  }
  return ret;
}

// try to get space ...
int _mbm_get_sp (BMID bm, USER* us, int size, int** ptr)  {
  CONTROL *ctrl = bm->ctrl;
  char *bitmap = bm->bitmap;
  int shift = ctrl->shift_p_Bit;
  int bit, nbit = (size + ctrl->bytes_p_Bit) >> shift;  // round size to block
  ctrl->last_alloc = 0;
  //int status = BF_alloc(bitmap+ctrl->last_alloc,ctrl->bm_size-(ctrl->last_alloc<<3),nbit,&bit);
  //if (!lib_rtl_is_success(status))  {
    //ctrl->last_alloc = 0;
    int status = BF_alloc(bitmap,ctrl->bm_size,nbit,&bit);
  //}
  if (lib_rtl_is_success(status))  {
    _mbm_printf("%s Allocated %d bits at position %d ",bm->bm_name, nbit, bit);
    bit += ctrl->last_alloc<<3 ;
    _mbm_printf(" [%d] \n", bit);
    ctrl->last_alloc  = (bit+nbit)>>3;
    ctrl->last_bit = bit;
    ctrl->i_space -= nbit;
    us->ws_ptr = (bit << ctrl->shift_p_Bit);
    *ptr = (int*)(bm->buffer_add + (bit << shift));
    us->space_add  = bit  << shift;    // keep space info
    us->space_size = nbit << shift;
    _mbm_printf("Got space: %08X %d Bytes\n",us->ws_ptr, size);
    return MBM_NORMAL;
  }
  return MBM_NO_ROOM;
}

// try to get a single event ... 
int _mbm_get_ev(BMID bm, USER* u)  {
  MBMQueue<EVENT> que(bm->evDesc, -EVENT_next_off);
  for(EVENT* e = que.get(); e != 0; e = que.get() )  {
    if ( e->isValid() && (e->busy != 2) && (e->busy !=0) )  {
      int req_one = e->umask2.test(u->uid);
      if ( req_one || e->umask0.test(u->uid) || e->umask1.test(u->uid) )  {
        u->we_ptr    = e->ev_add;
        u->we_size   = e->ev_size;
        u->we_evtype = e->ev_type;
        u->we_trmask = e->tr_mask;
        u->held_eid  = e->eid;
        e->held_mask.set(u->uid);
        if ( req_one )  { // BM_REQ_ONE consumer: No other may see this event
          e->umask2.clear();
          e->umask2.set(u->uid);
        }
        u->ev_seen++;
        bm->ctrl->tot_seen++;
        return MBM_NORMAL;
      }
    }
  }
  return MBM_NO_EVENT;
}

// add user in wait_event queue
int _mbm_add_wev(BMID bm, USER *us, int** ptr, int* size, int* evtype, TriggerMask* trmask, 
     int part_id, RTL_ast_t astadd, void* astpar)  {
  static int calls = 0;
  us->c_state       = S_wevent;
  us->c_partid      = part_id;
  us->we_ptr_add    = ptr;
  us->we_size_add   = size;
  us->we_evtype_add = evtype;
  us->we_trmask_add = trmask;
  us->c_astadd      = astadd;
  us->c_astpar      = astpar;
  us->held_eid      = EVTID_NONE;
  us->sleep_calls++;
  _mbm_printf("WEV ADD> %d State:%d\n",calls++, us->c_state);
  insqti(&us->wenext, &bm->usDesc->wev_head);
  return MBM_NORMAL;
}

// del user from the wait_event queue
int _mbm_del_wev (BMID /* bm */, USER* u) {
  static int calls = 0;
  if ( u->c_state != S_wevent )  {
    lib_rtl_signal_message(0,"INCONSISTENCY: Delete user from WEV queue "
                            "without state S_wevent");
  }
  _mbm_printf("WEV DEL> %d\n",calls++);
  u->c_state = S_wevent_ast_queued;
  remqent(&u->wenext);
  return MBM_NORMAL;
}

// check wait event queue
int _mbm_check_wev (BMID bm, EVENT* e)  {
  MBMQueue<USER> que(&bm->usDesc->wev_head, -USER_we_off);
  for(USER* u = que.get(); u != 0; u = que.get() )  {
    u->isValid();
    int req_one = e->umask2.test(u->uid);
    if ( req_one || e->umask0.test(u->uid) || e->umask1.test(u->uid) )  {
      u->we_ptr    = e->ev_add;
      u->we_size   = e->ev_size;
      u->we_evtype = e->ev_type;
      u->we_trmask = e->tr_mask;
      u->held_eid  = e->eid;
      e->held_mask.set(u->uid);
      if ( req_one )  { // BM_REQ_ONE consumer: No other may see this event
        e->umask2.clear();
        e->umask2.set(u->uid);
      }
      u->ev_seen++;
      bm->ctrl->tot_seen++;
      _mbm_del_wev (bm, u);
      u->get_wakeups++;
      _mbm_printf("EVENT: id=%d  %d %08X -> %s\n",e->eid, e->count, e->ev_add, u->name);
      _mbm_wake_process(bm, BM_K_INT_EVENT, u);
    }
  }
  return MBM_NORMAL;
}

// add user in the wait_space queue
int _mbm_add_wsp (BMID bm, USER* us, int size, int** ptr, RTL_ast_t astadd, void* astpar) {
  us->p_state     = S_wspace;
  us->p_astadd    = astadd;
  us->p_astpar    = astpar;
  us->ws_size     = size;
  us->ws_ptr_add  = ptr;
  us->sleep_calls++;
  insqti (&us->wsnext, &bm->usDesc->wsp_head);
  return MBM_NORMAL;
}

// del user from the wait_space queue
int _mbm_del_wsp (BMID /* bm */, USER* us) {
  if (us->p_state != S_wspace)  {
    _mbm_printf("INCONSISTENCY: Delete user from WSP queue without state S_wspace\n");
  }
  remqent(&us->wsnext);
  us->p_state = S_wspace_ast_queued;
  return MBM_NORMAL;
}

// check wait space queue
int _mbm_check_wsp (BMID bm)  {
  CONTROL *ctrl   = bm->ctrl;
  char    *bitmap = bm->bitmap;
  int      shift  = ctrl->shift_p_Bit;

  MBMQueue<USER> que(&bm->usDesc->wsp_head, -USER_ws_off);
  for (USER* u=que.get(); u; u = que.get() )  {
    u->isValid();
    //int s, l;

    _mbm_printf("WSP: User %s\n",u->name);
    if ( u->p_state == S_wspace )      {
      int ubit = (u->ws_size + ctrl->bytes_p_Bit) >> shift;
      int bit = 0, nbit = ubit;
      if( BF_count(bitmap,ctrl->bm_size,&bit,&nbit) == 1) {    // find largest block 
        ctrl->last_alloc = 0;
        int status = BF_alloc(bitmap,ctrl->bm_size,ubit,&bit);
        if (lib_rtl_is_success(status))       {
          _mbm_printf("%s Allocated %d bits at position %d ",bm->bm_name, nbit, bit);
          bit += ctrl->last_alloc<<3;
          _mbm_printf(" [%d] \n", bit);
          ctrl->last_alloc = (bit+ubit)>>3;
          ctrl->last_bit   = bit;
          ctrl->i_space   -= ubit;
          u->ws_ptr        = bit << shift;
          u->space_size    = ubit << shift;
          u->space_add     = u->ws_ptr;
          _mbm_del_wsp (bm, u);
          _mbm_wake_process(bm, BM_K_INT_SPACE, u);
          break;
        }
      }
    }
  }
  return MBM_NORMAL;
}

// find matching req
int _mbm_match_req (BMID bm, int partid, int evtype, TriggerMask& trmask, 
                    UserMask& mask0, UserMask& mask1, UserMask& mask2)  
{
  int i;
  REQ *rq;
  UserMask dummy;
  MBMQueue<USER> que(bm->usDesc, -USER_next_off);
  for(USER* u=que.get(); u; u=que.get() )  {
    u->isValid();
    if (((u->partid != 0) && (partid != 0) && (u->partid != partid)) || (u->c_state == S_pause))  {
      continue;
    }
    for (i = 0, rq = u->req; i < u->n_req; i++, rq++)  {
      if (evtype != rq->ev_type)
	continue;
      else if (rq->masktype == BM_MASK_ALL)  {
	if ( trmask != rq->tr_mask )
	  continue;
      }
      else if ( !dummy.mask_and(trmask, rq->tr_mask) )
	continue;
      else if ( dummy.mask_and(trmask, rq->vt_mask) )  
	continue;
      else if ( (float(::rand())/float(RAND_MAX)*100.0) > rq->freq)
        continue;

      if ( rq->user_type == BM_REQ_ONE )    
        mask2.set(u->uid);
      else if ( rq->user_type == BM_REQ_VIP )
        mask0.set(u->uid);
      else
        mask1.set(u->uid);
      u->ev_actual++;
      bm->ctrl->tot_actual++;
      break;
    }
  }
  return MBM_NORMAL;
}

// check existance of name
int _mbm_findnam (BMID bm, const char* name) {
  MBMQueue<USER> que(bm->usDesc,-USER_next_off);
  for(USER* u=que.get(); u; u=que.get())  {
    if ( u->isValid() && ::strncmp(u->name, name, sizeof(u->name)) == 0 )  {
      return u->uid;
    }
  }
  return (-1);
}

// alloc user slot
USER* _mbm_ualloc (BMID bm)  {
  USER *u = bm->user;
  for (int i = 0; i < bm->ctrl->p_umax; ++i, ++u)  {
    if (u->busy == 0)    {
      u->busy = 1;
      u->uid  = i;
      insqti(u, bm->usDesc);
      return u;
    }
  }
  return 0;
}

// alloc event slot
EVENT* _mbm_ealloc (BMID bm, USER* us)  {
  int i = 0;
  static int cnt = 0;
  EVENT *e = bm->event;
  for (CONTROL *ctrl = bm->ctrl; i < ctrl->p_emax; ++i, ++e)  {
    if (e->isValid() && e->busy == 0)    {
      e->busy = 2;
      e->eid  = i;
      e->count = cnt++;
      ctrl->i_events++;
      if ( bm->alloc_event )   {
        void* pars[4];
        pars[0] = bm;
        pars[1] = bm->alloc_event_param;
        pars[2] = (void*)(us->space_add+bm->buffer_add);
        pars[3] = e;
        us->alloc_calls++;
        (*bm->alloc_event)(pars);
      }
      insqti(e, bm->evDesc);
      return e;
    }
  }
  return 0;
}

// free user slot
int _mbm_ufree (BMID /* bm */, USER* u)  {
  if ( u->busy == 1 )  {
    u->busy = 0;
    u->uid  = 0;
    remqent(u);
    return MBM_NORMAL;
  }
  _mbm_return_err (MBM_INTERNAL);
}

// free event slot
int _mbm_efree (BMID bm, EVENT* e)  {
  if (e == 0)  {
    _mbm_return_err(MBM_INTERNAL);
  }
  else if (e->busy == 0)  {
    _mbm_return_err (MBM_INTERNAL);
  }
  USER* u = bm->_user();
  int* evadd  = (int*)(e->ev_add+bm->buffer_add);
  _mbm_printf("Free slot: %d %d\n",e->eid, e->count);
  if ( bm->free_event )  {
    void* pars[4];
    pars[0] = bm;
    pars[1] = bm->free_event_param;
    pars[2] = evadd;
    pars[3] = e;
    u->free_calls++;
    (*bm->free_event)(pars);
  }
  _mbm_reset_memory(bm->ctrl,evadd,0xDDDDDDDD,e->ev_size);
  e->umask0.clear();
  e->umask1.clear();
  e->umask2.clear();
  e->held_mask.clear();
  e->busy = 0;
  u->ev_freed++;
  remqent(e);
  bm->ctrl->i_events--;
  _mbm_check_wes (bm);
  return MBM_NORMAL;
}

int _mbm_del_event(BMID bm, EVENT* e, int len)  {
  int add = e->ev_add;
  _mbm_efree (bm, e);           // de-allocate event 
  _mbm_sfree (bm, add, len);    // de-allocate event slot/space
  return MBM_NORMAL;
}

// release event held by this user
int _mbm_rel_event (BMID bm, USER* u)  {
  EVENT *e = bm->event + u->held_eid;
  u->held_eid = EVTID_NONE;
  e->umask0.clear(u->uid);
  e->umask1.clear(u->uid);
  e->umask2.clear(u->uid);
  e->held_mask.clear(u->uid);
  if ( !e->umask0.mask_or(e->umask1,e->umask2) )  {  // no more consumers
    _mbm_del_event(bm, e, e->ev_size);               // de-allocate event slot/space
  }
  return MBM_NORMAL;
}

// clean-up this user
int _mbm_uclean (BMID bm)  {
  int   uid = bm->owner;
  USER* u  = bm->user + uid;
  if (u->c_state == S_wevent)  {
    _mbm_del_wev (bm, u);
  }
  if (u->p_state == S_wspace)  {
    _mbm_del_wsp (bm, u);
  }
  if ( u->p_state == S_weslot )  {
    _mbm_del_wes (bm, u);
  }

  if ( u->space_size )   {              // free the held space
    _mbm_sfree (bm, u->space_add, u->space_size);
    u->space_add = 0;
    u->space_size = 0;
  }
  if ( u->held_eid != EVTID_NONE )    { // free the held event
    _mbm_rel_event (bm, u);             // release event
  }
  MBMQueue<EVENT> que(bm->evDesc,-EVENT_next_off);
  for(EVENT* e=que.get(); e; e=que.get() )  {
    e->isValid();
    e->umask0.clear(uid);
    e->umask1.clear(uid);
    e->umask2.clear(uid);
    e->held_mask.clear(uid);
    if ( !e->umask0.mask_or(e->umask1,e->umask2) || ((e->busy == 2) && e->held_mask.mask_summ()))  {
      _mbm_del_event(bm, e, e->ev_size);          // de-allocate event slot/space
    }
  }
  _mbm_ufree (bm, u);  // de-allocate user slot
  bm->ctrl->i_users--;
  return MBM_NORMAL;
}

// deallocate space
int _mbm_sfree (BMID bm, int add, int size)  {
  CONTROL *ctrl = bm->ctrl;
  int bit   =  add >> ctrl->shift_p_Bit;
  int nbit  = (size + ctrl->bytes_p_Bit) >> ctrl->shift_p_Bit;
  BF_free(bm->bitmap,bit,nbit);
  ctrl->last_alloc = 0;
  ctrl->i_space += nbit;
  _mbm_printf("%s Free space  add=%08X  size=%d [ %d bits at position %d]\n",
    bm->bm_name, add, size, nbit, bit);
  if ( bm->usDesc->next )  {
    _mbm_check_wsp(bm);   // check the space wait queue 
  }
  return MBM_NORMAL;
}

int mbm_grant_update (BMID bm)   {
  UserLock user(bm);
  USER* u = user.user();
  if ( u )  {
    if (u->held_eid == EVTID_NONE)  {
      _mbm_return_err (MBM_NO_EVENT);
    }
    EVENT* e = bm->event + u->held_eid;  // convert event to space
    int held_eid = u->held_eid;
    int add = (int)e->ev_add ;
    int siz = e->ev_size;
    u->held_eid = EVTID_NONE;
    e->held_mask.clear(u->uid);
    e->umask0.clear(u->uid);
    e->umask1.clear(u->uid);
    e->umask2.clear(u->uid);
    if ( !e->umask0.mask_or(e->umask1,e->umask2) )  {
      //  no more consumers - convert event into space de-allocate event slot
      _mbm_efree (bm, e);
      u->space_add  = add;
      u->space_size = siz;
      return 0;
    }
    // Still other consumers for this event. Restore old situation and return error
    e->held_mask.set(u->uid);
    e->umask0.set(u->uid);
    e->umask1.set(u->uid);
    e->umask2.set(u->uid);
    u->held_eid = held_eid;
    _mbm_return_err (MBM_ILL_REQ);
  }
  return user.status();
}

// clean-up this user in all buffers
int _mbm_shutdown (void* /* param */) {
  qentry_t *q, *bmq = desc_head;
  if (bmq == 0)  {
    return MBM_NORMAL;
  }
  for(int sc=remqhi(bmq,&q); lib_rtl_queue_success(sc); sc=remqhi(bmq,&q))  {
    BMID bm = (BMID )q;
    if ( bm->lockid )  {
      int status = lib_rtl_cancel_lock(bm->lockid);
      if (!lib_rtl_is_success(status))    { 
        lib_rtl_signal_message(LIB_RTL_OS,"Error cancelling lock %s. Status %d",
                               bm->mutexName,status);
      }
    }
    else  {
      lib_rtl_signal_message(0,"Error cancelling lock %s [Invalid Mutex].", bm->mutexName);
    }
    if (disable_rundown == 1)    {
      continue;
    }
    _mbm_lock_tables(bm);
    _mbm_uclean (bm);
    lib_rtl_delete_event(bm->WES_event_flag);
    lib_rtl_delete_event(bm->WEV_event_flag);
    lib_rtl_delete_event(bm->WSP_event_flag);
    lib_rtl_delete_event(bm->WSPA_event_flag);
    lib_rtl_delete_event(bm->WEVA_event_flag);
    lib_rtl_unmap_section(bm->buff_add);
    lib_rtl_unmap_section(bm->bitm_add);
    lib_rtl_unmap_section(bm->user_add);
    lib_rtl_unmap_section(bm->event_add);
    lib_rtl_unmap_section(bm->ctrl_add);
    _mbm_unlock_tables(bm);
  }
  /*  bm_exh_unlink (); */
  return MBM_NORMAL;
}

int _mbm_find_buffer (const char* bm_name)  {
  qentry_t *next, *bmq = desc_head;
  int l = strlen (bm_name);
  for(BMID bm=(BMID)add_ptr(bmq,bmq->next); bmq != bm; bm=add_ptr(bm,next))  {
    int l1 = strlen (bm->bm_name);
    next  = bm->next ;
    if (l1 == l)    {
      if (strncmp (bm->bm_name, bm_name, l) == 0)      {
        return 0;
      }
    }
  }
  return -1;
}

// add user in the wait_event_slot queue
int _mbm_add_wes (BMID bm, USER *us, RTL_ast_t astadd)  {
  us->p_state   = S_weslot;
  us->p_astadd  = astadd;
  us->p_astpar  = bm;
  insqti (&us->wesnext, &bm->usDesc->wes_head);
  return MBM_NORMAL;
}

// del user from the wait_event_slot queue
int _mbm_del_wes (BMID /* bm */, USER* us)   {
  us->p_state = S_weslot_ast_queued;
  remqent(&us->wesnext);
  return MBM_NORMAL;
}

// check wait event slot
int _mbm_check_wes (BMID bm)   {
  MBMQueue<USER> que(&bm->usDesc->wes_head,-USER_wes_off);
  for(USER* u=que.get(); u; u=que.get() )  {
    if (u->isValid() && u->p_state == S_weslot)    {
      _mbm_del_wes (bm, u);
      _mbm_wake_process (bm, BM_K_INT_ESLOT, u);
      break;
    }
  }
  return MBM_NORMAL;
}


// Statistics routines
int mbm_events_actual (BMID bm, int *events)   {
  UserLock user(bm);
  if ( user.user() )  {
    *events = user.user()->ev_actual;
  }
  return user.status();
}

int mbm_events_produced (BMID bm, int *events)   {
  UserLock user(bm);
  if ( user.user() )  {
    *events = user.user()->ev_produced;
  }
  return user.status();
}

int mbm_events_seen (BMID bm, int *events)   {
  UserLock user(bm);
  if ( user.user() )  {
    *events = user.user()->ev_seen;
  }
  return user.status();
}

int mbm_reset_statistics (BMID bm) {
  UserLock user(bm);
  if ( user.user() )  {
    user.user()->ev_actual   = 0;
    user.user()->ev_produced = 0;
    user.user()->ev_seen     = 0;
  }
  return user.status();
}

int mbm_buffer_size (BMID bm, int* size) {
  Lock lock(bm);
  if ( lock )  {
    if ( !bm->_control() )  {
      _mbm_return_err (MBM_ILL_CONS);
    }
    *size = bm->_control()->buff_size;
  }
  return lock.status();
}

int mbm_events_in_buffer (BMID bm, int* events)  {
  Lock lock(bm);
  if ( lock )  {
    if ( !bm->_control() )  {
      _mbm_return_err (MBM_ILL_CONS);
    }
    *events = bm->_control()->i_events;
  }
  return lock.status();
}

int mbm_space_in_buffer (BMID bm, int* total, int* large)  {
  Lock lock(bm);
  if ( lock )  {
    CONTROL *ctrl = bm->_control();
    if ( ctrl )  {
      int s, l = ctrl->i_space, shift = ctrl->shift_p_Bit;
      *total = ctrl->i_space << shift;
      // find largest block (since asking for full space)
      BF_count(bm->bitmap, ctrl->bm_size, &s, &l);
      *large = l << shift;
      return MBM_NORMAL;
    }
    _mbm_return_err (MBM_ILL_CONS);
  }
  return lock.status();
}

int mbm_process_exists (BMID bm, const char* name, int* exists)  {
  if (bm->owner == -1)  {
    _mbm_return_err (MBM_ILL_CONS);
  }
  int status = _mbm_findnam (bm, name);
  *exists =  (status == -1) ? 0 : 1;
  return MBM_NORMAL;
}

int  mbm_wait_event(BMID bm)    {
  int sc;
  if (bm->owner == -1)  {
    _mbm_return_err (MBM_ILL_CONS);
  }
  USER* us = bm->_user();
  if ( us->held_eid != EVTID_NONE )  {
    Lock lock(bm);
    _mbm_printf("WEV: State=%d  %s\n", us->c_state, us->c_state == S_wevent_ast_queued ? "OK" : "BAAAAD");
    if ( us->c_state == S_wevent_ast_queued )  {
      // lib_rtl_clear_event(bm->WEV_event_flag);
      us->reason = BM_K_INT_EVENT;
      us->get_wakeups++;
      lib_rtl_run_ast(us->c_astadd, us->c_astpar, 3);
      us->c_state = S_active;
      return MBM_NORMAL;
    }
  }
Again:
  sc = 1;
  if ( us->c_state != S_wevent_ast_queued )  {
    sc = lib_rtl_wait_for_event(bm->WEV_event_flag);
  }
  if ( lib_rtl_is_success(sc) )  {
    lib_rtl_clear_event(bm->WEV_event_flag);
    Lock lock(bm);
    if ( us->held_eid == EVTID_NONE )    {
      if ( us->c_state == S_active )  {
        return MBM_REQ_CANCEL;
      }
      goto Again;
    }
    us->reason = BM_K_INT_EVENT;
    us->get_wakeups++;
    lib_rtl_run_ast(us->c_astadd, us->c_astpar, 3);
    us->c_state = S_active;
    return MBM_NORMAL;
  }
  MBM_ERROR;
  _mbm_return_err (MBM_ILL_CONS);
}

int mbm_wait_event_a(BMID bm)    {
  UserLock user(bm);
  if ( user.user() )  {
    int sc = lib_rtl_wait_for_event_a(bm->WEVA_event_flag,(lib_rtl_thread_routine_t)mbm_wait_event,bm);
    if ( lib_rtl_is_success(sc) )  {
      return MBM_NORMAL;
    }
    return MBM_INTERNAL;
  }
  return user.status();
}

int  mbm_wait_space(BMID bm)    {
  USER* us = bm->_user();
  if ( !us )  {
    _mbm_return_err (MBM_ILL_REQ);
  }
wait:
  if ( us->p_state == S_wspace_ast_ready )    {
    us->p_state = S_wspace_ast_queued;
    Lock lock(bm);
    lib_rtl_run_ast(us->p_astadd, us->p_astpar, 3);
    _mbm_check_memory(*us->ws_ptr_add, 0xDDDDDDDD,us->ws_size);
    return MBM_NORMAL;
  }
  else if ( us->p_state == S_weslot_ast_queued )  {
    lib_rtl_wait_for_event(bm->WES_event_flag);
    lib_rtl_clear_event(bm->WES_event_flag);
    if ( us->p_state == S_active )   {
      return MBM_REQ_CANCEL;
    }
    lib_rtl_run_ast(us->p_astadd, us->p_astpar, 3);
  }
  else if ( us->p_state == S_wspace )    {
    lib_rtl_wait_for_event(bm->WSP_event_flag);
    if ( us->p_state == S_active )  {
      return MBM_REQ_CANCEL;
    }
    lib_rtl_clear_event(bm->WSP_event_flag);
  }
  else if ( us->p_state == S_wspace_ast_queued )    {
    //lib_rtl_wait_for_event(bm->WSP_event_flag);
    lib_rtl_clear_event(bm->WSP_event_flag);
    Lock lock(bm);
    lib_rtl_run_ast(us->p_astadd, us->p_astpar, 3);
    return MBM_NORMAL;
  }
  goto wait;
}

int _mbm_wait_space_a(void* param)  {
  BMID bm = (BMID)param;
  for(;;)  {
    lib_rtl_clear_event(bm->WSPA_event_flag);
    int sc = mbm_wait_space(bm);
    if ( sc != MBM_NORMAL )  {
      bm->pThreadState = 1;
      return 1;
    }
    lib_rtl_wait_for_event(bm->WSPA_event_flag);
  }
  return 1;
}

int  mbm_wait_space_a(BMID bm)    {
  UserLock user(bm);
  if ( user.user() )  {
    if ( 0 != bm->pThread && 1 == bm->pThreadState )  {
      lib_rtl_join_thread(bm->pThread);
      bm->pThread = 0;
    }
    if ( 0 == bm->pThread )  {
      int sc = lib_rtl_start_thread(_mbm_wait_space_a, bm, &bm->pThread);
      if ( !(sc & 1) )  {
        lib_rtl_signal_message(0,"Failed to manipulate producer BM thread");
        return MBM_INTERNAL;
      }
      bm->pThreadState = 2;
    }
    lib_rtl_set_event(bm->WSPA_event_flag);
    return MBM_NORMAL;
  }
  return user.status();
}

int _mbm_get_user_flag(std::map<long long int,lib_rtl_event_t>& flags, USER* us, const char* name, lib_rtl_event_t* flg)  {
  typedef std::map<long long int,lib_rtl_event_t> event_map;
  long long int idx = (((long long int)us->uid)<<32) + (long long int)us->pid;
  event_map::const_iterator i = flags.find(idx);
  if ( i == flags.end() ) {
    int sc = ::lib_rtl_create_event(name, flg);
    if ( sc == 1 ) {
      flags.insert(event_map::value_type(idx,*flg));
      return MBM_NORMAL;
    }
    return MBM_ERROR;
  }
  *flg = (*i).second;
  return MBM_NORMAL;
}

// Wakeup process to continue processing
#ifdef _WIN32
int _mbm_wake_process (BMID bm, int reason, USER* us) {
  us->reason = reason;
  lib_rtl_event_t flag = 0;
  int status = MBM_NORMAL;
  switch(reason)  {
    case BM_K_INT_EVENT:
      status = _mbm_get_user_flag(bm->us_wev_flags, us, us->wev_flag, &flag);
      break;
    case BM_K_INT_ESLOT:
      status = _mbm_get_user_flag(bm->us_wes_flags, us, us->wes_flag, &flag);
      break;
    case BM_K_INT_SPACE:
      status = _mbm_get_user_flag(bm->us_wsp_flags, us, us->wsp_flag, &flag);
      break;
    default:
      status = MBM_ERROR;
  }
  if ( status == MBM_NORMAL ) {
    status = lib_rtl_set_event(flag);
    if (!lib_rtl_is_success(status))  {
      lib_rtl_start_debugger();
    }
    return MBM_NORMAL;
  }
  return status;
}
#else
int _mbm_wake_process (BMID, int reason, USER* us) {
  us->reason = reason;
  switch(reason)  {
    case BM_K_INT_EVENT:
      return ::sem_post(&us->wev_handle)==0 ? MBM_NORMAL : MBM_ERROR;
    case BM_K_INT_ESLOT:
      return ::sem_post(&us->wes_handle)==0 ? MBM_NORMAL : MBM_ERROR;
    case BM_K_INT_SPACE:
      return ::sem_post(&us->wsp_handle)==0 ? MBM_NORMAL : MBM_ERROR;
  }
  return MBM_ERROR;
}
#endif

int _mbm_fill_offsets() {
  byte_offset(USER,next,USER_next_off);
  byte_offset(USER,wsnext,USER_ws_off);
  byte_offset(USER,wenext,USER_we_off);
  byte_offset(USER,wesnext,USER_wes_off);
  byte_offset(USERDesc,next,USER_active_off);
  byte_offset(USERDesc,wev_head,CONTROL_cwe_off);
  byte_offset(USERDesc,wsp_head,CONTROL_pws_off);
  byte_offset(USERDesc,wes_head,CONTROL_wes_off);
  byte_offset(EVENT,next,EVENT_next_off);
  byte_offset(EVENTDesc,next,CONTROL_ev_off);
  return MBM_NORMAL;
}

int mbm_get_event_ast(void* par) {
  BMID  bm = (BMID)par;
  USER *us = bm->user+bm->owner;
  if (us == 0)  {
    return 1;
  }
  us->get_asts_run++;
  if (us->c_state != S_wevent_ast_queued)  {
    lib_rtl_signal_message(0,"us->c_state Not S_wevent_ast_queued, but is %d",us->c_state);
    return 1;
  }
  us->c_state = S_active;
  EVENT* ev  = bm->event+us->held_eid;
  if (us->reason != BM_K_INT_EVENT)  {
    lib_rtl_signal_message(0,"Event_ast spurious wakeup reason = %d",us->reason);
    us->reason = S_wevent_ast_handled;
    return 1;
  }
  us->reason = S_wevent_ast_handled;
  *us->we_ptr_add    = (int*)(ev->ev_add+bm->buffer_add);
  *us->we_size_add   = ev->ev_size;
  *us->we_evtype_add = ev->ev_type;
  *us->we_trmask_add = ev->tr_mask;
  return MBM_NORMAL;
}

int mbm_get_space_ast(void* par) {
  BMID bm = (BMID)par;
  USER *us = bm->_user();
  if ( us == 0 )  {
    return MBM_NORMAL;
  }
  if (us->p_state != S_wspace_ast_queued)   {
    lib_rtl_signal_message(0,"us->p_state Not S_wspace_ast_queued, but is %d",us->p_state);
    us->p_state = S_wspace_ast_handled;
    us->reason = -1;
    return MBM_NORMAL;
  }
  us->p_state = S_active;
  if ( !(us->reason&BM_K_INT_SPACE) )   {
    lib_rtl_signal_message(0,"space_ast spurious wakeup reason = %d",us->reason);
    us->p_state = S_wspace_ast_handled;
    us->reason = -1;
    return MBM_NORMAL;
  }
  *us->ws_ptr_add = (int*)(us->space_add+bm->buffer_add);
  us->p_state = S_wspace_ast_handled;
  us->reason = -1;
  return MBM_NORMAL;
}

// Wait event space AST function
int _mbm_wes_ast(void* par)   {
  BMID bm = (BMID)par;
  USER *us  = bm->user+bm->owner;
  if (us->p_state == S_weslot_ast_queued)  {
    us->p_state  = S_active;
    return 1;
  }
  lib_rtl_signal_message(0,"_mbm_wes_ast state is not S_weslot_ast_queued is %d. IGNORED",us->p_state);
  return 0;
}

int _mbm_declare_event (BMID bm, int len, int evtype, TriggerMask& trmask,
                        const char* dest, void** free_add, int* free_size, int part_id)
{
  UserMask mask0(0), mask1(0), mask2(0);
  CONTROL* ctrl  = bm->ctrl;
  if (bm->owner == -1)  {
    _mbm_return_err(MBM_ILL_CONS);
  }
  USER* us = bm->user + bm->owner;
  if (us->uid != bm->owner)  {
    _mbm_return_err(MBM_INTERNAL);
  }
  if (!us->space_size)  {
    _mbm_return_err(MBM_NO_EVENT);
  }
  if (len <= 0)   {
    _mbm_return_err(MBM_ZERO_LEN);
  }
  if (len > us->space_size)  {
    _mbm_return_err(MBM_EV_TOO_BIG);
  }
  int   rlen = ((len + ctrl->bytes_p_Bit) >> ctrl->shift_p_Bit) << ctrl->shift_p_Bit;
  char* add  = us->space_add+bm->buffer_add;
  if (dest)  {                       // find all destinations 
    int uid = _mbm_findnam (bm, dest);
    if (uid != -1)    {
      mask0.set(uid);
    }
  }
  EVENT* ev = 0;
  do  {
    ev = _mbm_ealloc(bm,us);
    if ( ev == 0 )    {
      if (_mbm_check_freqmode(bm) > 0)  {
        ev = _mbm_ealloc(bm,us);
      }
    }
    if ( ev == 0 )   {    // add on the wait event slot queue
      _mbm_add_wes(bm,us,_mbm_wes_ast_add);
      ::lib_rtl_clear_event(bm->WES_event_flag);
      int sp = ctrl->spare1;
      if ( us->p_state != S_weslot_ast_queued )  {
        _mbm_unlock_tables(bm);
        ::lib_rtl_wait_for_event(bm->WES_event_flag);
        _mbm_lock_tables(bm);
      }
      // This means a mbm_cancel_request came while waiting!
      if ( us->p_state == S_active )   {
        ::lib_rtl_printf("MBM[%s]> I got hit by a cancel request!\n",us->name);
        _mbm_return_err(MBM_REQ_CANCEL);
      }
      ctrl->spare1 = sp;
      if ( us->p_state == S_weslot_ast_queued )  {
        ::lib_rtl_run_ast(us->p_astadd, us->p_astpar, 3);
      }
      else  {
        ::lib_rtl_printf("%s Failed lib_rtl_wait_for_event(bm->WES_event_flag)\n",bm->bm_name);
      }
      if ( us->p_state != S_active )   {
        _mbm_return_err(MBM_REQ_CANCEL); // other reasons
      }
    }
  } while ( ev == 0 );
  _mbm_match_req(bm, part_id, evtype, trmask, mask0, mask1, mask2);
  ev->ev_add  = us->space_add;
  if ( mask0.mask_or(mask1,mask2) )  {
    ev->busy    = 2;              // exclusive use of event
    ev->partid  = part_id;
    ev->ev_type = evtype;
    ev->tr_mask = trmask;
    ev->ev_add  = us->space_add;
    ev->ev_size = len;
    ev->umask0  = mask0;
    ev->umask1  = mask1;
    ev->umask2  = mask2;
    ev->held_mask.clear();
    ev->held_mask.set(bm->owner);
    _mbm_printf("Got slot:%d %d  %08X\n",ev->eid, ev->count, ev->ev_add);
  }
  else  {
    _mbm_del_event(bm, ev, rlen);          // de-allocate event slot/space
  }
  *free_add = add + rlen;
  *free_size = us->space_size - rlen;
  us->space_add  = long(*free_add) - (long)bm->buffer_add;
  us->space_size = *free_size;
  if (us->space_size == 0)  {
    us->space_add = 0;                    // if size zero, address zero
  }
  ctrl->tot_produced++;
  us->ev_produced++;
  return MBM_NORMAL;
}

int _mbm_check_cons (BMID bm)  {
  int owner = bm->owner;
  if (owner == -1)  {
    _mbm_return_err (MBM_ILL_CONS);
  }
  USER* us = bm->user + owner;
  if (us->uid != owner)  {
    _mbm_return_err (MBM_INTERNAL);
  }
  MBMQueue<EVENT> que(bm->evDesc, -EVENT_next_off);
  for(EVENT* e=que.get(); e; e=que.get() )  {
    e->isValid();
    if (e->busy != 2)     {
      continue;
    }
    if ( e->held_mask.test(owner) )      {
      e->held_mask.clear(owner);
      e->busy = 1;
      _mbm_printf("_mbm_check_cons EVENT: %d %d\n",e->eid, e->count);
      _mbm_check_wev (bm, e);  /* check wev queue */
    }
  }
  return MBM_NORMAL;
}

int _mbm_send_space (BMID bm)  {
  int sc = _mbm_check_cons(bm);
  if ( sc == MBM_NORMAL )   {
    USER* u = bm->user + bm->owner;
    if ( u->space_size )  {
      _mbm_printf("_mbm_send_space> %s Free event space @ %08X of size:%d\n",
        bm->bm_name, u->space_add, u->space_size);
      _mbm_sfree (bm, u->space_add, u->space_size);  /* free space */
    }
    u->space_add = 0;
    u->space_size = 0;
  }
  return sc;
}

int _mbm_lock_tables(BMID bm)  {
  if ( bm && bm != MBM_INV_DESC )  {
    if ( bm->lockid )  {
      int status = lib_rtl_lock(bm->lockid);
      if (!lib_rtl_is_success(status))    { 
        lib_rtl_signal_message(LIB_RTL_OS,"error in unlocking tables. Status %d",status);
      }
      return status;
    }
    lib_rtl_signal_message(0,"Error in locking MBM tables [Invalid Mutex] %s",bm->mutexName);
    return 0;
  }
  lib_rtl_signal_message(0,"Error in locking MBM tables [Invalid BMID]");
  return 0;
}

int _mbm_unlock_tables(BMID bm)    {
  if ( bm && bm != MBM_INV_DESC )  {
    if ( bm->lockid )  {
      int status = lib_rtl_unlock(bm->lockid);
      if (!lib_rtl_is_success(status))    { 
        lib_rtl_signal_message(LIB_RTL_OS,"Error in unlocking tables %s. Status %d",bm->mutexName,status);
      }
      return status;
    }
    lib_rtl_signal_message(0,"Error in unlocking tables %s [Invalid Mutex].",bm->mutexName);
    return 0;
  }
  lib_rtl_signal_message(0,"Error in unlocking MBM tables [Invalid BMID]");
  return 0;
}

int _mbm_delete_lock(BMID bm)    {
  if ( bm->lockid )  {
    int status = lib_rtl_delete_lock(bm->lockid);
    if (!lib_rtl_is_success(status))    { 
      lib_rtl_signal_message(LIB_RTL_OS,"Error deleting lock %s. Status %d",bm->mutexName,status);
    }
    else  {
      bm->lockid = 0;
    }
    return status;
  }
  lib_rtl_signal_message(0,"Error deleting lock %s [Invalid Mutex].", bm->mutexName);
  return 0;
}

// MAP buffer manager sections
int _mbm_map_sections(BMID bm)  {
  char text[128];
  const char* bm_name = bm->bm_name;
  sprintf(text, "bm_ctrl_%s", bm->bm_name);
  int len, status  = lib_rtl_map_section(text, sizeof(CONTROL), &bm->ctrl_add);
  if (!lib_rtl_is_success(status))    {
    lib_rtl_signal_message(LIB_RTL_OS,"Error mapping control section for %s.Status=%d",
                           bm_name,status);
    return MBM_ERROR;
  }
  bm->ctrl = (CONTROL*)bm->ctrl_add->address;
  sprintf(text, "bm_event_%s",  bm_name);
  len = sizeof(EVENTDesc)+(bm->ctrl->p_emax-1)*sizeof(EVENT);
  status  = lib_rtl_map_section(text, len, &bm->event_add);
  if (!lib_rtl_is_success(status))  {
    lib_rtl_signal_message(LIB_RTL_OS,"Error mapping event section for %s. Status=%d",
                           bm_name,status);
    _mbm_unmap_sections(bm);
    return MBM_ERROR;
  }
  bm->evDesc = (EVENTDesc*)bm->event_add->address;
  bm->event  = &bm->evDesc->events[0];
  sprintf(text, "bm_user_%s",   bm_name);
  len = sizeof(USERDesc)+(bm->ctrl->p_umax-1)*sizeof(USER);
  status  = lib_rtl_map_section(text, len, &bm->user_add);
  if (!lib_rtl_is_success(status))  {
    lib_rtl_signal_message(LIB_RTL_OS,"Error mapping user section for %s. Status=%d",
                           bm_name,status);
    _mbm_unmap_sections(bm);
    return MBM_ERROR;
  }
  bm->usDesc = (USERDesc*)bm->user_add->address;
  bm->user   = &bm->usDesc->users[0];
  sprintf(text, "bm_bitmap_%s", bm_name);
  len = bm->ctrl->bm_size>>3;
  status  = lib_rtl_map_section(text, len, &bm->bitm_add);
  if (!lib_rtl_is_success(status))  {
    lib_rtl_signal_message(LIB_RTL_OS,"Error mapping bit-map section for %s. Status=%d",
                           bm_name,status);
    _mbm_unmap_sections(bm);
    return MBM_ERROR;
  }
  bm->bitmap = (char*)bm->bitm_add->address;
  sprintf(text, "bm_buffer_%s", bm_name);
  status  = lib_rtl_map_section(text, bm->ctrl->buff_size, &bm->buff_add);
  if (!lib_rtl_is_success(status))  {
    lib_rtl_signal_message(LIB_RTL_OS,"Error mapping buffer section for %s. Status=%d",
                           bm_name,status);
    _mbm_unmap_sections(bm);
    return MBM_ERROR;
  }
  //::lib_rtl_printf("Control: %p  %p\n",bm->ctrl,((char*)bm->ctrl)-((char*)bm->ctrl));
  //::lib_rtl_printf("User:    %p  %p  %p\n",bm->user,((char*)bm->user)-((char*)bm->ctrl),bm->usDesc);
  //::lib_rtl_printf("Event:   %p  %p  %p\n",bm->event,((char*)bm->event)-((char*)bm->ctrl),bm->evDesc);
  //::lib_rtl_printf("Bitmap:  %p  %p\n",bm->bitmap,((char*)bm->bitmap)-((char*)bm->ctrl));
  bm->buffer_add  = (char*)bm->buff_add->address;
  bm->bitmap_size = bm->ctrl->bm_size;
  bm->buffer_size = bm->ctrl->buff_size;
  return MBM_NORMAL;
}

int _mbm_unmap_sections(BMID bm)  {
  if ( bm->buffer_add ) lib_rtl_unmap_section(bm->buff_add);
  if ( bm->bitmap     ) lib_rtl_unmap_section(bm->bitm_add);
  if ( bm->user       ) lib_rtl_unmap_section(bm->user_add);
  if ( bm->event      ) lib_rtl_unmap_section(bm->event_add);
  if ( bm->ctrl       ) lib_rtl_unmap_section(bm->ctrl_add);
  return MBM_NORMAL;
}

int _mbm_flush_sections(BMID bm)   {
  lib_rtl_flush_section(bm->ctrl_add);
  lib_rtl_flush_section(bm->event_add);
  lib_rtl_flush_section(bm->user_add);
  lib_rtl_flush_section(bm->buff_add);
  lib_rtl_flush_section(bm->bitm_add);
  return 1;
}

