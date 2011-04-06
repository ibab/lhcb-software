
#include "AdderSvc.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IHistogramSvc.h"

#include "Gaucho/Utilities.h"
#include "Gaucho/IGauchoMonitorSvc.h"

DECLARE_SERVICE_FACTORY(AdderSvc)

namespace
{
  void EORSaver(void *arg,void* ,int , MonMap *, MonAdder *)
  {
    SaveTimer *tim = (SaveTimer*) arg;
    tim->timerHandler();
  }
}

AdderSvc::AdderSvc(const std::string& name, ISvcLocator* sl) : Service(name,sl),m_incidentSvc(0)
{
  declareProperty("MyName",m_MyName="");
  declareProperty("InDNS",           m_InputDNS="");
  declareProperty("OutDNS",          m_OutputDNS="");
  declareProperty("SaveRootDir",     m_SaveRootDir = "/home/beat/Hist/Savesets");
  declareProperty("IsSaver",         m_isSaver = false);
  declareProperty("SaveInterval",    m_SaveInterval = 900);
  declareProperty("PartitionName",   m_PartitionName="LHCb");
  declareProperty("SaveSetTaskName", m_SaverTaskName="Moore");
  declareProperty("ExpandRate",      m_ExpandRate=false);
  declareProperty("TaskPattern",     m_TaskPattern);
  declareProperty("ServicePattern",  m_ServicePattern);
  declareProperty("DoHistos",        m_dohisto=true);
  declareProperty("AdderClass",      m_AdderClass="hists"); //Possible values are 'hists' for histigrams or 'counter' for counters.

  m_SaveTimer = 0;
  m_started = false;
  m_errh =0;
  m_funcsvc = 0;
  m_pMonitorSvc = 0;
  m_phistsvc=0;
  m_arrhist = 0;
}

AdderSvc::~AdderSvc()
{
}

