#ifndef _MBM_INTERNALS_H
#define _MBM_INTERNALS_H
#include "RTL/rtl.h"
#include "RTL/que.h"
#include "RTL/bits.h"
#include "MBM/bmdef.h"

#ifdef MBM_IMPLEMENTATION
struct BMDESCRIPT;
struct CONTROL;
struct EVENT;
struct USER;
#else
typedef long BMDESCRIPT;
typedef long CONTROL;
typedef long EVENT;
typedef long USER;
typedef long TriggerMask;
typedef long UserMask;
#endif

extern "C" {
  ///  find free user slot 
  USER* _mbm_ualloc     (BMID bmid);
  int _mbm_uclean       (BMID bmid);
  /// free user slot
  int _mbm_ufree        (BMID bm, USER* us);
  int _mbm_flush_sections(BMID bm);
  EVENT* _mbm_ealloc    (BMID id, USER* us);
  int _mbm_efree        (BMID bm, EVENT* e);  /* de-allocate event slot */
  int _mbm_sfree        (BMID bm, int , int);

  int _mbm_del_event    (BMID bm, EVENT* e, int len);
  int _mbm_rel_event    (BMID bm, USER* us);

  /* try to get event ... */
  int _mbm_get_ev       (BMID bm, USER* us);
  int _mbm_del_wes      (BMID bm, USER* us);
  int _mbm_check_wes    (BMID);
  /// add user in the wait_event_slot queue
  int _mbm_add_wes      (BMID bm, USER *us, RTL_ast_t astadd);
  /// add user in wait_event queue
  int _mbm_add_wev      (BMID bm, USER *us, int** ptr, int* size, int* evtype, TriggerMask* trmask, int part_id, RTL_ast_t astadd, void* astpar);

  int _mbm_del_wsp      (BMID bmid, USER*);
  int _mbm_get_sp       (BMID bmid, USER*, int , int**);
  int _mbm_check_rmode  (BMID bmid);
  int _mbm_check_wsp    (BMID bmid);
  /// add user in the wait_space queue
  int _mbm_add_wsp      (BMID bm, USER* us, int size, int** ptr, RTL_ast_t astadd, void* astpar);

  int _mbm_findnam      (BMID bm, const char* name);
  /// find matching req
  int _mbm_match_req    (BMID bm, int partid, int evtype, 
                         TriggerMask& trmask, UserMask& mask0, UserMask& mask1, UserMask& mask2);
  int _mbm_check_wev    (BMID bmid,EVENT*);
  int _mbm_del_wev      (BMID bmid,USER*);
  /// Wakeup process to continue processing
  int _mbm_wake_process (BMID bmid, int reason, USER* us);

  int _mbm_shutdown     (void* param);
  int _mbm_fill_offsets ();
  int _mbm_find_buffer  (const char* bm_name);

  int _mbm_exit(void* param);

  int _mbm_declare_event(BMID bm, int len, int evtype, TriggerMask& trmask,
                         const char* dest, void** free_add, 
                         int* free_size, int part_id, int wait);

  int _mbm_check_freqmode(BMID bm);
  int _mbm_send_space   (BMID bm);

  int _mbm_lock_tables  (BMID bmid);
  int _mbm_unlock_tables(BMID bmid);
  int _mbm_delete_lock  (BMID bmid);
  /// Wait event space AST function
  int _mbm_wes_ast      (void* par);
  /// Map buffer manager memory sections
  int _mbm_map_sections (BMID bmid);
  /// Unmap buffer manager memory sections
  int _mbm_unmap_sections(BMID bmid);
}

#define _CHECK( x )  { int sc = x ; if ( !(sc&1) ) { lib_rtl_printf ( "Error in:%s, status=%d\n", #x , sc ); return sc; } }

#endif // _MBM_INTERNALS_H
