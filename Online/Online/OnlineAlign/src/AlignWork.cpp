//#include "GaudiKernel/SmartIF.h"
#include "AlignWork.h"
#include "Gaucho/Utilities.h"
#include "Gaucho/IGauchoMonitorSvc.h"
#include "GaudiKernel/IIncidentSvc.h"

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,AlignWork)
using namespace LHCb;

//static DumAligWork *WorkInstance;
extern "C"
{
  int WorkerThreadFunction(void *t)
  {
    LHCb::AlignWork* WorkInstance = (LHCb::AlignWork*)t;
    int run_count = 0;
    while (1)
    {
      WorkInstance->waitRunOnce();
      run_count++;
      if (run_count == 1) sleep(10);
      else usleep(100000);
      WorkInstance->i_run();
    }
    return 1;
  }
}

StatusCode LHCb::AlignWork::stop()
{
  return StatusCode::SUCCESS;
}
void LHCb::AlignWork::waitRunOnce()
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
void LHCb::AlignWork::setRunOnce()
{
  m_runonce=true;
}
StatusCode LHCb::AlignWork::i_continue()
{
  setRunOnce();
  return StatusCode::SUCCESS;
}
StatusCode LHCb::AlignWork::start()
{
  OnlineService::start();
  return StatusCode::SUCCESS;
}

StatusCode LHCb::AlignWork::initialize()
{
  OnlineService::initialize();
  service("ToolSvc",m_ToolSvc,true);
  service("MonitorSvc",m_MonSvc,true);
  m_incidentSvc = incidentSvc();
  m_incidentSvc->addListener(this,"DAQ_CONTINUE");
  m_incidentSvc->addListener(this,"APP_RUNNING");
  ::lib_rtl_start_thread(WorkerThreadFunction,this,&m_thread);
  if (m_RefFileName.empty())
  {
    m_RefFileName = "/group/online/dataflow/options/"+m_PartitionName+"/Alignment_Reference_File.txt";
  }
  StatusCode sc=m_ToolSvc->retrieveTool(m_FitFcnClass,m_FitFcnName,m_fitterFcn,this,true);
//  m_fitterFcn->setParent((void*)((IDumAligWork*)this));
  return StatusCode::SUCCESS;
}

void LHCb::AlignWork::handle(const Incident& inc)
{
  if (inc.type() == "APP_RUNNING")
  {
  }
  if (inc.type() == "DAQ_CONTINUE")
  {
    setRunOnce();
  }

}

StatusCode LHCb::AlignWork::finalize()
{
  StatusCode sc;
  if (m_thread != 0)
  {
    ::lib_rtl_cancel_thread(m_thread);
  }
  m_ToolSvc->release();
  m_MonSvc->release();
  OnlineService::finalize();
  return StatusCode::SUCCESS;
}

StatusCode LHCb::AlignWork::i_run()
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
StatusCode LHCb::AlignWork::run()
{
  setRunOnce();
  return StatusCode::SUCCESS;
}


LHCb::AlignWork::AlignWork(const std::string& name, ISvcLocator* sl) :
        base_class(name,sl),m_MonSvc(0),m_fitterFcn(0),m_Reference(0),m_thread(0),m_runonce(false)
{
  declareProperty("PartitionName",   m_PartitionName= "LHCb");
  declareProperty("ReferenceFileName",  m_RefFileName);
  declareProperty("FitterFunktionClass",  m_FitFcnClass);
  declareProperty("FitterFunktionName",  m_FitFcnName);
}

LHCb::AlignWork::~AlignWork()
{
}

void LHCb::AlignWork::readReference()
{
  FILE *f = fopen(m_RefFileName.c_str(),"r");
  fscanf(f,"%lu",&m_Reference);
  fclose(f);
}
