#include "AlignDrv.h"
//#include "GaudiKernel/SmartIF.h"
//#include "GaudiKernel/SvcFactory.h"
//#include "GaudiKernel/IIncidentSvc.h"
//#include "GaudiKernel/IHistogramSvc.h"
//
//#include "GauchoAppl/HistAdder.h"
//#include "GauchoAppl/CounterAdder.h"
//#include "GauchoAppl/SaveTimer.h"
//#include "GauchoAppl/AdderSys.h"
//#include "Gaucho/Utilities.h"
//#include "Gaucho/IGauchoMonitorSvc.h"
//#include "Gaucho/MonCounter.h"
//#include "Trending/ITrendingTool.h"
//#include "Trending/ISimpleTrendWriter.h"
//#include "OnlineBase/WT/wtdef.h"
//#include "OnlineBase/WT/wt_facilities.h"
#include <cstdio>
#include <cstdlib>
#include "TMinuit.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,AlignDrv)

using namespace LHCb;
static AlignDrv *DrvInstance;

void AlignDrv::waitRunOnce()
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
void AlignDrv::setRunOnce()
{
  m_runonce=true;
}


StatusCode AlignDrv::pause()
{
  setRunOnce();
  return StatusCode::SUCCESS;
}
StatusCode AlignDrv::start()
{
  Service::start();
  m_fitter->i_start();
  return StatusCode::SUCCESS;
}
StatusCode AlignDrv::stop()
{
  return StatusCode::SUCCESS;
}

StatusCode AlignDrv::initialize()
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
  sc = m_ToolSvc->retrieveTool(m_FitterClass,m_FitterName,m_fitter,this,true);
  if ( !sc.isSuccess() )  {
    return error("Failed to retriev Fitter Tool: "+m_FitterName+" of Class "+m_FitterClass);
  }

//  m_fitter = new Fitter(this);
  m_incidentSvc->addListener(this,"DAQ_PAUSE");
  if (m_RefFileName.empty()) {
     m_RefFileName = "/group/online/dataflow/options/"+m_PartitionName+"/Alignment_Reference_File.txt";
  }
  return StatusCode::SUCCESS;
}

void AlignDrv::handle(const Incident& inc)
{
  if (inc.type() == "DAQ_PAUSE")
  {
    pause();
  }

}

StatusCode AlignDrv::finalize()
{
  StatusCode sc;
  OnlineService::finalize();
  return StatusCode::SUCCESS;
}

StatusCode AlignDrv::run()
{
  return StatusCode::SUCCESS;
}

IGauchoMonitorSvc *AlignDrv::getMonSvc()
{
  return this->m_MonSvc;
}
void AlignDrv::setReferenceBase(long ref)
{
  if (m_RefBase == 0)
  {
    m_RefBase = new long;
  }
  *m_RefBase = ref;
}
long AlignDrv::rdRef(FILE *f)
{
  long ref;
  int stat = fscanf(f, "%ld", &ref);
  if (stat == 0)
  {
    ref = 0;
  }
  return ref;
}
void AlignDrv::writeReference()
{
  long ref=0;
  FILE *f = fopen(m_RefFileName.c_str(), "r+");
  if (f == 0)
  {
    if (m_firstRef)
    {
      if (m_RefBase != 0)
      {
        ref = *m_RefBase;
      }
      else
      {
        ref = 0;
      }
      m_firstRef = false;
    }
    f = fopen(m_RefFileName.c_str(), "w");
  }
  else
  {
    if (m_firstRef)
    {
      if (m_RefBase != 0)
      {
        ref = *m_RefBase;
      }
      else
      {
        ref = rdRef(f);
      }
      m_firstRef = false;
    }
    else
    {
      ref = rdRef(f);
    }
    rewind(f);
  }
  ref++;
  fprintf(f, "%ld", ref);
  fflush(f);
  fclose(f);
}


AlignDrv::AlignDrv(const std::string& name, ISvcLocator* sl) : base_class(name,sl),m_MonSvc(0),m_fitter(0),m_ToolSvc(0)
{
  declareProperty("PartitionName", m_PartitionName = "LHCbA");
  declareProperty("FitterClass", m_FitterClass);
  declareProperty("FitterName", m_FitterName);
  declareProperty("RefFileName", m_RefFileName = "");
  m_runonce = false;
  DrvInstance = this;
  m_RefBase = 0;
  m_firstRef = true;
}

AlignDrv::~AlignDrv()
{
}

void AlignDrv::doContinue()
{
  incidentSvc()->fireIncident(Incident(name(),"DAQ_CONTINUE"));
}
void AlignDrv::doStop()
{
  incidentSvc()->fireIncident(Incident(name(),"DAQ_STOP"));
}
