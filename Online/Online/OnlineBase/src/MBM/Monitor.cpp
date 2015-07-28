/*-----------------------------------------------------------------------*/
/*                                                                       */
/*                       OS-9 BUFFER MANAGER                             */
/*                         MONITOR PROGRAM                               */
/*                                                                       */
/* Edition History                                                       */
/*                                                                       */
/*  #   Date    Comments                                       By        */
/* -- -------- ----------------------------------------------- ---       */
/*  0  28/09/88  Initial version                               PM        */
/*  1  11/11/88  Released version 1.0                          PM        */
/*  2  29/11/88  Spy and Multibuffer included                  PM        */
/*  3  15/12/88  Released version 2.0                          PM        */
/*  4  21/02/90  Now it can allocate buffer across memories    PM        */
/*  5  19/01/08  Adaption and cleanup for Linux/WIN32          MF        */
/*-----------------------------------------------------------------------*/
#define MBM_IMPLEMENTATION
#include "MBM/Monitor.h"
#include "MBM/bmstruct.h"
#include "MBM/Manager.h"
#include "CPP/Event.h"
#include "CPP/TimeSensor.h"
#include "CPP/Interactor.h"

#include "bm_internals.h"
#include <sys/time.h>
#ifdef _WIN32
#define vsnprintf _vsnprintf
#endif
#include <utility>
using namespace MBM;
using namespace std;
namespace
{
  static const char *sstat[17] =
    { " nl", "   ", "*SL", "*EV", "*SP", "WSL", "WEV", "WSP", "wsl", "wev", "wsp",
      " ps", " ac", "SPR", "WER", "   " };

  static int cont = 1;
}

namespace MBM
{
  struct ManagerImp: public Manager
  {
    virtual int optparse(const char*)
    {
      return 1;
    }
    ManagerImp()
    {
    }
    virtual ~ManagerImp()
    {
    }
  };

  struct DisplayDescriptor
  {
    ManagerImp m_mgr;
    BUFFERS::BUFF* m_buff;
    DisplayDescriptor() :
      m_buff(0)
    {
    }
    ~DisplayDescriptor()
    {
    }
  };
}

Monitor::Monitor(int argc, char** argv, MonitorDisplay* disp) :
  m_bms(0), m_bmid(0), m_bm_all(0), m_buffers(0), m_display(disp), m_updating(false)

{
  m_display->setClient(this);
  rateMode = false;
  LastTime = 0;
  getOptions(argc, argv);
  m_BMMap.clear();
}

Monitor::~Monitor()
{
  TimeSensor::instance().remove(this);
  for(int i=0; i<20 && m_updating; ++i)
    ::lib_rtl_sleep(10);
  if (m_display)
    delete m_display;
}

size_t Monitor::print(void* ctxt, int, const char* format, va_list args)
{
  char buffer[1024];
  Monitor* m = (Monitor*) ctxt;
  size_t res = ::vsnprintf(buffer, sizeof(buffer), format, args);
  m->display()->draw_line_normal(buffer);
  return res;
}

void Monitor::getOptions(int argc, char** argv)
{
  while (--argc > 0)
  { /* process options  */
    const char* cptr = *++argv;
    if (*cptr == '-' || *cptr == '/')
      optparse(cptr + 1);
  }
}

int Monitor::monitor()
{
  int status = initMonitor();
  if (!lib_rtl_is_success(status))
  {
    exit(status);
  }
  display()->setup_window();
  if (cont)
  {
    updateDisplay();
    TimeSensor::instance().add(this, 1, 0);
    TimeSensor::instance().run();
  }
  else
  {
    show_information();
  }
  display()->reset_window();
  return 1;
}

void Monitor::handle(const Event& ev)
{
  switch (ev.eventtype)
  {
  case TimeEvent:
    updateDisplay();
    TimeSensor::instance().add(this, 2, 0);
    break;
  default:
    display()->draw_line_normal("Unknown event received of type: %d",
                                ev.eventtype);
    break;
  }
}

int Monitor::initMonitor()
{
  //signal (SIGINT,handler);
  //signal (SIGABRT,handler);
  int status = ::mbmsrv_map_global_buffer_info(&m_bm_all);
  if (!lib_rtl_is_success(status))
  {
    ::printf("Cannot map global buffer information....\n");
    return status;
  }
  m_buffers = (BUFFERS*) m_bm_all->address;
  m_bms = new DisplayDescriptor[m_buffers->p_bmax];
  return 1;
}

