#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IHistogramSvc.h"

// workaround for https://its.cern.ch/jira/browse/GAUDI-1168
#include "GaudiKernel/Algorithm.h"

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

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IToolSvc.h"
#include "GauchoAppl/MonAdder.h"
#include "PubSvc.h"

// Forward declarations
class DimService;
//class ISimpleTrendWriter;

class MoorePub : public PubSvc
{
public:
  MoorePub(const std::string& name, ISvcLocator* sl);
  virtual ~MoorePub();
  //IInterface pure virtual member functions
  StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);
  StatusCode start();
  StatusCode initialize();
  StatusCode finalize();
  void analyze(void *buff, int siz,MonMap* mmap);
//  unsigned long long m_prevupdate;
  bool m_enableTrending;
  DimService *m_FarmLoad;
  float m_fLoad;
  std::string m_TrendName;
//  ISimpleTrendWriter* m_trender;
};

DECLARE_SERVICE_FACTORY(MoorePub)
namespace MOOREPUB
{
  void Analyze(void *arg, void* buff ,int siz, MonMap *mmap, MonAdder *)
  {
    MoorePub *tis = (MoorePub*)arg;
    tis->analyze(buff,siz, mmap);
  }
}

StatusCode MoorePub::start()
{
  PubSvc::start();
  m_adder->SetCycleFn(MOOREPUB::Analyze,this);
  return StatusCode::SUCCESS;
}

StatusCode MoorePub::initialize()
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
    sc = tools->retrieveTool("SimpleTrendWriter","MoorePubWriter",m_trender,this);
    if (sc.isSuccess() && m_trender != 0)
    {
      std::string nnam("Moores");
      m_trender->setPartitionAndName(this->m_PartitionName,m_TrendName);
//      nnam = "CoresRunning";
//      m_trender->setPartitionAndName(this->m_PartitionName,nnam);
      m_trender->setMaxTimeNoWrite(600);
      if (m_enableTrending)
      {
        m_trender->startEvent();
        double ftasks = 0.0;
        m_trender->addEntry(m_TrendName, ftasks);
        m_trender->saveEvent();
      }
    }
  }
  return sc;
}

StatusCode MoorePub::finalize()
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

void MoorePub::analyze(void *, int ,MonMap* mmap)
{
  MonMap::iterator i;
  i = mmap->find("UpdateAndReset/Tasks");
  int tasks;
  CntrDescr* tsk_d;

  tsk_d = (CntrDescr*)MonCounter::de_serialize((*i).second);
  tasks = tsk_d->i_data;
//  if (m_FarmLoad == 0)
//  {
//    m_FarmLoad = new DimService(m_adder->m_ServiceDns,(char*)(m_PartitionName+"/FarmCPULoad").c_str(),m_fLoad);
//  }
//  m_FarmLoad->updateService(m_fLoad);
  if (m_enableTrending)
  {
    m_trender->startEvent();
    double ftasks = tasks;
    m_trender->addEntry(m_TrendName, ftasks);
    m_trender->saveEvent();
  }
}

MoorePub::MoorePub(const std::string& name, ISvcLocator* sl)
 : PubSvc(name,sl)
{
  declareProperty("TrendingOn",  m_enableTrending);
  declareProperty("TrendName",m_TrendName="");
  m_FarmLoad = 0;
  m_trender = 0;
  m_fLoad = 0.0;
}

MoorePub::~MoorePub()
{
}

StatusCode MoorePub::queryInterface(const InterfaceID& riid, void** ppvIF)
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

