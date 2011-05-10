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

template <typename T> class DetData_T : public std::map<std::string,T>
{
public:
  static const char* format();
  void Zero()
  {
    typename DetData_T<T>::iterator i;
    for (i = this->begin();i!=this->end();i++)
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
template <> const char* DetData_T<long>::format()  { return "%12li"; }
template <> const char* DetData_T<double>::format()  { return "%12.2f"; }

template <typename T> DetData_T<T> operator-(const DetData_T<T> &a,const DetData_T<T> &b)
{
  DetData_T<T> out;
  typename DetData_T<T>::const_iterator i,j;
  for (i = b.begin();i!=b.end();i++)
  {
    std::string nam = i->first;
    j = a.find(nam);
    out[nam] = j->second-i->second;
  }
  return out;
}

template <typename T>class DetMap_T : public std::map<std::string,DetData_T<T> >
{
public:
  void dump()
  {
    typename DetMap_T<T>::iterator i;
    typename DetData_T<T>::iterator k;
    i = this->begin();
    printf("Detector ");
    for (k=i->second.begin();k!=i->second.end();k++)
    {
      printf("%12s ",k->first.c_str());
    }
    printf("\n");
    for (i=this->begin();i!=this->end();i++)
    {
      printf("%8s ",i->first.c_str());
      for (k=i->second.begin();k!=i->second.end();k++)
      {
        printf(DetData_T<T>::format(),(long)k->second);
      }
      printf("\n");
    }
  }
  void Zero()
  {
    typename DetMap_T::iterator i;
    for (i=this->begin();i!=this->end();i++)
    {
      i->second.Zero();
    }
  }
};

template <typename T> DetMap_T<T> operator-(const DetMap_T<T> &a,const DetMap_T<T> &b)
{
  DetMap_T<T> out;
  typename DetMap_T<T>::const_iterator i;
  for (i= b.begin();i!= b.end();i++)
  {
    std::string nam = i->first;
    typename DetMap_T<T>::const_iterator k = a.find(nam);
    out[nam] = k->second -i->second;
  }
  return out;
}
DetMap_T<double> operator/(const DetMap_T<long> &a,long l)
{
  DetMap_T<double> out;
  DetData_T<double>::iterator k;
  DetData_T<long>::iterator ii;
  DetMap_T<double>::iterator j;
  DetMap_T<long>::const_iterator i;
  for (i = a.begin();i!=a.end();i++)
  {
    j = out.find(i->first);
    if (j != out.end())
    {
      DetData_T<long> q=i->second;
//      DetData_T<double> oo;
      for (ii=q.begin();ii!=q.end();ii++)
      {
        k = j->second.find(ii->first);
        if (k != j->second.end())
        {
          long a = ii->second;
          double o;
          o = double(a)/l;
          o = o*1.0e9;
          k->second = o;
        }
      }
    }
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
  DetMap_T<long> m_DetMap;
  DetMap_T<long> m_DetMap_old;
  DetMap_T<long> m_DetMap_diff;

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
  unsigned long long m_prevupdate;
};
#endif // ONLINE_GAUCHO_PUBSVC_H
