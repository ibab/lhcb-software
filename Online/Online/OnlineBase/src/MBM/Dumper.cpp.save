#define MBM_IMPLEMENTATION
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "bm_struct.h"
#include "Manager.h"

namespace MBM {
  struct Dumper : public Manager {
    int All;
    char   buff_id[16];
    template <class T> void print_item(const char* format,T item)  {
      printf(format,item,item);
    }
    template <class T> void print_sitem(const char* format,T item)  {
      printf(format,item);
    }
    template <class T> void print_mask(const char* format,const T* item) {
      printf(format,item[0],item[1],item[2],item[3]);
    }
    template <class T> void print_queue(const char* format,const T* ptr, int type)  {
      switch (type)  {
      case 0:
        printf(format,*ptr,*ptr,add_ptr(ptr,*ptr));
        break;
      case 1:
        printf(format,*ptr,*ptr,add_ptr(ptr,(*ptr-4)));
        break;
      }
    }
    void print_user_table(const USER* user);
    void print_control_table(const CONTROL* ctrl);
    void print_event_table(const EVENT* event);
    int print();
    virtual int  optparse (const char* c);
    Dumper(int argc , char** argv) : All(0) {
      getOptions(argc, argv);
    }
  };
}

extern "C" int mbm_dump(int argc , char** argv) {
  MBM::Dumper dmp(argc, argv);
  return dmp.print();
}

int MBM::Dumper::optparse (const char* c)   {
  int iret;
  switch (*c | 0x20)  {
  case 'a':      /*      size of buffer        */   
    All = 1;
    break;
  case 'i':      /*      buffer_id        */   
    iret = sscanf(c+1,"=%s",buff_id);
    if( iret != 1 )      {
      lib_rtl_output(LIB_RTL_ALWAYS,"Error reading Buffer identifier parameter\n");
      exit(0);
    }
    bm_id = buff_id;
    break;
  case '?':
  default:
  case 'h':
    lib_rtl_output(LIB_RTL_ALWAYS,"bm - Buffer Manager Monitor\n");
    lib_rtl_output(LIB_RTL_ALWAYS,"Options:\n");
    lib_rtl_output(LIB_RTL_ALWAYS,"    -i=<bm_name>   Select Buffer Identifier\n");
    lib_rtl_output(LIB_RTL_ALWAYS,"    -a           Dumper all table entries\n");
    exit(0);
    break;
  }
  return 1;
}

int MBM::Dumper::print()  {
  int status  = mapSections();
  if ( lib_rtl_is_success(status) )  {
    int i;
    print_control_table(m_bm->ctrl);
    USERDesc* u = m_bm->usDesc;
    EVENTDesc* e = m_bm->evDesc;
    print_queue("USERDesc->u_head\t%8x %d\t-----> user at address %x\n",   (long*)&u->next,0);
    print_queue("USERDesc->u_tail\t%8x %d\t-----> user at address %x\n",   (long*)&u->prev,1);
    print_queue("USERDesc->wev_head\t%8x %d\t-----> user at address %x\n", (long*)&u->wev_head.next,0);
    print_queue("USERDesc->wev_tail\t%8x %d\t-----> user at address %x\n", (long*)&u->wev_head.prev,1);
    print_queue("USERDesc->wsp_head\t%8x %d\t-----> user at address %x\n", (long*)&u->wsp_head,0);
    print_queue("USERDesc->wsp_tail\t%8x %d\t-----> user at address %x\n", (long*)&u->wsp_head.prev,1);
    print_queue("USERDesc->wes_head\t%8x %d\t-----> user at address %x\n", (long*)&u->wes_head,0);
    print_queue("USERDesc->wes_tail\t%8x %d\t-----> user at address %x\n", (long*)&u->wes_head.prev,1);
    for (i=0;i<m_bm->ctrl->p_umax;i++) 
      print_user_table(m_bm->user+i);
    print_queue("EVENTDesc->e_head\t%8x %d\t-----> event at address %x\n",  (long*)&e->next,0);
    print_queue("EVENTDesc->e_tail\t%8x %d\t-----> event at address %x\n",  (long*)&e->prev,1);
    for (i=0;i<m_bm->ctrl->p_emax;i++) 
      print_event_table(m_bm->event+i);
  }
  return status;
}

void MBM::Dumper::print_control_table(const CONTROL* ctrl)  {
  printf("Control table starts at %p(hex)\n",(void*)ctrl);
  print_item("ctrl->buff_size\t%8x %d\n",ctrl->buff_size);
  print_item("ctrl->p_umax\t%8x %d\n",ctrl->p_umax);
  print_item("ctrl->p_emax\t%8x %d\n",ctrl->p_emax);
  print_item("ctrl->p_base\t%8x %d\n",ctrl->p_base);
  print_item("ctrl->spy_base\t%8x %d\n",ctrl->spy_base);
  print_item("ctrl->tot_produced\t%8x %d\n",ctrl->tot_produced);
  print_item("ctrl->tot_actual\t%8x %d\n",ctrl->tot_actual);
  print_item("ctrl->tot_seen\t%8x %d\n",ctrl->tot_seen);
  print_item("ctrl->i_events\t%8x %d\n",ctrl->i_events);
  print_item("ctrl->i_space\t%8x %d\n",ctrl->i_space);
  print_item("ctrl->i_users\t%8x %d\n",ctrl->i_users);
  print_item("ctrl->last_bit\t%8x %d\n",ctrl->last_bit);
  print_item("ctrl->bm_size\t%8x %d\n",ctrl->bm_size);
  printf("\n\n");
}

