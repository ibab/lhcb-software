#ifndef RateService_h
#define RateService_h
#include "dis.hxx"

class RateService : public DimService
{
public:
  //void *p;
#define MAX_C 512
  struct DATA
  {
    double d1,d2;
    char c[MAX_C+1];
  };
  DATA m_data;
  RateService ();
  virtual ~RateService();
  RateService (const char *name, char *format, void *buff, int siz);
  RateService (const char *name, char *format);
  void Updater(int dsize);
  void setup(void);
};
//class RateServiceDescr
//{
//public:
//  std::string m_serviceName;
//  RateService *m_svc;
//  RateServiceDescr(char *name, char *tname)
//  {
//    m_serviceName = tname;
//    m_serviceName += "/";
//    m_serviceName += name;
//    last_update = 0;
////    printf("Output Service Descriptor: %s\n",m_serviceName.c_str());
//   m_svc = new RateService(m_serviceName.c_str(),(char*)"D:2;C",&empty,4,&m_buffer,&m_buffersize);
//  }
//  ~RateServiceDescr()
//  {
//    delete m_svc;
//  }
//};
#endif
