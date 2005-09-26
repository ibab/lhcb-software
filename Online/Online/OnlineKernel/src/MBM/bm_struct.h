#ifndef _MBM_MBM_STRUCTS_H
#define _MBM_MBM_STRUCTS_H

#define MBM_IMPLEMENTATION

#include "RTL/bits.h"
#include "RTL/que.h"
#include "RTL/rtl.h"

typedef Bits::BitMask<4> TriggerMask;
typedef Bits::BitMask<4> UserMask;
typedef Bits::BitMask<4> VetoMask;

namespace MBM  {
  enum BufferStates   {
    S_nil    = -1,
    S_active = 11,
    S_pause	 = 10,
    S_wspace =  9,
    S_wevent =  8,
    S_weslot =  7,
    S_wspace_ast_queued  = 6,
    S_wevent_ast_queued  = 5,
    S_weslot_ast_queued  = 4,
    S_wspace_ast_handled = 3,
    S_wevent_ast_handled = 2,
    S_weslot_ast_handled = 1
  };

  enum BIDs {
    BID_USER  = 0xfeed0001,
    BID_EVENT = 0xfeed0002
  };

  enum BufferReasons {
    BM_K_INT_EVENT	= 1,
    BM_K_INT_SPACE	= 2,
    BM_K_INT_ESLOT	= 4
  };
};
#define TOPTYP	  1
#define NAME_LENGTH 8
#define Bits_p_kByte	2
#define Shift_p_Bit	(10-Bits_p_kByte+1) /* = 9 */
#define Bytes_p_Bit	((1<<Shift_p_Bit)-1)	/* = 511 */

typedef int (*_MBM_ast_t)(void*);

struct REQ  {				        /*requirements structures xx bytes each */
	int           ev_type;		/* event type			 */
	TriggerMask   tr_mask;	/* event Mask			 */
	UserMask      vt_mask;	/* veto mask			 */
	int           masktype;		/* requirement type for trigger mask */
	int           user_type;	/* Requirement Type (VIP/normal User) */
	int           freqmode;		/* requirement mode		 */
	float         freq;		    /* Frequency limit		 */
	int           ev_seen;		/* Events seen for this requirement */
};

template <class T> struct QUEUE {
  T* next;
  T* prev;
  QUEUE() : next(0), prev(0) {}
};
struct USER;
struct USER : public qentry_t  {
  qentry wsnext;
  qentry_t wenext;
  qentry_t wesnext;
  unsigned int  block_id;	
  int  busy;		/*36*/	/* slot busy flag  	 */
  int  uid;		/*40*/	/* BM user id  	 */
  int  c_state;	        /*44*/	/* consumer state (Active,Pause) */
  int  p_state;	        /*48*/	/* producer state (Active,Wspace)*/
  int  partid;	        /*52*/	/* user partition ID		 */
  char name[16];	/*68*/	/* user name			 */
  int pid;		/*72*/	/* process id			 */
  int fork;             /*76*/	/* time when forked		 */
  _MBM_ast_t c_astadd;	/*72*/	/* consumer signal to be send	 */
  _MBM_ast_t p_astadd;	/*76*/	/* producer signal to be send	 */
  int ws_size;	        /*80*/	/* size of waiting space	 */
  int ws_ptr;           /*84*/	/* pointer of waiting space	 */
  int we_ptr;           /*88*/	/* pointer of waiting exent	 */
  int we_size;	        /*92*/	/* size of waiting exent	 */
  int we_evtype;        /*96*/	/* evtype of waiting exent	 */
  TriggerMask we_trmask;/*112*/	/* trmask of waiting exent	 */
  int **ws_ptr_add;     /*116*/	/* pointer of waiting space	 */
  int **we_ptr_add;     /*120*/	/* pointer of waiting exent	 */
  int *we_size_add;     /*124*/	/* size of waiting exent	 */
  int *we_evtype_add;   /*128*/	/* evtype of waiting exent	 */
  TriggerMask *we_trmask_add;	  /*132*/	/* trmask of waiting exent	 */
  int space_add;        /*136*/	/* address of allocated space  */
  int space_size;       /*140*/	/* size in bytes		 */
  int held_eid;	        /*144*/	/* held event index		 */
  int ev_produced;      /*148*/	/* events produced counter	 */
  int ev_actual;        /*152*/	/* events matching req 		 */
  int ev_seen;          /*156*/	/* events seen			 */
  int n_req;            /*160*/	/* number of requierements	 */
  int c_partid;         /*164*/
  void* c_astpar;	/*168*/
  void* p_astpar;	/*172*/
  int reason;		/*176*/
  int get_ev_calls;	/*180*/
  int get_wakeups;	/*184*/
  int get_asts_run;	/*188*/
  int spare1,spare2;
  REQ req[8];   /*576*/	/* 8 requirement maximum	 */
  char wes_flag[32];
  char wev_flag[32];
  char wsp_flag[32];
  char weva_flag[32];
  char wspa_flag[32];
  int  wev_ready;
};

