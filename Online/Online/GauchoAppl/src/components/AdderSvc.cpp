#include "AdderSvc.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IHistogramSvc.h"

#include "GauchoAppl/HistAdder.h"
#include "GauchoAppl/CounterAdder.h"
#include "GauchoAppl/SaveTimer.h"
#include "GauchoAppl/AdderSys.h"
#include "Gaucho/Utilities.h"
#include "Gaucho/IGauchoMonitorSvc.h"
#include "AIDA/IHistogram.h"
#include "Gaucho/SegvHandler.h"

DECLARE_SERVICE_FACTORY(AdderSvc)

namespace    {
  void EORSaver(void *arg,void* /* b */ ,int bsiz, MonMap* /* m */, MonAdder *caller)
  {
//    fprintf(caller->logFile,"Buffer at %p Size %d Buffer End %p\n",b,bsiz,AddPtr(b,bsiz));
//    fprintf(caller->logFile,"Map Dump at entry of file saving\n");
//    m->dumpKeys(caller->logFile);
    SaveTimer *tim = (SaveTimer*) arg;
    tim->timerHandler();
  }
}
namespace ADDERSVC
{
  void doPause(void *arg)
  {
    AdderSvc *tis = (AdderSvc*)arg;
    tis->gotoPause();
  }
}

class MyErrh : public DimErrorHandler
{
public:
  bool m_flag;
  void errorHandler(int severity, int code, char *msg)
  {
    if (m_flag)
      {
  ::lib_rtl_output(LIB_RTL_INFO,"DIM(AdderSvc): Code %d.%x %s\n",severity,code,msg);
      }
    return;
  }
  MyErrh () : DimErrorHandler()
  {
    m_flag = true;
  }
  void start()
  {
    m_flag = true;
  }
  void stop()
  {
    m_flag = false;
  }
};

AdderSvc::AdderSvc(const std::string& name, ISvcLocator* sl) : Service(name,sl),m_incidentSvc(0)
{
  SegvHandler::instance();
  declareProperty("MyName",          m_MyName       = "");
  declareProperty("InDNS",           m_InputDNS     = "");
  declareProperty("OutDNS",          m_OutputDNS     = "<dns>");
  declareProperty("SaveRootDir",     m_SaveRootDir  = "/home/beat/Hist/Savesets");
  declareProperty("IsSaver",         m_isSaver      = false);
  declareProperty("SaveInterval",    m_SaveInterval = 900);
  declareProperty("PartitionName",   m_PartitionName= "LHCb");
  declareProperty("SaveSetTaskName", m_SaverTaskName= "Moore");
  declareProperty("ExpandRate",      m_expandRate   = false);
  declareProperty("DoHistos",        m_dohisto      = true);
  declareProperty("AdderClass",      m_AdderClass   = "hists"); //Possible values are 'hists' for histigrams or 'counter' for counters.
  declareProperty("TaskPattern",     m_TaskPattern);
  declareProperty("ServicePattern",  m_ServicePattern);
  declareProperty("ReceiveTimeout",  m_recvtmo=0);
  declareProperty("GotoPause",  m_doPause=false);
  declareProperty("PauseOnEOR",m_PauseOnEOR = false);
  declareProperty("SaveonUpdate",m_SaveonUpdate=false);

  m_started     = false;
  m_SaveTimer   = 0;
  m_errh        = 0;
  m_pMonitorSvc = 0;
  m_phistsvc    = 0;
  m_arrhist     = 0;
  m_MyServiceName = name;
  m_AdderSys = 0;
  m_SaveonUpdate = false;
}

AdderSvc::~AdderSvc()
{
}

StatusCode AdderSvc::queryInterface(const InterfaceID& riid, void** ppvIF)
{
  if ( IIncidentListener::interfaceID().versionMatch(riid) )
  {
    *ppvIF = (IIncidentListener*)this;
  }
  else
  {
    return Service::queryInterface(riid, ppvIF);
  }
  addRef();
  return StatusCode::SUCCESS;
}

