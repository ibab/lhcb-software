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
  typedef int (*wt_callback_type)(int,int);
#define DEF_ARG(x)   =x
#else
  typedef int (*wt_callback_type)();
#define DEF_ARG(x) 
#endif

int wtc_init(void);
int wtc_subscribe (int facility, wt_callback_type rearm,wt_callback_type action);
int wtc_remove(int);
int wtc_insert(int facility, int parameter DEF_ARG(0));
int wtc_insert_head(int facility, int parameter DEF_ARG(0));
int wtc_test_input(void);
int wtc_wait(int*,int*,int*);
int wtc_create_enable_mask(void **p);
#ifdef IMPLEMENTING
int wtc_wait_with_mask(int*,int*,int*, wt_enabled_fac_header*);
int wtc_add_to_en_fac(wt_enabled_fac_header *,int);
#else
int wtc_wait_with_mask(int*,int*,int*, int);
int wtc_add_to_en_fac(int,int);
#endif
int wtc_flush (int);
int wtc_flushp (int,int);
int wtc_add_stack(int ,int);
int wtc_spy_next_entry(int*,int*);
int wtc_error(int status);
#ifdef __cplusplus
}
#endif
