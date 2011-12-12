#include "RateSvc.h"
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
#include "Gaucho/RateService.h"
#include "Trending/ITrendingTool.h"
#include "Trending/ISimpleTrendWriter.h"

DECLARE_SERVICE_FACTORY(RateSvc)

namespace RATESVC
{
  void Analyze(void *arg, void* buff ,int siz, MonMap *mmap, MonAdder *)
  {
    RateSvc *tis = (RateSvc*)arg;
    tis->analyze(buff,siz, mmap);
  }
}

class MyErrh : public DimErrorHandler
{
public:
  bool m_flag;
  void errorHandler(int severity, int code, char *msg)
  {
    if (m_flag)
      {
        ::lib_rtl_output(LIB_RTL_INFO,"DIM(RateSvc): Code %d.%x %s\n",severity,code,msg);
      }
    return;
  }
  MyErrh () : DimErrorHandler()
  {
    m_flag = true;
  }
  void start()
  {
    m_flag = true;
  }
  void stop()
  {
    m_flag = false;
  }
};


void RateSvc::analyze(void *, int ,MonMap* mmap)
{
//  unsigned long long tim = GetTime();
//  unsigned long long delta;
//  if (m_prevupdate == 0)
//  {
//    m_prevupdate = tim;
//    return;
//  }
//  else
//  {
//    delta = tim - m_prevupdate;
//  }
//  m_prevupdate = tim;
  makerate(mmap);
}



void RateSvc::makerate(MonMap* mmap)
{
#define MAX_SERVICE_SIZE 128
  std::string nnam;
  MonMap::iterator it;
  double rate;
  if (m_enableTrending && m_trender)
  {
    m_trender->startEvent();
  }
  for (it = mmap->begin();it != mmap->end();it++)
  {
    DimBuffBase *m_RateBuff = (DimBuffBase*)it->second;
    if (m_RateBuff != 0)
    {
      std::string nams = it->first;
      switch (m_RateBuff->type)
      {
        case C_RATEFLOAT:
        {
          float *a = (float*)AddPtr(m_RateBuff,m_RateBuff->dataoff);
          rate = (double)*a;
          break;
        }
        case C_RATEDOUBLE:
        {
          double *a = (double*)AddPtr(m_RateBuff,m_RateBuff->dataoff);
          rate = *a;
//          printf("Quantity: %s %f\n",nams.c_str(),*a);
          break;
        }
        default:
        {
          continue;
        }
      }
      std::string sname;
      OUTServiceDescr *outs;
      outs = findOUTService(nams);
      RateService *s=0;
      if (outs != 0)
      {
        s = (RateService*) outs->m_svc;
      }
      else
      {
        DimServer::autoStartOn();
        sname = m_NamePrefix + nams;
        if (sname.size() < MAX_SERVICE_SIZE)
        {
          s = new RateService(sname.c_str(), (char*) "D:1");
          outs = new OUTServiceDescr((char*) nams.c_str(), (void*) s);
          this->m_outputServicemap.insert(OUTServicePair(nams, outs));
        }
      }
      if (s!=0) s->Updater(sizeof(rate));
      if (m_enableTrending && m_trender)
      {
        std::string tnam;
        size_t i=nams.find('/');
        tnam = nams.substr(i+1);
        m_trender->addEntry(tnam, rate);
      }
    }
  }
  if (m_enableTrending && m_trender)
  {
    m_trender->saveEvent();
  }
}

