// $Id: $
//==========================================================================
//  LHCb Online software suite
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see OnlineSys/LICENSE.
//
// Author     : M.Frank
//
//==========================================================================
#ifndef _MBM_MBMDEF_H 
#define _MBM_MBMDEF_H

#include "MBM/bmssdef.h"
#include "RTL/rtl.h"

enum MBM_communications  {
  BM_COM_NONE = 0,
  BM_COM_FIFO = 1,
  BM_COM_ASIO = 2
};

enum MBM_dimensions  {
  BM_MASK_SIZE     =  4,
  BM_BUFF_NAME_LEN = 32,
  BM_USER_NAME_LEN = 64
};

enum MBM_Internals  {
  BM_MAX_REQS   = 8,
  BM_MAX_EVTYPE = 10,
  BM_MAX_REQONE = 12,   // This must be a multiple of 4!!!
  BM_MAX_THREAD = 32
};

enum MBM_MaskTypes  {
  BM_MASK_ANY   = 0,
  BM_MASK_ALL   = 1,
  BM_MASK_REQ   = 0xFF  // Only used if Request type is BM_REQ_ONE
};

enum MBM_RequestTypes  {
  BM_NOTALL       = 0,
  BM_REQ_USER     = 0,
  BM_REQ_VIP      = 1,
  BM_REQ_ALL      = 1,
  BM_REQ_ONE      = 2
};

enum MBM_FrequencyTypes  {
  BM_FREQ_MANY = 0,
  BM_FREQ_PERC = 1
};

typedef int (*RTL_ast_t)(void*);
struct ServerBMID_t;
typedef ServerBMID_t* ServerBMID;

#ifdef MBM_IMPLEMENTATION
struct BMDESCRIPT;
struct CONTROL;
struct USER;
struct EVENT;
struct BufferMemory;
typedef BMDESCRIPT* BMID;
#else
typedef void* BMID;
typedef void* USER;
typedef void* CONTROL;
typedef void* EVENT;
typedef void  BufferMemory;
#endif

#define MBM_INV_DESC   ((BMID)-1)
#define MBM_INV_MEMORY ((BufferMemory*)-1)

#ifdef __cplusplus
#include <map>
#include <string>
#ifdef __cplusplus
std::map<std::string,ServerBMID> mbm_multi_install(int argc , char** argv);
#endif
#define __MBM_CONST const
extern "C"  {
#endif
  struct BUFFERS;
  int  mbm_qmtest_check_no_active_buffers(int, char**);
  int  mbm_qmtest_check_start();

  ServerBMID  mbm_install_server(int argc , char** argv);
  /// Add consumer requirement to server
  int mbmsrv_require_consumer(ServerBMID bm, const char* name, int partid, int evtype, const unsigned int mask[]);
  /// Deregister consumer requirement from server
  int mbmsrv_unrequire_consumer(ServerBMID bm, const char* name, int partid, int evtype, const unsigned int mask[]);

  int  mbm_deinstall(int argc , char** argv);
  int  mbm_dump(int argc , char** argv);
  int  mbm_mon(int argc , char** argv);

  /// Include as client in buffer manager
#ifdef __cplusplus
  BMID mbm_include (const char* bm_name, const char* name, int partid, int com_type=BM_COM_FIFO);
#else
  BMID mbm_include (const char* bm_name, const char* name, int partid, int com_type);
#endif
  int  mbm_exclude (BMID bm);
  __MBM_CONST char* mbm_buffer_address(BMID);
  int  mbm_clear(BMID bm);

  int  mbm_add_req (BMID bm, int evtype, __MBM_CONST unsigned int* trmask, __MBM_CONST unsigned int* veto, int masktype, 
                    int usertype, int freqmode, float freq);
  int  mbm_del_req    (BMID bm, int evtype, __MBM_CONST unsigned int* trmask, __MBM_CONST unsigned int* veto, int masktype, int usertype);
  int  mbm_get_event_a(BMID bm, int** ptr, int* size, int* evtype, unsigned int* trmask, int part_id, RTL_ast_t astadd, void* ast_par);
  int  mbm_get_event  (BMID bm, int** ptr, int* size, int* evtype, unsigned int* trmask, int part_id);
  int  mbm_free_event (BMID bm);
  int  mbm_pause      (BMID bm);

  // Not implemented: int  mbm_get_space  (BMID bm, int size, int** ptr);
  int  mbm_get_space_a(BMID bm, int size, int** ptr, RTL_ast_t astadd, void* ast_par);
  // Returns MBM_NO_ROOM if space is not availible
  int  mbm_get_space_try(BMID bm, int size, int** ptr, RTL_ast_t astadd, void* ast_par);
  int  mbm_declare_event(BMID bm, int len, int evtype, __MBM_CONST unsigned int* trmask, const char* dest,
                         void** free_add, int* free_size, int part_id);
  // Returns MBM_NO_ROOM if no event slot is not availible
  int  mbm_declare_event_try(BMID bm, int len, int evtype, __MBM_CONST unsigned int* trmask, const char* dest,
                             void** free_add, int* free_size, int part_id);
  int  mbm_declare_event_and_send (BMID bm, int len, int evtype, __MBM_CONST unsigned int* trmask,
                                   const char* dest, void** free_add, int* free_size, int part_id);
  int  mbm_free_space  (BMID bm);
  int  mbm_send_space  (BMID bm);
  int  mbm_wait_space  (BMID bm);
  int  mbm_wait_space_a(BMID bm);

  int  mbm_cancel_request   (BMID bm);
  int  mbm_stop_consumer    (BMID bm);
  int  mbm_stop_producer    (BMID bm);
  int  mbm_grant_update     (BMID bm);
  int  mbm_events_actual    (BMID bm, int *events);
  int  mbm_events_produced  (BMID bm, int *events);
  int  mbm_events_seen      (BMID bm, int *events);
  int  mbm_reset_statistics (BMID bm);
  int  mbm_buffer_size      (BMID bm, int* size);
  /// Access to minimal buffer allocation size
  int  mbm_min_alloc        (BMID bm, int* size);
  int  mbm_events_in_buffer (BMID bm, int* events);
  int  mbm_space_in_buffer  (BMID bm, int* total, int* large);
  int  mbm_process_exists   (BMID bm, const char* name, int* exists);

  int  mbm_wait_event(BMID bm);
  int  mbm_wait_event_a(BMID bm);

  /// Map monitoring shared memory section
  BufferMemory* mbm_map_mon_memory(__MBM_CONST char* bm_name);
  /// Unmap monitoring shared memory section
  int  mbm_unmap_mon_memory(BufferMemory* bmid);
  /// Access control table in monitoring section
  CONTROL* mbm_get_control_table(BufferMemory* bmid);
  /// Access user table in monitoring section
  USER* mbm_get_user_table(BufferMemory* bmid);
  /// Access event table in monitoring section
  EVENT* mbm_get_event_table(BufferMemory* bmid);
  /// Map global buffer information on this machine
  int mbm_map_global_buffer_info(lib_rtl_gbl_t* handle, bool create=true);
  /// Unmap global buffer information on this machine
  int mbm_unmap_global_buffer_info(lib_rtl_gbl_t handle, bool remove=false);

#ifdef __cplusplus
#undef __MBM_CONST
}
#endif

#endif // _MBM_MBMDEF_H
