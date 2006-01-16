/*-----------------------------------------------------------------------
*
*   OS-9 BUFFER MANAGER
*
* Edition History
*
*  #   Date     Comments                                              By
* -- -------- ------------------------------------------------------ ---
*  0  28/09/88  Initial version                                       MF
*-----------------------------------------------------------------------*/
#define MBM_IMPLEMENTATION

#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <memory>
#include "RTL/Lock.h"
#include "bm_struct.h"
#include "MBM/mepdef.h"
#define MAGIC_PATTERN int(0xFEEDBABE)

static int print_release = false;
// static int print_addref = false;


struct MEPDESC : public _MEPID  {
  int             owner;
  lib_rtl_lock_t  lockid;
  char            mutexName[64];
  MEPDESC() : owner(0), lockid(0) {
    resBuffer = MBM_INV_DESC;
    evtBuffer = MBM_INV_DESC;
    mepBuffer = MBM_INV_DESC; 
  }
};

static int _mep_change_refcount(MEPDESC* dsc,MEP_SINGLE_EVT* evt, int change)  {
  if ( evt )   {
    MEPEVENT* e = (MEPEVENT*)(int*)(dsc->mepStart + evt->begin);
    static int counter = 0;
    int cnt = 0;
    {
      //RTL::Lock lock(dsc->lockid);
      //if ( !lock )  {
      //  ::printf("Failed to aquire lock:%s\n",dsc->mutexName);
      //}
      switch(change) {
      case 2:
        ++e->refCount;
      case 1:
        cnt = ++e->refCount;
        break;
      case -2:
        --e->refCount;
      case -1:
        cnt = --e->refCount;
        break;
      default:
        ::printf("Unknown MEP change request:%d\n",change);
        break;
      }
    }
    //if ( (change < 0 && print_release) || (change>0 && print_addref) )  {
    if ( print_release )  {
      ::printf("MEP RefCount[%d] [%d, %d] %s [%d] Event at address %p MEP:%p [%d,%d] Pattern:%08X\n",
        ++counter, e->refCount, change, change > 0 ? "AddRef" : "DelRef", cnt, 
        (void*)evt, (void*)e, e->evID, evt->evID, e->magic);
    }
    if ( cnt < 1 || e->magic != MAGIC_PATTERN || e->valid != 1 )  {
      ::printf("MEP RefCount ERROR %s [%d] Event at address %p MEP:%p [%d,%d] Pattern:%08X\n",
        change > 0 ? "AddRef" : "DelRef", cnt, 
        (void*)evt, (void*)e, e->evID, evt->evID, e->magic);
    }
    return MBM_NORMAL;
  }
  ::printf("MEP RefCount ERROR ----------------------------- NO EVT -----------------------------\n");
  // Error
  return MBM_ERROR;
}
void mep_print_release(bool val)  {
  print_release = val;
}
static int mep_free_mep(void* param)   {
  void** pars = (void**)param;
  MEPEVENT* e = (MEPEVENT*)pars[2];
  MEPDESC* dsc = (MEPDESC*)pars[1];
  if ( e->refCount < 1 || e->magic != MAGIC_PATTERN )  {
    printf("MEP RefCount ERROR(2) [%d] Event at address %08X MEP:%p [%d] Pattern:%08X [Release MEP]\n",
      e->refCount, dsc->mepStart+e->begin, (void*)e, e->evID, e->magic);
  }
  int cnt = 0;
  while ( 1 )  {
#ifdef _WIN32
    lib_rtl_sleep(1);
#else
    lib_rtl_usleep(100);
#endif
    {
      printf(".");
      if ( (++cnt%50)==0 )  {
        printf("WAIT MEP release [%d] Event at address %08X MEP:%p [%d] Pattern:%08X\n",
          e->refCount, dsc->mepStart+e->begin, (void*)e, e->evID, e->magic);
      }
      RTL::Lock lock(dsc->lockid);
      if ( e->refCount <=1 )    {
        printf("MEP release [%d] Event at address %08X MEP:%p [%d] Pattern:%08X\n",
          e->refCount, dsc->mepStart+e->begin, (void*)e, e->evID, e->magic);
        e->valid = 0;
        e->refCount = 0;
        break;
      }
    }
  }
  e->valid = 0;
  e->refCount = 0;
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
    : mbm_map_memory(mep_buff_name.c_str());
  if ( bm->mepBuffer == MBM_INV_DESC )  {
    lib_rtl_delete_lock(bm->lockid);
    return MEP_INV_DESC;
  }
  bm->mepStart = (int)bm->mepBuffer->buffer_add;
  //mbm_register_free_event(bm->mepBuffer, mep_free, bm.get());
  //mbm_register_alloc_event(bm->mepBuffer, mep_declare, bm.get());

  bm->evtBuffer = ( selection&USE_EVT_BUFFER )
    ? mbm_include(evt_buff_name.c_str(), name, partid)
    : mbm_map_memory(evt_buff_name.c_str());
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
int mep_decrement(MEPID dsc, MEPEVENT* e, int val)  {
  if ( val != 0 )  {
    MEPDESC* bm = (MEPDESC*)dsc;
    if ( bm && bm != MEP_INV_DESC && bm->owner != -1 )  {
      RTL::Lock lock(bm->lockid);
      if ( !lock )  {
        ::printf("Failed to aquire lock:%s\n",bm->mutexName);
      }
      e->refCount -= val;
      return MBM_NORMAL;
    }
    return MBM_ILL_CONS;
  }
  return MBM_NORMAL;
}
int mep_increment(MEPID dsc, MEPEVENT* e, int val)  {
  if ( val != 0 )  {
    MEPDESC* bm = (MEPDESC*)dsc;
    if ( bm && bm != MEP_INV_DESC && bm->owner != -1 )  {
      RTL::Lock lock(bm->lockid);
      if ( !lock )  {
        ::printf("Failed to aquire lock:%s\n",bm->mutexName);
      }
      e->refCount += val;
      return MBM_NORMAL;
    }
    return MBM_ILL_CONS;
  }
  return MBM_NORMAL;
}
int mep_check(MEPID dsc, MEPEVENT* e)  {
  MEPDESC* bm = (MEPDESC*)dsc;
  if ( bm && bm != MEP_INV_DESC && bm->owner != -1 )  {
    printf("MEPCheck: [%d] Event at address %08X MEP:%p [%d] Pattern:%08X [Release MEP]\n",
      e->refCount, dsc->mepStart+e->begin, (void*)e, e->evID, e->magic);
    if ( e->refCount == 4 )  {
      RTL::Lock lock(bm->lockid);
      e->refCount -= 2;
    }
    return MBM_NORMAL;
  }
  return MBM_ILL_CONS;
}

int mep_magic_pattern()   {
  return MAGIC_PATTERN;
}
