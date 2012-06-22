#include "StorageSvc.h"
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

DECLARE_SERVICE_FACTORY(StorageSvc)

namespace STORAGESVC
{
  void Analyze(void *arg, void* buff, int siz, MonMap *mmap, MonAdder *)
  {
    StorageSvc *tis = (StorageSvc*) arg;
    tis->analyze(buff, siz, mmap);
  }
}

StatusCode StorageSvc::start()
{
  PubSvc::start();
  m_adder->SetCycleFn(STORAGESVC::Analyze, this);
  return StatusCode::SUCCESS;
}

StatusCode StorageSvc::initialize()
{
  StatusCode sc = PubSvc::initialize();
  if (sc.isSuccess() && m_enableTrending && m_trender == 0)
  {
    SmartIF<IToolSvc> tools;
    sc = serviceLocator()->service("ToolSvc", tools.pRef());
    if (!sc.isSuccess())
    {
      ::lib_rtl_output(LIB_RTL_FATAL, "DIM(OvrSvc): Failed to access ToolsSvc.\n");
      return sc;
    }
    sc = tools->retrieveTool("SimpleTrendWriter", "DiskWrPubWriter", m_trender,
        this);
    if (sc.isSuccess() && m_trender != 0)
    {
      std::string nnam("Storage");
      m_trender->setPartitionAndName(this->m_PartitionName, this->m_MyName);
      m_trender->setMaxTimeNoWrite(600);
    }
  }
  return sc;
}

StatusCode StorageSvc::finalize()
{
  StatusCode sc;
  if (m_trender != 0)
  {
    //    m_trender->close();
    SmartIF<IToolSvc> tools;
    sc = serviceLocator()->service("ToolSvc", tools.pRef());
    if (!sc.isSuccess())
    {
      ::lib_rtl_output(LIB_RTL_FATAL,
          "DIM(RateSvc): Failed to access ToolsSvc.\n");
      return sc;
    }
    sc = tools->releaseTool(m_trender);
    m_trender = 0;
  }
  Service::finalize();
  return StatusCode::SUCCESS;
}

void StorageSvc::analyze(void *, int, MonMap* mmap)
{
  MonMap::iterator i, j, k, l;
  j = mmap->find("R_Runable/EvtCount");
  if (j == mmap->end())
    return;
  double evreq, evcount, evout, bout;
  //  double RMeps;
  evcount = ((CntrDescr*) MonCounter::de_serialize((*j).second))->d_data;

  if (m_enableTrending)
  {
    m_trender->startEvent();
    m_trender->addEntry(m_MyName+" Events Processed", evcount);
    m_trender->saveEvent();
  }
}

StorageSvc::StorageSvc(const std::string& name, ISvcLocator* sl) :
    PubSvc(name, sl)
{
  declareProperty("TrendingOn", m_enableTrending);
  m_trender = 0;
}

StorageSvc::~StorageSvc()
{
}

StatusCode StorageSvc::queryInterface(const InterfaceID& riid, void** ppvIF)
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

