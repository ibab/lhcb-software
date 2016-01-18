/*-----------------------------------------------------------------------*/
/*                                                                       */
/*                       OS-9 BUFFER MANAGER                             */
/*                         SUMMARY PROGRAM                               */
/*                                                                       */
/*-----------------------------------------------------------------------*/
#define MBM_IMPLEMENTATION
#include "MBM/bmstruct.h"
#include "MBM/Manager.h"
#include "MBM/bmserver.h"
#include "bm_internals.h"

using namespace MBM;
using namespace std;

namespace {
  static const char *sstat[17] = {" nl", "   ", "*SL","*EV","*SP","WSL","WEV","WSP","wsl","wev","wsp"," ps"," ac", "SPR", "WER", "   "};

  struct ManagerImp : public Manager {
    virtual int  optparse (const char*) { return 1; }
    ManagerImp()                        {           }
    virtual ~ManagerImp()               {           }
  };
  struct Summary {
    typedef map<string,USER*> Users;
    Users m_oneTasks;
    virtual int show(bool show_states);
    virtual int show(BufferMemory* dsc, bool show_states);
    Summary()                           {           }
    virtual ~Summary()                  {           }
  };
}

int Summary::show(bool show_states) {
  lib_rtl_gbl_t bm_all;
  int status = ::mbmsrv_map_global_buffer_info(&bm_all,false);
  if(!lib_rtl_is_success(status))   {   
    ::lib_rtl_output(LIB_RTL_ERROR,"Cannot map global buffer information....\n");
    ::exit(status);
  }
  ::printf("+++++ MBM buffer section sucessfully mapped.\n");
  BUFFERS* buffs = (BUFFERS*)bm_all->address;
  try {
    typedef map<string,int> Buffs;
    Buffs bs;
    for (int i = 0; i < buffs->p_bmax; ++i)  {
      if ( buffs->buffers[i].used == 1 )  {
        bs[buffs->buffers[i].name] = i;
      }
    }
    for(Buffs::const_iterator j=bs.begin(); j!=bs.end();++j) {
      ManagerImp bms;
      bms.setup(buffs->buffers[(*j).second].name);
      int sc = bms.mapMonitorSections();
      if ( !lib_rtl_is_success(sc) ) continue;
      show(bms.m_bm,show_states);
      bms.unmapSections();
    }
  }
  catch(...) {
    ::printf(" Exception during buffer monitoring.\n");
  }
  ::mbmsrv_unmap_global_buffer_info(bm_all,false);
  ::printf("\n+++++ MBM summary finished.\n");
  return 1;
}

