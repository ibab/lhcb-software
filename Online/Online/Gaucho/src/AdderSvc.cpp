
#include "AdderSvc.h"
#include "GaudiKernel/SvcFactory.h"


#include "TPad.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TApplication.h"
#include "Gaucho/Utilities.h"


//DECLARE_SERVICE_FACTORY(AdderSvc)
//DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb, AdderSvc)
DECLARE_SERVICE_FACTORY(AdderSvc)
extern "C"
{
  void EORSaver(void *arg,void* ,int , HistMap *, MonAdder *)
  {
    SaveTimer *tim = (SaveTimer*) arg;
    tim->timerHandler();
  }
}
AdderSvc::AdderSvc(const std::string& name, ISvcLocator* sl) : Service(name,sl)
{
//  StatusCode sc = Service::initialize();
//  printf("AdderSvc... Constructing...\n");
  declareProperty("SourceTaskPattern", m_TaskName="");
  declareProperty("MyName",m_MyName="Adder");
//  declareProperty("SourceServiceName",m_ServiceName="Data");
  declareProperty("InDNS",m_InputDNS="");
  declareProperty("OutDNS",m_OutputDNS="");
  declareProperty("TopLevel",m_TopLevel=false);
  declareProperty("SaveRootDir", m_SaveRootDir = "/home/beat/Hist/Savesets");
  declareProperty("IsSaver", m_isSaver = false);
  declareProperty("SaveInterval", m_SaveInterval = 900);
  declareProperty("PartitionName",m_PartitionName="LHCb");
  declareProperty("SaveSetTaskName",m_SaverTaskName="Moore");
//  declareProperty("IsEOR",m_isEOR=false);
  declareProperty("ExpandRate",m_ExpandRate=false);
  declareProperty("TaskPattern",m_TaskPattern="");
  declareProperty("ServicePattern",m_ServicePattern="");
  declareProperty("AdderType",m_AdderType="node"); //Possible values are
//  'node' for a node adder,
//  'sf' or 'subfarm' for subfarm adder
//  'top' or 'part' for top or partition adder
  m_SaveTimer = 0;
}
AdderSvc::~AdderSvc()
{
  return;
}
StatusCode AdderSvc::queryInterface(const InterfaceID& riid, void** ppvIF)
{
  if(IService::interfaceID().versionMatch(riid))
  {
    *ppvIF = dynamic_cast<AdderSvc*> (this);
  }
  else
  {
    return Service::queryInterface(riid, ppvIF);
  }
  addRef();
  return StatusCode::SUCCESS;
}
TApplication *app;
StatusCode AdderSvc::initialize()
{
  Service::initialize();
  return StatusCode::SUCCESS;
}
#include "TBrowser.h"
StatusCode AdderSvc::start()
{
  Service::start();
  std::string myservicename;
  m_MyName = RTL::processName();
  toLowerCase(m_TaskPattern);
  toLowerCase(m_ServicePattern);
  toLowerCase(m_AdderType);
  std::string nodename = RTL::nodeNameShort();
  toLowerCase(nodename);
// Nodeadders:
// Source task names:
//  Reconstruction task structure: <Partition>_<Node>_Brunel_xx
//  HLT Task structure: <node>_GAUCHOJOB_xx
//
// subfarm adders:
// Source task names (all Farms):
//  <node>_Adder_xx
  if (m_AdderType == "node")
  {
    if (m_TaskPattern == "")
    {
      m_TaskPattern = nodename+std::string("_")+m_TaskName+std::string("(.*)");
    }
    else
    {
      m_TaskPattern = nodename+std::string("_")+m_TaskPattern+std::string("(.*)");
    }
//    m_MyName = nodename+std::string("_Adder");
    m_ServicePattern = std::string("MON_")+m_TaskPattern+std::string("/Histos/");//+m_ServiceName;
    if (m_InputDNS == "")
    {
      m_InputDNS = nodename.substr(0,nodename.size()-2);
    }
    myservicename = m_MyName;
  }
  else if (m_AdderType == "sf" || m_AdderType == "subfarm")
  {
    if (m_TaskPattern == 0)
    {
      m_TaskPattern = nodename+std::string("[0-9][0-9]_Adder(.*)");
    }
    else
    {
      m_TaskPattern = nodename+m_TaskPattern+std::string("(.*)");
    }
//    m_MyName = nodename+std::string("_Adder");
    m_ServicePattern = std::string("MON_")+m_TaskPattern+std::string("/Histos/");//+m_ServiceName;
    if (m_InputDNS == "")
    {
      m_InputDNS = nodename;
    }
    myservicename = m_MyName.replace(m_MyName.find("_"),1,std::string("_")+m_PartitionName+std::string("_"));
  }
  else if (m_AdderType == "top" || m_AdderType == "part")
  {
    m_TaskPattern = std::string("_Adder(.*)");
    m_MyName = m_PartitionName+std::string("_Adder");
    m_ServicePattern = std::string("MON_hlt[a-z][0-9][0-9]_")+m_PartitionName+m_TaskPattern+std::string("/Histos/");//+m_ServiceName;
    if (m_InputDNS == "")
    {
      m_InputDNS = std::string("hlt01");
    }
    myservicename = m_PartitionName+std::string("_Adder");
  }
  else
  {
    printf("FATAL... Unknown Adder Type %s\n",m_AdderType.c_str());
  }
  DimClient::setDnsNode(m_InputDNS.c_str());
//  m_adder = new HistAdder((char*)m_TaskName.c_str(), (char*)m_MyName.c_str(), (char*)m_ServiceName.c_str());
  m_adder = new HistAdder((char*)m_TaskName.c_str(), (char*)myservicename.c_str(), (char*)"Data");
  m_adder->setOutDNS(m_OutputDNS);
  m_adder->m_IsEOR = false;
  m_adder->m_expandRate = m_ExpandRate;
  m_adder->m_taskPattern = m_TaskPattern;
  m_adder->m_servicePattern = m_ServicePattern+std::string("Data");
  m_adder->setIsSaver(m_isSaver);
  m_adder->Configure();
  if (m_isSaver)
  {
    m_SaveTimer = new SaveTimer(m_adder,m_SaveInterval);
    m_SaveTimer->setPartName(m_PartitionName);
    m_SaveTimer->setRootDir(m_SaveRootDir);
    m_SaveTimer->setTaskName(m_SaverTaskName);
    m_SaveTimer->setEOR(false);
    m_SaveTimer->Start();
  }

  m_EoRadder = new HistAdder((char*)m_TaskName.c_str(), (char*)myservicename.c_str(), (char*)"EOR");
  m_EoRadder->setOutDNS(m_OutputDNS);
  m_EoRadder->m_IsEOR = true;
  m_EoRadder->m_expandRate = false;
  m_EoRadder->m_taskPattern = m_TaskPattern;
  m_EoRadder->m_servicePattern = m_ServicePattern+std::string("EOR");
  m_EoRadder->m_noRPC = true;
  m_EoRadder->setIsSaver(m_isSaver);
  m_EoRadder->Configure();
  if (m_isSaver)
  {
    m_EoRSaver = new SaveTimer(m_EoRadder,m_SaveInterval);
    m_EoRSaver->setPartName(m_PartitionName);
    m_EoRSaver->setRootDir(m_SaveRootDir);
    m_EoRSaver->setTaskName(m_SaverTaskName);
    m_EoRSaver->setEOR(true);
    m_EoRadder->SetCycleFn(EORSaver,(void*)m_SaveTimer);
  }
  return StatusCode::SUCCESS;
}
StatusCode AdderSvc::stop()
{
  Service::stop();
  if (m_isSaver)
  {
    m_SaveTimer->Stop();
  }
  return StatusCode::SUCCESS;
}
StatusCode AdderSvc::finalize()
{
  delete m_SaveTimer;
  delete m_adder;
  Service::finalize();
  return StatusCode::SUCCESS;
}