int Monitor::updateDisplay()
{
  m_updating = true;
  try
  {
    if (LastTime == 0)      {
      LastTime = this->myGetTime();
      get_bm_list();
      CopyData();
      drop_bm_list();
      m_updating = false;
      return 1;
    }
    display()->begin_update();
    get_bm_list();
    currentTime = this->myGetTime();
    deltaTime = currentTime-LastTime;
    show_information();
    LastTime = currentTime;
    CopyData();
    drop_bm_list();
  }
  catch (...)
  {
    display()->draw_line_normal(" Exception during buffer monitoring.");
  }
  MonitorDisplay* m_disp = display();
  m_disp->end_update();
  m_updating = false;
  return 1;
}

size_t Monitor::draw_buffer(const char* name, CONTROL* ctr)
{
  char txt[256], rate1[64], rate2[64], rate3[64];
  ::snprintf(txt, sizeof(txt), " Buffer \"%s\"", name);
  ::snprintf(rate1, sizeof(rate1), "----");
  ::snprintf(rate2, sizeof(rate2), "----");
  ::snprintf(rate3, sizeof(rate3), "----");
  BMMap_t::iterator bmit = m_BMMap.find(name);
  
  if (bmit != m_BMMap.end())  {
    BMSTAT *st = (*bmit).second;
    ::snprintf(rate1, sizeof(rate1), "%.3f", double(ctr->tot_produced-st->evprod)/deltaTime*1000.0);
    ::snprintf(rate2, sizeof(rate2), "%.3f", double(ctr->tot_actual-st->evactual)/deltaTime*1000.0);
    ::snprintf(rate3, sizeof(rate3), "%.3f", double(ctr->tot_seen-st->evcons)/deltaTime*1000.0);
  }
  display()->draw_line_normal(
                              "%-26s  Events: Produced:%d (%s kHz) Actual:%d (%s kHz) Seen:%d (%s kHz) Pending:%d Max:%d",
                              txt,
                              ctr->tot_produced,rate1,
                              ctr->tot_actual,  rate2,
                              ctr->tot_seen,    rate3,
                              ctr->i_events, ctr->p_emax);
  display()->draw_line_normal(
                              "%-26s  Space(kB):[Tot:%d Free:%d] Users:[Tot:%d Max:%d]", "",
                              (ctr->bm_size * ctr->bytes_p_Bit) / 1024,
                              (ctr->i_space * ctr->bytes_p_Bit) / 1024, ctr->i_users, ctr->p_umax);

  display()->draw_line_normal(" Occupancy [Events]:");
  display()->draw_bar(30, display()->currLine() - 1,
                      float(ctr->i_events) / float(ctr->p_emax), display()->width() - 30);
  display()->draw_line_normal("           [Space]: ");
  display()->draw_bar(30, display()->currLine() - 1,
                      float(ctr->bm_size - ctr->i_space) / float(ctr->bm_size),
                      display()->width() - 30);
  return 1;
}

