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

typedef std::map<std::string,COutServiceBase*> COUTServiceMap;


#endif /* COUTSERVICE_H_ */
