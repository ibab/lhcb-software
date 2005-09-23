#include "RTL/que.h"
#include "RTL/rtl.h"
#define IMPLEMENTING
#include "WT/wtdef.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cerrno>

enum Booleans { FALSE, TRUE };

/*----------------------------DEFINITIONS-------------------------------*/
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
  wt_enabled_fac_header(unsigned int f) : qentry(0,0), facility(f) {}
}; 

void wtc_print_space();
wt_fac_entry* _wtc_find_facility(unsigned int facility,qentry* fac_head);
static int _wtc_add_fired(wt_queue_entry* entry,wt_enabled_fac_header* mask_ptr,wt_fac_entry* fac);
int _wtc_restore_stack(void);
void _wtc_print_entry(wt_queue_entry *e);

/*----------------------------STATIC STORAGE----------------------------*/

static qentry    *fac_list;
static qentry    *wt_queue;
static qentry    *wt_stack;
static qentry    *wt_fired;
static int        inited = FALSE;
static int        wt_EventFlag;
static char       wt_mutex_name[32];
static void*      wt_mutex_id = 0;

class WTLock  {
  int m_status;
public:
  WTLock() {
    if ( inited )  {
      m_status = lib_rtl_lock(wt_mutex_name,wt_mutex_id);
      if ( !lib_rtl_is_success(m_status) )   {
        errno = m_status;
        ::printf("error in locking WT tables. Status %d\n",m_status);
      }
      return;
    }
    m_status = WT_NOTINIT;
  }
  virtual ~WTLock() {
    m_status = lib_rtl_unlock(wt_mutex_name,wt_mutex_id);
    if ( !m_status )  {
      // throw exception ?
    }
  }
  operator int ()   {
    return lib_rtl_is_success(m_status);
  }
  int status()      const   {
    return m_status;
  }
};


/*----------------------------------------------------------------------*/
static inline qentry *q_remove_head(qentry *head )   {
  qentry *entry = 0;
  remqhi (head, &entry);
  return entry;
}
/*----------------------------------------------------------------------*/
static inline qentry *q_remove( qentry *entry )  {
  qentry* head    = (qentry*)((int)entry+(int)entry->prev);
  return q_remove_head(head);
}
/*----------------------------------------------------------------------*/