int Monitor::show_information()
{
  int i, j, k;
  static const char* fmt_def;
  static const char* fmt_prod;
  static const char* fmt_cons;
  static const char* head;
  if (rateMode)
  {
    fmt_def =  " %1d/%-34s%5x%7d%5s          %40s%5s %7s";
    fmt_prod = " %1d/%-34s%5x%7d%5s%6s    %7.3f   %3.0f%32s %7s";
    fmt_cons = " %1d/%-34s%5x%7d%5s%6s                    %7.3f    %7.3f   %3.0f%5s %7s";
    head =    " Srv/Name                        Partition    Pid Type State   Produced %%prod     #seen     #freed %%seen Reqs Buffer";
  }
  else
  {
    fmt_def =  " %1d/%-34s%5x%7d%5s          %40s%5s %7s";
    fmt_prod = " %1d/%-34s%5x%7d%5s%6s%11d   %3.0f%32s %7s";
    fmt_cons = " %1d/%-34s%5x%7d%5s%6s               %12d%11d   %3.0f%5s %7s";
    head =     " Srv/Name                        Partition    Pid Type State   Produced %%prod     #seen     #freed %%seen Reqs Buffer";
  }
  char line[256];
  display()->draw_line_normal("");
  display()->draw_line_reverse(
                               "                 Buffer Manager Monitor [%s]  pid:%d on %s",
                               ::lib_rtl_timestr("%a %d %b %Y  %H:%M:%S", 0), lib_rtl_pid(),
                               RTL::nodeName().c_str());
  display()->draw_line_normal("");
  for (i = 0; m_numBM > 0 && i < m_buffers->p_bmax; i++)
  {
    if (m_bms[i].m_buff != 0)
    {
      BufferMemory* dsc = m_bms[i].m_mgr.m_bm;
      draw_buffer(dsc->bm_name, dsc->ctrl);
      display()->draw_line_normal("");
    }
  }
  display()->draw_line_reverse(m_numBM <= 0 ? "               No active buffers present" : head);
  for (i = 0; m_numBM > 0 && i < m_buffers->p_bmax; i++)
  {
    if (m_bms[i].m_buff != 0)
    {
      USER *us, *utst = (USER*) ~0x0;
      BufferMemory* dsc = m_bms[i].m_mgr.m_bm;
      CONTROL* ctr = dsc->ctrl;
      BMMap_t::iterator it = m_BMMap.find(dsc->bm_name);
      BMSTAT *st = (it != m_BMMap.end()) ? (*it).second : 0;
      for (j = 0, us = dsc->user; j < ctr->p_umax; j++, us++)
      {
        if (us == utst || us == 0)
          break;
        if (us->block_id != BID_USER)
          continue;
        if (us->busy == 0)
          continue;
        USSTAT* ust = 0;
        if ( st )  {
          UMap_t::iterator uit = st->UserMap.find(string(us->name));
          ust = (uit == st->UserMap.end()) ? 0 : (*uit).second;
        }
        char spy_val[5] = { ' ', ' ', ' ', ' ', 0 };
        for (k = 0; k < us->n_req; ++k)
        {
          if (us->req[k].user_type == BM_REQ_USER)
            spy_val[2] = 'U';
          else if (us->req[k].user_type == BM_REQ_ALL)
            spy_val[3] = 'A';
          else if (us->req[k].user_type == BM_REQ_ONE)
            spy_val[1] = 'a' + us->req[k].user_type_one;
          if (us->req[k].freq < 100.0)
            spy_val[0] = 'F';
        }
        if (us->ev_produced > 0 || us->get_sp_calls > 0)
        {
          float perc = 0;
          if (ctr->tot_produced > 0)
            perc = ((float) us->ev_produced / (float) ctr->tot_produced) * 100;
          if (!rateMode)
          {
            ::snprintf(line, sizeof(line), fmt_prod, us->serverid, us->name,
                       us->partid, us->pid, "P", sstat[us->state + 1], us->ev_produced,
                       perc + 0.1, spy_val, dsc->bm_name);
          }
          else
          {
            ::snprintf(line, sizeof(line), fmt_prod, us->serverid, us->name,
                       us->partid, us->pid, "P", sstat[us->state + 1], 
                       ust ? double(us->ev_produced-ust->evprod)/deltaTime*1000 : 0.0,
                       perc + 0.1, spy_val, dsc->bm_name);
          }
        }
        else if (us->ev_actual > 0 || us->get_ev_calls > 0 || us->n_req > 0)
        {
          float perc = 0;
          if (ctr->tot_produced > 0)
            perc = ((float) us->ev_seen / (float) ctr->tot_produced) * 100;
          if (!rateMode)
          {
            ::snprintf(line, sizeof(line), fmt_cons, us->serverid, us->name,
                       us->partid, us->pid, "C", sstat[us->state + 1], us->ev_seen,
                       us->ev_freed, perc + 0.1, spy_val, dsc->bm_name);
          }
          else
          {
            ::snprintf(line, sizeof(line), fmt_cons, us->serverid, us->name,
                       us->partid, us->pid, "C", sstat[us->state + 1], 
                       ust ? double(us->ev_seen-ust->evseen)/deltaTime*1000 : 0.0,
                       ust ? double(us->ev_freed - ust->evfreed)/deltaTime*1000 : 0.0, 
                       perc + 0.1, spy_val, dsc->bm_name);
          }
        }
        else
        {
          ::snprintf(line, sizeof(line), fmt_def, us->serverid, us->name,
                     us->partid, us->pid, "?", "", spy_val, dsc->bm_name);
        }
        display()->draw_line_normal(line);
      }
    }
  }
  return 1;
}