StatusCode AdderSvc::queryInterface(const InterfaceID& riid, void** ppvIF)
{
  if ( IIncidentListener::interfaceID().versionMatch(riid) ) {
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
  m_AdderSys = &AdderSys::Instance();
  sc = serviceLocator()->service("IncidentSvc",m_incidentSvc,true);
  if( !sc.isSuccess() ) {
    return sc;
  }
  m_incidentSvc->addListener(this,"APP_INITIALIZED");
  m_incidentSvc->addListener(this,"APP_RUNNING");
  m_incidentSvc->addListener(this,"APP_STOPPED");
  return StatusCode::SUCCESS;
}
#include "TBrowser.h"

StatusCode AdderSvc::start()
{
  Service::start();
  if (m_errh == 0) m_errh = new MyErrh();
  std::string myservicename;
//  m_MyName = RTL::processName();
  toLowerCase(m_TaskPattern);
  toLowerCase(m_ServicePattern);
//  toLowerCase(m_AdderType);
  std::string nodename = RTL::nodeNameShort();
  toLowerCase(nodename);
  toLowerCase(m_AdderClass);
  toLowerCase(m_InputDNS);
  std::string servicename;
  if (m_AdderClass == "hists")
  {
    servicename = "Histos";
  }
  else if(m_AdderClass == "counter")
  {
    servicename = "Counter";
  }
  if (m_dohisto)
  {
    m_arrhist = m_phistsvc->book(name()+"/arrivaltime","Adder Packet Arrival Time in seconds",100,0.0,50.0);
    m_pMonitorSvc->declareInfo("ArrivalTimes",(const AIDA::IBaseHistogram*)m_arrhist,"",this);
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
  StringReplace(m_InputDNS,"<node>",nodename);
  StringReplace(m_InputDNS,"<dns>",ddns);

//  if (m_AdderType == "node")
//  {
////    m_MyName = nodename+std::string("_Adder");
//    if (m_ServicePattern == "")
//    {
//      m_ServicePattern = "MON_"+m_TaskPattern+"/"+servicename+"/";
//    }
//    if (m_InputDNS == "")
//    {
//      m_InputDNS = nodename.substr(0,nodename.size()-2);
//    }
//  }
//  else if (m_AdderType == "sf" || m_AdderType == "subfarm")
//  {
////    m_MyName = nodename+std::string("_Adder");
//    if (m_ServicePattern == "")
//    {
//      m_ServicePattern = "MON_"+m_TaskPattern+"/"+servicename+"/";//+m_ServiceName;
//    }
//    if (m_InputDNS == "")
//    {
//      m_InputDNS = nodename;
//    }
//  }
//  else if (m_AdderType == "top" || m_AdderType == "part")
//  {
//    if (m_ServicePattern != "")
//    {
//      m_ServicePattern +="/"+servicename+"/";
//    }
//    else
//    {
//      m_ServicePattern = "MON_(.*)[a-z][0-9][0-9]_"+ m_PartitionName+ "_Adder(.*)/Histos/";
//    }
//    if (m_InputDNS == "")
//    {
//      m_InputDNS = std::string("hlt01");
//    }
//  }
//  else
//  {
//    printf("FATAL... Unknown Adder Type %s\n",m_AdderType.c_str());
//  }
  m_errh->start();
  if (m_started) return StatusCode::SUCCESS;
  if (m_errh != 0) DimClient::addErrorHandler(m_errh);
//  printf("=======>AdderSvc Option Summary:\n\tTask Pattern %s\n\tService Pattern %s+Data or EOR\n",m_TaskPattern.c_str(),m_ServicePattern.c_str());
  DimServer::autoStartOn();
  if (!m_InputDNS.empty()) DimClient::setDnsNode(m_InputDNS.c_str());
//  m_adder = new HistAdder((char*)m_TaskName.c_str(), (char*)m_MyName.c_str(), (char*)m_ServiceName.c_str());
  if (m_AdderClass == "hists")
  {
    m_adder = new HistAdder((char*)myservicename.c_str(), (char*)"Data");
  }
  else if (m_AdderClass == "counter")
  {
    m_adder = new CounterAdder((char*)myservicename.c_str(), (char*)"Data");
  }
//  m_adder->setOutDNS(m_OutputDNS);
  m_adder->m_IsEOR = false;
  m_adder->m_expandRate = m_ExpandRate;
  if (m_ExpandRate)
  {
    m_adder->m_NamePrefix = m_PartitionName+"_";
  }
  m_adder->m_taskPattern = m_TaskPattern;
  m_adder->m_servicePattern = m_ServicePattern+std::string("Data");
  m_adder->setIsSaver(m_isSaver);
  m_adder->m_dohisto = m_dohisto;
  m_adder->m_histo = m_arrhist;
  m_adder->m_monsvc = dynamic_cast<IGauchoMonitorSvc*>(m_pMonitorSvc);
  m_adder->Configure();
  m_AdderSys->Add(m_adder);
  if (m_isSaver)
  {
    m_SaveTimer = new SaveTimer(m_adder,m_SaveInterval);
    m_SaveTimer->setPartName(m_PartitionName);
    m_SaveTimer->setRootDir(m_SaveRootDir);
    m_SaveTimer->setTaskName(m_SaverTaskName);
    m_SaveTimer->setEOR(false);
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
  m_EoRadder->setOutDNS(m_OutputDNS);
  m_EoRadder->m_IsEOR = true;
  m_EoRadder->m_expandRate = false;
  m_EoRadder->m_taskPattern = m_TaskPattern;
  m_EoRadder->m_servicePattern = m_ServicePattern+std::string("EOR");
  m_EoRadder->m_noRPC = true;
  m_EoRadder->setIsSaver(m_isSaver);
  m_EoRadder->Configure();
  m_AdderSys->Add(m_EoRadder);
  if (m_isSaver)
  {
    m_EoRSaver = new SaveTimer(m_EoRadder,m_SaveInterval);
    m_EoRSaver->setPartName(m_PartitionName);
    m_EoRSaver->setRootDir(m_SaveRootDir);
    m_EoRSaver->setTaskName(m_SaverTaskName);
    m_EoRSaver->setEOR(true);
    m_EoRadder->SetCycleFn(EORSaver,(void*)m_EoRSaver);
  }
  m_funcsvc = 0;
//  if (m_AdderType == "top" || m_AdderType == "part")
//  {
//    m_Function = m_PartitionName+"_"+m_TaskName;
//    m_funcsvc = new DimService((char*)m_Function.c_str(),(char*)m_utgid.c_str());
//    m_funcsvc->updateService();
//  }
  m_started = true;
  return StatusCode::SUCCESS;
}

StatusCode AdderSvc::stop()
{

  if (m_isSaver)
  {
//    m_SaveTimer->Stop();
  }
  m_errh->stop();
  return Service::stop();
}

StatusCode AdderSvc::finalize()
{
  if ( m_incidentSvc ) {
    m_incidentSvc->removeListener(this);
    releasePtr(m_incidentSvc);
  }
  releasePtr(m_phistsvc);
  releasePtr(m_pMonitorSvc);
  dim_lock();
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
  deletePtr(m_funcsvc);
  dim_unlock();
  return Service::finalize();
}

void AdderSvc::handle(const Incident& inc)
{
  if (inc.type() == "APP_INITIALIZED")
  {
  }
  else if (inc.type() == "APP_RUNNING")
  {
    m_AdderSys->start();
  }
  else if (inc.type() == "APP_STOPPED")
  {
    m_AdderSys->stop();
  }
}
