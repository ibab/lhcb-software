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
static double gx[101];
static double gy[101];
static double gdy[101];
static int gndat;
static int analrun = 0;

double analyze(int npar,double *params)
{
  if (analrun == 0)
  {
    FILE *f=fopen("aligdata.dat","r");
    fscanf(f,"%d",&gndat);
    for (int i=0;i<gndat;i++)
    {
       fscanf(f,"%lf %lf %lf",&gx[i],&gy[i],&gdy[i]);
    }
    fclose(f);
  }
  analrun++;
  double result = 0.0;
  size_t i;
  for (i=0;i<gndat;i++)
  {
    double x=gx[i];
    double y;
    size_t j;
    y = 0.0;
    for (j=0;j<npar;j++)
    {
      y = x*y+params[j];
    }
    double chi = (y-gy[i])/gdy[i];
    result += chi*chi;
  }
  return result;
}


extern "C"
{
  int FitterThreadFunction(void *t)
  {
    Fitter *f = (Fitter*)t;
    f->run();
    return 1;
  }
  void Chi2(int &npar, double *grad, double &fval, double *params, int flag)
  {
    DrvInstance->m_fitter->write_params(npar,params);
    printf("Parameters:\n");
    for (int i= 0;i<npar;i++)
    {
      printf("Paramter %d %15g\n",i,params[i]);
    }
    DrvInstance->incidentSvc()->fireIncident(Incident(DrvInstance->name(),"DAQ_CONTINUE"));
    double lres = analyze(npar,params);
    DrvInstance->waitRunOnce();
//    DrvInstance->m_fitter->m_Lock->lockMutex();
    fval = DrvInstance->m_fitter->getIterationResult();
    printf ("Chi2: Function value: %15g  Value from local analysis %15g\n",fval,lres);
    printf("\n");
    fflush(stdout);
//    DrvInstance->m_fitter->m_Lock->unlockMutex();
    return;
  };
};

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


StatusCode DumAligDrv::stop()
{
  return StatusCode::SUCCESS;
}
StatusCode DumAligDrv::pause()
{
  setRunOnce();
//  m_fitter->m_Lock->unlockMutex();
//  m_fitter->m_Lock->lockMutex();
  return StatusCode::SUCCESS;
}

StatusCode DumAligDrv::start()
{
  Service::start();
//  FILE *f;
//  m_params.clear();
//  f = fopen(m_ParamFileName.c_str(),"r");
//  while (!feof(f))
//  {
//    double p;
//    fscanf(f,"%15g",&p);
//    if (feof(f)) break;
//    m_params.insert(m_params.end(),p);
//  }
//  fclose(f);
//  printf("Number of Parameters %d\n",m_params.size());
  m_fitter->m_Minuit->SetFCN(&Chi2);
  m_fitter->start();
  m_SvcName = "Chi2";
//  m_MonSvc->declareInfo(m_SvcName,m_result,"Chi Square",this);
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
  printf("Counter DNS: %s, Counter Task: %s\n Counter Names:\n",m_CntDNS.c_str(),m_CntTask.c_str());
  for (int i=0;i<this->m_CounterNames.size();i++)
  {
    printf("%s\n",m_CounterNames[i].c_str());
  }
  m_Minuit = new TMinuit(3);
  m_fitter = new Fitter(this,this->m_ParamFileName);
  m_fitter->m_Minuit = m_Minuit;
  m_incidentSvc->addListener(this,"DAQ_PAUSE");
  m_fitter->init();
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
  if (m_Minuit) delete m_Minuit;
  return StatusCode::SUCCESS;
}

StatusCode DumAligDrv::run()
{
//  m_result = analyze();
//  m_MonSvc->updateSvc(m_SvcName,0,this);
//  m_incidentSvc->fireIncident(Incident(name(),"DAQ_PAUSE"));
  return StatusCode::SUCCESS;
}

//double DumAligDrv::analyze()
//{
//  double result = 0.0;
//  size_t i;
//  for (i=0;i<m_dat_x.size();i++)
//  {
//    double x=m_dat_x[i];
//    double y;
//    size_t j;
//    y = 0.0;
//    for (j=0;j<m_params.size();j++)
//    {
//      y = x*m_result+m_params[j];
//    }
//    double chi = (y-m_dat_y[i])/m_dat_dy[i];
//    result += chi*chi;
//  }
//  return result;
//}

