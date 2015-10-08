#include "BusyPub.h"
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

DECLARE_SERVICE_FACTORY(BusyPub)

namespace BUSYPUB
{
  void Analyze(void *arg, void* buff ,int siz, MonMap *mmap, MonAdder *)
  {
    BusyPub *tis = (BusyPub*)arg;
    tis->analyze(buff,siz, mmap);
  }
}

StatusCode BusyPub::start()
{
  PubSvc::start();
  m_adder->SetCycleFn(BUSYPUB::Analyze,this);
  return StatusCode::SUCCESS;
}

StatusCode BusyPub::initialize()
{
  StatusCode sc = PubSvc::initialize();
  if (sc.isSuccess() && m_enableTrending && m_trender == 0)
  {
    SmartIF<IToolSvc> tools;
    sc = serviceLocator()->service("ToolSvc", tools.pRef());
    if ( !sc.isSuccess() ) {
      ::lib_rtl_output(LIB_RTL_FATAL,"DIM(RateSvc): Failed to access ToolsSvc.\n");
      return sc;
    }
    sc = tools->retrieveTool("SimpleTrendWriter","BusyPubWriter",m_trender,this);
    if (sc.isSuccess() && m_trender != 0)
    {
      std::string nnam("FarmCPULoad");
      m_trender->setPartitionAndName(this->m_PartitionName,nnam);
//      nnam = "CoresRunning";
//      m_trender->setPartitionAndName(this->m_PartitionName,nnam);
      m_trender->setMaxTimeNoWrite(600);
    }
  }
  return sc;
}

StatusCode BusyPub::finalize()
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

void BusyPub::analyze(void *, int ,MonMap* mmap)
{
  MonMap::iterator i,j,k;
  i = mmap->find("BusySvc/BusyFraction");
  if (i == mmap->end()) return;
  j = mmap->find("BusySvc/BogoMIPS");
  if (j == mmap->end()) return;
  k = mmap->find("BusySvc/NumCores");
  if (k == mmap->end()) return;
  double bsy,bogo;
  int nCores;
  CntrDescr* bsy_d;
  CntrDescr* bogo_d;
  CntrDescr* Cores_d;

  bsy_d = (CntrDescr*)MonCounter::de_serialize((*i).second);
  bogo_d = (CntrDescr*)MonCounter::de_serialize((*j).second);
  Cores_d = (CntrDescr*)MonCounter::de_serialize((*k).second);
  bsy = bsy_d->d_data;
  bogo = bogo_d->d_data;
  nCores = Cores_d->i_data;
  if (bogo < 0.00001) return;
  m_fLoad = bsy/bogo;
//  printf("Farm Load: %f\n",m_fLoad);
  if (m_FarmLoad == 0)
  {
    m_FarmLoad = new DimService(m_adder->m_ServiceDns,(char*)(m_PartitionName+"/FarmCPULoad").c_str(),m_fLoad);
  }
  m_FarmLoad->updateService(m_fLoad);
  long memtot;
  long memfree;
  long swaptot;
  long swapfree;
  long buffers;
  i = mmap->find("BusySvc/TotMemory");
  memtot = ((CntrDescr*)MonCounter::de_serialize((*i).second))->l_data;
  i = mmap->find("BusySvc/FreeMemory");
  memfree = ((CntrDescr*)MonCounter::de_serialize((*i).second))->l_data;
  i = mmap->find("BusySvc/SwapSpaceTot");
  swaptot = ((CntrDescr*)MonCounter::de_serialize((*i).second))->l_data;
  i = mmap->find("BusySvc/SwapSpaceFree");
  swapfree = ((CntrDescr*)MonCounter::de_serialize((*i).second))->l_data;
  i = mmap->find("BusySvc/MemBuffers");
  buffers = ((CntrDescr*)MonCounter::de_serialize((*i).second))->l_data;
  if (m_enableTrending)
  {
    m_trender->startEvent();
    double fload = m_fLoad;
    double f_Cores = nCores;
    m_trender->addEntry("FarmCPULoad", fload);
    m_trender->addEntry("CoresRunning", f_Cores);
    double d;
    d = double(memfree)/double(memtot);
    m_trender->addEntry("FreeMemoryRatio", d);
    d = double(buffers)/double(memtot);
    m_trender->addEntry("BufferMemoryRatio", d);
    d = double(swaptot-swapfree)/double(memtot);
    m_trender->addEntry("SwapRatio", d);
    d = double(buffers)/double(memtot);
    m_trender->addEntry("BufferRatio", d);
    m_trender->saveEvent();
  }
}

BusyPub::BusyPub(const std::string& name, ISvcLocator* sl)
 : PubSvc(name,sl)
{
  declareProperty("TrendingOn",  m_enableTrending);
  m_FarmLoad = 0;
  m_trender = 0;
  m_fLoad = 0.0;
}

BusyPub::~BusyPub()
{
}

StatusCode BusyPub::queryInterface(const InterfaceID& riid, void** ppvIF)
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

