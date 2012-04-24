#include "GauchoAppl/RateService.h"

RateService::RateService (const char *name, const char *format):DimService((char*)name, (char*)format, (void*)&m_data, sizeof(m_data))
{
}

RateService::~RateService()
{
}

void RateService::Updater(int siz)
{
  updateService((void*)&m_data,siz);
}
