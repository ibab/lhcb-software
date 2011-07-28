/*
 * COutService.h
 *
 *  Created on: Jul 25, 2011
 *      Author: beat
 */

#ifndef COUTSERVICE_H_
#define COUTSERVICE_H_
#include <string>
#include <map>
#include "dis.hxx"
//class DimService;

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
  COutService(std::string nam);
//  {
//    printf("New Output Service %s of type %s\n",nam.c_str(),typeid(T).name());
//    m_data = -1;
//    m_serv = new DimService(m_nam.c_str(),m_data);
//  }
  void Update(T &dat);
//  {
//    printf("Update Service %s of type %s\n",m_nam.c_str(),typeid(T).name());
//    m_serv->updateService(dat);
//  }
  //COutService(std::string nam);

  ~COutService()
  {
    delete m_serv;
  }
};

//template <> COutService<int>::COutService(const std::string&);

//template <typename T> COutService<T>::COutService(std::string nam):COutServiceBase(nam)
//{
//  printf("New Output Service %s of type %s\n",nam.c_str(),typeid(T).name());
//  m_data = -1;
//  m_serv = new DimService(m_nam.c_str(),m_data);
//}
//template <typename T> void COutService<T>::Update(T &dat)
//{
//  printf("Update Service %s of type %s\n",m_nam.c_str(),typeid(T).name());
//  m_serv->updateService(dat);
//}
//template <> COutService<double>::COutService(std::string nam) : COutServiceBase(nam)
//{
//  m_data = -1;
//  printf("New SPECIAL Output Service %s of type double\n",nam.c_str());
//  m_serv = new DimService(m_nam.c_str(),(float&)m_data);
//}
//
//template <> COutService<long long>::COutService(std::string nam) : COutServiceBase(nam)
//{
//  m_data = -1;
//  printf("New SPECIAL Output Service %s of type long long\n", nam.c_str());
//  m_serv = new DimService(m_nam.c_str(),"I",0,0);
//}
//
//template <> void COutService<double>::Update(double &d)
//{
//  float dat;
//  dat = (float)d;
//  printf("SPECIAL Update Output Service %s of type double\n", m_nam.c_str());
//  m_serv->updateService(dat);
//}
//
//template <> void COutService<long long>::Update(long long &d)
//{
//  printf("SPECIAL Update Output Service %s of type long long\n", m_nam.c_str());
//  unsigned int dat[2];
//  dat[0] = d&0xffffffff;
//  dat[1] = d>>32;
//  m_serv->updateService(dat,sizeof(dat));
//}
//
typedef std::map<std::string,COutServiceBase*> COUTServiceMap;


#endif /* COUTSERVICE_H_ */
