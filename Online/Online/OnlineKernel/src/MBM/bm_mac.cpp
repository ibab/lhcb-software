#define MBM_IMPLEMENTATION

#include <cstdio>
#include <cstdlib>
#include "bm_struct.h"

#ifdef VMS
#define lock_prio  14
typedef struct  {
  short len,code;
  int addr,retaddr;
}  TABNAM_LIST;
static int def_prio;
#include <secdef.h>
#include <psldef.h>

#elif linux

#include <sys/mman.h>

#elif _WIN32
#include <windows.h>
#endif

#ifdef _WIN32
static char mutexName[32] = "";
#endif

int _mbm_create_section(const char* sec_name,int size, void* address) {
  return lib_rtl_create_section(sec_name, size, address);
}

int _mbm_delete_section(const char *sec_name)  {
  return lib_rtl_delete_section(sec_name);
}

int _mbm_map_section(const char* sec_name, void* address)   {
  return lib_rtl_map_section(sec_name,address);
}

/// Unmap global section: address is quadword: void*[2]
int _mbm_unmap_section(void* address)   {
  return lib_rtl_unmap_section(address);
}

int _mbm_flush_sections(BMDESCRIPT* bm)   {
  lib_rtl_flush_section(bm->ctrl_add, sizeof(CONTROL));
  lib_rtl_flush_section(bm->event_add,bm->ctrl->p_emax*sizeof(EVENT));
  lib_rtl_flush_section(bm->user_add, bm->ctrl->p_umax*bm->ctrl->p_emax*sizeof(USER));
  lib_rtl_flush_section(bm->buff_add, bm->ctrl->buff_size);
  lib_rtl_flush_section(bm->bitm_add, bm->ctrl->bm_size);
  return 1;
}

