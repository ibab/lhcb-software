#include "MemTrnd.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/Memory.h"
#include "Trending/ITrendingTool.h"
#include "Trending/ISimpleTrendWriter.h"

DECLARE_SERVICE_FACTORY(MemTrnd)

StatusCode MemTrnd::initialize()
{
  std::string syst;
  if (m_trender == 0)
  {
    const IInterface *a3( m_isvc ) ;
    const std::string & nam( "SimpleTrendWriter" ) ;
    IAlgTool *intf = ROOT::Reflex::PluginService::Create< IAlgTool *>( nam , nam , nam , a3 ) ;
    m_trender = dynamic_cast< ISimpleTrendWriter * >( intf ) ;
  }
  syst = RTL::processName();
  if (m_trender != 0)
  {
    m_trender->initialize();
    m_trender->setAverageTime(20);
    m_trender->setPartitionAndName(this->m_PartName,syst);
    m_trender->setMaxTimeNoWrite(600);
  }
  m_timer = new TrndTimer(m_trender,m_interv);
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
  Service::finalize();
  return StatusCode::SUCCESS;
}

MemTrnd::MemTrnd(const std::string& name, ISvcLocator* sl) : Service(name,sl)
{
  declareProperty("TrendInterval",m_interv=60);
  declareProperty("PartitionName",m_PartName);
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

TrndTimer::TrndTimer(ISimpleTrendWriter *trender,int period) : GenTimer(0,period*1000)
{
  m_trender = trender;
}
void TrndTimer::timerHandler()
{
  long rss;
  rss = System::mappedMemory(System::kByte,System::Memory,lib_rtl_pid());
  long vm = System::virtualMemory(System::kByte,System::Memory,lib_rtl_pid());
  m_trender->addEntry("VirtMem",double(vm));
  m_trender->addEntry("ResidentSize",double(rss));
  m_trender->saveEvent();
}
void TrndTimer::Stop()
{
  GenTimer::Stop();
}