int Monitor::optparse(const char* c)
{
  int iret;
  switch (*c | 0x20)
  {
  case 's': /*      Single Update*/
    cont = 0;
    break;
  case 'i': /*      buffer_id        */
    iret = ::sscanf(c + 1, "=%s", m_buffID);
    if (iret != 1)
    {
      ::lib_rtl_output(LIB_RTL_ERROR,
                       "Error reading Buffer identifier parameter.\n");
      ::exit(0);
    }
    m_bmid = m_buffID;
    break;
  case 'r':
    rateMode = true;
    break;
  case '?':
  case 'h':
  default:
    ::lib_rtl_output(LIB_RTL_ALWAYS, "mbm_mon - Buffer Manager Monitor\n");
  ::lib_rtl_output(LIB_RTL_ALWAYS, "Options:\n");
  ::lib_rtl_output(LIB_RTL_ALWAYS,
                   "    -i=<bm_name>   Select Buffer Identifier\n");
  ::lib_rtl_output(LIB_RTL_ALWAYS, "    -s             Single update \n");
  exit(0);
  }
  return 1;
}
void Monitor::CopyData()
{
  BMMap_t::iterator it;
  BMSTAT *st;
  int j;
  for (int i = 0; i < m_buffers->p_bmax; ++i)
  {
    if ( !m_bms[i].m_buff ) continue;
    if (m_buffers->buffers[i].used == 1)    {
      it = m_BMMap.find(string(m_buffers->buffers[i].name));
      if (it == m_BMMap.end())     {
        st = new BMSTAT;
        m_BMMap.insert(make_pair(string(m_buffers->buffers[i].name), st));
      }
      else      {
        st = (*it).second;
      }
      st->evactual = m_bms[i].m_mgr.m_bm->ctrl->tot_actual;
      st->evcons = m_bms[i].m_mgr.m_bm->ctrl->tot_seen;
      st->evprod = m_bms[i].m_mgr.m_bm->ctrl->tot_produced;
    }
    if (st != 0 && m_bms[i].m_buff != 0)
    {
      USER *us, *utst = (USER*) ~0x0;
      BufferMemory* dsc = m_bms[i].m_mgr.m_bm;
      CONTROL* ctr = dsc->ctrl;
      for (j = 0, us = dsc->user; j < ctr->p_umax; j++, us++)
      {
        if (us == utst || us == 0)
          break;
        if (us->block_id != BID_USER)
          continue;
        if (us->busy == 0)
          continue;
        UMap_t::iterator uit;
        USSTAT *ust;
        uit = st->UserMap.find(string(us->name));
        if (uit == st->UserMap.end())
        {
          ust = new USSTAT;
          st->UserMap.insert(make_pair(string(us->name),ust));
        }
        else
        {
          ust = (*uit).second;
        }
        if (string(us->name) != ust->Name)
        {
          ust->Name = us->name;
        }
        ust->evprod = us->ev_produced;
        ust->evseen = us->ev_seen;
        ust->evfreed = us->ev_freed;
      }
    }
  }
}

int Monitor::get_bm_list()
{
  m_numBM = 0;
  int status = ::mbmsrv_map_global_buffer_info(&m_bm_all, false);
  if (!lib_rtl_is_success(status))
  {
    //::lib_rtl_output(LIB_RTL_ERROR,"Cannot map global buffer information....\n");
    m_bm_all = 0;
    m_buffers = 0;
    m_bms = 0;
    return 0;
  }
  m_buffers = (BUFFERS*) m_bm_all->address;
  m_bms = new DisplayDescriptor[m_buffers->p_bmax];
  for (int i = 0; i < m_buffers->p_bmax; ++i)
  {
    if (m_buffers->buffers[i].used == 1)
    {
      if (m_bmid != 0 && strcmp(m_bmid, m_buffers->buffers[i].name) != 0)
      {
        continue;
      }
      if (m_bms[i].m_buff == 0)
      {
        m_bms[i].m_mgr.setup(m_buffers->buffers[i].name);
        int sc = m_bms[i].m_mgr.mapMonitorSections();
        if (!lib_rtl_is_success(sc))
          continue;
        m_bms[i].m_buff = &m_buffers->buffers[i];
      }
      m_numBM++;
    }
    else if (m_bms[i].m_buff != 0)
    {
      m_bms[i].m_mgr.unmapSections();
      m_bms[i].m_buff = 0;
    }
  }
  return 1;
}

int Monitor::drop_bm_list()
{
  if (m_numBM > 0)
  {
    for (int i = 0; i < m_buffers->p_bmax; ++i)
    {
      if (m_bms[i].m_buff != 0)
      {
        m_bms[i].m_mgr.unmapSections();
        m_bms[i].m_buff = 0;
      }
    }
  }
  m_numBM = 0;
  if (m_bm_all)
  {
    ::mbmsrv_unmap_global_buffer_info(m_bm_all, false);
    m_bm_all = 0;
  }
  m_buffers = 0;
  if (m_bms)
    delete[] m_bms;
  m_bms = 0;
  return 1;
}
unsigned long long Monitor::myGetTime()
{
  struct timeval tv;
  unsigned long long v;
  gettimeofday(&tv, 0);
  v = tv.tv_sec;
  v = v * 1000 * 1000 + tv.tv_usec;
  return v;
}
