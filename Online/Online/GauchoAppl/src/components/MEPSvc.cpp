#include "MEPSvc.h"
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
#include "Trending/ITrendingTool.h"
#include "Trending/ISimpleTrendWriter.h"

DECLARE_SERVICE_FACTORY(MEPSvc)
static std::string DetNames[] = {"TDet","VeloA","VeloC","TT","IT","OTA","OTC","RICH1","RICH2","ECal","HCal",
    "MuonA","MuonC","L0DU","TMuA","TMuC","TPU","PRS","TCalo","LHCb","HRC"};
static std::vector<std::string> s_counterTypes;

static std::map<std::string,std::string> s_nammap;



Tell1Stats::Tell1Stats(std::vector<std::string> &TellList): m_CSvc(0),m_RSvc(0)
{
  m_nents = TellList.size();
  m_LossRate = (float*)::malloc(m_nents*sizeof(float));
  m_LossCount = (unsigned int*)::malloc(m_nents*sizeof(int));
  for (int i=0;i<5;i++)
  {
    compList.push_back(s_counterTypes[i]);
  }
}
void Tell1Stats::fillBuffers(MonMap *mmap)
{
  ::memset(m_LossRate,0,m_nents*sizeof(float));
  ::memset(m_LossCount,0,m_nents*sizeof(unsigned int));
  {
    for (size_t j=0;j<compList.size();j++)
    {
      auto i=mmap->find(std::string("Runable/")+compList[j]);
      if (i!=mmap->end())
      {
        CntrDescr *h = (CntrDescr*)MonCounter::de_serialize(i->second);
        long *idat = (long*)h->ptr;
        for (int k=0;k<h->nel;k++)
        {
          m_LossCount[k]+= (unsigned int)(idat[k]);
        }
      }
      i=mmap->find(std::string("R_Runable/")+compList[j]);
      if (i!=mmap->end())
      {
        CntrDescr *h = (CntrDescr*)MonCounter::de_serialize(i->second);
        double *ddat = (double*)h->ptr;
        for (int k=0;k<h->nel;k++)
        {
          m_LossRate[k]+= float(ddat[k]);
        }
      }
    }
  }
}
void Tell1Stats::makeServices(std::string prefix,DimServerDns *dns)
{
  m_CName = prefix+"Tell1/LossCount";
  m_RName = prefix+"Tell1/LossRate";
  if (dns == 0)
  {
    DimServer::autoStartOn();
    if (m_CSvc == 0) m_CSvc = new DimService(m_CName.c_str(),"I",m_LossCount,m_nents*sizeof(int));
    if (m_RSvc ==0) m_RSvc = new DimService(m_RName.c_str(),"F",m_LossRate,m_nents*sizeof(double));
  }
  else
  {
    dns->autoStartOn();
    if (m_CSvc == 0) m_CSvc = new DimService(dns,m_CName.c_str(),"I",m_LossCount,m_nents*sizeof(int));
    if (m_RSvc ==0) m_RSvc = new DimService(dns,m_RName.c_str(),"F",m_LossRate,m_nents*sizeof(double));
  }
}
void Tell1Stats::Update()
{
  m_CSvc->updateService(m_LossCount,m_nents*sizeof(int));
  m_RSvc->updateService(m_LossRate,m_nents*sizeof(float));
}







