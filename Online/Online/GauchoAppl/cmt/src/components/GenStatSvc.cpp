#include "GenStatSvc.h"
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

DECLARE_SERVICE_FACTORY(GenStatSvc)
namespace GenStatSVC
{
  void Analyze(void *arg, void* buff ,int siz, MonMap *mmap, MonAdder *)
  {
    GenStatSvc *tis = (GenStatSvc*)arg;
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
        ::lib_rtl_output(LIB_RTL_INFO,"DIM(GenStatSvc): Code %d.%x %s\n",severity,code,msg);
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

StatusCode GenStatSvc::initialize()
{
  PubSvc::initialize();
  StringReplace(m_prefix,"<part>",m_PartitionName);
  return StatusCode::SUCCESS;
}
StatusCode GenStatSvc::start()
{
  DimServer::autoStartOn();
  PubSvc::start();
  m_adder->SetCycleFn(GenStatSVC::Analyze,this);
  return StatusCode::SUCCESS;
}
void GenStatSvc::analyze(void *, int ,MonMap* mmap)
{
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
      case   C_RATEFLOAT:
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
      case  C_RATEDOUBLE:
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

GenStatSvc::GenStatSvc(const std::string& name, ISvcLocator* sl) : PubSvc(name,sl)
{
  declareProperty("ServicePrefix",          m_prefix       = "");
  m_outmap.clear();
}

GenStatSvc::~GenStatSvc()
{
  COUTServiceMap::iterator i;
  for (i=m_outmap.begin();i!=m_outmap.end();i++)
  {
    delete i->second;
  }
  m_outmap.clear();
}

StatusCode GenStatSvc::queryInterface(const InterfaceID& riid, void** ppvIF)
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