int Summary::show(BufferMemory* dsc, bool show_states)   {
  int j, k;
  static const char* fmt_def  = " %-20s%5x%5s          %40s%5s%7s\n";
  static const char* fmt_prod = " %-20s%5x%5s%6s%11d   %3.0f%32s%7s\n";
  static const char* fmt_cons = " %-20s%5x%5s%6s               %12d   %3.0f%5s%7s\n";
  static const char* head=" Name            Partition Type State   Produced %prod     #seen %seen Reqs Buffer\n";
  char txt[256];
  USER*    us, *utst=(USER*)~0x0;
  CONTROL* ctr = dsc->ctrl;

  ::printf("\n======================== MBM Bufer summary for buffer \"%s\" ========================\n\n",dsc->bm_name);
  ::snprintf(txt,sizeof(txt)," Buffer \"%s\"",dsc->bm_name);
  ::printf("%-26s  Events: Produced:%ld Seen:%ld Pending:%ld Max:%ld\n",
           txt, long(ctr->tot_produced), long(ctr->tot_seen), long(ctr->i_events), long(ctr->p_emax));
  ::printf("%-26s  Space(kB):[Tot:%ld Free:%ld] Users:[Tot:%ld Max:%ld]\n\n",
           "",long((ctr->bm_size*ctr->bytes_p_Bit)/1024), 
           long((ctr->i_space*ctr->bytes_p_Bit)/1024), 
           long(ctr->i_users), long(ctr->p_umax));

  bool first = true;
  Users users;
  for (j=0,us=dsc->user;j<ctr->p_umax;j++,us++)    {
    if ( us == utst || us == 0    ) break;
    if ( us->block_id != BID_USER ) continue;
    if ( us->busy     == 0        ) continue;
    users[us->name] = us;
  }
  USER cons_one, prod_one;
  ::memset(&cons_one,0,sizeof(cons_one));
  ::memset(&prod_one,0,sizeof(prod_one));
  for(Users::const_iterator ui=users.begin(); ui!=users.end();++ui) {
    char spy_val[5] = {' ',' ',' ',' ',0};
    us = (*ui).second;
    for (k=0; k<us->n_req; ++k )  {
      if      ( us->req[k].user_type == BM_REQ_ONE  ) spy_val[1] = '1';
      else if ( us->req[k].user_type == BM_REQ_USER ) spy_val[2] = 'U';
      else if ( us->req[k].user_type == BM_REQ_ALL  ) spy_val[3] = 'A';
      else if ( us->req[k].freq < 100.0             ) spy_val[0] = 'S';
    }
    if ( first )    {
      first = false;
      ::puts(head);
    }
    if ( us->ev_produced>0 || us->get_sp_calls>0 )   {
      float perc = 0;
      if ( ctr->tot_produced>0 ) perc = ((float)us->ev_produced/(float)ctr->tot_produced)*100;
      if ( m_oneTasks.find(us->name) != m_oneTasks.end() ) {
        prod_one.ev_produced += us->ev_produced;
        prod_one.state  = 0;
        prod_one.partid = us->partid;
        ::strncpy(prod_one.name,"PROD_ONE",sizeof(prod_one.name)-1);
        continue;
      }
      const char* st = show_states ? sstat[us->state+1] : "";
      ::printf(fmt_prod,us->name,us->partid,"P",st,us->ev_produced,
               perc+0.1, spy_val, dsc->bm_name);
    }
    else if ( us->ev_actual>0 || us->get_ev_calls>0 || us->n_req>0 ) {
      float perc = 0;
      if ( spy_val[1]=='1' )   {
        m_oneTasks[us->name] = 0;
        cons_one.ev_seen += us->ev_seen;
        cons_one.state    = 0;
        cons_one.partid   = us->partid;
        ::strncpy(cons_one.name,"CONS_ONE",sizeof(cons_one.name)-1);
        continue;
      }
      if ( ctr->tot_produced>0 ) {
        perc = ((float)us->ev_seen/(float)ctr->tot_produced)*100;
      }
      const char* st = show_states ? sstat[us->state+1] : "";
      ::printf(fmt_cons,us->name,us->partid,"C",st,
               us->ev_seen, perc+0.1, spy_val, dsc->bm_name);
    }
    else        {
      ::printf(fmt_def,us->name,us->partid,"?","",spy_val, dsc->bm_name);    
    }
  }
  if ( prod_one.name[0] ) {
    us = &prod_one;
    float perc = 0;
    if ( ctr->tot_produced>0 ) perc = ((float)us->ev_produced/(float)ctr->tot_produced)*100;
    ::printf(fmt_prod,us->name,us->partid,"P","",us->ev_produced,
             perc+0.1, "", dsc->bm_name);
  }
  if ( cons_one.name[0] ) {
    us = &cons_one;
    float perc = 0;
    if ( ctr->tot_produced>0 ) perc = ((float)us->ev_seen/(float)ctr->tot_produced)*100;
    ::printf(fmt_cons,us->name,us->partid,"C","",us->ev_seen,perc+0.1," 1 ",dsc->bm_name);
  }
  return 1;
}

namespace {
  void help()  {
    ::printf("mbm_summary -opt [-opt]\n");
    ::printf("    -nostates      Do cnot show client states \n");
  }
}

extern "C" int mbm_summary(int argc, char** argv) {
  RTL::CLI cli(argc,argv,help);
  bool show_states = cli.getopt("nostates",1) == 0;
  Summary sum;
  return sum.show(show_states);
}