int wtc_init()    {
  if ( !inited )  {
    ::sprintf(wt_mutex_name,"WT_%08X",lib_rtl_pid());
    int status = lib_rtl_create_lock(wt_mutex_name, &wt_mutex_id);
    if ( lib_rtl_is_success(status) )  {
      if ( !inited ) inited = TRUE;
      WTLock lock;
      if ( lock )  {
        int status = lib_rtl_create_event(&wt_EventFlag);
        if ( !lib_rtl_is_success(status) )  {
          return WT_NOEF;
        }
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

/*----------------------------------------------------------------------*/
int wtc_subscribe( int facility, wt_callback_t rearm, wt_callback_t action, void* par)   {
  WTLock lock;
  if ( lock )  {
    wt_fac_entry* fac = _wtc_find_facility(facility, fac_list);
    if ( fac == fac_list )   {
      fac = new wt_fac_entry(facility, rearm, action);
      insqti( fac, fac_list );        
    }
    if (rearm)  {
      wt_queue_entry *entry = new wt_queue_entry(facility, par);
      insqti(entry, wt_fired);        
    }
    return WT_SUCCESS;    
  }
  return lock.status();
}

/*----------------------------------------------------------------------*/
int wtc_remove(unsigned int facility)    {
  WTLock lock;
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
/*----------------------------------------------------------------------*/
int wtc_insert(unsigned int facility, void* userpar1)    {
  WTLock lock;
  if ( lock )  {
    qentry* e = new wt_queue_entry(facility, userpar1);
    if ( !e )   {
      wtc_print_space();
      exit (WT_NOROOM);
    }
    int status  = insqti(e,wt_queue);
    if (status == QUE_ONEENTQUE)  {
      lib_rtl_set_event(wt_EventFlag);
    }
    return WT_SUCCESS;
  }
  return lock.status();
}

/*----------------------------------------------------------------------*/
int wtc_insert_head(unsigned int facility, void* userpar1)   {
  WTLock lock;
  if ( lock )  {
    qentry* e = new wt_queue_entry(facility, userpar1);
    if ( !e )   {
      wtc_print_space();
      exit(WT_NOROOM);
    }
    int status  = insqhi(e,wt_queue);
    if (status == QUE_ONEENTQUE)    {
      lib_rtl_set_event (wt_EventFlag);
    }
    return WT_SUCCESS;
  }
  return lock.status();
}

/*----------------------------------------------------------------------*/
int wtc_test_input()    {
  WTLock lock;
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

/*----------------------------------------------------------------------*/
int wtc_wait_with_mask (unsigned int* facility, void** userpar1, int* sub_status, wt_enabled_fac_header* mask_ptr) {
  wt_queue_entry  *entry;
  wt_fac_entry    *fac;
  int mask_ok   = 0;

  if (mask_ptr != 0)  {
    if (mask_ptr->facility == WT_PATTERN) {
      mask_ok = 1;
    }
  }
  *facility   = 0;
  if (sub_status != 0)  {
    *sub_status = 0;
  }
  WTLock lock;
  if ( lock )  {
    while(TRUE)  {
      if (mask_ok != 1)   {
        while ( 0 != (entry=(wt_queue_entry*)q_remove_head(wt_fired)) ) {
          wt_fac_entry* fac = _wtc_find_facility(entry->facility,fac_list);
          if ( fac != fac_list)    {
            if ( fac->rearm != 0 )    {
              lib_rtl_unlock(wt_mutex_name,wt_mutex_id);
              (*fac->rearm)(fac->facility,entry->userpar1);
              lib_rtl_lock(wt_mutex_name,wt_mutex_id);
            }
          }
          delete entry;
        }
      }
      do    {
        if( !(entry = (wt_queue_entry*)q_remove_head( wt_queue )))  {
          lib_rtl_clear_event (wt_EventFlag);
          if( !(entry = (wt_queue_entry*)q_remove_head( wt_queue )))   {
            lib_rtl_unlock(wt_mutex_name,wt_mutex_id);
            lib_rtl_wait_for_event (wt_EventFlag);
            lib_rtl_lock(wt_mutex_name,wt_mutex_id);
          }
        }
      } while (!entry);
      *facility   = entry->facility;
      if (userpar1 != 0)    {
        *userpar1   = entry->userpar1;
      }
      if (mask_ok ==1)    {
        if ( _wtc_find_facility(*facility,mask_ptr) == (wt_fac_entry*)mask_ptr)  {
          wt_queue_entry* entry_ptr = new wt_queue_entry(*facility,*userpar1);
          insqhi(entry_ptr,wt_stack);
          delete entry;
          continue;
        }
      }
      fac = _wtc_find_facility(entry->facility,fac_list);
      if ( fac != fac_list )  {
        if ( fac->action )    {
          lib_rtl_unlock(wt_mutex_name,wt_mutex_id);
          int status = (*fac->action)(entry->facility, entry->userpar1);
          lib_rtl_lock(wt_mutex_name,wt_mutex_id);
          if ( status != WT_SUCCESS )    {
            if (sub_status != 0)   {
              *sub_status = status;
            }
            if (mask_ok ==1)   {
              _wtc_restore_stack();
            }
            _wtc_add_fired(entry,mask_ptr,fac);
            return WT_BADACTIONSTAT;
          }
          _wtc_add_fired(entry,mask_ptr,fac);
        }
        else   {
          if (mask_ok ==1)  {
            _wtc_restore_stack();
          }
          _wtc_add_fired(entry,mask_ptr,fac);
          return(WT_NOACTION);
        }
      }
      else    {
        if (mask_ok ==1)   {
          _wtc_restore_stack();
        }
        _wtc_add_fired(entry,mask_ptr,0);
        return(WT_NOSUBSCRIBED);
      }
    }
  }
  return lock.status();
}

/*----------------------------------------------------------------------*/
int wtc_flush (unsigned int facility ) {
  WTLock lock;
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

/*----------------------------------------------------------------------*/
int wtc_flushp (unsigned int facility, void* param )   {
  WTLock lock;
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

/*----------------------------------------------------------------------*/
int wtc_get_wait_ef(int* ef)  {
  *ef = wt_EventFlag;
  return WT_SUCCESS;
}

/*----------------------------------------------------------------------*/
int wtc_spy_next_entry(unsigned int* fac,void** par)   {
  WTLock lock;
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

/*----------------------------------------------------------------------*/
int wtc_create_enable_mask(wt_enabled_fac_header** p)  {
  *p = new wt_enabled_fac_header(WT_PATTERN);
  return WT_SUCCESS;
}

/*----------------------------------------------------------------------*/
int wtc_add_to_en_fac(wt_enabled_fac_header *h, unsigned int fac)  {
  if (h->facility == WT_PATTERN)  {
    WTLock lock;
    if ( lock )  {
      wt_enabled_fac_header *p1 = new wt_enabled_fac_header(fac);
      insqti(p1, h);
      return WT_SUCCESS;
    }
    return lock.status();
  }
  return WT_BADACTIONSTAT;
}

/*----------------------------------------------------------------------*/
int wtc_get_routines(unsigned int fac,wt_callback_t* rea,wt_callback_t* act) {
  WTLock lock;
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

/*----------------------------------------------------------------------*/
int wtc_wait(unsigned int* facility,void** userpar1,int* sub_status)   {
  return wtc_wait_with_mask(facility,userpar1,sub_status,0);
}

/*----------------------------------------------------------------------*/
void wtc_print_space()   {
  for (wt_queue_entry *fac = (wt_queue_entry*)((int)wt_queue->next+(int)wt_queue);
    fac != (void*)wt_queue ; fac = (wt_queue_entry*)((int)fac->next+(int)fac))
    _wtc_print_entry(fac);
}

/*----------------------------------------------------------------------*/
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

/*----------------------------------------------------------------------*/
int _wtc_restore_stack() {
  qentry  *entry;
  while ( 0 != (entry = q_remove_head(wt_stack)) )  {
    insqhi(entry,wt_queue);
  }
  return WT_SUCCESS;
}

/*----------------------------------------------------------------------*/
wt_fac_entry* _wtc_find_facility(unsigned int facility,qentry* fac_head)  {
  wt_fac_entry *fac = 0;
  for(fac = (wt_fac_entry*)((int)fac_head->next+(int)fac_head);
    fac != (wt_fac_entry*)fac_head ; fac = (wt_fac_entry*)((int)fac->next+(int)fac))
    if( fac->facility == facility ) break;
  return fac;
}

/*----------------------------------------------------------------------*/
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

/*----------------------------------------------------------------------*/
void _wtc_print_entry(wt_queue_entry *e)  {
  ::printf(" Entry facility: %8X  parameter:%p\n",e->facility, e->userpar1);
}

