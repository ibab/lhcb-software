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
#include <cerrno>
#include <memory>
#include "RTL/Lock.h"
#include "bm_struct.h"
#include "MBM/mepdef.h"

struct MEPDESC : public _MEPID  {
  int             owner;
  lib_rtl_lock_t  lockid;
  char            mutexName[64];
  MEPDESC() : lockid(0), owner(0)  {
    resBuffer = MBM_INV_DESC;
    evtBuffer = MBM_INV_DESC;
    mepBuffer = MBM_INV_DESC; 
  }
};

static int _mep_change_refcount(MEPDESC* dsc,MEP_SINGLE_EVT* evt, int change)  {
  if ( evt )   {
    RTL::Lock lock(dsc->lockid);
    if ( lock )  {
      MEPEVENT* e = (MEPEVENT*)(int*)(dsc->mepStart + evt->begin);
      e->refCount += change;
      if ( e->refCount < 1 )  {
        printf("MEP RefCount ERROR %s [%d] Event at address %p MEP:%p [%d] [Release MEP]\n",
          change > 0 ? "AddRef" : "DelRef", e->refCount, 
          (int)(dsc->mepStart+evt->event), e, e->mepBufferID);
      }
      return MBM_NORMAL;
    }
    // Error
    return MBM_ERROR;
  }
  // Error
  return MBM_ERROR;
}

static int mep_free_mep(void* param)   {
  void** pars = (void**)param;
  MEPEVENT* e = (MEPEVENT*)pars[2];
  MEPDESC* dsc = (MEPDESC*)pars[1];
  int slp = 0;
  if ( e->refCount < 1 )  {
    printf("MEP RefCount ERROR(2) [%d] Event at address %p MEP:%p [%d] [Release MEP]\n",
      e->refCount, (int)(dsc->mepStart+e->begin), e, e->mepBufferID);
  }
  while ( e->refCount > 1 )  {
    //printf("Sleep...");
    slp++;
    lib_rtl_sleep(1);
  }
  // if ( slp )  printf("\n");
  e->refCount--;
  return MBM_NORMAL;
}

static int mep_free(void* param)   {
  void** pars = (void**)param;
  return _mep_change_refcount((MEPDESC*)pars[1],(MEP_SINGLE_EVT*)pars[2],-1);
}

static int mep_declare(void* param)   {
  void** pars = (void**)param;
  return _mep_change_refcount((MEPDESC*)pars[1],(MEP_SINGLE_EVT*)pars[2],1);
}

void _mep_exclude(BMID id, int full_buffer)  {
  full_buffer ? mbm_exclude(id) : mbm_unmap_memory(id);
}

int mep_set_watch(MEPID dsc)   {
  MEPDESC* bm = (MEPDESC*)dsc;
  if ( bm && bm != MEP_INV_DESC && bm->owner != -1 )  {
    return mbm_register_free_event(bm->mepBuffer, mep_free_mep, bm);
  }
  return MBM_ERROR;
}

MEPID mep_include (const char* name, int partid, int selection) {
  std::string mep_buff_name("MEP");
  std::string evt_buff_name("EVENT");
  std::string res_buff_name("RESULT");
  std::auto_ptr<MEPDESC> bm(new MEPDESC);
  ::memset(bm.get(),0,sizeof(MEPDESC));
  ::strcpy(bm->mutexName,"MEP_Management");
  int status = lib_rtl_create_lock(bm->mutexName, &bm->lockid);
  if (!lib_rtl_is_success(status))    {
    lib_rtl_signal_message(LIB_RTL_OS,"Failed to create lock %s.",bm->mutexName);
    return MEP_INV_DESC;
  }
  bm->owner = lib_rtl_pid();
  bm->selection = selection;
  bm->mepBuffer  = ( selection&USE_MEP_BUFFER ) 
    ? mbm_include(mep_buff_name.c_str(), name, partid)
    : bm->mepBuffer = mbm_map_memory(mep_buff_name.c_str());
  if ( bm->mepBuffer == MBM_INV_DESC )  {
    lib_rtl_delete_lock(bm->lockid);
    return MEP_INV_DESC;
  }
  bm->mepStart = (int)bm->mepBuffer->buffer_add;

  bm->evtBuffer = ( selection&USE_EVT_BUFFER )
    ? mbm_include(evt_buff_name.c_str(), name, partid)
    : bm->evtBuffer = mbm_map_memory(evt_buff_name.c_str());
  if ( bm->evtBuffer == MBM_INV_DESC )  {
    _mep_exclude(bm->mepBuffer, bm->selection&USE_MEP_BUFFER);
    lib_rtl_delete_lock(bm->lockid);
    return MEP_INV_DESC;
  }
  bm->evtStart = (int)bm->evtBuffer->buffer_add;
  mbm_register_free_event(bm->evtBuffer, mep_free, bm.get());
  mbm_register_alloc_event(bm->evtBuffer, mep_declare, bm.get());

  bm->resBuffer = ( selection&USE_RES_BUFFER ) 
    ? mbm_include(res_buff_name.c_str(), name, partid)
    : mbm_map_memory(res_buff_name.c_str());
  if ( bm->resBuffer == MBM_INV_DESC )  {
    _mep_exclude(bm->evtBuffer, bm->selection&USE_EVT_BUFFER);
    _mep_exclude(bm->mepBuffer, bm->selection&USE_MEP_BUFFER);
    lib_rtl_delete_lock(bm->lockid);
    return MEP_INV_DESC;
  }
  bm->resStart = (int)bm->resBuffer->buffer_add;
  mbm_register_free_event(bm->resBuffer, mep_free, bm.get());
  mbm_register_alloc_event(bm->resBuffer, mep_declare, bm.get());
  return bm.release();
}

/// Exclude from buffer manager
int mep_exclude (MEPID dsc)  {
  MEPDESC* bm = (MEPDESC*)dsc;
  if ( bm && bm != MEP_INV_DESC && bm->owner != -1 )  {
    _mep_exclude(bm->resBuffer, bm->selection&USE_RES_BUFFER);
    _mep_exclude(bm->evtBuffer, bm->selection&USE_EVT_BUFFER);
    _mep_exclude(bm->mepBuffer, bm->selection&USE_MEP_BUFFER);
    return MBM_NORMAL;
  }
  return MBM_ILL_CONS;
}