struct EVENT : public qentry_t {
  unsigned int block_id;	
  int   busy;		    /*12*/	/* event busy flag		 */
  int   eid;		      /*16*/	/* event ID			 */
  UserMask umask0;    /*32*/
  UserMask umask1;    /*48*/
  UserMask held_mask; /*64*/
  int partid;		      /*68*/	/* Partition ID			 */
  int ev_type;	      /*72*/	/* event type			 */
  TriggerMask tr_mask;	/*88*/	/* trigger Mask			 */
  int ev_add;		      /*92*/	/* event pointer		 */
  int ev_size;	      /*96*/	/* event size			 */
  int count;
  int spare1,spare2,spare3;
};

struct CONTROL  {
  qentry_t u_head;		/* active consumers		 */
  qentry_t wev_head;		/* consumers waiting events  */
  qentry_t wsp_head;		/* producers waiting space	 */
  qentry_t wes_head;		/* producers waiting event slots */
  qentry_t e_head;		/* general event queue		 */
  char *buff_ptr;		/* Event Buffer pointer		 */
  int buff_size;		/* Event Buffer size		 */
  USER *user;			/* user reserved space 		 */
  EVENT *event;		/* event reserved space		 */
  int p_umax;		/* maximum users		 */
  int p_emax;		/* maximum events		 */
  int p_base;		/* Memory base address            */
  int spy_base;		/* spy memory base address        */
  int tot_produced;	/* events produced counter	  */
  int tot_actual;	/* events matching req 		  */
  int tot_seen;		/* events seen			  */
  int i_events;		/* instantaneous event number     */
  int i_space;		/* instantaneous free  space      */
  int i_users;	        /* instantaneous sctive users	  */
  int last_bit;		/* last bit on the bipmap	  */
  int bm_size;		/* size of bit map in bytes	  */
  int last_alloc;	/* Byte Offset of last allocation */
  int spare1;
};

struct BMDESCRIPT : public qentry_t  {
  int           used;			        /*12*/
  int           BM_ident;		      /*16*/
  CONTROL*      ctrl;		          /*20*/
  USER*         user;	            /*24*/
  EVENT*        event;            /*28*/
  char*         bitmap;		        /*32*/
  unsigned int  bitmap_size;		  /*36*/
  char*         buffer_add;		    /*40*/
  unsigned int  buffer_size;      /*44*/
  unsigned int  bpb;			        /*48*/
  int           owner;			      /*52*/
  void*         lockid;			      /*56*/
  CONTROL*      ctrl_add[2];		  /*64*/
  EVENT*        event_add[2];		  /*72*/
  USER*         user_add[2];		  /*80*/
  char*         bitm_add[2];		  /*88*/
  char*         buff_add[2];		  /*96*/
  int spare4;			  /*100*/
  int spare3;			  /*104*/
  int spare2;			  /*108*/
  int spare1;			  /*112*/
  char bm_name[32];		  /*144*/
  int  WES_event_flag;
  int  WEV_event_flag;
  int  WSP_event_flag;
  int  WEVA_event_flag;
  int  WSPA_event_flag;
  char mutexName[128];
  lib_rtl_thread_t pThread;
  lib_rtl_thread_t cThread;
  inline USER* _user()  {
    return owner==-1 ? 0 : user + owner;
  }
  inline CONTROL* _control()  {
    return owner==-1 ? 0 : ctrl;
  }
};

#include "bm_internals.h"
#endif // _MBM_MBM_STRUCTS_H
