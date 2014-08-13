//#include "GaudiKernel/SmartIF.h"
//#include "GaudiKernel/SvcFactory.h"
//#include "GaudiKernel/IIncidentSvc.h"
//#include "GaudiKernel/IHistogramSvc.h"
//
//#include "GauchoAppl/HistAdder.h"
//#include "GauchoAppl/CounterAdder.h"
//#include "GauchoAppl/SaveTimer.h"
//#include "GauchoAppl/AdderSys.h"
#include "Gaucho/Utilities.h"
#include "Gaucho/IGauchoMonitorSvc.h"
//#include "Gaucho/MonCounter.h"
//#include "Trending/ITrendingTool.h"
//#include "Trending/ISimpleTrendWriter.h"
#include "DumAligWork.h"
#include "FitterFcn.h"

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,DumAligWork)
using namespace LHCb;

//static DumAligWork *WorkInstance;
extern "C"
{
  int WorkerThreadFunction(void *t)
  {
    LHCb::DumAligWork* WorkInstance = (LHCb::DumAligWork*)t;
    int run_count = 0;
    while (1)
    {
      WorkInstance->waitRunOnce();
      run_count++;
      if (run_count == 1) sleep(10);
      else usleep(7500000);
      WorkInstance->i_run();
    }
    return 1;
  }
};

StatusCode LHCb::DumAligWork::stop()
{
  if (m_thread != 0)
  {
    ::lib_rtl_cancel_thread(m_thread);
  }
  return StatusCode::SUCCESS;
}
void LHCb::DumAligWork::waitRunOnce()
{
  while (1)
  {
    if (m_runonce)
    {
      m_runonce= false;
      break;
    }
    usleep(500000);
  }
}
void LHCb::DumAligWork::setRunOnce()
{
  m_runonce=true;
}
StatusCode LHCb::DumAligWork::i_continue()
{
//  m_Lock->unlockMutex();
//  m_Lock->lockMutex();
//  lib_rtl_unlock(m_Lock);
//  lib_rtl_lock(m_Lock);
  setRunOnce();
  return StatusCode::SUCCESS;
}
StatusCode LHCb::DumAligWork::start()
{
  OnlineService::start();
//  m_Lock->unlockMutex();
//  m_Lock->lockMutex();
  return StatusCode::SUCCESS;
}

StatusCode LHCb::DumAligWork::initialize()
{
  OnlineService::initialize();
  this->
  m_incidentSvc->addListener(this,"DAQ_CONTINUE");
  m_incidentSvc->addListener(this,"APP_RUNNING");
  ::lib_rtl_start_thread(WorkerThreadFunction,this,&m_thread);
  return StatusCode::SUCCESS;
}

void LHCb::DumAligWork::handle(const Incident& inc)
{
  if (inc.type() == "APP_RUNNING")
  {
  }
  if (inc.type() == "DAQ_CONTINUE")
  {
    setRunOnce();
  }

}

StatusCode LHCb::DumAligWork::finalize()
{
  StatusCode sc;
  OnlineService::finalize();
  return StatusCode::SUCCESS;
}

StatusCode LHCb::DumAligWork::i_run()
{
  m_fitterFcn->run();
  m_fitterFcn->ReadParams();
  readReference();
  m_result = m_fitterFcn->analyze();
  m_fitterFcn->PubResult(m_Reference);
//  printf("Function Result: %15g\n",m_result);
//  fflush (stdout);
//  m_MonSvc->updatePerSvc(m_Reference);
  m_incidentSvc->fireIncident(Incident(name(),"DAQ_PAUSE"));
  return StatusCode::SUCCESS;
}
StatusCode LHCb::DumAligWork::run()
{
  setRunOnce();
  return StatusCode::SUCCESS;
}


LHCb::DumAligWork::DumAligWork(const std::string& name, ISvcLocator* sl) : OnlineService(name,sl),m_result(0),m_MonSvc(0),m_Reference(0)
{
  declareProperty("PartitionName",   m_PartitionName= "LHCb");
  declareProperty("ReferenceFileName",  m_RefFileName);
  m_fitterFcn = new FitterFcn(this);
  m_Lock=0;
  m_runonce = false;
}
LHCb::DumAligWork::~DumAligWork()
{
}
void LHCb::DumAligWork::readReference()
{
  FILE *f;
  f = fopen(m_RefFileName.c_str(),"r");
  fscanf(f,"%ld",&m_Reference);
  fclose(f);
}
StatusCode LHCb::DumAligWork::queryInterface(const InterfaceID& riid, void** ppvIF)
{
  if (IRunable::interfaceID().versionMatch(riid))  {
    *ppvIF = (IRunable*) this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return OnlineService::queryInterface(riid, ppvIF);
}