namespace MEPSVC
{
  void Analyze(void *arg, void* buff ,int siz, MonMap *mmap, MonAdder *)
  {
    MEPSvc *tis = (MEPSvc*)arg;
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
        ::lib_rtl_output(LIB_RTL_INFO,"DIM(MEPSvc): Code %d.%x %s\n",severity,code,msg);
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

void MEPSvc::filldata(const std::string &cnam,MonMap *mmap)
{
  filldata("Runable/",cnam,mmap);
}
void MEPSvc::filldata(const std::string &prefix,const std::string &cnam,MonMap *mmap)
{
  MonMap::iterator it;
  DetData_T<long long>& lb = m_DetMap.find("LHCb")->second;
  it = mmap->find(prefix+cnam);
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
void MEPSvc::fillrate(const std::string &prefix,const std::string &cnam,MonMap *mmap)
{
  MonMap::iterator it;
  DetData_T<double>& lb = m_DetMap_rate.find("LHCb")->second;
  it = mmap->find(prefix+cnam);
  if ( it != mmap->end() )
  {
    CntrDescr *h = (CntrDescr*)MonCounter::de_serialize(it->second);
    double *p = (double*)h->ptr;
    for (int i=0;i<h->nel;i++)
    {
      TellMap_T::iterator t=m_TellMap.find(i);
      if (t != m_TellMap.end())
      {
//        printf("%d %s %s\n", i, t->second.det.c_str(),t->second.name.c_str());
        DetData_T<double>& d= m_DetMap_rate.find(t->second.det)->second;
        DetData_T<double>::iterator k = d.find(cnam);

        if (k != d.end())
        {
          double pp;
          pp = p[i];
          k->second += pp;
          lb[cnam] += pp;
        }
        else
        {
//          printf ("Cannot find %s in Detector Data Map\n",cnam.c_str());
        }
      }
    }
  }
}
void MEPSvc::fillsums()//const std::string &cnam,MonMap *mmap)
{
  DetMap_T<long long>::iterator i;
  DetMap_T<double>::iterator k;
//  DetData_T<long long>& lb = m_DetMap.find("LHCb")->second;
  for (i=m_DetMap.begin();i!=m_DetMap.end();i++)
  {
    for (int j=0;j<5;j++)
    {
      i->second["TLostMEP"] += i->second[s_counterTypes[j]];
    }
  }
  for (k=m_DetMap_rate.begin();k!=m_DetMap_rate.end();k++)
  {
    for (int j=0;j<5;j++)
    {
      k->second["TLostMEP"] += k->second[s_counterTypes[j]];
    }
  }
}
StatusCode MEPSvc::initialize()
{
  StatusCode sc = PubSvc::initialize();
  if (sc.isSuccess() && m_enableTrending && m_trender == 0)
  {
    SmartIF<IToolSvc> tools;
    sc = serviceLocator()->service("ToolSvc", tools.pRef());
    if ( !sc.isSuccess() ) {
      ::lib_rtl_output(LIB_RTL_FATAL,"DIM(OvrSvc): Failed to access ToolsSvc.\n");
      return sc;
    }
    sc = tools->retrieveTool("SimpleTrendWriter","MEPSvc",m_trender,this);
    if (sc.isSuccess() && m_trender != 0)
    {
      std::string nnam = m_TrendPrefix+std::string("MEPRx_Trends");
      m_trender->setPartitionAndName(this->m_PartitionName,nnam);
      m_trender->setMaxTimeNoWrite(600);
    }
  }
  return sc;
}

StatusCode MEPSvc::finalize()
{
  StatusCode sc;
  if (m_trender != 0)
  {
//    m_trender->finalize();
//    m_trender->close();
    SmartIF<IToolSvc> tools;
    sc = serviceLocator()->service("ToolSvc", tools.pRef());
    if ( !sc.isSuccess() ) {
      ::lib_rtl_output(LIB_RTL_FATAL,"DIM(RateSvc): Failed to access ToolsSvc.\n");
      return sc;
    }
    sc = tools->releaseTool(m_trender);
    m_trender = 0;
  }
  Service::finalize();
  return StatusCode::SUCCESS;
}

StatusCode MEPSvc::start()
{
  PubSvc::start();
  fillTellMap();
  if (m_Tell1Stats == 0) m_Tell1Stats = new Tell1Stats(m_TellNames);
  if (m_Tell1Service ==0)
  {
    std::string nam = "Stat/"+m_myservicename+"/Tell1/NameList";
    m_Tell1Service = new TellService(nam,m_TellNames,m_adder->m_ServiceDns);
  }
  m_DetMap_old = m_DetMap;
  this->m_DetMap_rate.setServiceName("Stat/"+m_myservicename+"/Rate");
  this->m_DetMap.setServiceName("Stat/"+m_myservicename+"/Count");
  m_adder->SetCycleFn(MEPSVC::Analyze,this);
  m_Tell1Service->Update();
  m_Tell1Stats->makeServices("Stat/"+m_myservicename+"/",m_adder->m_ServiceDns);
  return StatusCode::SUCCESS;
}
void MEPSvc::analyze(void *, int ,MonMap* mmap)
{
  m_DetMap.Zero();
  m_DetMap_rate.Zero();
  MonMap::iterator Meptotit,MepOvrit,MepMooreit;
  Meptotit = mmap->find("R_Runable/totMEPproduced");
  MepOvrit = mmap->find("R_Runable/totMEPproducedOvfl");
  MepMooreit = mmap->find("R_Runable/totMEPproducedLive");
  m_Tell1Stats->fillBuffers(mmap);
  m_Tell1Stats->Update();
  double Mepstot, MepsMoore, Mepsovr;
  Mepstot = 0.0;
  MepsMoore = 0.0;
  Mepsovr = 0.0;
  if (Meptotit != mmap->end())
  {
    CntrDescr *h = (CntrDescr*)MonCounter::de_serialize(Meptotit->second);
    Mepstot = h->d_data;
  }
  if (MepMooreit != mmap->end())
  {
    CntrDescr *h = (CntrDescr*)MonCounter::de_serialize(MepMooreit->second);
    MepsMoore = h->d_data;
  }
  if (MepOvrit != mmap->end())
  {
    CntrDescr *h = (CntrDescr*)MonCounter::de_serialize(MepOvrit->second);
    Mepsovr = h->d_data;
  }
  for(size_t i=0; i<s_counterTypes.size();i++)
  {
    filldata(s_counterTypes[i],mmap);
    fillrate(std::string("R_Runable/"),s_counterTypes[i],mmap);
  }
  this->fillsums();
  m_DetMap.setDNS(m_adder->m_ServiceDns);
  m_DetMap.Update();
//  m_DetMap.dump();
//  m_DetMap_rate.dump();
  m_DetMap_rate.setDNS(m_adder->m_ServiceDns);
  m_DetMap_rate.Update();
  DetMap_T<double>::iterator ii = m_DetMap_rate.find(m_PartitionName/*"LHCb"*/);
  if (ii!=m_DetMap_rate.end())
  {
    DetData_T<double>::iterator jj = ii->second.find("rxOct");
    if (jj!=ii->second.end())
    {
      m_DataRate = (float)jj->second;
      if (m_LHCbDataRate == 0)
      {
        m_LHCbDataRate = new DimService(m_adder->m_ServiceDns,(char*)(m_PartitionName+"_DataRate").c_str(),m_DataRate);
      }
      m_LHCbDataRate->updateService(m_DataRate);
//      printf("%s Total Data Rate %f\n",m_PartitionName.c_str(),m_DataRate);
    }
  }
  if (m_enableTrending)
  {
    m_trender->startEvent();
    m_trender->addEntry("TotalDataRate", (double)m_DataRate/1.0e09);
    DetMap_T<double>::iterator ii;// = m_DetMap_rate.find(m_PartitionName/*"LHCb"*/);
    for (ii=m_DetMap_rate.begin();ii!=m_DetMap_rate.end();ii++)
    {
      DetData_T<double>::iterator jj = ii->second.find("TLostMEP");
      if (jj!=ii->second.end())
      {
        double d = (double)jj->second;
        m_trender->addEntry(ii->first, d);
      }
    }
    m_trender->addEntry("MEPrx Output Rate",Mepstot);
    m_trender->addEntry("MEPrx Rate to Moore",MepsMoore);
    m_trender->addEntry("MEPrx Overflow Rate",Mepsovr);
    if (Mepstot >0.0)
    {
      double deffrac = 100.0*Mepsovr/Mepstot;
      m_trender->addEntry("Deferred HLT fraction",deffrac);
    }
    m_trender->saveEvent();
  }
  m_MEPRate = Mepstot;
  m_MEPDeferred = Mepsovr;
  if (m_MEPTotRateSvc == 0)
  {
    m_MEPTotRateSvc = new DimService(m_adder->m_ServiceDns,(char*)(m_PartitionName+"_InputMEPRate").c_str(),m_MEPRate);
  }
  m_MEPTotRateSvc->updateService(m_MEPRate);
  if (m_MEPDefRateSvc == 0)
  {
    m_MEPDefRateSvc = new DimService(m_adder->m_ServiceDns,(char*)(m_PartitionName+"_DeferMEPRate").c_str(),m_MEPDeferred);
  }
  m_MEPDefRateSvc->updateService(m_MEPDeferred);
}

void MEPSvc::dump()
{
  printf("Dump of cumulated values\n");
  m_DetMap.dump();
  printf("Dump of differential values\n");
  m_DetMap_diff.dump();
}
MEPSvc::MEPSvc(const std::string& name, ISvcLocator* sl) : PubSvc(name,sl)
{
  declareProperty("Tell1List",       m_tell1List);
  declareProperty("TrendingOn",  m_enableTrending = true);
  declareProperty("TrendPrefix",m_TrendPrefix="");
  m_trender = 0;

//  m_started     = false;
//  m_SaveTimer   = 0;
//  m_errh        = 0;
//  m_pMonitorSvc = 0;
//  m_phistsvc    = 0;
//  m_arrhist     = 0;
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
    s_nammap["hrctell"] = "HRC";
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
  m_LHCbDataRate = 0;
  m_MEPTotRateSvc = 0;
  m_MEPDefRateSvc =0;

}

MEPSvc::~MEPSvc()
{
  deletePtr(m_LHCbDataRate);
  deletePtr(m_MEPTotRateSvc);
  deletePtr(m_MEPDefRateSvc);
}

StatusCode MEPSvc::queryInterface(const InterfaceID& riid, void** ppvIF)
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

void MEPSvc::fillTellMap()
{
  typedef std::map<std::string,std::string> StringMap;
  unsigned int i;
  for (i = 0;i<m_tell1List.size();i+=3)
  {
    std::string nam = m_tell1List[i+1];
    m_TellNames.push_back(nam);
    m_TellStrings += nam;
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
  DetData_T<double> f;
  for(size_t i=0; i<s_counterTypes.size();i++)
  {
    a[s_counterTypes[i]] = 0;
  }
  for(size_t i=0; i<s_counterTypes.size();i++)
  {
    f[s_counterTypes[i]] = 0.0;
  }
  for(StringMap::const_iterator i=s_nammap.begin(); i!=s_nammap.end();++i)
  {
    m_DetMap.insert(std::make_pair((*i).second,a));
    m_DetMap_rate.insert(std::make_pair((*i).second,f));
  }
  m_DetMap.insert(std::make_pair("LHCb",a));
  m_DetMap_rate.insert(std::make_pair("LHCb",f));
}

