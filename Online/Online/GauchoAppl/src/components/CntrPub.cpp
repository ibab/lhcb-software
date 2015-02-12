#include "CntrPub.h"
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
//#include "Trending/ITrendingTool.h"
//#include "Trending/ISimpleTrendWriter.h"
//#include <string>
DECLARE_SERVICE_FACTORY (CntrPub)

namespace CNTRPUB
{
  void Analyze(void *arg, void* buff, int siz, MonMap *mmap, MonAdder *)
  {
    CntrPub *tis = (CntrPub*) arg;
    tis->analyze(buff, siz, mmap);
  }
}

StatusCode CntrPub::start()
{
  PubSvc::start();
  m_adder->SetCycleFn(CNTRPUB::Analyze, this);
  return StatusCode::SUCCESS;
}

StatusCode CntrPub::initialize()
{
  StatusCode sc = PubSvc::initialize();
  m_counterPatternRegex = boost::regex(m_counterPattern.c_str(),
      boost::regex_constants::icase);
  return sc;
}

StatusCode CntrPub::finalize()
{
  StatusCode sc;
  Service::finalize();
  return StatusCode::SUCCESS;
}

void CntrPub::analyze(void *, int, MonMap* mmap)
{
  for (auto it = mmap->begin(); it != mmap->end(); it++)
  {
    bool status = boost::regex_search((*it).first, m_counterPatternRegex);
    if (status)
    {
      SVCDescr *sdes;
      auto mit = m_cntrSvcMap.find((*it).first);
      if (mit == m_cntrSvcMap.end())
      {
        std::string svcnam = (*it).first;
        sdes = new SVCDescr;

        if (svcnam[0] == 'R')
        {
          sdes->name = "stat/"+m_PartitionName+"_x_"+svcnam.substr(2,std::string::npos)+"/Rate";
          sdes->svc = new DimService(m_adder->m_ServiceDns, (char*) sdes->name.c_str(),sdes->rdata);
          sdes->type = 1;
        }
        else
        {
          sdes->name = "stat/"+m_PartitionName+"_x_"+svcnam+"/Count";
          sdes->svc = new DimService(m_adder->m_ServiceDns, (char*) sdes->name.c_str(),sdes->idata);
          sdes->type = 0;
        }
        m_cntrSvcMap.insert(std::make_pair(svcnam,sdes));
      }
      else
      {
        sdes = (*mit).second;
      }
      d = (CntrDescr*) MonCounter::de_serialize((*mit).second);
      if (sdes->type == 1)
      {
        sdes->rdata = d->d_data;
      }
      else
      {
        sdes->idata = d->i_data;
      }
      sdes->svc->updateService();
    }
  }
}
//  printf("\n");
//  INServiceMap::iterator it;
//  printf("dumping Input Service Map\n");
//  for (it = m_adder->m_inputServicemap.begin();
//      it != m_adder->m_inputServicemap.end(); it++)
//  {
//    printf("%s ", (*it).first.c_str());
//  }
//  printf("\n");
//  CntrDescr *d;
//  evtCnt = mmap->find("Runable/EvtCount");
//  if (evtCnt != mmap->end())
//  {
//    long long Evts;
//    d = (CntrDescr*) MonCounter::de_serialize((*evtCnt).second);
//    Evts = d->i_data;
//    m_NoEvts = Evts;
//    if (m_noEvtsSvc == 0)
//    {
//      std::string nam;
//      std::string svc;
//      svc = m_adder->m_inputServicemap.begin()->first;
//      dyn_string &ds = *Strsplit(svc.c_str(), "/");
//      svc = ds[0];
//      delete &ds;
//      ds = *Strsplit(svc.c_str(), "_");
//      svc = ds[ds.size() - 2] + "_" + ds[ds.size() - 1];
//      delete &ds;
//      nam = m_PartitionName + "_X_" + svc + "/Runable/EvtCount";
//      m_noEvtsSvc = new DimService(m_adder->m_ServiceDns, (char*) nam.c_str(),
//          m_NoEvts);
//    }
//    m_noEvtsSvc->updateService(m_NoEvts);
//  }
//  evtRate = mmap->find("R_Runable/EvtCount");
//  if (evtRate != mmap->end())
//  {
//    double REvts;
//    d = (CntrDescr*) MonCounter::de_serialize((*evtRate).second);
//    REvts = d->d_data;
//    m_EvtRate = REvts;
//    if (m_EvtRateSvc == 0)
//    {
//      std::string svc;
//      svc = m_adder->m_inputServicemap.begin()->first;
//      dyn_string &ds = *Strsplit(svc.c_str(), "/");
//      svc = ds[0];
//      delete &ds;
//      ds = *Strsplit(svc.c_str(), "_");
//      svc = ds[ds.size() - 2] + "_" + ds[ds.size() - 1];
//      delete &ds;
//      std::string nam = m_PartitionName + "_X_" + svc + "/R_Runable/EvtCount";
//      m_EvtRateSvc = new DimService(m_adder->m_ServiceDns, (char*) nam.c_str(),
//          m_EvtRate);
//    }
//    m_EvtRateSvc->updateService(m_EvtRate);
//  }
}

CntrPub::CntrPub(const std::string& name, ISvcLocator* sl) :
    PubSvc(name, sl), m_noEvtsSvc(0), m_EvtRateSvc(0)
{
  m_NoEvts = 0.0;
  m_EvtRate = 0.0;
  declareProperty("ServiceInfix", m_SvcInfix);
  declareProperty("CounterPattern", m_counterPattern);
}

CntrPub::~CntrPub()
{
}

StatusCode CntrPub::queryInterface(const InterfaceID& riid, void** ppvIF)
{
  if (IIncidentListener::interfaceID().versionMatch(riid))
  {
    *ppvIF = (IIncidentListener*) this;
  }
  else
  {
    return Service::queryInterface(riid, ppvIF);
  }
  addRef();
  return StatusCode::SUCCESS;
}

