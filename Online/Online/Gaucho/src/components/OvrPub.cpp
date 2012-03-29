#include "OvrPub.h"
#include "GaudiKernel/SmartIF.h"
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

DECLARE_SERVICE_FACTORY(OvrPub)

namespace OVRPUB
{
  void Analyze(void *arg, void* buff ,int siz, MonMap *mmap, MonAdder *)
  {
    OvrPub *tis = (OvrPub*)arg;
    tis->analyze(buff,siz, mmap);
  }
}

StatusCode OvrPub::start()
{
  PubSvc::start();
  m_adder->SetCycleFn(OVRPUB::Analyze,this);
  return StatusCode::SUCCESS;
}

StatusCode OvrPub::initialize()
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
    sc = tools->retrieveTool("SimpleTrendWriter","OvrPubWriter",m_trender,this);
    if (sc.isSuccess() && m_trender != 0)
    {
      std::string nnam("HLTOverflowWriter");
      m_trender->setPartitionAndName(this->m_PartitionName,nnam);
      m_trender->setMaxTimeNoWrite(600);
    }
  }
  return sc;
}

StatusCode OvrPub::finalize()
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


void OvrPub::analyze(void *, int ,MonMap* mmap)
{
  MonMap::iterator i,j,k;
  i = mmap->find("OverflowWr/BytesOut");
  if (i == mmap->end()) return;
  j = mmap->find("OverflowWr/NumberofFiles");
  if (j == mmap->end()) return;
  k = mmap->find("OverflowWr/MEPsOut");
  if (k == mmap->end()) return;
  long long BOut,NFiles,NMEPs;
  BOut = ((CntrDescr*)MonCounter::de_serialize((*i).second))->l_data;
  NFiles = ((CntrDescr*)MonCounter::de_serialize((*j).second))->l_data;
  NMEPs = ((CntrDescr*)MonCounter::de_serialize((*k).second))->l_data;
  m_NoBytes = (float)BOut/1.0e09;
  m_NoFiles = NFiles;
  m_NoMEPs  = NMEPs;
  if (m_NoBytesSvc == 0)
  {
    m_NoBytesSvc = new DimService((char*)(std::string("Stat/")+m_PartitionName+"_X_Overflow/NoBytes").c_str(),m_NoBytes);
  }
  m_NoBytesSvc->updateService(m_NoBytes);
  if (m_NoFilesSvc == 0)
  {
    m_NoFilesSvc = new DimService((char*)(std::string("Stat/")+m_PartitionName+"_X_Overflow/NoFiles").c_str(),m_NoFiles);
  }
  m_NoFilesSvc->updateService(m_NoFiles);
  if (m_NoMEPsSvc == 0)
  {
    m_NoMEPsSvc = new DimService((char*)(std::string("Stat/")+m_PartitionName+"_X_Overflow/NoMEPs").c_str(),m_NoMEPs);
  }
  m_NoMEPsSvc->updateService(m_NoMEPs);

  if (m_enableTrending)
  {
    m_trender->startEvent();
    m_trender->addEntry("BytesWrittenToOverflow", (double)BOut/1.0e09);
    m_trender->addEntry("NumberOfFilesinOverflow", (double)NFiles);
    m_trender->addEntry("NumberOfMEPsinOverflow", (double)NMEPs);
    m_trender->saveEvent();
  }
}

OvrPub::OvrPub(const std::string& name, ISvcLocator* sl)
 : PubSvc(name,sl)
{
  declareProperty("TrendingOn",  m_enableTrending);
  m_NoBytes = 0.0;
  m_NoFiles = 0.0;
  m_NoMEPs = 0.0;
}

OvrPub::~OvrPub()
{
}

StatusCode OvrPub::queryInterface(const InterfaceID& riid, void** ppvIF)
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

