#include "MemTrnd.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/Memory.h"
#include "Trending/ITrendingTool.h"
#include "Trending/ISimpleTrendWriter.h"

DECLARE_SERVICE_FACTORY(MemTrnd)

StatusCode MemTrnd::initialize()
{
  StatusCode sc;
  Service::initialize();
  std::string syst;
  if (m_trender == 0)
  {
    SmartIF<IToolSvc> tools;
    sc = serviceLocator()->service("ToolSvc", tools.pRef());
    if ( !sc.isSuccess() )
    {
      ::lib_rtl_output(LIB_RTL_FATAL,"DIM(OvrSvc): Failed to access ToolsSvc.\n");
      return sc;
    }
    sc = tools->retrieveTool("SimpleTrendWriter","MEPSvc",m_trender,this);
    if ( !sc.isSuccess() )
    {
      ::lib_rtl_output(LIB_RTL_FATAL,"MemTrnder: Failed to retrieve SimpleTrendWriter tool\n");
      return sc;
    }
  }
  syst = RTL::nodeNameShort()+"_Adder";
  if (m_trender != 0)
  {
    m_trender->initialize();
//    m_trender->setAverageTime(20);
    m_trender->setPartitionAndName(m_PName,syst);
    m_trender->setMaxTimeNoWrite(600);
  }
  m_timer = new TrndTimer(m_trender,this,m_interv);
  m_timer->Start();
  return StatusCode::SUCCESS;
}

StatusCode MemTrnd::start()
{
  Service::start();
  return StatusCode::SUCCESS;
}

StatusCode MemTrnd::stop()
{
  Service::stop();
  return StatusCode::SUCCESS;
}
StatusCode MemTrnd::finalize()
{
  m_timer->Stop();
  std::string t1 = "VirtMem";
  std::string t2 = "ResidentSize";
  m_trender->addEntry(t1,double(0.0));
  m_trender->addEntry(t2,double(0.0));
  m_trender->saveEvent();
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

MemTrnd::MemTrnd(const std::string& name, ISvcLocator* sl) : Service(name,sl)
{
  declareProperty("TrendInterval",m_interv=60);
  declareProperty("PartitionName",m_PName);
  sl->getService( "ToolSvc" , m_isvc ) ;//  printf("%x\n",isvc);
}

MemTrnd::~MemTrnd()
{
}

StatusCode MemTrnd::queryInterface(const InterfaceID& riid, void** ppvIF)
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

TrndTimer::TrndTimer(ISimpleTrendWriter *trender,MemTrnd *mtrend,int period) : GenTimer(0,period*1000)
{
  m_trender = trender;
  m_MemTrnd = mtrend;
  m_lock.m_name = "TrendLock";
}
void TrndTimer::timerHandler()
{
  long rss;
  rss = System::mappedMemory(System::kByte,System::Memory,lib_rtl_pid());
  long vm = System::virtualMemory(System::kByte,System::Memory,lib_rtl_pid());
  std::string t1 = "VirtMem";
  std::string t2 = "ResidentSize";
  m_lock.lockMutex();
  m_trender->addEntry(t1,double(vm));
  m_trender->addEntry(t2,double(rss));
  m_trender->saveEvent();
  m_lock.unlockMutex();
}
void TrndTimer::Stop()
{
  m_lock.lockMutex();
  GenTimer::Stop();
  m_lock.unlockMutex();
}