void RateSvc::makecounters(MonMap* mmap)
{
#define MAX_SERVICE_SIZE 128
  MonMap::iterator it;
  DimBuffBase *b;
  for (it=mmap->begin();it!= mmap->end();it++)
  {
    b = (DimBuffBase*)it->second;
    std::string snam;
    char *n = (char*)AddPtr(b,b->nameoff);
    void *dat = AddPtr(b,b->dataoff);
    snam = m_prefix+"/"+n;
    COutServiceBase *ob;
    COUTServiceMap::iterator it = this->m_outmap.find(snam);
    ob = it->second;
    if(it == m_outmap.end())
    {
      ob = 0;
    }
    switch(b->type)
    {
      case   C_INT:
      {
        COutService<int> *os;
        if (ob == 0)
        {
          ob = os = new COutService<int>(snam);
          m_outmap[snam] = os;
        }
        os = (COutService<int> *)ob;
        int *d = (int*)dat;;
        os->Update(*d);
        break;
      }
      case   C_LONGLONG:
      {
        COutService<long long> *os;
        if (ob == 0)
        {
          ob = os  = new COutService<long long>(snam);
          m_outmap[snam] = os;
        }
        os = (COutService<long long> *)ob;
        long long *d = (long long *)dat;;
        os->Update(*d);
        break;
      }
      case   C_FLOAT:
      {
        COutService<float> *os;
        if (ob == 0)
        {
          ob = os = new COutService<float>(snam);
          m_outmap[snam] = os;
        }
        os = (COutService<float> *)ob;
        float *d = (float*)dat;;
        os->Update(*d);
        break;
      }
      case   C_DOUBLE:
      {
        COutService<double> *os;
        if (ob == 0)
        {
          ob = os = new COutService<double>(snam);
          m_outmap[snam] = os;
        }
        os = (COutService<double> *)ob;
        double *d = (double*)dat;;
        os->Update(*d);
        break;
      }
      default:
      {
        break;
      }
    }
  }
}



RateSvc::RateSvc(const std::string& name, ISvcLocator* sl)
: PubSvc(name,sl)
{
  declareProperty("TrendingOn",   m_enableTrending= false);
  declareProperty("ServicePrefix",          m_prefix       = "");
  m_outmap.clear();
  m_trender = 0;//dynamic_cast< ISimpleTrendWriter * >( intf ) ;
  m_oldProf = 0;
}
OUTServiceDescr *RateSvc::findOUTService(std::string servc)
{
  OUTServIter i = m_outputServicemap.find(servc);
  if (i != m_outputServicemap.end())
  {
    OUTServiceDescr *sd = i->second;
    return sd;
  }
  return 0;
}

RateSvc::~RateSvc()
{
}

StatusCode RateSvc::queryInterface(const InterfaceID& riid, void** ppvIF)
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

StatusCode RateSvc::start()
{
  StatusCode sc;
  if (sc.isSuccess() && m_enableTrending && m_trender == 0)
  {
    SmartIF<IToolSvc> tools;
    sc = serviceLocator()->service("ToolSvc", tools.pRef());
    if ( !sc.isSuccess() ) {
      ::lib_rtl_output(LIB_RTL_FATAL,"DIM(RateSvc): Failed to access ToolsSvc.\n");
      return sc;
    }
    sc = tools->retrieveTool("SimpleTrendWriter","RateSvcWriter",m_trender,this);
    if (sc.isSuccess() && m_trender != 0)
    {
      std::string syst = "HLT";
      m_trender->setPartitionAndName(this->m_PartitionName,syst);
      m_trender->setAverageTime(20);
      m_trender->setMaxTimeNoWrite(600);
    }
  }
  PubSvc::start();
  m_adder->SetCycleFn(RATESVC::Analyze,this);
  m_started = true;
  m_NamePrefix = m_PartitionName+"_";
  return StatusCode::SUCCESS;
}

StatusCode RateSvc::initialize()
{
  StatusCode sc = PubSvc::initialize();
  StringReplace(m_prefix,"<part>",m_PartitionName);
  return StatusCode::SUCCESS;
}
StatusCode RateSvc::stop()
{
  StatusCode sc;
  if (m_trender != 0)
  {
    m_trender->close();
    SmartIF<IToolSvc> tools;
    sc = serviceLocator()->service("ToolSvc", tools.pRef());
    if ( !sc.isSuccess() ) {
      ::lib_rtl_output(LIB_RTL_FATAL,"DIM(RateSvc): Failed to access ToolsSvc.\n");
      return sc;
    }
    sc = tools->releaseTool(m_trender);
    m_trender = 0;
  }
  sc = PubSvc::stop();
  return sc;
}
StatusCode RateSvc::finalize()
{
  return StatusCode::SUCCESS;
}
