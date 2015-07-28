#ifndef ONLINEKERNEL_WTDEF_H
#define ONLINEKERNEL_WTDEF_H 1

#include "WT/wt_facilities.h"

enum WT_Constants{
  WT_FAC=114,
  WT_SUCCESS        =1,
  WT_ERROR          =0,
  WT_ALREADYINIT    =(WT_FAC*256+1),
  WT_NOTINIT        =(WT_FAC*256+2),
  WT_NOROOM         =(WT_FAC*256+4),
  WT_NOSUBSCR       =(WT_FAC*256+6),
  WT_BADACTIONSTAT  =(WT_FAC*256+8),
  WT_NOACTION       =(WT_FAC*256+10),
  WT_NOSUBSCRIBED   =(WT_FAC*256+12),
  WT_CHILDDEATH     =(WT_FAC*256+14),
  WT_NOEF           =(WT_FAC*256+16)
};

#ifdef __cplusplus
extern "C" {
  typedef int (*wt_callback_t)(unsigned int,void*);
#define DEF_ARG(x)   =x
#else
  typedef int (*wt_callback_t)();
#define DEF_ARG(x) 
#endif

  int wtc_init(void);
  int wtc_subscribe (int facility, wt_callback_t rearm,wt_callback_t action, void* param DEF_ARG(0));
  int wtc_remove(unsigned int facility);
  int wtc_insert(unsigned int facility, void* parameter DEF_ARG(0));
  int wtc_insert_head(unsigned int facility, void* parameter DEF_ARG(0));
  int wtc_test_input(void);
  int wtc_wait(unsigned int* facility,void** userparameter,int* sub_status);
  int wtc_get_routines(unsigned int fac,wt_callback_t* rea,wt_callback_t* act);
#ifdef IMPLEMENTING
  struct wt_enabled_fac_header;
  int wtc_create_enable_mask(wt_enabled_fac_header** p);
  int wtc_wait_with_mask(unsigned int*,void** userpar, int* substatus, wt_enabled_fac_header*);
  int wtc_add_to_en_fac(wt_enabled_fac_header *,unsigned int);
#else
  int wtc_create_enable_mask(int* p);
  int wtc_wait_with_mask(unsigned int*,void** userpar, int* substatus, int);
  int wtc_add_to_en_fac(int,int);
#endif
  int wtc_flush (unsigned int facility);
  int wtc_flushp (unsigned int facility , void* userparameter);
  int wtc_spy_next_entry(unsigned int* facility, void** parameter);
  int wtc_error(int status);
  int wtc_restore_stack();
  int wtc_add_stack(unsigned int fac,void* param);
#ifdef __cplusplus
}
#endif

#endif /* ONLINEKERNEL_WTDEF_H  */
