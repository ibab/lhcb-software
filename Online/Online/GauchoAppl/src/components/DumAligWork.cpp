#include "DumAligWork.h"
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
#include "Gaucho/MonCounter.h"
#include "Trending/ITrendingTool.h"
#include "Trending/ISimpleTrendWriter.h"

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,DumAligWork)
using namespace LHCb;

//static DumAligWork *WorkInstance;
extern "C"
{
  int WorkerThreadFunction(void *t)
  {
    DumAligWork *WorkInstance = (DumAligWork*)t;
    int run_count = 0;
    while (1)
    {
//      WorkInstance->m_Lock->lockMutex();
//      WorkInstance->m_Lock->unlockMutex();
      WorkInstance->waitRunOnce();
//      lib_rtl_lock(WorkInstance->m_Lock);
//      sleep(1);
//      lib_rtl_unlock(WorkInstance->m_Lock);
      run_count++;
      if (run_count == 1) sleep(10);
      else sleep(1);
      WorkInstance->ReadParams();
      WorkInstance->i_run();
//      WorkInstance->incidentSvc()->fireIncident(Incident(WorkInstance->name(),"DAQ_PAUSE"));
    }
    return 1;
  }
};
MyTimer::MyTimer(DumAligWork *tis)
{
  m_Worker = tis;
}
void MyTimer::timerHandler()
{
  m_Worker->i_continue();
}

StatusCode DumAligWork::stop()
{
  if (m_thread != 0)
  {
    lib_rtl_cancel_thread(m_thread);
  }
  return StatusCode::SUCCESS;
}
void DumAligWork::waitRunOnce()
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
void DumAligWork::setRunOnce()
{
  m_runonce=true;
}
void DumAligWork::ReadParams()
{
  FILE *f;
  m_params.clear();
  f = fopen(m_ParamFileName.c_str(),"r");
  while (!feof(f))
  {
    double p;
    fscanf(f,"%lf",&p);
    if (feof(f)) break;
    m_params.insert(m_params.end(),p);
  }
  fclose(f);
  printf("Number of Parameters %d\n",m_params.size());
  for (size_t i=0;i<m_params.size();i++)
  {
    printf ("Parameter %d %lf\n",i,m_params[i]);
  }
  printf("\n");
  fflush(stdout);
}
StatusCode DumAligWork::i_continue()
{
//  m_Lock->unlockMutex();
//  m_Lock->lockMutex();
//  lib_rtl_unlock(m_Lock);
//  lib_rtl_lock(m_Lock);
  setRunOnce();
  return StatusCode::SUCCESS;
}
StatusCode DumAligWork::start()
{
  OnlineService::start();
//  m_Lock->unlockMutex();
//  m_Lock->lockMutex();
  return StatusCode::SUCCESS;
}

StatusCode DumAligWork::initialize()
{
  StatusCode sc;
  sc = service(m_monitorSvcType,m_MonSvc,true);
  if ( !sc.isSuccess() )  {
    return error("Cannot access monitoring service of type "+m_monitorSvcType+".");
  }
  OnlineService::initialize();
  size_t i;
  FILE *f;
  size_t ndat=0;
  m_SvcName = "Chi2";
  m_MonSvc->declareInfo(m_SvcName,m_result,"Chi Square",this);
  f = fopen(m_DataFileName.c_str(),"r");
  std::fscanf(f,"%d",(int*)(&ndat));
  m_dat_x.reserve(ndat);
  m_dat_y.reserve(ndat);
  m_dat_dy.reserve(ndat);
  for (i=0;i<ndat;i++)
  {
    double x,y,dy;
    fscanf(f,"%lf %lf %lf\n",&x,&y,&dy);
    m_dat_x.insert(m_dat_x.end(), x);
    m_dat_y.insert(m_dat_y.end(), y);
    m_dat_dy.insert(m_dat_dy.end(), dy);
    printf("%15f %15f %15f\n",m_dat_x[i],m_dat_y[i],m_dat_y[i]);
  }
  fclose(f);
  fflush(stdout);
  m_incidentSvc->addListener(this,"DAQ_CONTINUE");
  m_incidentSvc->addListener(this,"APP_RUNNING");
//  m_Lock = new BRTLLock();
//  lib_rtl_create_lock(0,&m_Lock);
//  m_Lock->m_name = "Pause-Continue Lock";
//  lib_rtl_lock(m_Lock);
//m_Lock->lockMutex();
  int status = ::lib_rtl_start_thread(WorkerThreadFunction,this,&m_thread);
  return StatusCode::SUCCESS;
}

void DumAligWork::handle(const Incident& inc)
{
  if (inc.type() == "APP_RUNNING")
  {
//    lib_rtl_unlock(m_Lock);
//    lib_rtl_lock(m_Lock);
//    run();
  }
  if (inc.type() == "DAQ_CONTINUE")
  {
//    if (m_Timer == 0)
//    {
//      m_Timer = new MyTimer(this);
//    }
//    m_Timer->start(1);
//    m_Lock->unlockMutex();
//    m_Lock->lockMutex();
//    lib_rtl_unlock(m_Lock);
//    lib_rtl_lock(m_Lock);
    setRunOnce();
  }

}

StatusCode DumAligWork::finalize()
{
  StatusCode sc;
  OnlineService::finalize();
  return StatusCode::SUCCESS;
}

StatusCode DumAligWork::i_run()
{
  m_result = analyze();
  m_MonSvc->updatePerSvc();
  m_incidentSvc->fireIncident(Incident(name(),"DAQ_PAUSE"));
  return StatusCode::SUCCESS;
}
StatusCode DumAligWork::run()
{
//  m_Lock->unlockMutex();
//  m_Lock->lockMutex();
//  lib_rtl_unlock(m_Lock);
//  lib_rtl_lock(m_Lock);
  setRunOnce();
  return StatusCode::SUCCESS;
}

double DumAligWork::analyze()
{
  double result = 0.0;
  size_t i;
  for (i=0;i<m_dat_x.size();i++)
  {
    double x=m_dat_x[i];
    double y;
    size_t j;
    y = 0.0;
    for (j=0;j<m_params.size();j++)
    {
      y = x*y+m_params[j];
    }
    double chi = (y-m_dat_y[i])/m_dat_dy[i];
    result += chi*chi;
  }
  m_result = result;
  printf("Function Result: %15lf",result);
  fflush (stdout);
  return result;
}

DumAligWork::DumAligWork(const std::string& name, ISvcLocator* sl) : OnlineService(name,sl),m_result(0),m_MonSvc(0)
{
  declareProperty("PartitionName",   m_PartitionName= "LHCb");
  declareProperty("DataFileName",   m_DataFileName= "/home/beat/aligndata.dat");
  declareProperty("ParamFileName",   m_ParamFileName= "/home/beat/aligparam.dat");
  m_Timer=0;
  m_Lock=0;
  m_runonce = false;
}

DumAligWork::~DumAligWork()
{
}

StatusCode DumAligWork::queryInterface(const InterfaceID& riid, void** ppvIF)
{
  if (IRunable::interfaceID().versionMatch(riid))  {
    *ppvIF = (IRunable*) this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return OnlineService::queryInterface(riid, ppvIF);
}

