#include "Gaucho/RateService.h"
#include <vector>
#include "Gaucho/MonSys.h"
//#include "iCCPCHist.h"
#ifdef WIN32
#include "windows.h"
#endif
RateService::RateService()
{
};

RateService::~RateService()
{
};

RateService::RateService (const char *name, char *format, void *buff, int siz) : DimService(name,format,buff,siz)
{

};

RateService::RateService (const char *name, char *format):DimService(name, format, (void*)&m_data, sizeof(m_data))
{
//  setup();
}

void RateService::setup()
{
}

void RateService::Updater(int siz)
{
  updateService((void*)&m_data,siz);
  return;
}