StatusCode AdderSvc::initialize()
{
  StatusCode sc = Service::initialize();
  MsgStream msg( msgSvc(), name() );
//  dis_set_debug_on();
//  dic_set_debug_on();
  if( !sc.isSuccess() )
  {
    return sc;
  }
  if ( m_TaskPattern.empty() )
  {
    msg << MSG::FATAL << "The option \"TaskPattern\" MUST be set!" << endmsg;
    return StatusCode::FAILURE;
  }
  if ( m_ServicePattern.empty() )
  {
    msg << MSG::FATAL << "The option \"ServicePattern\" MUST be set!" << endmsg;
    return StatusCode::FAILURE;
  }
  if(m_dohisto)
  {
    sc = serviceLocator()->service("MonitorSvc", m_pMonitorSvc, true);
    if( sc.isSuccess() )
    {
      msg << MSG::DEBUG << "Found the IMonitorSvc interface" << endreq;
    }
    else
    {
      msg << MSG::FATAL << "Unable to locate the IGauchoMonitorSvc interface." << endreq;
      return StatusCode::FAILURE;
    }
    sc = serviceLocator()->service("HistogramDataSvc",m_phistsvc,true);
    if( sc.isSuccess() )
    {
      msg << MSG::DEBUG << "Found the Histogram Data Service interface" << endreq;
    }
    else
    {
      msg << MSG::FATAL << "Unable to locate the Histogram Data Service interface." << endreq;
      return StatusCode::FAILURE;
    }
  }
  m_adder = 0;
  m_EoRadder = 0;
  m_SaveTimer = 0;
  sc = serviceLocator()->service("IncidentSvc",m_incidentSvc,true);
  if( !sc.isSuccess() ) {
    return sc;
  }
  m_incidentSvc->addListener(this,"APP_INITIALIZED");
  m_incidentSvc->addListener(this,"APP_RUNNING");
  m_incidentSvc->addListener(this,"APP_STOPPED");
  // Book arrival time histograms
  if (m_dohisto)
  {
    m_arrhist = m_phistsvc->book(name()+"/arrivaltime","Adder Packet Arrival Time in seconds",100,0.0,50.0);
    m_pMonitorSvc->declareInfo("ArrivalTimes",(const AIDA::IBaseHistogram*)m_arrhist,"",this);
  }
  return StatusCode::SUCCESS;
}

