#ifndef MBM_MEPDEF_H
#define MBM_MEPDEF_H 1

#include "MBM/bmdef.h"

enum MepBufferUsage  {
  USE_MEP_BUFFER = (1<<0),
  USE_EVT_BUFFER = (1<<1),
  USE_RES_BUFFER = (1<<2)
};

enum EvTypes  {
  EVENT_TYPE_OK = 0,
  EVENT_TYPE_MEP = 1,
  EVENT_TYPE_EVENT = 2,
  EVENT_TYPE_ERROR = 3,
  EVENT_TYPE_MISC  = 4,
  EVENT_TYPE_BADPROC = 5
};

enum MepPacking  {
  MEP_MAX_PACKING = 16
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
  long begin;
  int  evID;
};

struct MEPEVENT   {
  long begin;
  int  evID;
  int  packing;
  int  refCount;
  int  valid;
  int  magic;
  struct _SingleEvt  {
    int begin;
    int evID;
    int status;
    int signal;
  } events[MEP_MAX_PACKING];
  char data[4];
  static size_t hdrSize() {  return sizeof(MEPEVENT) - 4; }
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
  /* Set flag to trigger printout when MEP is released              */
  void mep_print_release(bool val);
  /* Increment reference count of MEP multi-event                   */
  int mep_increment(MEPID dsc, MEPEVENT* e, int val);
  /* Decrement reference count of MEP multi-event                   */
  int mep_decrement(MEPID dsc, MEPEVENT* e, int val);
  /* Install MEP buffer(s)                                          */
  int mep_install(int argc , char** argv);
  /* Scan and release MEP event until reference count is NULL.      */
  int mep_scan(MEPID bm, int delay);
  /* Set Flag to not map unused buffers.                            */
  void mep_map_unused_buffers(bool value);
  /* Optional callback to handle with MEP events, where the processing had errors  */
  int mep_set_error_call(MEPID dsc, void* param, void(*callback)(void* param, void* data,size_t length));
  /* Install signal handling to mark MEP events at destruction      */
  int mep_set_signal_handler(MEPID dsc, bool catch_signals);

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
