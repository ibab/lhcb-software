#include "FileTestSvc.h"
#include "GaudiKernel/SvcFactory.h"
//#include "GaudiKernel/IIncidentSvc.h"
//#include "GaudiKernel/IHistogramSvc.h"

//#include "GauchoAppl/HistAdder.h"
//#include "GauchoAppl/CounterAdder.h"
//#include "GauchoAppl/SaveTimer.h"
//#include "GauchoAppl/AdderSys.h"
//#include "Gaucho/Utilities.h"
//#include "Gaucho/IGauchoMonitorSvc.h"
//#include "Gaucho/MonCounter.h"
//#include "GauchoAppl/RateService.h"
//#include "Trending/ITrendingTool.h"
//#include "Trending/ISimpleTrendWriter.h"
#include "stdio.h"
DECLARE_SERVICE_FACTORY(FileTestSvc)






FileTestSvc::FileTestSvc(const std::string& name, ISvcLocator* sl)
: Service(name,sl)
{
  declareProperty("FileName",   m_InputFile= "");
}

FileTestSvc::~FileTestSvc()
{
}

StatusCode FileTestSvc::queryInterface(const InterfaceID& riid, void** ppvIF)
{
  return Service::queryInterface(riid, ppvIF);
}

StatusCode FileTestSvc::start()
{
  StatusCode sc;
  sc = Service::start();
  char line[4096];
//  printf("File %s opening\n",m_InputFile.c_str());
  FILE *f=::fopen(m_InputFile.c_str(),"r");
  if (f!=0)
  {
//    printf("File %s opened successfully\n",m_InputFile.c_str());
  }
  else
  {
    printf("File %s opening failed\n",m_InputFile.c_str());
    return StatusCode::FAILURE;
  }
  while (!feof(f))
  {
    fgets(line,sizeof(line),f);
  }
//  printf("File read and being closed\n");
  fclose(f);
  return StatusCode::SUCCESS;
}

StatusCode FileTestSvc::initialize()
{
  StatusCode sc = Service::initialize();
  return StatusCode::SUCCESS;
}
StatusCode FileTestSvc::stop()
{
  return StatusCode::SUCCESS;
}
StatusCode FileTestSvc::finalize()
{
  return StatusCode::SUCCESS;
}
