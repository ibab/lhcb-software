#ifndef MBM_MEPDEF_H
#define MBM_MEPDEF_H 1

#include "MBM/bmdef.h"

enum MepBufferUsage  {
  USE_MEP_BUFFER = (1<<0),
  USE_EVT_BUFFER = (1<<1),
  USE_RES_BUFFER = (1<<2)
};

typedef struct _MEPID {
  BMID    resBuffer;
  BMID    evtBuffer;
  BMID    mepBuffer;
  int     resStart;
  int     evtStart;
  int     mepStart;
  int     selection;
} *MEPID;

struct MEP_SINGLE_EVT  {
  int event;
  int begin;
};

struct MEPEVENT   {
  int mepBufferID;
  int packing;
  int refCount;
  int begin;
  int valid;
  MEP_SINGLE_EVT events[1];
  static size_t length(int num_evt)   {
    return 3*sizeof(int)+num_evt*sizeof(MEP_SINGLE_EVT);
  }
};

#ifdef __cplusplus
extern "C"  {
#endif
  MEPID mep_include (const char* name, int partid, int selection);
  int mep_set_watch(MEPID bm);
  int mep_exclude (MEPID bm);
#ifdef __cplusplus
}
#endif

#define MEP_INV_DESC   ((MEPID)-1)
#define mep_add_req             mbm_add_req
#define mep_del_req             mbm_del_req
#define mep_get_event           mbm_get_event
#define mep_free_event          mep_free_event
#define mep_pause               mbm_pause
#define mep_get_space           mbm_get_space
#define mep_get_space_a         mbm_get_space_a
#define mep_wait_space          mbm_wait_space
#define mep_wait_space_a        mbm_wait_space_a
#define mep_free_space          mbm_free_space
#define mep_declare_event       mbm_declare_event
#define mep_send_space          mbm_send_space
#define mep_mep_cancel_request  mbm_cancel_request

#endif // MBM_MEPDEF_H
