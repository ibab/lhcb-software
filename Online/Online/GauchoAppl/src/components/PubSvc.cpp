#include "PubSvc.h"
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
#include "Gaucho/MonCounter.h"

DECLARE_SERVICE_FACTORY(PubSvc)
class MyErrh : public DimErrorHandler
{
public:
  bool m_flag;
  void errorHandler(int severity, int code, char *msg)
  {
    if (m_flag)
      {
        ::lib_rtl_output(LIB_RTL_INFO,"DIM(PubSvc): Code %d.%x %s\n",severity,code,msg);
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

PubSvc::PubSvc(const std::string& name, ISvcLocator* sl) : Service(name,sl),m_incidentSvc(0)
{
  declareProperty("MyName",          m_MyName       = "");
  declareProperty("InDNS",           m_InputDNS     = "");
  declareProperty("PartitionName",   m_PartitionName= "LHCb");
  declareProperty("TaskPattern",     m_TaskPattern);
  declareProperty("ServicePattern",  m_ServicePattern);
  declareProperty("AdderClass",      m_AdderClass   = "counter"); //Possible values are 'hists' for histigrams or 'counter' for counters.

  m_started     = false;
  m_SaveTimer   = 0;
  m_errh        = 0;
  m_pMonitorSvc = 0;
//  m_phistsvc    = 0;
  m_arrhist     = 0;
  m_trender = 0;

}

PubSvc::~PubSvc()
{
}

StatusCode PubSvc::queryInterface(const InterfaceID& riid, void** ppvIF)
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

StatusCode PubSvc::initialize()
{
  StatusCode sc = Service::initialize();
  MsgStream msg( msgSvc(), name() );
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
//  if(m_dohisto)
//  {
//    sc = serviceLocator()->service("MonitorSvc", m_pMonitorSvc, true);
//    if( sc.isSuccess() )
//    {
//      msg << MSG::DEBUG << "Found the IMonitorSvc interface" << endreq;
//    }
//    else
//    {
//      msg << MSG::FATAL << "Unable to locate the IGauchoMonitorSvc interface." << endreq;
//      return StatusCode::FAILURE;
//    }
//    sc = serviceLocator()->service("HistogramDataSvc",m_phistsvc,true);
//    if( sc.isSuccess() )
//    {
//      msg << MSG::DEBUG << "Found the Histogram Data Service interface" << endreq;
//    }
//    else
//    {
//      msg << MSG::FATAL << "Unable to locate the Histogram Data Service interface." << endreq;
//      return StatusCode::FAILURE;
//    }
//  }
  m_adder = 0;
  m_EoRadder = 0;
  m_SaveTimer = 0;
  m_AdderSys = &AdderSys::Instance();
  sc = serviceLocator()->service("IncidentSvc",m_incidentSvc,true);
  if( !sc.isSuccess() ) {
    return sc;
  }
  m_incidentSvc->addListener(this,"APP_INITIALIZED");
  m_incidentSvc->addListener(this,"APP_RUNNING");
  m_incidentSvc->addListener(this,"APP_STOPPED");
  // Book arrival time histograms
  return StatusCode::SUCCESS;
}

StatusCode PubSvc::start()
{
  std::string servicename;
  std::string myservicename;
  std::string nodename = RTL::nodeNameShort();
  m_prevupdate = 0;
  Service::start();
  if (m_errh == 0) m_errh = new MyErrh();
  toLowerCase(m_TaskPattern);
  toLowerCase(m_ServicePattern);
  toLowerCase(nodename);
//  m_AdderClass = "counter";
  toLowerCase(m_AdderClass);
  toLowerCase(m_InputDNS);
  servicename = "Counter";
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
  m_myservicename = m_MyName;
  StringReplace(m_myservicename, "<part>", m_PartitionName);
  StringReplace(m_myservicename, "<node>", nodename);
  const char *dnsnode = getenv("DIM_DNS_NODE");
  std::string ddns(dnsnode ? dnsnode : "");
  StringReplace(m_InputDNS,"<node>",nodename);
  StringReplace(m_InputDNS,"<dns>",ddns);
  m_errh->start();
  if (m_started) return StatusCode::SUCCESS;
  if (m_errh != 0) DimClient::addErrorHandler(m_errh);
  ::lib_rtl_output(LIB_RTL_DEBUG,"=======>PubSvc Option Summary:\tTask Pattern %s\n",m_TaskPattern.c_str());
  ::lib_rtl_output(LIB_RTL_DEBUG,"        Service Pattern %s+Data or EOR\n",m_ServicePattern.c_str());
  DimServer::autoStartOn();
  if (!m_InputDNS.empty()) DimClient::setDnsNode(m_InputDNS.c_str());
  if (m_AdderClass == "counter")
  {
    m_adder = new CounterAdder((char*)m_myservicename.c_str(), (char*)"Data");
  }
  else
  {
    m_adder = new HistAdder((char*)m_myservicename.c_str(), (char*)"Data");
  }
  m_adder->m_IsEOR = false;
  m_adder->m_expandRate = false;
  m_adder->m_taskPattern = m_TaskPattern;
  m_adder->m_servicePattern = m_ServicePattern+std::string("Data");
  m_adder->setIsSaver(false);
  m_adder->m_dohisto = false;
  m_adder->m_histo = 0;
  m_adder->m_rectmo = 0;
  m_adder->m_disableOutput = true;
  m_adder->m_noRPC = true;
  m_adder->m_monsvc = dynamic_cast<IGauchoMonitorSvc*>(m_pMonitorSvc);
  m_adder->Configure();
  m_AdderSys->Add(m_adder);
  m_started = true;
  return StatusCode::SUCCESS;
}

StatusCode PubSvc::stop()
{
  IGauchoMonitorSvc* psvc = dynamic_cast<IGauchoMonitorSvc*>(m_pMonitorSvc);
  if (psvc) psvc->resetHistos(this);

  m_errh->stop();
  return Service::stop();
}

StatusCode PubSvc::finalize()
{
  m_arrhist = 0;
  if ( m_pMonitorSvc )
  {
    m_pMonitorSvc->undeclareAll(this);
  }
  if ( m_incidentSvc )
  {
    m_incidentSvc->removeListener(this);
  }
  releasePtr(m_incidentSvc);
//  releasePtr(m_phistsvc);
  releasePtr(m_pMonitorSvc);
  //printf("PubSvc: Locking DIM\n");
//  DimLock l;
  if (m_SaveTimer != 0)
  {
    m_SaveTimer->Stop();
    deletePtr(m_SaveTimer);
  }
  if (m_adder != 0)
  {
    m_AdderSys->Remove(m_adder);
    deletePtr(m_adder);
  }
  if (m_EoRadder != 0)
  {
    m_AdderSys->Remove(m_EoRadder);
    deletePtr(m_EoRadder);
  }
  //printf("PubSvc: UNLocking DIM\n");
  return Service::finalize();
}

void PubSvc::handle(const Incident& inc)
{
  if (inc.type() == "APP_RUNNING")
  {
    m_AdderSys->start();
  }
  else if (inc.type() == "APP_STOPPED")
  {
    m_AdderSys->stop();
  }
}