StatusCode AdderSvc::start()
{
  std::string servicename;
  std::string myservicename;
  std::string nodename = RTL::nodeNameShort();

  Service::start();
  if (m_errh == 0) m_errh = new MyErrh();
  toLowerCase(m_TaskPattern);
  toLowerCase(m_ServicePattern);
  toLowerCase(nodename);
  toLowerCase(m_AdderClass);
  toLowerCase(m_InputDNS);
  toLowerCase(m_OutputDNS);
  if (m_AdderClass == "hists")
  {
    servicename = "Histos";
  }
  else if(m_AdderClass == "counter")
  {
    servicename = "Counter";
  }
// Nodeadders:
// Source task names:
//  Reconstruction task structure: <Partition>_<Node>_RecBrunel_xx
//  HLT Task structure: <node>_GAUCHOJOB_xx
//  Nodeadder output name:
//   Reconstruction: <part>_<node>_RecAdder_xx
//   HLT: <node>_Adder_xx
//
// subfarm adders:
// Source task names (all Farms):
//  <node>_Adder_xx
//  subfarm adder output name:
//   Reconstruction: <part>_<node>_RecAdder_xx
//   HLT: <sfnname>_Adder_xx     sfname = hltxyy
//
  StringReplace(m_TaskPattern,"<node>",nodename);
  StringReplace(m_TaskPattern,"<part>", m_PartitionName);
  m_TaskPattern += "(.*)";
  StringReplace(m_ServicePattern,"<node>",nodename);
  StringReplace(m_ServicePattern,"<part>", m_PartitionName);
  myservicename = m_MyName;
  StringReplace(myservicename, "<part>", m_PartitionName);
  StringReplace(myservicename, "<node>", nodename);
  const char *dnsnode = getenv("DIM_DNS_NODE");
  std::string ddns(dnsnode ? dnsnode : "");
  toLowerCase(ddns);
  StringReplace(m_InputDNS,"<node>",nodename);
  StringReplace(m_InputDNS,"<dns>",ddns);
  if (m_OutputDNS.length() != 0)
  {
    StringReplace(m_OutputDNS,"<node>",nodename);
    StringReplace(m_OutputDNS,"<dns>",ddns);
  }
  else
  {
    m_OutputDNS = ddns;
  }
  if (m_OutputDNS != ddns)
  {
    if (MonAdder::m_ServiceDns == 0)
    {
      MonAdder::m_ServiceDns = new DimServerDns(m_OutputDNS.c_str());
    }
  }
  m_errh->start();
  if (m_started) return StatusCode::SUCCESS;
  if (m_errh != 0) DimClient::addErrorHandler(m_errh);
  ::lib_rtl_output(LIB_RTL_DEBUG,"=======>AdderSvc Option Summary:\tTask Pattern %s\n",m_TaskPattern.c_str());
  ::lib_rtl_output(LIB_RTL_DEBUG,"        Service Pattern %s+Data or EOR\n",m_ServicePattern.c_str());
  DimServer::autoStartOn();
  if (!m_InputDNS.empty()) DimClient::setDnsNode(m_InputDNS.c_str());
  m_AdderSys = &AdderSys::Instance();
  if (m_AdderClass == "hists")
  {
    m_adder = new HistAdder((char*)myservicename.c_str(), (char*)"Data");
  }
  else if (m_AdderClass == "counter")
  {
    m_adder = new CounterAdder((char*)myservicename.c_str(), (char*)"Data");
  }
  m_adder->setParent(this);
  m_adder->setPause(m_doPause);
  if (m_doPause)
  {
    m_adder->SetPauseFn(ADDERSVC::doPause,this);
  }
  m_adder->m_IsEOR = false;
  m_adder->m_expandRate = m_expandRate;
  if (m_expandRate)
  {
    m_adder->m_NamePrefix = m_PartitionName+"_";
  }
  m_adder->m_taskPattern = m_TaskPattern;
  m_adder->m_servicePattern = m_ServicePattern+std::string("Data");
  m_adder->setIsSaver(m_isSaver);
  m_adder->m_dohisto = m_dohisto;
  m_adder->m_histo = m_arrhist;
  m_adder->m_rectmo = m_recvtmo;
  m_adder->m_monsvc = dynamic_cast<IGauchoMonitorSvc*>(m_pMonitorSvc);
  m_adder->m_MyServiceName = m_MyServiceName;
  m_adder->setSaveonUpdate(m_SaveonUpdate);
  m_adder->Configure();
  m_AdderSys->Add(m_adder);
  if (m_isSaver)
  {
    m_SaveTimer = new SaveTimer(m_adder,m_SaveInterval);
    m_SaveTimer->setPartName(m_PartitionName);
    m_SaveTimer->setRootDir(m_SaveRootDir);
    m_SaveTimer->setTaskName(m_SaverTaskName);
    m_SaveTimer->setEOR(false);
    m_SaveTimer->setSaveSetService();
    m_SaveTimer->Start();
  }

  if (m_AdderClass == "hists")
  {
    m_EoRadder = new HistAdder((char*)myservicename.c_str(), (char*)"EOR");
  }
  else if (m_AdderClass == "counter")
  {
    m_EoRadder = new CounterAdder((char*)myservicename.c_str(), (char*)"EOR");
  }
  m_EoRadder->setParent(this);
  m_EoRadder->setPause(m_PauseOnEOR);
  if (m_PauseOnEOR)
  {
    m_EoRadder->SetPauseFn(ADDERSVC::doPause,this);
  }
  m_EoRadder->m_IsEOR = true;
  m_EoRadder->m_expandRate = false;
  m_EoRadder->m_taskPattern = m_TaskPattern;
  m_EoRadder->m_servicePattern = m_ServicePattern+std::string("EOR");
  m_EoRadder->m_noRPC = true;
  m_EoRadder->setIsSaver(m_isSaver);
  m_EoRadder->m_rectmo = 500*m_recvtmo;
  m_EoRadder->m_MyServiceName = m_MyServiceName;
  m_EoRadder->Configure();
  m_AdderSys->Add(m_EoRadder);
  if (m_isSaver)
  {
    m_EoRSaver = new SaveTimer(m_EoRadder,m_SaveInterval);
    m_EoRSaver->setPartName(m_PartitionName);
    m_EoRSaver->setRootDir(m_SaveRootDir);
    m_EoRSaver->setTaskName(m_SaverTaskName);
    m_EoRSaver->setEOR(true);
    m_EoRSaver->setSaveSetService(m_SaveTimer->getSaveSetService());
    m_EoRadder->SetCycleFn(EORSaver,(void*)m_EoRSaver);
  }
  m_started = true;
  return StatusCode::SUCCESS;
}

