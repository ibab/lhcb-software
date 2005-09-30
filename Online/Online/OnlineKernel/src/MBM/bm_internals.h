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
  ///	find free user slot 
  int _mbm_ualloc (BMDESCRIPT *);
  int _mbm_uclean (BMDESCRIPT *);
  /// free user slot
  int _mbm_ufree (BMDESCRIPT *bm, int i);
  int _mbm_flush_sections(BMDESCRIPT* bm);
  int _mbm_ealloc (BMDESCRIPT*);
  int _mbm_efree (BMDESCRIPT *, int);	/* de-allocate event slot */

  int _mbm_sfree (BMDESCRIPT *, int , int);

  int _mbm_rel_event (BMDESCRIPT *, int);
  /* try to get event ... */
  int _mbm_get_ev (BMDESCRIPT *bm, USER* us);
  int _mbm_del_wes ( BMDESCRIPT* , USER* );
  int _mbm_check_wes (BMDESCRIPT*);
  /// add user in the wait_event_slot queue
  int _mbm_add_wes (BMDESCRIPT *bm, USER *us, MBM_ast_t astadd);
  /// add user in wait_event queue
  int _mbm_add_wev (BMDESCRIPT *bm, USER *us, int** ptr, int* size, int* evtype, TriggerMask* trmask, int part_id, MBM_ast_t astadd, void* astpar);

  int _mbm_del_wsp (BMDESCRIPT *, USER*);
  int _mbm_get_sp (BMDESCRIPT *, USER*, int , int**);
  int _mbm_check_rmode (BMDESCRIPT *);
  int _mbm_check_wsp (BMDESCRIPT *bmid, int bit, int nbit);
  /// add user in the wait_space queue
  int _mbm_add_wsp (BMDESCRIPT *bm, USER* us, int size, int** ptr, MBM_ast_t astadd, void* astpar);

  int _mbm_findnam (BMDESCRIPT *bm, const char* name);
  /// find matching req
  int _mbm_match_req (BMDESCRIPT *bm, int partid, int evtype, TriggerMask& trmask, UserMask& mask0, UserMask& mask1);
  int _mbm_check_wev (BMDESCRIPT*,EVENT*);
  int _mbm_del_wev (BMDESCRIPT*,USER*);
  int _mbm_req_upd (BMDESCRIPT*,USER*);
  /// Wakeup process to continue processing
  int _mbm_wake_process (int reason, USER* us);

  int _mbm_shutdown (void* param);
  int _mbm_fill_offsets();
  int _mbm_find_buffer (const char* bm_name);

  int _mbm_exit(void* param);

  int _mbm_declare_event (BMDESCRIPT *bm, int len, int evtype, TriggerMask& trmask,
    const char* dest, void** free_add, int* free_size, int part_id);

  int _mbm_restore_park_queue(qentry_t *p, qentry_t *d);

  int _mbm_check_freqmode (BMDESCRIPT *bm);
  int _mbm_send_space (BMDESCRIPT *bm);

  int _mbm_create_section(const char *section_name, int size, lib_rtl_gbl_t* address);
  int _mbm_delete_section(lib_rtl_gbl_t handle);
  /// Map global section: address is quadword: void*[2]
  int _mbm_map_section(const char *section_name, int size, lib_rtl_gbl_t* address);
  /// Unmap global section: address is quadword: void*[2]
  int _mbm_unmap_section(lib_rtl_gbl_t address);

  int _mbm_create_lock(BMDESCRIPT *);
  int _mbm_lock_tables(BMDESCRIPT *);
  int _mbm_unlock_tables(BMDESCRIPT *);
  int _mbm_delete_lock(BMDESCRIPT *);
  int _mbm_cancel_lock (BMDESCRIPT *);
  /// Wait event space AST function
  int _mbm_wes_ast(void* par);
};

#define _CHECK( x )  { int sc = x ; if ( !(sc&1) ) { printf ( "Error in:%s, status=%d\n", #x , sc ); return sc; } }

#endif // _MBM_INTERNALS_H
