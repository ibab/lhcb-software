#ifndef ONLINE_GAUCHO_MEPSVC_H
#define ONLINE_GAUCHO_MEPSVC_H

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IIncidentListener.h"
#include "Gaucho/MonAdder.h"
#include "PubSvc.h"

// Forward declarations
class COutServiceBase
{
public:
  std::string m_nam;
  COutServiceBase(const std::string nam)
  {
    m_nam = nam;
  }
};
template <typename T> class COutService : public COutServiceBase
{
public:
  T m_data;
  DimService *m_serv;
  COutService(std::string nam):COutServiceBase(nam)
  {
    m_data = -1;
    m_serv = new DimService(m_nam.c_str(),m_data);
  }
  void Update(T &dat)
  {
    m_serv->updateService(dat);
  }
  ~COutService()
  {
    delete m_serv;
  }
};

template <> COutService<double>::COutService(std::string nam) : COutServiceBase(nam)
{
  m_data = -1;
  printf("New Output Service %s of type double\n",nam.c_str());
  m_serv = new DimService(m_nam.c_str(),(float&)m_data);
}

template <> COutService<long long>::COutService(std::string nam) : COutServiceBase(nam)
{
  m_data = -1;
  printf("New Output Service %s of type long long\n", nam.c_str());
  m_serv = new DimService(m_nam.c_str(),"I",0,0);
}

template <> void COutService<double>::Update(double &d)
{
  float dat;
  dat = (float)d;
  printf("Update Output Service %s of type double\n", m_nam.c_str());
  m_serv->updateService(dat);
}

template <> void COutService<long long>::Update(long long &d)
{
  printf("Update Output Service %s of type long long\n", m_nam.c_str());
  unsigned int dat[2];
  dat[0] = d&0xffffffff;
  dat[1] = d>>32;
  m_serv->updateService(dat,sizeof(dat));
}

typedef std::map<std::string,COutServiceBase*> OUTS;
class GenStatSvc : public PubSvc
{
public:
  std::string m_prefix;
  OUTS m_outmap;
  GenStatSvc(const std::string& name, ISvcLocator* sl);
  virtual ~GenStatSvc();
  //IInterface pure virtual member functions
  StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);
  StatusCode start();
  StatusCode initialize();
//  DetData_T m_DetData;
//  DetData_T m_DetData_diff;
  void analyze(void *buff, int siz,MonMap* mmap);
};
#endif // ONLINE_GAUCHO_MEPSVC_H