void MBM::Dumper::print_user_table(const USER* user)  {
  if (All == 0)  {
    if (user->busy == 0)    {
      return;
    }
  }
  printf("User table starts at %p(hex)\n",(void*)user);
  print_queue("user->next\t%x %d\t-----> user at address %x\n",&user->next,0);
  print_queue("user->prev\t%x %d\t-----> user at address %x\n",&user->prev,1);
  print_queue("user->wsnext\t%x %d\t-----> user at address %x\n",(long*)&user->wsnext,0);
  print_queue("user->wsprev\t%x %d\t-----> user at address %x\n",(long*)&user->wsnext.prev,1);
  print_queue("user->wenext\t%x %d\t-----> user at address %x\n",&user->wenext.next,0);
  print_queue("user->weprev\t%x %d\t-----> user at address %x\n",&user->wenext.prev,1);
  print_queue("user->wesnext\t%x %d\t-----> user at address %x\n",(long*)&user->wesnext,0);
  print_queue("user->wesprev\t%x %d\t-----> user at address %x\n",(long*)&user->wesnext.prev,1);
  print_item("user->block_id\t%x %d\n",user->block_id);
  print_item("user->busy\t%x %d\n",user->busy);
  print_item("user->uid\t%x %d\n",user->uid);
  print_item("user->c_state\t%x %d\n",user->c_state);
  print_item("user->p_state\t%x %d\n",user->p_state);
  print_item("user->partid\t%x %d\n",user->partid);
  print_sitem("user->name\t%s\n",user->name);
  print_item("user->pid\t%x %d\n",user->pid);
  print_item("user->c_astadd\t%x %d\n",user->c_astadd);
  print_item("user->p_astadd\t%x %d\n",user->p_astadd);
  print_item("user->ws_size\t%x %d\n",user->ws_size);
  print_item("user->ws_ptr\t%x %d\n",user->ws_ptr);
  print_item("user->we_ptr\t%x %d\n",user->we_ptr);
  print_item("user->we_size\t%x %d\n",user->we_size);
  print_item("user->we_evtype\t%x %d\n",user->we_evtype);
  print_item("user->we_trmask\t%x\n",user->we_trmask.bits());
  print_item("user->ws_ptr_add\t%x %d\n",user->ws_ptr_add);
  print_item("user->we_ptr_add\t%x %d\n",user->we_ptr_add);
  print_item("user->we_size_add\t%x %d\n",user->we_size_add);
  print_item("user->we_evtype_add\t%x %d\n",user->we_evtype_add);
  print_item("user->we_trmask_add\t%x %d\n",user->we_trmask_add);
  print_item("user->space_add\t%x %d\n",user->space_add);
  print_item("user->space_size\t%x %d\n",user->space_size);
  print_item("user->held_eid\t%x %d\n",user->held_eid);
  print_item("user->ev_produced\t%x %d\n",user->ev_produced);
  print_item("user->ev_actual\t%x %d\n",user->ev_actual);
  print_item("user->ev_seen\t%x %d\n",user->ev_seen);
  print_item("user->n_req\t%x %d\n",user->n_req);
  print_item("user->c_partid\t%x %d\n",user->c_partid);
  print_item("user->c_astpar\t%x %d\n",user->c_astpar);
  print_item("user->p_astpar\t%x %d\n",user->p_astpar);
  print_item("user->reason\t%x %d\n",user->reason);
  print_item("user->get_ev_calls\t%x %d\n",user->get_ev_calls);
  print_item("user->get_wakeups\t%x %d\n",user->get_wakeups);
  print_item("user->get_asts_run\t%x %d\n",user->get_asts_run);
  printf ("\nNumber of Requirements declared %d\n",user->n_req);
  for(int i=0;i<8;i++)  {
    printf("Requirement Number %d    Event Type %d\n",i,user->req[i].ev_type);
    print_mask("\tReq. Trigger Mask \t%0x %0x %0x %0x\n",user->req[i].tr_mask.bits());
    print_mask("\tVeto Mask\t%0x %0x %0x %0x\n",user->req[i].vt_mask.bits());
    print_item("\tRequirement Type  \t%d\n",user->req[i].masktype);
    print_item("\tRequirement mode\t%d\n",user->req[i].freqmode);
  }
  printf("\n\n");
}

void MBM::Dumper::print_event_table(const EVENT* event)  {
  if (All == 0)  {
    if (event->busy == 0)    {
      return;
    }
  }
  printf("Event table starts at %p(hex)\n",(void*)event);
  print_queue("event->next\t%x %d\t-----> Event at address %x\n",(long*)&event->next,0);
  print_queue("event->prev\t%x %d\t-----> Event at address %x\n",(long*)&event->prev,1);
  print_item("event->block_id\t%x %d\n",event->block_id);
  print_item("event->busy\t%d\n",event->busy);
  print_item("event->eid\t%d\n",event->eid);
  print_mask("event->umask0\t%x %x %x %x \n",event->umask0.bits());
  print_mask("event->umask1\t%x %x %x %x \n",event->umask1.bits());
  print_mask("event->held_mask\t%x %x %x %x \n",event->held_mask.bits());
  print_item("event->partid\t%x %d\n",event->partid);
  print_item("event->ev_type\t%d\n",event->ev_type);
  print_mask("event->tr_mask\t%x %x %x %x \n",event->tr_mask.bits());
  print_item("event->ev_add\t%x \n",event->ev_add);
  print_item("event->ev_size\t%x %d\n",event->ev_size);
  printf("\n\n");
}