StatusCode AdderSvc::stop()
{
  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "AdderSvc Stop called" << endreq;
  IGauchoMonitorSvc* psvc = dynamic_cast<IGauchoMonitorSvc*>(m_pMonitorSvc);
  if (psvc) psvc->resetHistos(this);

  if (m_isSaver)
  {
//    m_SaveTimer->Stop();
  }
  m_errh->stop();
  return Service::stop();
}

StatusCode AdderSvc::finalize()
{
  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "AdderSvc finalize called" << endreq;
  {
    DimLock l;
    if (m_arrhist != 0)
    {
  //    m_adder->m_histo = 0;
  //    m_phistsvc->unregisterObject((AIDA::IBaseHistogram*)m_arrhist);
    }
    m_arrhist = 0;
    if (m_adder != 0)
    {
      m_adder->m_histo = 0;
    }
    if ( m_pMonitorSvc )
    {
      m_pMonitorSvc->undeclareAll(this);
    }
    if ( m_incidentSvc )
    {
      m_incidentSvc->removeListener(this);
    }
    releasePtr(m_incidentSvc);
    releasePtr(m_phistsvc);
    releasePtr(m_pMonitorSvc);
    //printf("AdderSvc: Locking DIM\n");
    if (m_SaveTimer != 0)
    {
      m_SaveTimer->Stop();
      deletePtr(m_SaveTimer);
    }
    if (m_AdderSys != &AdderSys::Instance())
    {
      printf("****** m_AdderSys was overwritten!!! Reestablishing it...\n");
      m_AdderSys = &AdderSys::Instance();
    }
    if (m_AdderSys !=0)  m_AdderSys->stop();
    if (m_adder != 0)
    {
      m_adder->stop();
      m_AdderSys->Remove(m_adder);
      deletePtr(m_adder);
    }
    if (m_EoRadder != 0)
    {
      m_EoRadder->stop();
      m_AdderSys->Remove(m_EoRadder);
      deletePtr(m_EoRadder);
    }
    //printf("AdderSvc: UNLocking DIM\n");
  }
  return Service::finalize();
}
void AdderSvc::gotoPause()
{
  m_incidentSvc->fireIncident(Incident(name(),"DAQ_PAUSE"));
}
void AdderSvc::handle(const Incident& inc)
{
  if (inc.type() == "APP_RUNNING")
  {
    m_AdderSys->start();
  }
//  else if (inc.type() == "APP_STOPPED")
//  {
//    m_AdderSys->stop();
//  }
}
