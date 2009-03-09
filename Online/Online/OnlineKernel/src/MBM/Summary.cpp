/*-----------------------------------------------------------------------*/
/*                                                                       */
/*                       OS-9 BUFFER MANAGER                             */
/*                         SUMMARY PROGRAM                               */
/*                                                                       */
/*-----------------------------------------------------------------------*/
#define MBM_IMPLEMENTATION
#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <ctime>
#include "Manager.h"
#include "MBM/bmstruct.h"
#include "bm_internals.h"

/*
 * MBM namespace declaration
 */
namespace MBM {

  struct DisplayDescriptor;

  class Summary {
  protected:
    DisplayDescriptor* m_bms;
    int                m_numBM;
    char               m_buffID[32];
    char*              m_bmid;
    lib_rtl_gbl_t      m_bm_all;
    BUFFERS*           m_buffers;
  public:
    virtual int monitor();
    virtual int show_information();
    virtual size_t draw_buffer(const char* name, CONTROL* ctrl);
    int get_bm_list();
    int drop_bm_list();
    Summary();
    virtual ~Summary();
  };
}
#ifdef _WIN32
#define vsnprintf _vsnprintf
#endif

namespace {
  static const char *sstat[17] = {" nl", "   ", "*SL","*EV","*SP","WSL","WEV","WSP","wsl","wev","wsp"," ps"," ac", "SPR", "WER", "   "};
  struct ManagerImp : public MBM::Manager {
    virtual int  optparse (const char*) {
      return 1;
    }
    ManagerImp() {}
    virtual ~ManagerImp() {}
  };
}
namespace MBM {
  struct DisplayDescriptor   {
    ManagerImp      m_mgr;
    BUFFERS::BUFF*  m_buff;
    DisplayDescriptor() :  m_buff(0) {}
    ~DisplayDescriptor() {}
  };
}

MBM::Summary::Summary() 
: m_bms(0), m_bmid(0), m_bm_all(0), m_buffers(0)
{
}

MBM::Summary::~Summary() {
}

int MBM::Summary::monitor() {
  int status = ::mbm_map_global_buffer_info(&m_bm_all);
  if(!lib_rtl_is_success(status))   {   
    ::printf("Cannot map global buffer information....\n");
    exit(status);
  }
  m_buffers = (BUFFERS*)m_bm_all->address;
  m_bms = new DisplayDescriptor[m_buffers->p_bmax];
  try {
    get_bm_list();    
    show_information();
    drop_bm_list();
  }
  catch(...) {
    printf(" Exception during buffer monitoring.\n");
  }
  return 1;
}

size_t MBM::Summary::draw_buffer(const char* name, CONTROL* ctr)  {
  char txt[256];
  sprintf(txt," Buffer \"%s\"",name);
  ::printf("%-26s  Events: Produced:%d Actual:%d Seen:%d Pending:%d Max:%d\n",
    txt, ctr->tot_produced, ctr->tot_actual, ctr->tot_seen, ctr->i_events, ctr->p_emax);
  ::printf("%-26s  Space(kB):[Tot:%d Free:%d] Users:[Tot:%d Max:%d]\n",
    "",(ctr->bm_size*ctr->bytes_p_Bit)/1024, (ctr->i_space*ctr->bytes_p_Bit)/1024, 
    ctr->i_users, ctr->p_umax);
  return 1;
}

