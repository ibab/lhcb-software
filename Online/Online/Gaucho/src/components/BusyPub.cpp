#include "BusyPub.h"
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
  PubSvc::initialize();

  if (m_enableTrending)
  {
    if (m_trender == 0)
    {
      const IInterface *a3( m_isvc ) ;
      const std::string & nam( "SimpleTrendWriter" ) ;
      IAlgTool *intf = ROOT::Reflex::PluginService::Create< IAlgTool *>( nam , nam , nam , a3 ) ;
      m_trender = dynamic_cast< ISimpleTrendWriter* >( intf ) ;
    }
    if (m_trender != 0)
    {
      m_trender->initialize();
//      m_trender->setAverageTime(20);
      std::string nnam /*= std::string*/("FarmCPULoad");
      m_trender->setPartitionAndName(this->m_PartitionName,nnam);
      m_trender->setMaxTimeNoWrite(600);
    }
  }
  return StatusCode::SUCCESS;
}

void BusyPub::analyze(void *, int ,MonMap* mmap)
{
  MonMap::iterator i,j;
  i = mmap->find("BusySvc/BusyFraction");
  if (i == mmap->end()) return;
  j = mmap->find("BusySvc/BogoMIPS");
  if (j == mmap->end()) return;
  double bsy,bogo;
  CntrDescr* bsy_d;
  CntrDescr* bogo_d;
  bsy_d = (CntrDescr*)MonCounter::de_serialize((*i).second);
  bogo_d = (CntrDescr*)MonCounter::de_serialize((*j).second);
  bsy = bsy_d->d_data;
  bogo = bogo_d->d_data;
  m_fLoad = bsy/bogo;
//  printf("Farm Load: %f\n",m_fLoad);
  if (m_FarmLoad == 0)
  {
    m_FarmLoad = new DimService((char*)(m_PartitionName+"/FarmCPULoad").c_str(),m_fLoad);
  }
  m_FarmLoad->updateService(m_fLoad);
  if (m_enableTrending)
  {
    m_trender->startEvent();
    double fload = m_fLoad;
    m_trender->addEntry("FarmCPULoad", fload);
    m_trender->saveEvent();
  }
}

BusyPub::BusyPub(const std::string& name, ISvcLocator* sl) : PubSvc(name,sl)
{
  sl->getService( "ToolSvc" , m_isvc ) ;
  declareProperty("TrendingOn",  m_enableTrending);
  m_FarmLoad = 0;
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

