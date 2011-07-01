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
  unsigned long long tim = GetTime();
  unsigned long long delta;
  if (m_prevupdate == 0)
  {
    m_prevupdate = tim;
    return;
  }
  else
  {
    delta = tim - m_prevupdate;
  }
  m_prevupdate = tim;
  makerate(mmap);
}


//void RateSvc::makerate(MonMap* mmap)
//{
//#define MAX_SERVICE_SIZE 128
//  std::string nnam;
//  MonMap::iterator it= mmap->find("COUNTER_TO_RATE");
//  if (it == mmap->end()) return;
//  void *m_RateBuff = it->second;
//  if (m_RateBuff != 0)
//  {
//    DimHistbuff1 *pdiff;
//    DimHistbuff1 *RateBuff=(DimHistbuff1*)m_RateBuff;
//    DimHistbuff1 *oldProf = (DimHistbuff1*)m_oldProf;
//    if (m_oldProf !=0)
//    {
//      int ndble = (RateBuff->nxbin+2);
//      pdiff = (DimHistbuff1*)malloc(RateBuff->reclen);
//      double *obincont  = (double*)AddPtr(AddPtr(oldProf,oldProf->dataoff),0*(oldProf->nxbin+2)*sizeof(double));
//      double *nbincont  = (double*)AddPtr(AddPtr(RateBuff,RateBuff->dataoff),0*(RateBuff->nxbin+2)*sizeof(double));
//      double *dbincont  = (double*)AddPtr(AddPtr(pdiff,RateBuff->dataoff),0*(RateBuff->nxbin+2)*sizeof(double));
//      double *ocont  = (double*)AddPtr(AddPtr(oldProf,oldProf->dataoff),1*(oldProf->nxbin+2)*sizeof(double));
//      double *ncont  = (double*)AddPtr(AddPtr(RateBuff,RateBuff->dataoff),1*(RateBuff->nxbin+2)*sizeof(double));
//      double *dcont  = (double*)AddPtr(AddPtr(pdiff,RateBuff->dataoff),1*(RateBuff->nxbin+2)*sizeof(double));
//      std::vector<char*> labvec;
//      labvec.clear();
//      labvec.push_back(0);
//      {
//        int i;
//        int nbin=oldProf->nxbin;
//        char *labs = (char*)AddPtr(oldProf,oldProf->xlaboff);
//        char *lab = labs;
//        for (i = 1; i < (nbin+1) ; ++i)
//        {
//          labvec.push_back(lab);
//          lab = (char*)AddPtr(lab,strlen(lab)+1);
//        }
//      }
//      m_trender->startEvent();
//      for (int i = 9;i<ndble;i++)
//      {
//        dbincont[i] = nbincont[i]-obincont[i];
//        dcont[i] = ncont[i]-ocont[i];
////            //printf("%d\t%f = %f - %f\n\t%f = %f - %f\n",i,dbincont[i], nbincont[i], obincont[i],dcont[i], ncont[i], ocont[i]);
//      }
//      OUTServiceDescr *outs;
//      std::string nams;
//      int i;
//      int dim;
//      double rate;
//      double denom;
////          //printf("Time Bin %f\n",p->GetBinContent(8));
////          denom  = p->GetBinContent(8)/1.0e09;
////          denom = dbincont[8] == 0 ? 0 : dcont[8]/dbincont[8]/1.0e09;
//      denom = ncont[8]/1.0e09;
//      dim = RateBuff->nxbin;
//      std::string sname;
//      for (i=9;i<dim;i++)
//      {
//        nams  = labvec[i];
//        outs = findOUTService(nams);
//        RateService *s;
//        if (outs != 0)
//        {
//          s = (RateService*)outs->m_svc;
//        }
//        else
//        {
//          DimServer::autoStartOn();
//          sname = m_NamePrefix+nams;
//          if (sname.size() < MAX_SERVICE_SIZE)
//          {
//            s= new RateService(sname.c_str(),(char*)"D:2;C");
//            outs = new OUTServiceDescr((char*)nams.c_str(),(void*)s);
//            this->m_outputServicemap.insert(OUTServicePair(nams,outs));
//          }
//          else
//          {
//            continue;
//          }
//        }
////            rate = dbincont[i] == 0 ? 0 : dcont[i]/dbincont[i]/denom;
//        double c = dcont[i];
//        rate = c/denom;
////            //printf("%d %d %s %f %f\n",dim,i,nams.c_str(), pdiff->GetBinContent(i),rate);
//        s->m_data.d1 = rate;
//        s->m_data.d2 = c;
//        int upsiz = 2*sizeof(s->m_data.d1)+strlen(s->m_data.c);
//
//        s->Updater(upsiz);
//        m_trender->addEntry(nams,c);
//      }
//      m_trender->saveEvent();
//      delete pdiff;
//    }
//    else
//    {
//      m_oldProf = malloc(((DimHistbuff1*)m_RateBuff)->reclen);
//    }
//  memcpy(m_oldProf,m_RateBuff,((DimHistbuff1*)m_RateBuff)->reclen);
//  }
//}

void RateSvc::makerate(MonMap* mmap)
{
#define MAX_SERVICE_SIZE 128
  std::string nnam;
  MonMap::iterator it;
  double rate;
  if (m_enableTrending)
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
      if (m_enableTrending)
      {
        m_trender->addEntry(nams, rate);
      }
    }
  }
  if (m_enableTrending)
  {
    m_trender->saveEvent();
  }
}



RateSvc::RateSvc(const std::string& name, ISvcLocator* sl) : PubSvc(name,sl)
{

  declareProperty("TrendingOn",   m_enableTrending= false);
  // load trending tool
  sl->getService( "ToolSvc" , m_isvc ) ;
//  printf("%x\n",isvc);
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
  PubSvc::start();
  m_adder->SetCycleFn(RATESVC::Analyze,this);
  m_started = true;
  m_NamePrefix = m_PartitionName+"_";
  return StatusCode::SUCCESS;
}
//void RateSvc::handle(const Incident& inc)
//{
//  if (inc.type() == "APP_RUNNING")
//  {
//    m_AdderSys->start();
//  }
//  else if (inc.type() == "APP_STOPPED")
//  {
//    m_AdderSys->stop();
//  }
//}
StatusCode RateSvc::initialize()
{



//  ToolSvc()->retrieveTool ( “SimpleTrendWriter”, m_trender, 0, true   );
//  m_trender=tool<ISimpleTrendWriter>("SimpleTrendWriter");
  PubSvc::initialize();
  std::string syst = "HLT";
  if (m_enableTrending)
  {
    if (m_trender == 0)
    {
      const IInterface *a3( m_isvc ) ;
      const std::string & nam( "SimpleTrendWriter" ) ;
      IAlgTool *intf = ROOT::Reflex::PluginService::Create< IAlgTool *>( nam , nam , nam , a3 ) ;
      m_trender = dynamic_cast< ISimpleTrendWriter * >( intf ) ;
    }
    if (m_trender != 0)
    {
      m_trender->initialize();
      m_trender->setPartitionAndName(this->m_PartitionName,syst);
    }
  }
  return StatusCode::SUCCESS;
}
StatusCode RateSvc::finalize()
{
  if (m_trender != 0)
  {
    m_trender->close();
  }
  return StatusCode::SUCCESS;
}
