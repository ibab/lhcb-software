#include "EvtsPub.h"
#include "GaudiKernel/SmartIF.h"
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

DECLARE_SERVICE_FACTORY(EvtsPub)

namespace EVTSPUB
{
  void Analyze(void *arg, void* buff ,int siz, MonMap *mmap, MonAdder *)
  {
    EvtsPub *tis = (EvtsPub*)arg;
    tis->analyze(buff,siz, mmap);
  }
}

StatusCode EvtsPub::start()
{
  PubSvc::start();
  m_adder->SetCycleFn(EVTSPUB::Analyze,this);
  return StatusCode::SUCCESS;
}

StatusCode EvtsPub::initialize()
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
    sc = tools->retrieveTool("SimpleTrendWriter","EvtsPubWriter",m_trender,this);
    if (sc.isSuccess() && m_trender != 0)
    {
      std::string nnam("HLTOverflowWriter");
      m_trender->setPartitionAndName(this->m_PartitionName,nnam);
      m_trender->setMaxTimeNoWrite(600);
    }
  }
  return sc;
}

StatusCode EvtsPub::finalize()
{
  StatusCode sc;
  if (m_trender != 0)
  {
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


void EvtsPub::analyze(void *, int ,MonMap* mmap)
{
  MonMap::iterator evtCnt, evtRate;
//  MonMap::iterator  it;
//  printf("dumping Map\n");
//  for (it=mmap->begin();it!=mmap->end();it++)
//  {d
//    printf("%s ",(*it).first.c_str());
//  }
//  printf("\n");
//  INServiceMap::iterator it;
//  printf("dumping Input Service Map\n");
//  for (it=m_adder->m_inputServicemap.begin();it!=m_adder->m_inputServicemap.end();it++)
//  {
//    printf("%s ",(*it).first.c_str());
//  }
//  printf("\n");
  CntrDescr *d;
  evtCnt = mmap->find("Runable/EvtCount");
  if (evtCnt != mmap->end())
  {
    long long Evts;
    d = (CntrDescr*)MonCounter::de_serialize((*evtCnt).second);
    Evts = d->i_data;
    m_NoEvts  = Evts;
    if (m_noEvtsSvc == 0)
    {
      std::string nam;
      std::string svc;
      svc = m_adder->m_inputServicemap.begin()->first;
      dyn_string &ds=*Strsplit(svc.c_str(),"/");
      svc = ds[0];
      delete &ds;
      dyn_string &ds1 = *Strsplit(svc.c_str(),"_");
      svc = ds1[ds1.size()-2]+"_"+ds1[ds1.size()-1];
      delete &ds1;
      nam = m_PartitionName+"_X_"+svc+"/Runable/EvtCount";
      m_noEvtsSvc = new DimService(m_adder->m_ServiceDns,(char*)nam.c_str(),m_NoEvts);
    }
    m_noEvtsSvc->updateService(m_NoEvts);
  }
  evtRate = mmap->find("R_Runable/EvtCount");
  if (evtRate != mmap->end())
  {
    double REvts;
    d = (CntrDescr*)MonCounter::de_serialize((*evtRate).second);
    REvts = d->d_data;
    m_EvtRate = REvts;
    if (m_EvtRateSvc == 0)
    {
      std::string svc;
      svc = m_adder->m_inputServicemap.begin()->first;
      dyn_string &ds=*Strsplit(svc.c_str(),"/");
      svc = ds[0];
      delete &ds;
      dyn_string &ds1 = *Strsplit(svc.c_str(),"_");
      svc = ds1[ds1.size()-2]+"_"+ds1[ds1.size()-1];
      delete &ds1;
      std::string nam = m_PartitionName+"_X_"+svc+"/R_Runable/EvtCount";
      m_EvtRateSvc = new DimService(m_adder->m_ServiceDns,(char*)nam.c_str(),m_EvtRate);
    }
    m_EvtRateSvc->updateService(m_EvtRate);
  }
//  if (m_enableTrending)
//  {
//    m_trender->startEvent();
//    m_trender->addEntry("BytesWrittenToOverflow", double(m_NoBytes));
//    m_trender->addEntry("NumberOfFilesinOverflow", double(NFiles));
//    m_trender->addEntry("NumberOfMEPsinOverflow", double(NMEPs));
//    m_trender->addEntry("OverFlowMEPRate",RMeps);
//    m_trender->saveEvent();
//  }
}

EvtsPub::EvtsPub(const std::string& name, ISvcLocator* sl)
 : PubSvc(name,sl), m_noEvtsSvc(0), m_EvtRateSvc(0)
{
  declareProperty("TrendingOn",  m_enableTrending=false);
  m_NoEvts = 0.0;
  m_EvtRate = 0.0;
  declareProperty("ServiceInfix",m_SvcInfix);
}

EvtsPub::~EvtsPub()
{
}

StatusCode EvtsPub::queryInterface(const InterfaceID& riid, void** ppvIF)
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

