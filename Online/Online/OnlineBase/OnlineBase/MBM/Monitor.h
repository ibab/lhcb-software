// ====================================================================
//  MBM Monitor
// --------------------------------------------------------------------
//
//	Author    : Markus Frank
//
// ====================================================================
#ifndef ONLINE_ONLINEKERNEL_MBM_MONITOR_H
#define ONLINE_ONLINEKERNEL_MBM_MONITOR_H

#include <cstdarg>
#include "MBM/bmstruct.h"
#include "CPP/Interactor.h"
#include "CPP/MonitorDisplay.h"
#include <map>
#include <string>

/*
 * MBM namespace declaration
 */
namespace MBM {

  struct DisplayDescriptor;
  class USSTAT
  {
  public:
    std::string Name;
    unsigned long long evprod,evseen,evfreed;
    CONTROL *ctrl;
    USSTAT():
      Name(""),evprod(0),evseen(0),evfreed(0),ctrl(0)
    {
    }
  };
  typedef std::map<std::string,USSTAT*> UMap_t;
  typedef struct
  {
    UMap_t UserMap;
    unsigned long long evprod,evactual,evcons;
  }BMSTAT;
  typedef struct ShadowBMs
  {
    int nusers;
    USER *us;
    CONTROL *ctrl;
  }ShadowBMs_t;
  typedef std::map<std::string,BMSTAT*> BMMap_t;
  class Monitor : public Interactor  {
  protected:
    DisplayDescriptor*  m_bms;
    int                 m_numBM;
    char                m_buffID[32];
    char*               m_bmid;
    lib_rtl_gbl_t       m_bm_all;
    BUFFERS*            m_buffers;
    MonitorDisplay*     m_display;
    ShadowBMs_t         *m_shadowBMs;
    BMMap_t             m_BMMap;
    bool                m_updating;
  public:
    MonitorDisplay* display() { return m_display; }
    virtual int monitor();
    virtual int initMonitor();
    virtual int updateDisplay();
    virtual int show_information();
    virtual size_t draw_buffer(const char* name, CONTROL* ctrl);
    virtual void getOptions(int argc, char** argv);
    virtual int optparse (const char* c);
    int get_bm_list();
    int drop_bm_list();
    static size_t print(void* ctxt, int level, const char* format, va_list args);
    virtual void handle(const Event& ev);
    Monitor(int argc, char** argv, MonitorDisplay* disp);
    virtual ~Monitor();
    unsigned long long  myGetTime(); //in microsecs since epoche
    unsigned long long LastTime;
    unsigned long long currentTime;
    unsigned long long deltaTime;
    bool rateMode;
    void CopyData();
  };
}

#endif // ONLINE_ONLINEKERNEL_MBM_MONITOR_H
