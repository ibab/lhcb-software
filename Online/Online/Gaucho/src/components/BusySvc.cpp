#include "BusySvc.h"
#include "GaudiKernel/SvcFactory.h"
//#include "GaudiKernel/IIncidentSvc.h"
//#include "GaudiKernel/IHistogramSvc.h"

#include "Gaucho/Utilities.h"
#include "Gaucho/IGauchoMonitorSvc.h"

DECLARE_SERVICE_FACTORY(BusySvc)

void IdleTimer::timerHandler(void)
{
  this->m_bsysvc->calcIdle();
}

IdleTimer::~IdleTimer()
{

}
StatusCode BusySvc::initialize()
{
  Service::initialize();
  StatusCode sc;
  sc = serviceLocator()->service("MonitorSvc", m_pMonitorSvc, true);
  if( sc.isSuccess() )
  {
  }
  else
  {
    return StatusCode::FAILURE;
  }
  if (m_bogus == 0.0)
  {
    getBogus(m_bogus);
  }
  calcIdle();
  m_timer = new IdleTimer(this);
  m_pMonitorSvc->declareInfo("IdleFraction",m_idlebogus,"",this);
  m_pMonitorSvc->declareInfo("BogoMIPS",m_bogus,"",this);
  m_pMonitorSvc->declareInfo("BusyFraction",m_busybogus,"",this);

  return StatusCode::SUCCESS;
}
StatusCode BusySvc::start()
{
  Service::start();
  m_timer->Start();
  return StatusCode::SUCCESS;
}

BusySvc::BusySvc(const std::string& name, ISvcLocator* sl) : Service(name,sl)
{
  declareProperty("BogusMips",          m_bogus       = 0.0);
  m_first = true;
  m_pMonitorSvc = 0;
}

BusySvc::~BusySvc()
{
}

//StatusCode BusySvc::queryInterface(const InterfaceID& riid, void** ppvIF) {
//  return Service::queryInterface(riid, ppvIF);
//  addRef();
//  return StatusCode::SUCCESS;
//}

//StatusCode BusySvc::queryInterface(const InterfaceID& riid, void** ppvIF)
//{
//  if ( IService::interfaceID().versionMatch(riid) )
//  {
//    *ppvIF = (IService*)this;
//  }
//  else
//  {
//    return Service::queryInterface(riid, ppvIF);
//  }
//  addRef();
//  return StatusCode::SUCCESS;
//}
void BusySvc::getBogus(double &bogus)
{
  char line[256];
  float bogo;
  FILE *g = fopen("/proc/cpuinfo","r");
  char *stat = line;
  while (stat != 0)
  {
    stat = fgets(line,sizeof(line),g);
    if (stat == 0) break;
    if (strstr(line,"bogomips")!=0)
    {
      dyn_string *l = Strsplit(line,":");
      sscanf(l->at(1).c_str(), "%f",&bogo);
      break;
    }
  }
  fclose(g);
  bogus = bogo;
//  printf ("Bogomips %f %f\n",bogo,bogus);
  return;
}
void BusySvc::calcIdle()
{
  char line[256];
  long dtimes[7],times[7];
  double p_id;
  FILE *f = fopen("/proc/stat","r");
  char *stat = line;
  while (stat != 0)
  {
    stat = fgets(line,sizeof(line),f);
    if (stat == 0) break;
    if (strstr(line,"cpu ")!=0)
    {
      dyn_string *l = Strsplit(line," ");
      long t=0;
      for (int i=1;i<=7;i++)
      {
        sscanf(l->at(i).c_str(), "%ld",&times[i-1]);
      }
      if (!m_first)
      {
        for (int i=0;i<7;i++)
        {
          dtimes[i] = times[i]-m_times_old[i];
          t +=dtimes[i];
        }
        p_id = double(dtimes[3])/t;
      }
      memcpy(m_times_old,times,sizeof(times));
      delete (l);
      m_first = false;
      break;
    }
  }
  m_idlebogus = p_id*m_bogus;
  m_busybogus = (1.0-p_id)*m_bogus;
//  printf ("Idle Percentage %f weighted %f\n",p_id, m_idlebogus);
  fclose(f);
}