int MBM::Summary::show_information()   {
  int i, j, k;
  static const char* fmt_def  = " %-36s%5x%7d%5s          %40s%5s%7s";
  static const char* fmt_prod = " %-36s%5x%7d%5s%6s%11d   %3.0f%32s%7s";
  static const char* fmt_cons = " %-36s%5x%7d%5s%6s               %12d%11d   %3.0f%5s%7s";
  static const char* head=" Name                            Partition    Pid Type State   Produced %%prod     #seen     #freed %%seen Reqs Buffer";

  char line[256];
  for (i=0;m_numBM>0 && i<m_buffers->p_bmax;i++)  {
    if ( m_bms[i].m_buff != 0 )  {
      BMDESCRIPT* dsc = m_bms[i].m_mgr.m_bm;
      draw_buffer(dsc->bm_name, dsc->ctrl);
    }
  }
  printf(m_numBM<=0 ? "               No active buffers present\n" : head);

  for (i=0;m_numBM>0 && i<m_buffers->p_bmax;i++)  {
    if ( m_bms[i].m_buff != 0 )  {
      USER *us, *utst=(USER*)~0x0;
      BMDESCRIPT* dsc = m_bms[i].m_mgr.m_bm;
      CONTROL*    ctr = dsc->ctrl;
      for (j=0,us=dsc->user;j<ctr->p_umax;j++,us++)    {
        if ( us == utst || us == 0    ) break;
        if ( us->block_id != BID_USER ) continue;
        if ( us->busy     == 0        ) continue;
        char spy_val[5] = {' ',' ',' ',' ',0};
        for (k=0; k<us->n_req; ++k )  {
          if      ( us->req[k].user_type == BM_REQ_ONE  ) spy_val[1] = '1';
          else if ( us->req[k].user_type == BM_REQ_USER ) spy_val[2] = 'U';
          else if ( us->req[k].user_type == BM_REQ_ALL  ) spy_val[3] = 'A';
          else if ( us->req[k].freq < 100.0             ) spy_val[0] = 'S';
        }
        if ( us->ev_produced>0 || us->get_sp_calls>0 )   {
          float perc = 0;
          if ( ctr->tot_produced>0 ) perc = ((float)us->ev_produced/(float)ctr->tot_produced)*100;
          ::sprintf(line,fmt_prod,
            us->name,us->partid,us->pid,"P",sstat[us->p_state+1],us->ev_produced,
            perc+0.1, spy_val, dsc->bm_name
            );
        }
        else if ( us->ev_actual>0 || us->get_ev_calls>0 || us->n_req>0 ) {
          float perc = 0;
          if ( ctr->tot_produced>0 ) perc = ((float)us->ev_seen/(float)ctr->tot_produced)*100;
          ::sprintf(line,fmt_cons,
            us->name,us->partid,us->pid,"C",sstat[us->c_state+1],
            us->ev_seen, us->ev_freed, perc+0.1, spy_val, dsc->bm_name
            );
        }
        else        {
          ::sprintf(line,fmt_def,us->name,us->partid,us->pid,"?","",spy_val, dsc->bm_name);    
        }
        ::printf("%s\n",line);
      }
    }
  }
  return 1;
}


int MBM::Summary::get_bm_list()   {
  m_numBM = 0;
  int status = ::mbm_map_global_buffer_info(&m_bm_all,false);
  if( !lib_rtl_is_success(status) )   {   
    ::lib_rtl_output(LIB_RTL_ERROR,"Cannot map global buffer information....\n");
    m_bm_all = 0;
    m_buffers = 0;
    m_bms = 0;
    return 0;
  }
  m_buffers = (BUFFERS*)m_bm_all->address;
  m_bms = new DisplayDescriptor[m_buffers->p_bmax];
  for (int i = 0; i < m_buffers->p_bmax; ++i)  {
    if ( m_buffers->buffers[i].used == 1 )  {
      if ( m_bmid != 0 && strcmp(m_bmid,m_buffers->buffers[i].name) != 0 )  {
        continue;
      }
      if ( m_bms[i].m_buff == 0 )  {
        m_bms[i].m_mgr.setup(m_buffers->buffers[i].name);
        int sc = m_bms[i].m_mgr.mapMonitorSections();
        if ( !lib_rtl_is_success(sc) ) continue;
        m_bms[i].m_buff = &m_buffers->buffers[i];
      }
      m_numBM++;
    }
    else if ( m_bms[i].m_buff != 0 )  {
      m_bms[i].m_mgr.unmapSections();
      m_bms[i].m_buff = 0;
    }
  }
  return 1;
}

int MBM::Summary::drop_bm_list()   {
  if ( m_numBM > 0 ) {
    for (int i = 0; i < m_buffers->p_bmax; ++i)  {
      if ( m_bms[i].m_buff != 0 )  {
        m_bms[i].m_mgr.unmapSections();
        m_bms[i].m_buff = 0;
      }
    }
  }
  m_numBM = 0;
  if ( m_bm_all ) {
    ::mbm_unmap_global_buffer_info(m_bm_all,false);
    m_bm_all = 0;
  }
  m_buffers = 0;
  if ( m_bms ) delete [] m_bms;
  m_bms = 0;
  return 1;
}
