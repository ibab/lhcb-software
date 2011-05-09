#ifndef ONLINE_GAUCHO_PUBSVC_H
#define ONLINE_GAUCHO_PUBSVC_H

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IIncidentListener.h"
#include "Gaucho/MonAdder.h"

// Forward declarations
class MonAdder;
class AdderSys;
class SaveTimer;
class IMonitorSvc;
class MyErrh;
class IIncidentSvc;
class IGauchoMonitorSvc;
class IHistogramSvc;
class TellDesc
{
public:
  TellDesc(std::string nam,std::string de){name = nam;det=de;}
  std::string name;
  std::string det;
};

class DetData_T : public std::map<std::string,long>
{
public:
  void Zero()
  {
    DetData_T::iterator i;
    for (i = begin();i!=end();i++)
    {
      i->second = 0;
    }
  }
//  DetData_T operator=(DetData_T&rhs)
//  {
//    DetData_T out;
//    DetData_T::iterator i;
//    for (i = rhs.begin();i!=rhs.end();i++)
//    {
//      out[i->first] = i->second;
//    }
//    return out;
//  }
//  DetData_T operator-(DetData_T&rhs)
//  {
//    DetData_T out;
//    DetData_T::iterator i;
//    for (i = rhs.begin();i!=rhs.end();i++)
//    {
//      out[i->first] = out[i->first]-i->second;
//    }
//    return out;
//  }
};
DetData_T operator-(const DetData_T &a,const DetData_T &b)
{
  DetData_T out;
  DetData_T::const_iterator i,j;
  for (i = b.begin();i!=b.end();i++)
  {
    std::string nam = i->first;
    j = a.find(nam);
    out[nam] = j->second-i->second;
  }
  return out;
}

class DetMap_T : public std::map<std::string,DetData_T>
{
public:
  void dump()
  {
    DetMap_T::iterator i;
    DetData_T::iterator k;
    i = begin();
    printf("Detector ");
    for (k=i->second.begin();k!=i->second.end();k++)
    {
      printf("%12s ",k->first.c_str());
    }
    printf("\n");
    for (i=begin();i!=end();i++)
    {
      printf("%8s ",i->first.c_str());
      for (k=i->second.begin();k!=i->second.end();k++)
      {
        printf("%12li ",k->second);
      }
      printf("\n");
    }
  }
  void Zero()
  {
    DetMap_T::iterator i;
    for (i=begin();i!=end();i++)
    {
      i->second.Zero();
    }
  }
};

DetMap_T operator-(const DetMap_T &a,const DetMap_T &b)
{
  DetMap_T out;
  for (DetMap_T::const_iterator i= b.begin();i!= b.end();i++)
  {
    std::string nam = i->first;
    DetMap_T::const_iterator k = a.find(nam);
    out[nam] = k->second -i->second;
  }
  return out;
}


//typedef std::map<std::string,long> DetData_T;
typedef std::map<int,TellDesc> TellMap_T;
//typedef std::map<std::string,DetData_T> DetMap_T;
class PubSvc : public Service, virtual public IIncidentListener
{
public:
  PubSvc(const std::string& name, ISvcLocator* sl);
  virtual ~PubSvc();
  //IInterface pure virtual member functions
  StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);
  StatusCode initialize();
  StatusCode start();
  StatusCode stop();
  StatusCode finalize();
  virtual void handle(const Incident&);
  TellMap_T m_TellMap;
  DetMap_T m_DetMap;
  DetMap_T m_DetMap_old;
  DetMap_T m_DetMap_diff;

//  DetData_T m_DetData;
//  DetData_T m_DetData_diff;
  void fillTellMap ();
  void analyze(void *buff, int siz,MonMap* mmap);
  void filldata(const std::string &cnam,MonMap *mmap);
  void dump();
private:
  IMonitorSvc        *m_pMonitorSvc;
  IHistogramSvc      *m_phistsvc;
  AIDA::IHistogram1D *m_arrhist;
  AdderSys  *m_AdderSys;
  MonAdder  *m_adder;
  MonAdder  *m_EoRadder;
  SaveTimer *m_SaveTimer;
  SaveTimer *m_EoRSaver;
  int m_recvtmo;
  MyErrh *m_errh;
  /// Reference to the IncidentSvc instance
  IIncidentSvc             *m_incidentSvc;

  std::string m_InputDNS;
  std::string m_SaveRootDir;
  std::string m_PartitionName;
  std::string m_SaverTaskName;
  std::string m_MyName;
  std::string m_TaskPattern;
  std::string m_ServicePattern;
  std::string m_AdderClass;
  std::vector<std::string> m_tell1List;
  bool m_dohisto;
  bool m_expandRate;
  bool m_isSaver;
  bool m_isEOR;
  bool m_started;
  int  m_SaveInterval; //in seconds
};
#endif // ONLINE_GAUCHO_PUBSVC_H
