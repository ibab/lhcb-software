#include "DumAligDrv.h"
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
#include "OnlineBase/WT/wtdef.h"
//#include "OnlineBase/WT/wt_facilities.h"
#include <stdio.h>
#include <stdlib.h>
#include "/cvmfs/lhcb.cern.ch/lib/lcg/external/ROOT/5.32.02/x86_64-slc5-gcc46-opt/root/include/TMinuit.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,DumAligDrv)

using namespace LHCb;
static DumAligDrv *DrvInstance;

void DumAligDrv::waitRunOnce()
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
void DumAligDrv::setRunOnce()
{
  m_runonce=true;
}


StatusCode DumAligDrv::pause()
{
  setRunOnce();
  return StatusCode::SUCCESS;
}
StatusCode DumAligDrv::start()
{
  Service::start();
  m_fitter->i_start();
  return StatusCode::SUCCESS;
}
StatusCode DumAligDrv::stop()
{
  return StatusCode::SUCCESS;
}

StatusCode DumAligDrv::initialize()
{
  StatusCode sc;
  sc = service(m_monitorSvcType,m_MonSvc,true);
  if ( !sc.isSuccess() )  {
    return error("Cannot access monitoring service of type "+m_monitorSvcType+".");
  }
  OnlineService::initialize();
//  printf("Counter DNS: %s, Counter Task: %s\n Counter Names:\n",m_CntDNS.c_str(),m_CntTask.c_str());
//  for (int i=0;i<this->m_CounterNames.size();i++)
//  {
//    printf("%s\n",m_CounterNames[i].c_str());
//  }
  service("ToolSvc",m_ToolSvc,true);
  sc = m_ToolSvc->retrieveTool("LHCb::Fitter","bbb",m_fitter,this,true);

//  m_fitter = new Fitter(this);
  m_incidentSvc->addListener(this,"DAQ_PAUSE");
  m_RefFileName = "/group/online/dataflow/options/"+m_PartitionName+"/Alignement_Reference_File.txt";
  return StatusCode::SUCCESS;
}

void DumAligDrv::handle(const Incident& inc)
{
  if (inc.type() == "DAQ_PAUSE")
  {
    pause();
  }

}

StatusCode DumAligDrv::finalize()
{
  StatusCode sc;
  OnlineService::finalize();
  return StatusCode::SUCCESS;
}

StatusCode DumAligDrv::run()
{
  return StatusCode::SUCCESS;
}

IGauchoMonitorSvc *DumAligDrv::getMonSvc()
{
  return this->m_MonSvc;
}
void DumAligDrv::writeReference()
{
  FILE *f;
  f = fopen(m_RefFileName.c_str(), "r+");
  unsigned long ref;
  if (f == 0)
  {
    ref = 0;
    f = fopen(m_RefFileName.c_str(), "w");
  }
  else
  {
    int stat = fscanf(f, "%ld", &ref);
    if (stat == 0)
      ref = 0;
    rewind(f);
  }
  ref++;
  fprintf(f, "%ld", ref);
  fflush(f);
  fclose(f);
}


DumAligDrv::DumAligDrv(const std::string& name, ISvcLocator* sl) : base_class(name,sl),m_MonSvc(0),m_fitter(0),m_ToolSvc(0)
{
  declareProperty("PartitionName",   m_PartitionName= "LHCbA");
  m_runonce = false;
  DrvInstance = this;
}

DumAligDrv::~DumAligDrv()
{
}

void DumAligDrv::doContinue()
{
  incidentSvc()->fireIncident(Incident(name(),"DAQ_CONTINUE"));
}
void DumAligDrv::doStop()
{
  incidentSvc()->fireIncident(Incident(name(),"DAQ_STOP"));
}
//StatusCode DumAligDrv::queryInterface(const InterfaceID& riid, void** ppvIF)
//{
//  if (IRunable::interfaceID().versionMatch(riid))
//  {
//    *ppvIF = (IRunable*) this;
//    addRef();
//    return StatusCode::SUCCESS;
//  }
//  else if (IDumAligDrv::interfaceID().versionMatch(riid))
//  {
//    *ppvIF = (IDumAligDrv*)this;
//    addRef();
//    return StatusCode::SUCCESS;
//  }
//  return OnlineService::queryInterface(riid, ppvIF);
//}

