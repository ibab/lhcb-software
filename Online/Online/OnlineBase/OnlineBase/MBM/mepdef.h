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

struct MEP_SINGLE_EVT  {
  long begin;
  int  evID;
};

struct MEPEVENT   {
  //long buffer[16];
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

/* Access MEP magic pattern                                       */
static inline int mep_magic_pattern() {   return 0xFEEDBABE; }

#endif // MBM_MEPDEF_H
