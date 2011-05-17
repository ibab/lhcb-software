#include "PubSvc.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IHistogramSvc.h"

#include "Gaucho/HistAdder.h"
#include "Gaucho/CounterAdder.h"
#include "Gaucho/SaveTimer.h"
#include "Gaucho/AdderSys.h"
#include "Gaucho/Utilities.h"
#include "Gaucho/IGauchoMonitorSvc.h"
#include "AIDA/IHistogram.h"
#include "Gaucho/MonCounter.h"

DECLARE_SERVICE_FACTORY(PubSvc)
static std::string DetNames[] = {"TDet","VeloA","VeloC","TT","IT","OTA","OTC","RICH1","RICH2","ECal","HCal",
    "MuonA","MuonC","L0DU","TMuA","TMuC","TPU","PRS","TCalo","LHCb"};
static std::vector<std::string> s_counterTypes;

static std::map<std::string,std::string> s_nammap;
namespace
{
  void Analyze(void *arg, void* buff ,int siz, MonMap *mmap, MonAdder *)
  {
    PubSvc *tis = (PubSvc*)arg;
    tis->analyze(buff,siz, mmap);
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

void PubSvc::filldata(const std::string &cnam,MonMap *mmap)
{
  MonMap::iterator it;
  DetData_T<long long>& lb = m_DetMap.find("LHCb")->second;
  it = mmap->find("Runable/"+cnam);
  if ( it != mmap->end() )
  {
    CntrDescr *h = (CntrDescr*)MonCounter::de_serialize(it->second);
    long long *p = (long long*)h->ptr;
    for (int i=0;i<h->nel;i++)
    {
      TellMap_T::iterator t=m_TellMap.find(i);
      if (t != m_TellMap.end())
      {
//        printf("%d %s %s\n", i, t->second.det.c_str(),t->second.name.c_str());
        DetData_T<long long>& d= m_DetMap.find(t->second.det)->second;
        DetData_T<long long>::iterator k = d.find(cnam);

        if (k != d.end())
        {
          k->second += p[i];
          lb[cnam] += p[i];
        }
        else
        {
//          printf ("Cannot find %s in Detector Data Map\n",cnam.c_str());
        }
      }
    }
  }
}
void PubSvc::fillsums()//const std::string &cnam,MonMap *mmap)
{
  DetMap_T<long long>::iterator i;
//  DetData_T<long long>& lb = m_DetMap.find("LHCb")->second;
  for (i=m_DetMap.begin();i!=m_DetMap.end();i++)
  {
    for (int j=0;j<5;j++)
    {
      i->second["TLostMEP"] += i->second[s_counterTypes[j]];
    }
  }
}

void PubSvc::analyze(void *, int ,MonMap* mmap)
{
  unsigned long long tim = GetTime();
  unsigned long long delta;
  if (m_prevupdate == 0)
  {
    m_prevupdate = tim;
    m_DetMap.Zero();
    for(size_t i=0; i<s_counterTypes.size();i++)
    {
      filldata(s_counterTypes[i],mmap);
    }
    fillsums();
  //  m_DetMap.dump();
  //  m_DetMap_old.dump();
    m_DetMap_old = m_DetMap;
    return;
  }
  else
  {
    delta = tim - m_prevupdate;

  }
  m_prevupdate = tim;
  m_DetMap.Zero();
  for(size_t i=0; i<s_counterTypes.size();i++)
  {
    filldata(s_counterTypes[i],mmap);
  }
//  m_DetMap.dump();
//  m_DetMap_old.dump();
  fillsums();
  m_DetMap_diff = m_DetMap - m_DetMap_old;
  m_DetMap_old = m_DetMap;
  m_DetMap_diff.divide(m_DetMap_rate,delta);
//  dump();
//  m_DetMap.dump();
//  printf("Rate Dump (delta-T = %f seconds)\n",(double)delta/1.0e9);
//  m_DetMap_rate.dump();
  m_DetMap.Update();
  m_DetMap_rate.Update();
}

void PubSvc::dump()
{
  printf("Dump of cumulated values\n");
  m_DetMap.dump();
  printf("Dump of differential values\n");
  m_DetMap_diff.dump();
}
PubSvc::PubSvc(const std::string& name, ISvcLocator* sl) : Service(name,sl),m_incidentSvc(0)
{
  declareProperty("MyName",          m_MyName       = "");
  declareProperty("InDNS",           m_InputDNS     = "");
  declareProperty("PartitionName",   m_PartitionName= "LHCb");
  declareProperty("TaskPattern",     m_TaskPattern);
  declareProperty("ServicePattern",  m_ServicePattern);
  declareProperty("Tell1List",       m_tell1List);

  m_started     = false;
  m_SaveTimer   = 0;
  m_errh        = 0;
  m_pMonitorSvc = 0;
  m_phistsvc    = 0;
  m_arrhist     = 0;
  if (s_nammap.empty())
  {
    s_nammap["ectell"]  = "ECAL";
    s_nammap["hctell"]  = "HCAL";
    s_nammap["tttell"]  = "TT";
    s_nammap["ittell"]  = "IT";
    s_nammap["mutella"] = "MUONA";
    s_nammap["mutellc"] = "MUONC";
    s_nammap["ottella"] = "OTA";
    s_nammap["ottellc"] = "OTC";
    s_nammap["pstell"]  = "PRS";
    s_nammap["tcatell"] = "TCALO";
    s_nammap["trgtell1"] = "L0DU";
    s_nammap["tputell"] = "TPU";
    s_nammap["r1ukl1"]    ="RICH1";
    s_nammap["r2ukl1"]    = "RICH2";
    s_nammap["vetella"] = "VELOA";
    s_nammap["vetellc"] = "VELOC";
    s_nammap["dumtell"] = "TDET";
    s_nammap["tmutellq01"] = "TMUA";
    s_nammap["tmutellq02"] = "TMUA";
    s_nammap["tmutellq12"] = "TMUA";
    s_nammap["tmutellq03"] = "TMUC";
    s_nammap["tmutellq04"] = "TMUC";
    s_nammap["tmutellq34"] = "TMUC";
    s_nammap["tfcodin"] = "TFC";
  }
  if (s_counterTypes.empty())
  {
    s_counterTypes.push_back("badLenPkt");
    s_counterTypes.push_back("misPkt");
    s_counterTypes.push_back("badPckFktPkt");
    s_counterTypes.push_back("truncPkt");
    s_counterTypes.push_back("multipleEvt");
    s_counterTypes.push_back("rxOct");
    s_counterTypes.push_back("rxPkt");
    s_counterTypes.push_back("rxEvt");
    s_counterTypes.push_back("rxMEP");
    s_counterTypes.push_back("TLostMEP");
  }

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
  // Book arrival time histograms
  return StatusCode::SUCCESS;
}
void PubSvc::fillTellMap()
{
  typedef std::map<std::string,std::string> StringMap;
  unsigned int i;
  for (i = 0;i<m_tell1List.size();i+=3)
  {
    std::string nam = m_tell1List[i+1];
    std::string snam = nam.substr(0,nam.length()-2);
    if (snam == "tmutellq")
    {
      snam = nam;
    }
    if (nam.substr(0,7) == "tfcodin") snam = "tfcodin";
    StringMap::const_iterator j = s_nammap.find(snam);
    m_TellMap.insert(std::make_pair(i/3,TellDesc(nam,j==s_nammap.end() ? "UNKNOWN" : (*j).second)));
  }

  DetData_T<long long> a;
  for(size_t i=0; i<s_counterTypes.size();i++)
  {
    a[s_counterTypes[i]] = 0;
  }
  for(StringMap::const_iterator i=s_nammap.begin(); i!=s_nammap.end();++i)
  {
    m_DetMap.insert(std::make_pair((*i).second,a));
  }
  m_DetMap.insert(std::make_pair("LHCb",a));
}

StatusCode PubSvc::start()
{
  std::string servicename;
  std::string myservicename;
  std::string nodename = RTL::nodeNameShort();
  m_prevupdate = 0;
  Service::start();
  fillTellMap();
  m_DetMap_old = m_DetMap;
  if (m_errh == 0) m_errh = new MyErrh();
  toLowerCase(m_TaskPattern);
  toLowerCase(m_ServicePattern);
  toLowerCase(nodename);
  m_AdderClass = "counter";
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
  myservicename = m_MyName;
  StringReplace(myservicename, "<part>", m_PartitionName);
  StringReplace(myservicename, "<node>", nodename);
  const char *dnsnode = getenv("DIM_DNS_NODE");
  std::string ddns(dnsnode ? dnsnode : "");
  StringReplace(m_InputDNS,"<node>",nodename);
  StringReplace(m_InputDNS,"<dns>",ddns);
  this->m_DetMap_rate.setServiceName("Stat/"+myservicename+"/Rate");
  this->m_DetMap.setServiceName("Stat/"+myservicename+"/Count");
  m_errh->start();
  if (m_started) return StatusCode::SUCCESS;
  if (m_errh != 0) DimClient::addErrorHandler(m_errh);
  ::lib_rtl_output(LIB_RTL_DEBUG,"=======>PubSvc Option Summary:\tTask Pattern %s\n",m_TaskPattern.c_str());
  ::lib_rtl_output(LIB_RTL_DEBUG,"        Service Pattern %s+Data or EOR\n",m_ServicePattern.c_str());
  DimServer::autoStartOn();
  if (!m_InputDNS.empty()) DimClient::setDnsNode(m_InputDNS.c_str());
  m_adder = new CounterAdder((char*)myservicename.c_str(), (char*)"Data");
  m_adder->m_IsEOR = false;
  m_adder->m_expandRate = m_expandRate;
  m_adder->m_taskPattern = m_TaskPattern;
  m_adder->m_servicePattern = m_ServicePattern+std::string("Data");
  m_adder->setIsSaver(m_isSaver);
  m_adder->m_dohisto = m_dohisto;
  m_adder->m_histo = m_arrhist;
  m_adder->m_rectmo = m_recvtmo;
  m_adder->m_disableOutput = true;
  m_adder->m_noRPC = true;
  m_adder->m_monsvc = dynamic_cast<IGauchoMonitorSvc*>(m_pMonitorSvc);
  m_adder->Configure();
  m_AdderSys->Add(m_adder);
  m_adder->SetCycleFn(Analyze,this);
//  m_EoRadder = new CounterAdder((char*)myservicename.c_str(), (char*)"EOR");
//  m_EoRadder->m_IsEOR = true;
//  m_EoRadder->m_expandRate = false;
//  m_EoRadder->m_taskPattern = m_TaskPattern;
//  m_EoRadder->m_servicePattern = m_ServicePattern+std::string("EOR");
//  m_EoRadder->m_noRPC = true;
//  m_EoRadder->setIsSaver(m_isSaver);
//  m_adder->m_rectmo = 5*m_recvtmo;
//  m_EoRadder->Configure();
//  m_AdderSys->Add(m_EoRadder);
  m_started = true;
  return StatusCode::SUCCESS;
}

StatusCode PubSvc::stop()
{
  IGauchoMonitorSvc* psvc = dynamic_cast<IGauchoMonitorSvc*>(m_pMonitorSvc);
  if (psvc) psvc->resetHistos(this);

  if (m_isSaver)
  {
//    m_SaveTimer->Stop();
  }
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
  releasePtr(m_phistsvc);
  releasePtr(m_pMonitorSvc);
  //printf("PubSvc: Locking DIM\n");
  DimLock l;
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
