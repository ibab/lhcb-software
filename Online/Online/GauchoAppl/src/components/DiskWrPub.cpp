#include "DiskWrPub.h"
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

DECLARE_SERVICE_FACTORY(DiskWrPub)

namespace DISKWRPUB
{
  void Analyze(void *arg, void* buff ,int siz, MonMap *mmap, MonAdder *)
  {
    DiskWrPub *tis = (DiskWrPub*)arg;
    tis->analyze(buff,siz, mmap);
  }
}

StatusCode DiskWrPub::start()
{
  PubSvc::start();
  m_adder->SetCycleFn(DISKWRPUB::Analyze,this);
  return StatusCode::SUCCESS;
}

StatusCode DiskWrPub::initialize()
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
    sc = tools->retrieveTool("SimpleTrendWriter","DiskWrPubWriter",m_trender,this);
    if (sc.isSuccess() && m_trender != 0)
    {
      std::string nnam("HLTDiskWr");
      m_trender->setPartitionAndName(this->m_PartitionName,nnam);
      m_trender->setMaxTimeNoWrite(600);
    }
  }
  return sc;
}

StatusCode DiskWrPub::finalize()
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


void DiskWrPub::analyze(void *, int ,MonMap* mmap)
{
  MonMap::iterator i,j,k,l;
  i = mmap->find("R_EventSelector/EventsReq");
  if (i == mmap->end()) return;
  j = mmap->find("R_Runable/EvtCount");
  if (j == mmap->end()) return;
  k = mmap->find("R_SND_0/EventsOut");
  if (k == mmap->end()) return;
  l = mmap->find("R_SND_0/BytesOut");
  if (k == mmap->end()) return;
  double evreq,evcount,evout,bout;
//  double RMeps;
  evreq = ((CntrDescr*)MonCounter::de_serialize((*i).second))->d_data;
  evcount = ((CntrDescr*)MonCounter::de_serialize((*j).second))->d_data;
  evout = ((CntrDescr*)MonCounter::de_serialize((*k).second))->d_data;
  bout =  ((CntrDescr*)MonCounter::de_serialize((*l).second))->d_data;
//  m_NoBytes = float(double(bout)/1.0e09);
//  m_NoFiles = NFiles;
//  m_NoMEPs  = NMEPs;
//  m_MEPRate = RMeps;
//  if (m_NoBytesSvc == 0)
//  {
//    m_NoBytesSvc = new DimService((char*)(std::string("Stat/")+m_PartitionName+"_X_Overflow/NoBytes").c_str(),m_NoBytes);
//  }
//  m_NoBytesSvc->updateService(m_NoBytes);
//  if (m_NoFilesSvc == 0)
//  {
//    m_NoFilesSvc = new DimService((char*)(std::string("Stat/")+m_PartitionName+"_X_Overflow/NoFiles").c_str(),m_NoFiles);
//  }
//  m_NoFilesSvc->updateService(m_NoFiles);
//  if (m_NoMEPsSvc == 0)
//  {
//    m_NoMEPsSvc = new DimService((char*)(std::string("Stat/")+m_PartitionName+"_X_Overflow/NoMEPs").c_str(),m_NoMEPs);
//  }
//  m_NoMEPsSvc->updateService(m_NoMEPs);

  if (m_enableTrending)
  {
    m_trender->startEvent();
    m_trender->addEntry("Diskwr Events Requested", evreq);
    m_trender->addEntry("Diskwr Events Processed", evcount);
    m_trender->addEntry("Diskwr Events sent", evout);
    m_trender->addEntry("Diskwr Data Rate [Gb/s]",bout/1.0e09);
    m_trender->saveEvent();
  }
}

DiskWrPub::DiskWrPub(const std::string& name, ISvcLocator* sl)
 : PubSvc(name,sl)
{
  declareProperty("TrendingOn",  m_enableTrending);
  m_NoBytes = 0.0;
  m_NoFiles = 0.0;
  m_NoMEPs = 0.0;
  m_NoBytesSvc = m_NoFilesSvc = m_NoMEPsSvc = 0;
}

DiskWrPub::~DiskWrPub()
{
}

StatusCode DiskWrPub::queryInterface(const InterfaceID& riid, void** ppvIF)
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

