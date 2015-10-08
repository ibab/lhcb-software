#include "BusySvc.h"
#include "GaudiKernel/SvcFactory.h"
//#include "GaudiKernel/IIncidentSvc.h"
//#include "GaudiKernel/IHistogramSvc.h"

#include "Gaucho/Utilities.h"
#include "Gaucho/IGauchoMonitorSvc.h"

DECLARE_SERVICE_FACTORY(BusySvc)

void IdleTimer::timerHandler(void)
{
  m_bsysvc->calcIdle();
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
    getBogus(m_Mybogus);
  }
  m_bogus = m_Mybogus;
  m_numCores = m_NumCores;
  calcIdle();
  m_timer = new IdleTimer(this);
  m_pMonitorSvc->declareInfo("IdleFraction",m_idlebogus,"",this);
  m_pMonitorSvc->declareInfo("BogoMIPS",m_bogus,"",this);
  m_pMonitorSvc->declareInfo("BusyFraction",m_busybogus,"",this);
  m_pMonitorSvc->declareInfo("NumCores",m_numCores,"",this);

  m_pMonitorSvc->declareInfo("TotMemory",m_Memtot,"",this);
  m_pMonitorSvc->declareInfo("FreeMemory",m_Memfree,"",this);
  m_pMonitorSvc->declareInfo("MemBuffers",m_Membuff,"",this);
  m_pMonitorSvc->declareInfo("SwapSpaceTot",m_MemSwaptot,"",this);
  m_pMonitorSvc->declareInfo("SwapSpaceFree",m_MemSwapfree,"",this);
  return StatusCode::SUCCESS;
}
StatusCode BusySvc::start()
{
  Service::start();
  m_bogus = m_Mybogus;
  m_timer->Start();
  return StatusCode::SUCCESS;
}

BusySvc::BusySvc(const std::string& name, ISvcLocator* sl) : Service(name,sl)
{

  m_idlebogus=0.0;
  m_busybogus=0.0;
  m_bogus=0.0;
  m_Mybogus=0.0;
  m_NumCores = 0;
  m_numCores = 0;
  m_timer = 0;
  m_Memtot = 0;
  m_Memfree = 0;
  m_Membuff = 0;
  m_MemSwaptot = 0;
  m_MemSwapfree = 0;
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
  char bogoline[256];
  char modelnameline[256];
  float bogo;
  int model;
  int proc = 0;
  bogus = 0;
  FILE *g = fopen("/proc/cpuinfo", "r");
  char *stat = line;
  while (stat != 0)
  {
    stat = fgets(line, sizeof(line), g);
    if (stat == 0)
      break;
    if (strstr(line, "processor\t") != 0)
    {
      strcpy(modelnameline, line);
      dyn_string *l = Strsplit(line, ":");
      sscanf(l->at(1).c_str(), "%d", &proc);
      m_NumCores++;
      continue;
    }
    else if (strstr(line, "bogomips\t") != 0)
    {
      strcpy(bogoline, line);
      dyn_string *l = Strsplit(line, ":");
      sscanf(l->at(1).c_str(), "%f", &bogo);
      bogus += bogo;
      continue;
    }
    else if (strstr(line, "model\t") != 0)
    {
      strcpy(modelnameline, line);
      dyn_string *l = Strsplit(line, ":");
      sscanf(l->at(1).c_str(), "%d", &model);
      continue;
    }
  }
  fclose(g);
  ::lib_rtl_output(LIB_RTL_INFO,"Number of Cores: %d.\n",m_NumCores);
//  if (model == 23)
//  {
//    bogus = 270.0;
//  }
//  else if (model == 44)
//  {
//    bogus = 960.0;
//  }
//  else if (model == 1)
//  {
//    bogus = 960.0;
//  }
//  bogus = bogo;
  //  printf ("Bogomips %f %f\n",bogo,bogus);
  return;
}
void BusySvc::calcIdle()
{
  char line[256];
  long dtimes[7],times[7];
  double p_id=0.0;
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
  m_bogus = m_Mybogus;
  m_idlebogus = p_id*m_bogus;
  m_busybogus = (1.0-p_id)*m_bogus;
  m_numCores = m_NumCores;
//  printf ("Idle Percentage %f weighted %f\n",p_id, m_idlebogus);
  fclose(f);
//
//
//
//
//
  f = fopen("/proc/meminfo","r");
  stat = line;
  while (stat != 0)
  {
    std::string tok;
    stat = fgets(line,sizeof(line),f);
    if (stat == 0) break;
    dyn_string *toks = Strsplit(line," ");
    tok = toks->at(0);
    if (toks->at(0).find("MemTotal") != std::string::npos)
    {
      tok = toks->at(1);
      m_Memtot = std::stol(toks->at(1));
      long fact=1;
      if (toks->size()>2)
      {
        tok = toks->at(2);
        if (toks->at(2) == "kB")
        {
          fact = 1000;
        }
        else if (toks->at(2) == "B")
        {
          fact = 1;
        }
        else if (toks->at(2) == "MB")
        {
          fact = 1000000;
        }
      }
      m_Memtot *= fact;
      delete toks;
      continue;
    }
    tok = toks->at(0);
    if (toks->at(0).find("MemFree") != std::string::npos)
    {
      tok = toks->at(1);
      m_Memfree = std::stol(toks->at(1));
      long fact=1;
      if (toks->size()>2)
      {
        tok = toks->at(2);
        if (toks->at(2) == "kB")
        {
          fact = 1000;
        }
        else if (toks->at(2) == "B")
        {
          fact = 1;
        }
        else if (toks->at(2) == "MB")
        {
          fact = 1000000;
        }
      }
      m_Memfree *= fact;
      delete toks;
      continue;
    }
    tok = toks->at(0);
    if (toks->at(0).find("Buffers") != std::string::npos)
    {
      tok = toks->at(1);
      m_Membuff = std::stol(toks->at(1));
      long fact=1;
      if (toks->size()>2)
      {
        tok = toks->at(2);
        if (toks->at(2) == "kB")
        {
          fact = 1000;
        }
        else if (toks->at(2) == "B")
        {
          fact = 1;
        }
        else if (toks->at(2) == "MB")
        {
          fact = 1000000;
        }
      }
      m_Membuff *= fact;
      delete toks;
      continue;
    }
    tok = toks->at(0);
    if (toks->at(0).find("SwapTotal") != std::string::npos)
    {
      tok = toks->at(1);
      m_MemSwaptot = std::stol(toks->at(1));
      long fact=1;
      if (toks->size()>2)
      {
        tok = toks->at(2);
        if (toks->at(2) == "kB")
        {
          fact = 1000;
        }
        else if (toks->at(2) == "B")
        {
          fact = 1;
        }
        else if (toks->at(2) == "MB")
        {
          fact = 1000000;
        }
      }
      m_MemSwaptot *= fact;
      delete toks;
      continue;
    }
    tok = toks->at(0);
    if (toks->at(0).find("SwapFree") != std::string::npos)
    {
      tok = toks->at(1);
      m_MemSwapfree = std::stol(toks->at(1));
      long fact=1;
      if (toks->size()>2)
      {
        tok = toks->at(2);
        if (toks->at(2) == "kB")
        {
          fact = 1000;
        }
        else if (toks->at(2) == "B")
        {
          fact = 1;
        }
        else if (toks->at(2) == "MB")
        {
          fact = 1000000;
        }
      }
      m_MemSwapfree *= fact;
      delete toks;
      continue;
    }
    delete toks;
  }
  fclose(f);
}
