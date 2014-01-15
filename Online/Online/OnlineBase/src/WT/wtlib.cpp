#include "RTL/que.h"
#include "RTL/rtl.h"
#include "RTL/Lock.h"
#define IMPLEMENTING
#include "WT/wtdef.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cerrno>

//#define WT_USE_PIPES 1
#ifdef WT_USE_PIPES
  #ifdef _WIN32
    #include <io.h>
    #include <fcntl.h>
    #define pipe(x) _pipe(x,1024,_O_BINARY)
  #else
    #include <unistd.h> 
  #endif
#endif

//----------------------------DEFINITIONS-------------------------------
enum Booleans { FALSE, TRUE };
#define WT_PATTERN 0xfeadbabe

struct wt_fac_entry : public qentry  {
  unsigned int facility;
  wt_callback_t rearm;
  wt_callback_t action;
  wt_fac_entry(unsigned int f, wt_callback_t arm, wt_callback_t act)
    : qentry(0,0), facility(f), rearm(arm), action(act) {}
}; 

struct wt_queue_entry : public qentry  {
  unsigned int  facility;
  void*         userpar1;
  wt_queue_entry(unsigned int f, void* p1) 
  : qentry(0,0), facility(f), userpar1(p1) {}
}; 

struct wt_enabled_fac_header : public qentry  {
  unsigned int  facility;
  explicit wt_enabled_fac_header(unsigned int f) : qentry(0,0), facility(f) {}
}; 

void wtc_print_space();
wt_fac_entry* _wtc_find_facility(unsigned int facility,qentry* fac_head);
static int _wtc_add_fired(wt_queue_entry* entry,wt_enabled_fac_header* mask_ptr,wt_fac_entry* fac);
void _wtc_print_entry(wt_queue_entry *e);

//----------------------------STATIC STORAGE----------------------------

static qentry    *fac_list = 0;
static qentry    *wt_queue = 0;
static qentry    *wt_stack = 0;
static qentry    *wt_fired = 0;
static int        inited = FALSE;
static void*      wt_mutex_id = 0;

#ifdef WT_USE_PIPES
static int        pipe_rd_bytes = 0;
static int        pipe_wr_bytes = 0;
static int        pipe_desc[2];
#else
static lib_rtl_event_t wt_EventFlag;
#endif

typedef RTL::Lock WTLock;

