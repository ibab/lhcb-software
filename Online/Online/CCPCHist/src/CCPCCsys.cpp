#include "CCPCCSys.h"
//#include "HistServer.h"
#include "HistService.h"
#include "HistRPC.h"
#include "iCCPCHist.h"
#include "HistTimer.h"
#include "CCPCSerializer.h"
#include "MonSys.h"
static int mpty;
CCPCCSys::CCPCCSys()
{
  bufsiz = 0;
  buffersize = 0;
  m_numObjs = 0;
  m_updateTimer = new HistTimer(this);
  MonSys::m_instance().addSubSys(this);
}
CCPCCSys::~CCPCCSys()
{
  delete m_updateTimer;
  delete (HistRPC*)m_rpc;
  delete m_ser;
}
void CCPCCSys::setup (char *n)
{
  char *nodename;
//#ifdef WIN32
//  nodename  = getenv("COMPUTERNAME");
//#else
//  nodename  = getenv("HOSTNAME");
//#endif
//  if (nodename != 0)
//  {
//    m_name  = nodename;
//    m_name += "_CNT_";
//    m_name  += n;
//  }
//  else
//  {
    m_name  = n;
  //}
  std::string nam (m_name);
  nam = nam+"/CounterCommand";
  //m_ser = new CCPCSerializer(&m_hists);
  m_ser = new CCPCSerializer(&m_Objmap);
  
  m_rpc = new HistRPC(m_ser, (char*)nam.c_str(), "I:1;C","C");
  nam = m_name;
  nam = nam+"/CounterData";
  //m_genSrv = new DimService(nam, "F",(void*)&mpty, 4);
  m_genSrv = new HistService(m_ser,(char*)nam.c_str(), "C",(void*)&mpty, 4);
  nam = m_name;
  nam = nam+"/CounterEORData";
  m_EORsvc = new HistService(m_ser,(char*)nam.c_str(),"C",(void*)&mpty,4);
  m_EORsvc->setEORflag(true);
  //m_serv->start(m_name);
  lib_rtl_create_lock(m_name.c_str(),&m_lockid);
  return;
}

CCPCCSys& CCPCCSys::m_instance()  {
  static CCPCCSys s;
  return s;
}