DumAligDrv::DumAligDrv(const std::string& name, ISvcLocator* sl) : OnlineService(name,sl),m_MonSvc(0),m_Minuit(0),m_fitter(0)
{
  declareProperty("PartitionName",   m_PartitionName= "LHCbA");
  declareProperty("ParamFileName",   m_ParamFileName= "/home/beat/aligparams.dat");
  declareProperty("CounterNames",m_CounterNames={"aaa/Chi2"});
  declareProperty("CounterDNS",m_CntDNS="mona08");
  declareProperty("CounterTask",m_CntTask="LHCbA_AligWrk_00");
  m_runonce = false;
  DrvInstance = this;
}

DumAligDrv::~DumAligDrv()
{
}

StatusCode DumAligDrv::queryInterface(const InterfaceID& riid, void** ppvIF)
{
  if (IRunable::interfaceID().versionMatch(riid))  {
    *ppvIF = (IRunable*) this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return OnlineService::queryInterface(riid, ppvIF);
}

Fitter::Fitter(DumAligDrv *t,std::string Pfname)
{
  m_Minuit = 0;
  m_parent = t;
  m_ParamFileName = Pfname;
  m_cntTask=0;
  m_CntTaskName = t->m_CntTask;
  m_CounterNames = t->m_CounterNames;
  m_CntDNS = t->m_CntDNS;
}

StatusCode Fitter::init()
{
//  m_Minuit = new TMinuit(100);
  printf("Counter DNS: %s, Counter Task: %s\n Counter Names:\n",m_CntDNS.c_str(),this->m_CntTaskName.c_str());
  for (int i=0;i<this->m_CounterNames.size();i++)
  {
    printf("%s\n",m_CounterNames[i].c_str());
  }
  int npar;
  std::vector<double> ps;
  read_params(npar,DrvInstance->m_params);
  for (size_t i=0;i<DrvInstance->m_params.size();i++)
  {
    char nam[2];
    char ii;
    ii = char(i);
    nam[0] = 'a'+ii;
    nam[1] = 0;
    m_Minuit->DefineParameter(i,nam,DrvInstance->m_params[i],10.0,0.0,0.0);
  };
  return StatusCode::SUCCESS;
}

StatusCode Fitter::run()
{
  int res=m_Minuit->Migrad();
  int npar;
  double par[100],dpar[100];
  printf("MIGRAD has finished with return code: %d\nParamters:\n",res);
  for (int i=0;i<DrvInstance->m_params.size();i++)
  {
    m_Minuit->GetParameter(i,par[i],dpar[i]);
    printf("Param #%d %15g +- %15g\n",i,par[i],dpar[i]);
  }
  fflush(stdout);
  DrvInstance->incidentSvc()->fireIncident(Incident(DrvInstance->name(),"DAQ_STOP"));
  return StatusCode::SUCCESS;
}
StatusCode Fitter::start()
{
  int status = ::lib_rtl_start_thread(FitterThreadFunction,this,&m_thread);
  return StatusCode::SUCCESS;
}

StatusCode Fitter::stop()
{
  return StatusCode::SUCCESS;
}

StatusCode Fitter::de_init()
{
  if (m_Minuit)
  {
    delete m_Minuit;
    m_Minuit = 0;
  }
  return StatusCode::SUCCESS;
}

double Fitter::getIterationResult()
{
  std::vector<CntrDescr*> cdesc;
  if(m_cntTask == 0)
  {
    m_cntTask = new CounterTask(m_CntTaskName,m_CntDNS);
  }
  m_cntTask->Counters(m_CounterNames,cdesc);
  if (cdesc.size() > 0)
  {
    return cdesc[0]->d_data;
  }
  else
  {
    printf("No Counters Found...\n");
  }
  return nan("");
}
void Fitter::write_params(int npar, std::vector<double> &params)
{
  FILE *f;
  f = fopen(m_ParamFileName.c_str(),"w");
  for (int i=0;i<npar;i++)
  {
    fprintf(f,"%15lf ",params[i]);
  }
  fprintf(f,"\n");
  fclose(f);
}

void Fitter::write_params(int npar, double *params)
{
  FILE *f;
  f = fopen(m_ParamFileName.c_str(),"w");
  for (int i=0;i<npar;i++)
  {
    fprintf(f,"%15lf ",params[i]);
  }
  fprintf(f,"\n");
  fclose(f);
}

void Fitter::read_params(int &npar, std::vector<double> &params)
{
  FILE *f;
  int i;
  params.clear();
  f = fopen(m_ParamFileName.c_str(),"r");
  while (!feof(f))
  {
    double p;
    fscanf(f,"%lf",&p);
    if (feof(f)) break;
    i++;
    params.insert(params.end(),p);
  }
  fclose(f);
  npar = i;
}

