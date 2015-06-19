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
  m_trender = 0;
  if (m_enableTrending)
  {
    SmartIF<IToolSvc> tools;
    sc = service("ToolSvc", tools.pRef());
    sc = tools->retrieveTool("SimpleTrendWriter",this->name(),m_trender,this);
    if (sc.isSuccess() && m_trender != 0)
    {
      std::string nnam(this->name());
      m_trender->setPartitionAndName(this->m_PartitionName,nnam);
      m_trender->setMaxTimeNoWrite(600);
    }
  }
  m_counterPatternRegex = boost::regex(m_counterPattern.c_str(),
      boost::regex_constants::icase);
  return sc;
}

StatusCode CntrPub::finalize()
{
  for (auto mit = m_cntrSvcMap.begin();mit!=m_cntrSvcMap.end();mit++)
  {
    delete (*mit).second;
  }
  m_cntrSvcMap.clear();
  StatusCode sc;
  if (m_enableTrending)
  {
    SmartIF<IToolSvc> tools;
    sc = serviceLocator()->service("ToolSvc", tools.pRef());
    if ( !sc.isSuccess() ) {
      ::lib_rtl_output(LIB_RTL_FATAL,"DIM(RateSvc): Failed to access ToolsSvc.\n");
      return sc;
    }
    sc = tools->releaseTool(m_trender);
    m_trender = 0;
  }
  PubSvc::finalize();
  return StatusCode::SUCCESS;
}

void CntrPub::analyze(void *, int, MonMap* mmap)
{
  if (m_enableTrending)
  {
    m_trender->startEvent();
  }

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
      CntrDescr *d = (CntrDescr*) MonCounter::de_serialize((*it).second);
      if (sdes->type == 1)
      {
        sdes->rdata = d->d_data;
      }
      else
      {
        sdes->idata = d->l_data;
      }
      if (m_enableTrending)
      {
        double fdata = sdes->rdata;
        m_trender->addEntry(sdes->name, fdata);
      }
      sdes->svc->updateService();
    }
  }
  if (m_enableTrending)
  {
    m_trender->saveEvent();
  }
}

CntrPub::CntrPub(const std::string& name, ISvcLocator* sl) :
    PubSvc(name, sl), m_noEvtsSvc(0), m_EvtRateSvc(0)
{
  m_NoEvts = 0.0;
  m_EvtRate = 0.0;
  declareProperty("ServiceInfix", m_SvcInfix);
  declareProperty("CounterPattern", m_counterPattern);
  declareProperty("TrendingOn",  m_enableTrending=false);
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

