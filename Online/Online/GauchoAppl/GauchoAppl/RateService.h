#ifndef ONLINE_GAUCHO_RateService_h
#define ONLINE_GAUCHO_RateService_h
#include "dis.hxx"

class RateService : public DimService
{
public:
#define MAX_C 512
  struct DATA
  {
    double d1,d2;
    char c[MAX_C+1];
  };
  DATA m_data;
  RateService (const char *name, const char *format);
  virtual ~RateService();
  void Updater(int dsize);
};

#endif  // ONLINE_GAUCHO_RateService_h
