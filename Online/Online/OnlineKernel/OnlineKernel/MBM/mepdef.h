#ifndef MBM_MEPDEF_H
#define MBM_MEPDEF_H 1

#include "MBM/bmdef.h"

enum MepBufferUsage  {
  USE_MEP_BUFFER = (1<<0),
  USE_EVT_BUFFER = (1<<1),
  USE_RES_BUFFER = (1<<2)
};

enum EvTypes  {
  EVENT_TYPE_MEP = 1,
  EVENT_TYPE_EVENT = 2,
  EVENT_TYPE_ERROR = 3,
  EVENT_TYPE_MISC  = 4
};

typedef struct _MEPID {
  BMID    resBuffer;
  BMID    evtBuffer;
  BMID    mepBuffer;
  long    resStart;
  long    evtStart;
  long    mepStart;
  int     selection;
  char    processName[32];
  int     partitionID;
} *MEPID;

struct MEP_SINGLE_EVT  {
  int  begin;
  int  evID;
};

struct MEPEVENT   {
  long begin;
  int  evID;
  int  packing;
  int  refCount;
  int  valid;
  int  magic;
  char data[4];
};

#ifdef __cplusplus
extern "C"  {
#endif
  /* Include tasks into MEP buffers                                 */
  MEPID mep_include (const char* name, int partid, int selection);
  /* Exclude from MEP buffer manager                                */
  int mep_exclude (MEPID bm);
  /* Access MEP magic pattern                                       */
  int mep_magic_pattern();
  /* Cancel pending requests to the MEP buffers                     */
  int mep_cancel_request(MEPID dsc);
  /* Pause access to MEP buffers                                    */
  int mep_pause (MEPID dsc);
  /* Not for public use                                             */
  int mep_set_watch(MEPID bm);
  void mep_print_release(bool val);
  int mep_increment(MEPID dsc, MEPEVENT* e, int val);
  int mep_decrement(MEPID dsc, MEPEVENT* e, int val);
  int mep_install(int argc , char** argv);
  int mep_scan(MEPID bm, int delay);
  void mep_map_unused_buffers(bool value);
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

#endif // MBM_MEPDEF_H
