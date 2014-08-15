//#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "Gaucho/Utilities.h"
#include "Gaucho/IGauchoMonitorSvc.h"
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
  StatusCode sc=m_ToolSvc->retrieveTool("LHCb::FitterFcn","bbb",m_fitterFcn,0,true);
  m_incidentSvc = incidentSvc();
  m_incidentSvc->addListener(this,"DAQ_CONTINUE");
  m_incidentSvc->addListener(this,"APP_RUNNING");
  ::lib_rtl_start_thread(WorkerThreadFunction,this,&m_thread);
  if (m_RefFileName.empty())
  {
    m_RefFileName = "/group/online/dataflow/options/"+m_PartitionName+"/Alignement_Reference_File.txt";
  }
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
  readReference();
  m_fitterFcn->i_run();
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
  service("ToolSvc",m_ToolSvc,true);
  m_Lock=0;
  m_runonce = false;
  service("MonitorSvc",m_MonSvc,true);
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