//----------------------------------------------------------------------
static inline qentry *q_remove_head(qentry *head )   {
  qentry *entry = 0;
  remqhi (head, &entry);
  return entry;
}
//----------------------------------------------------------------------
static inline qentry *q_remove( qentry *entry )  {
  qentry* head = (qentry*)((char*)entry+(long)entry->prev);
  return q_remove_head(head);
}
//----------------------------------------------------------------------
int wtc_exit(void*)  {
#ifdef WT_USE_PIPES
  if ( pipe_desc[0] ) ::close(pipe_desc[0]);
  if ( pipe_desc[1] ) ::close(pipe_desc[1]);
  pipe_desc[1] = pipe_desc[0] = 0;
#endif
  return lib_rtl_delete_lock(wt_mutex_id);
}
//----------------------------------------------------------------------
int wtc_init()    {
  if ( !inited )  {
    int status = lib_rtl_create_lock(0, &wt_mutex_id);
    if ( lib_rtl_is_success(status) )  {
      if ( !inited ) {
        inited = TRUE;
        lib_rtl_declare_exit(wtc_exit, 0);
      }
      WTLock lock(wt_mutex_id);
      if ( lock )  {
#ifdef WT_USE_PIPES
        status = pipe(pipe_desc);
        if ( 0 != pipe(pipe_desc) ) {
          lib_rtl_output(LIB_RTL_ERROR,"WT: failed to create pipe...\n");
        }
#else
        status = lib_rtl_create_event(0, &wt_EventFlag);
        if ( !lib_rtl_is_success(status) )  {
          return WT_NOEF;
        }
#endif
        wt_queue = new qentry(0,0);
        wt_fired = new qentry(0,0);
        fac_list = new qentry(0,0);
        wt_stack = new qentry(0,0);
        return WT_SUCCESS;    
      }
      return lock.status();
    }
    return status;
  }
  return WT_SUCCESS;    
}
//----------------------------------------------------------------------
int wtc_subscribe( int facility, wt_callback_t rearm, wt_callback_t action, void* par)   {
  WTLock lock(wt_mutex_id);
  if ( lock )  {
    wt_fac_entry* fac = _wtc_find_facility(facility, fac_list);
    if ( fac == fac_list )   {
      fac = new wt_fac_entry(facility, rearm, action);
      insqti( fac, fac_list );        
    }
    else {
      fac->facility = facility;
      fac->action = action;
      fac->rearm = rearm;
    }
    if (rearm)  {
      wt_queue_entry *entry = new wt_queue_entry(facility, par);
      insqti(entry, wt_fired);        
    }
    return WT_SUCCESS;    
  }
  return lock.status();
}
//----------------------------------------------------------------------
int wtc_remove(unsigned int facility)    {
  WTLock lock(wt_mutex_id);
  if ( lock )  {
    qentry *e, park(0,0);
    wt_queue_entry  *entry;
    wt_fac_entry* fac = _wtc_find_facility(facility,fac_list);
    if ( fac == fac_list ) return(WT_NOSUBSCR);
    q_remove( fac );
    delete fac;
    while( (entry=(wt_queue_entry*)q_remove_head(wt_fired)) != 0 )  {
      if( entry->facility == facility ) 
        delete entry;
      else
        insqti( entry, &park );
    }
    while( (e = q_remove_head(&park)) != 0 )  {
      insqti(e, wt_fired);
    }
    return WT_SUCCESS;
  }
  return lock.status();
}
//----------------------------------------------------------------------
int wtc_insert(unsigned int facility, void* userpar1)    {
  WTLock lock(wt_mutex_id);
  if ( lock )  {
    qentry* e = new wt_queue_entry(facility, userpar1);
    if ( !e )   {
      wtc_print_space();
      exit (WT_NOROOM);
    }
    int status  = insqti(e,wt_queue);
    //lib_rtl_output(LIB_RTL_DEBUG,"wtc_insert: Inserted entry: %d %p\n",facility,userpar1);
    if (status == QUE_ONEENTQUE)  {
#ifdef WT_USE_PIPES
      pipe_wr_bytes += write(pipe_desc[1],e,sizeof(e));
#else
      lib_rtl_set_event(wt_EventFlag);
#endif
    }
    return WT_SUCCESS;
  }
  return lock.status();
}
//----------------------------------------------------------------------
int wtc_insert_head(unsigned int facility, void* userpar1)   {
  WTLock lock(wt_mutex_id);
  if ( lock )  {
    qentry* e = new wt_queue_entry(facility, userpar1);
    if ( !e )   {
      wtc_print_space();
      exit(WT_NOROOM);
    }
    int status  = insqhi(e,wt_queue);
    if (status == QUE_ONEENTQUE)    {
#ifdef WT_USE_PIPES
      pipe_wr_bytes += write(pipe_desc[1],e,sizeof(e));
#else
      lib_rtl_set_event (wt_EventFlag);
#endif
    }
    return WT_SUCCESS;
  }
  return lock.status();
}
//----------------------------------------------------------------------
int wtc_test_input()    {
  WTLock lock(wt_mutex_id);
  if ( lock )  {
    qentry *e = q_remove_head( wt_queue );
    if( e )  {
      insqhi( e, wt_queue);
      return WT_SUCCESS;
    }
    return WT_ERROR;
  }
  return lock.status();
}
//----------------------------------------------------------------------
int wtc_wait_with_mask (unsigned int* facility, void** userpar1, int* sub_status, wt_enabled_fac_header* mask_ptr) {
  wt_queue_entry  *entry;
  wt_fac_entry    *fac;
  int mask_ok   = 0;
  int status;
  if (mask_ptr != 0)  {
    if (mask_ptr->facility == WT_PATTERN) {
      mask_ok = 1;
    }
  }
  *facility   = 0;
  if (sub_status != 0)  {
    *sub_status = 0;
  }
  WTLock lock(wt_mutex_id);
  if ( lock )  {
    for(;;)  {
      if (mask_ok != 1)   {
        while ( 0 != (entry=(wt_queue_entry*)q_remove_head(wt_fired)) ) {
          fac = _wtc_find_facility(entry->facility,fac_list);
          if ( fac != fac_list)    {
            if ( fac->rearm != 0 )    {
              WTLock unlock(wt_mutex_id,true);
              (*fac->rearm)(fac->facility,entry->userpar1);
            }
          }
          delete entry;
        }
      }
      do    {
        entry = (wt_queue_entry*)q_remove_head( wt_queue );
        if( !entry )  {
#ifdef WT_USE_PIPES
          WTLock unlock(wt_mutex_id,true);
          void* p = 0;
          int cnt = read(pipe_desc[0],&p,sizeof(p));
          if ( cnt == -1 ) {
            lib_rtl_signal_message(LIB_RTL_OS,"Error reading WT pipe!");
          }
          else  pipe_rd_bytes += cnt;
#else
          lib_rtl_clear_event (wt_EventFlag);
          WTLock unlock(wt_mutex_id, true);
          lib_rtl_wait_for_event(wt_EventFlag);
#endif
        }
      } while (!entry);
      *facility   = entry->facility;
      if (userpar1 != 0)    {
        *userpar1   = entry->userpar1;
      }
      if (mask_ok ==1)    {
        if ( _wtc_find_facility(*facility,mask_ptr) == (wt_fac_entry*)mask_ptr)  {
	  void* par = userpar1 ? *userpar1 : 0;
          wt_queue_entry* entry_ptr = new wt_queue_entry(*facility,par);
          insqhi(entry_ptr,wt_stack);
          delete entry;
          continue;
        }
      }
      fac = _wtc_find_facility(entry->facility,fac_list);
      if ( fac != fac_list )  {
        if ( fac->action )    {
          try  {
            WTLock unlock(wt_mutex_id,true);
            status = (*fac->action)(entry->facility, entry->userpar1);
          }
          catch(...)  {
            ::lib_rtl_output(LIB_RTL_ERROR,"wtc_wait_with_mask> Exception!\n");
            status = WT_BADACTIONSTAT;
          }
          if ( status != WT_SUCCESS )    {
            if (sub_status != 0)   {
              *sub_status = status;
            }
            if (mask_ok ==1)   {
              wtc_restore_stack();
            }
            _wtc_add_fired(entry,mask_ptr,fac);
            return WT_BADACTIONSTAT;
          }
          _wtc_add_fired(entry,mask_ptr,fac);
        }
        else   {
          if (mask_ok ==1)  {
            wtc_restore_stack();
          }
          _wtc_add_fired(entry,mask_ptr,fac);
          return WT_NOACTION;
        }
      }
      else    {
        if (mask_ok ==1)   {
          wtc_restore_stack();
        }
        _wtc_add_fired(entry,mask_ptr,0);
        return WT_NOSUBSCRIBED;
      }
    }
  }
  return lock.status();
}
//----------------------------------------------------------------------
int wtc_flush (unsigned int facility ) {
  WTLock lock(wt_mutex_id);
  if ( lock )  {
    wt_queue_entry  *entry;
    qentry *e, park(0,0);
    while( 0 != (entry=(wt_queue_entry*)q_remove_head(wt_queue)) )  {
      if( entry->facility == facility ) 
        delete entry;
      else
        insqti( entry, &park );
    }
    while( 0 != (e=q_remove_head(&park)) )  {
      insqti( e, wt_queue );
    }
    return WT_SUCCESS;
  }
  return lock.status();
}
//----------------------------------------------------------------------
int wtc_flushp (unsigned int facility, void* param )   {
  WTLock lock(wt_mutex_id);
  if ( lock )  {
    wt_queue_entry  *entry;
    qentry *e, park(0,0);
    if ( !inited ) return (WT_NOTINIT);
    while( 0 != (entry=(wt_queue_entry*)q_remove_head(wt_queue)) )  {
      if( (entry->facility == facility) && (entry->userpar1 == param ) )
        delete entry;
      else
        insqti( entry, &park );
    }
    while( 0 != (e=q_remove_head(&park)) ) {
      insqti( e, wt_queue );
    }
    return WT_SUCCESS;
  }
  return lock.status();
}
//----------------------------------------------------------------------
int wtc_get_wait_ef(lib_rtl_event_t* ef)  {
#ifdef WT_USE_PIPES
  *ef = pipe_desc;
#else
  *ef = wt_EventFlag;
#endif
  return WT_SUCCESS;
}
//----------------------------------------------------------------------
int wtc_spy_next_entry(unsigned int* fac,void** par)   {
  WTLock lock(wt_mutex_id);
  if ( lock )  {
    wt_queue_entry  *entry  = (wt_queue_entry*)q_remove_head( wt_queue );
    if( entry )  {
      *fac  = entry->facility;
      *par  = entry->userpar1;
      insqhi(entry, wt_queue);
      return WT_SUCCESS;
    }
    return WT_ERROR;
  }
  return lock.status();
}
//----------------------------------------------------------------------
int wtc_create_enable_mask(wt_enabled_fac_header** p)  {
  *p = new wt_enabled_fac_header(WT_PATTERN);
  return WT_SUCCESS;
}
//----------------------------------------------------------------------
int wtc_add_to_en_fac(wt_enabled_fac_header *h, unsigned int fac)  {
  if (h->facility == WT_PATTERN)  {
    WTLock lock(wt_mutex_id);
    if ( lock )  {
      wt_enabled_fac_header *p1 = new wt_enabled_fac_header(fac);
      insqti(p1, h);
      return WT_SUCCESS;
    }
    return lock.status();
  }
  return WT_BADACTIONSTAT;
}
//----------------------------------------------------------------------
int wtc_get_routines(unsigned int fac,wt_callback_t* rea,wt_callback_t* act) {
  WTLock lock(wt_mutex_id);
  if ( lock )  {
    wt_fac_entry *f = _wtc_find_facility(fac,fac_list);
    if ( f == fac_list )   {
      *rea  = 0;
      *act  = 0;
      return WT_NOSUBSCR;
    }
    else  {
      *act  = f->action;
      *rea  = f->rearm;
    }
    return WT_SUCCESS;
  }
  return lock.status();
}
//----------------------------------------------------------------------
int wtc_wait(unsigned int* facility,void** userpar1,int* sub_status)   {
  return wtc_wait_with_mask(facility,userpar1,sub_status,0);
}
//----------------------------------------------------------------------
void wtc_print_space()   {
  for (wt_queue_entry *fac = (wt_queue_entry*)((char*)wt_queue->next+(long)wt_queue);
    fac != (void*)wt_queue ; fac = (wt_queue_entry*)((char*)fac->next+(long)fac))
    _wtc_print_entry(fac);
}
//----------------------------------------------------------------------
int wtc_error(int status)  {
  switch(status)  {
    case WT_SUCCESS:
    case WT_ERROR:
    case WT_ALREADYINIT:
    case WT_NOTINIT:
    case WT_NOROOM:
    case WT_NOSUBSCR:
    case WT_BADACTIONSTAT:
    case WT_NOACTION:
    case WT_NOSUBSCRIBED:
    case WT_CHILDDEATH:
    case WT_NOEF:
    default:
      break;
  }
  return WT_SUCCESS;
}
//----------------------------------------------------------------------
int wtc_add_stack(unsigned int fac,void* param)   {
  if ( !inited ) return (WT_NOTINIT);
  wt_queue_entry *e = new wt_queue_entry(fac, param);
  insqhi(e, wt_stack);
  return WT_SUCCESS;
}
//----------------------------------------------------------------------
int wtc_restore_stack() {
  qentry  *entry;
  while ( 0 != (entry = q_remove_head(wt_stack)) )  {
    insqhi(entry,wt_queue);
  }
  return WT_SUCCESS;
}
//----------------------------------------------------------------------
wt_fac_entry* _wtc_find_facility(unsigned int facility,qentry* fac_head)  {
  wt_fac_entry *fac = 0;
  for(fac = (wt_fac_entry*)((char*)fac_head->next+(long)fac_head);
    fac != (wt_fac_entry*)fac_head ; fac = (wt_fac_entry*)((char*)fac->next+(long)fac))
    if( fac->facility == facility ) break;
  return fac;
}
//----------------------------------------------------------------------
static int _wtc_add_fired(wt_queue_entry* entry,wt_enabled_fac_header* mask_ptr,wt_fac_entry* fac)  {
  int mask_ok   = 0;
  if (mask_ptr != 0)  {
    if (mask_ptr->facility == WT_PATTERN)    {
      mask_ok = 1;
    }
  }
  if (mask_ok != 1)  {
    insqti(entry, wt_fired);
  }
  else  {
    if ((qentry*)_wtc_find_facility(entry->facility,mask_ptr) != mask_ptr)  {
      if (fac == (void*)0)      {
        delete entry;
        return WT_SUCCESS;
      }
      if (fac->rearm == 0)
        delete entry;
      else 
        insqti(entry, wt_fired);
    }
    else    {
      if (fac == 0)      {
        delete entry;
        return WT_SUCCESS;
      }
      insqti(entry, wt_fired);
    }
  }
  return WT_SUCCESS;
}
//----------------------------------------------------------------------
void _wtc_print_entry(wt_queue_entry *e)  {
  ::lib_rtl_output(LIB_RTL_ALWAYS," Entry facility: %8X  parameter:%p\n",e->facility, e->userpar1);
}
