#ifndef ONLINE_GAUCHO_MEPSVC_H
#define ONLINE_GAUCHO_MEPSVC_H

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GauchoAppl/MonAdder.h"
#include "PubSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include <vector>
// Forward declarations
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
  DetData_T()
  {
    m_buffer = 0;
    m_service = 0;
    m_TBuffer.clear();
    dns = 0;
  }
  int bsiz();
  DimServerDns *dns;
  std::string m_servicename;
  DimService *m_service;
  std::vector<T> m_TBuffer;
  void *m_buffer;
  int m_bsiz;
  void Update();
  void FillDim(void *buff,int &siz);
  DimService* makeService(std::string nam);
  static const char* format();
  void setDNS(DimServerDns *d)
  {
    dns = d;
  }
//  DetData_T<double> operator=(DetData_T<long long >&a);
  void Zero()
  {
    typename DetData_T<T>::iterator i;
    for (i = this->begin();i!=this->end();i++)
    {
      i->second = 0;
    }
  }
  void NegSuppress()
  {
    typename DetData_T<T>::iterator i;
    for (i = this->begin();i!=this->end();i++)
    {
      if (i->second < 0)
      {
        i->second = 0;
      }
    }
  }
};
template <> void DetData_T<long long>::FillDim(void *buff, int &siz)
{
  size_t i;
  int indx=0;
  unsigned int *b = (unsigned int*)buff;
  for (i=0;i<m_TBuffer.size();i++)
  {
    b[indx++] = (unsigned int)(m_TBuffer[i] & 0xffffffff);
    b[indx++] = (unsigned int)(m_TBuffer[i]>>32);
  }
  siz = 2*m_TBuffer.size()*sizeof(unsigned int);
}

template <> int DetData_T<double>::bsiz()
{
  return m_TBuffer.size()*sizeof(float);
}
template <> int DetData_T<long long>::bsiz()
{
  return 2*m_TBuffer.size()*sizeof(unsigned int);
}

template <> void DetData_T<double>::FillDim(void *buff, int &siz)
{
  size_t i;
  int indx=0;
  float *b = (float*)buff;
  for (i=0;i<m_TBuffer.size();i++)
  {
    b[indx++] = (float)m_TBuffer[i];
  }
  siz = m_TBuffer.size()*sizeof(float);
}

template <> DimService* DetData_T<long long>::makeService(std::string nam)
{
  std::string snam = nam;
  if (dns == 0)
  {
    DimServer::autoStartOn();
    return new DimService((char*)snam.c_str(),"I",0,0);
  }
  else
  {
    dns->autoStartOn();
    return new DimService(dns,(char*)snam.c_str(),"I",0,0);
  }
}
template <> DimService* DetData_T<double>::makeService(std::string nam)
{
  std::string snam = nam;
  if (dns == 0)
  {
    DimServer::autoStartOn();
    return new DimService((char*)snam.c_str(),"F",0,0);
  }
  else
  {
    dns->autoStartOn();
    return new DimService(dns,(char*)snam.c_str(),"F",0,0);
  }
}

template <> void DetData_T<long long>::Update()
{
  int nel = this->size();
  m_bsiz = 2*nel*sizeof(unsigned int);
  void *m_buffer = malloc(m_bsiz);
  int *buffer = (int*)m_buffer;
  DetData_T<long long>::iterator i;
  int indx=0;
  for (i=begin();i!= end();i++)
  {
    buffer[indx++] = (unsigned int)(i->second &0xffffffff);
    buffer[indx++] = (unsigned int)(i->second >> 32);
  }
//  makeService();
  m_service->updateService(buffer,2*nel*sizeof(unsigned int));
}
template <> void DetData_T<double>::Update()
{
  int nel = this->size();
  m_bsiz = nel*sizeof(float);
  void *m_buffer = malloc(m_bsiz);
  float *buffer = (float*)m_buffer;
  DetData_T<double>::iterator i;
  int indx=0;
  for (i=begin();i!= end();i++)
  {
    buffer[indx++] = (float)i->second;;
  }
//  makeService();
  m_service->updateService(buffer,2*nel*sizeof(unsigned int));

}
template <> const char* DetData_T<long long>::format()  { return "%12li "; }
template <> const char* DetData_T<double>::format()  { return "%12.0f "; }

void convert(const DetData_T<long long> &a, DetData_T<double> &b)
{
  DetData_T<long long>::const_iterator i;
  b.clear();
  for (i=a.begin();i!=a.end();i++)
  {
    b[i->first] = (double)i->second;
  }
}

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
typedef DetMap_T<T> MyType;
public:
  DetMap_T()
  {
    m_buffer = 0;
    m_service = 0;
    dns = 0;
    m_bsiz = 0;
  }
  std::map<std::string,DimService*> m_svcmap;
  std::string m_servicename;
  DimService *m_service;
  char *m_buffer;
  int m_bsiz;
  DimServerDns *dns;
  void divide(DetMap_T<double> &b, long long l); // b=this/l
  void setServiceName(std::string n)
  {
    m_servicename = n;
    typename DetMap_T<T>::iterator i;
    for (i=this->begin();i!=this->end();i++)
    {
      i->second.m_servicename = n;
    }
  }
  void setDNS(DimServerDns *d)
  {
    dns = d;
  }
  void Update();
//  {
//    typename MyType::const_iterator i;
//    m_bsiz = 0;
//
//    for (i=this->begin();i!=this->end();i++)
//    {
//      m_bsiz += (i->first.size());
//      m_bsiz++;
//    }
//  }
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
      if (i->first == "LHCb") continue;
      printf("%8s ",i->first.c_str());
      for (k=i->second.begin();k!=i->second.end();k++)
      {
        printf(DetData_T<T>::format(),k->second);
      }
      printf("\n");
    }

    i = this->find("LHCb");
    printf("%8s ",i->first.c_str());
    for (k=i->second.begin();k!=i->second.end();k++)
    {
      printf(DetData_T<T>::format(),k->second);
    }
    printf("\n");
  }
  void FillDim()
  {
    typename DetMap_T<T>::iterator i;
    typename DetMap_T<T>::iterator ii;
    typename DetData_T<T>::iterator k;
    typename DetData_T<T>::iterator kk;
    ii = this->begin();
    for (k=ii->second.begin();k!=ii->second.end();k++)
    {
      DetData_T<T> &q=ii->second;
      q.m_TBuffer.clear();
      for (i=this->begin();i!=this->end();i++)
      {
        kk = i->second.find(k->first);
        q.m_TBuffer.push_back(kk->second);
      }
      int siz = q.bsiz();
      void *ptr= malloc(siz);
      q.FillDim(ptr,siz);
      m_svcmap[k->first]->updateService(ptr,siz);
      free(ptr);
    }
  }
  void NegSuppress()
  {
    typename DetMap_T::iterator i;
    for (i=this->begin();i!=this->end();i++)
    {
      i->second.NegSuppress();
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
template <typename T> void DetMap_T<T>::Update()
{
  typename MyType::iterator i;
  int bsiz = 0;

  for (i=this->begin();i!=this->end();i++)
  {
    bsiz += (i->first.size());
    bsiz++;
  }
  if (m_buffer == 0)
  {
    m_bsiz = bsiz;
    m_buffer = (char*)malloc(m_bsiz);
  }
  int indx = 0;
  for (i=this->begin();i!=this->end();i++)
  {
    strcpy(&m_buffer[indx],i->first.c_str());
    indx += i->first.size();
    m_buffer[indx] = 0;
    indx++;
  }
  if(m_service == 0)
  {
    if (dns == 0)
    {
      DimServer::autoStartOn();
      m_service = new DimService((char*)(m_servicename+"/Detector").c_str(),"C",m_buffer,m_bsiz);
    }
    else
    {
      dns->autoStartOn();
      m_service = new DimService(dns,(char*)(m_servicename+"/Detector").c_str(),"C",m_buffer,m_bsiz);
    }
    i = this->begin();
    typename DetData_T<T>::iterator k;
    for (k=i->second.begin();k!=i->second.end();k++)
    {
//      printf("%s\n",k->first.c_str());
      i->second.setDNS(dns);
      m_svcmap[k->first] = i->second.makeService(m_servicename+"/"+k->first);
  //    i->second.makeService(k->first);
    }
//    for (i=this->begin();i!=this->end();i++)
//    {
//      i->second
//    }
  }
  m_service->updateService(m_buffer,m_bsiz);
  FillDim();
//  i->second.Update();
}

void convert(const DetMap_T<long long> &a, DetMap_T<double> &b)
{
  b.clear();
  DetMap_T<long long>::const_iterator i;
  for (i=a.begin();i!=a.end();i++)
  {
    convert(i->second,b[i->first]);
  }
}

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

template <typename T> void DetMap_T<T>::divide(DetMap_T<double> &b, long long l)
{
  DetData_T<double>::iterator k;
  DetData_T<long long>::iterator ii;
  DetMap_T<double>::iterator j;
  DetMap_T<long long>::const_iterator i;
  convert(*this,b);
  for (i = this->begin();i!=this->end();i++)
  {
      DetData_T<long long> q=i->second;
//      DetData_T<double> oo;
      for (ii=q.begin();ii!=q.end();ii++)
      {
          long long a = ii->second;
          double o;
          o = ((double)a)/l;
          o = o*1.0e9;
          b[i->first][ii->first]=o;
      }
  }
//  out.dump();
//  return *this;
}

//typedef std::map<std::string,long long> DetData_T;
typedef std::map<int,TellDesc> TellMap_T;
//typedef std::map<std::string,DetData_T> DetMap_T;
//class ISimpleTrendWriter;

class TellService
{
  public:
    char *m_tell1names;
    int m_names_size;
    DimService *m_DimService;
    TellService(std::string &servname, std::vector<std::string> &tell1s, DimServerDns *dns=0)
    {
      int bsiz=0;
      size_t i;
      for (i = 0; i < tell1s.size(); i++)
      {
        bsiz += tell1s[i].size();
        bsiz++;
      }
      m_names_size = bsiz;
      m_tell1names = (char*) ::malloc(m_names_size);
      int indx = 0;
      for (i = 0; i<tell1s.size(); i++)
      {
        ::strcpy(&m_tell1names[indx], tell1s[i].c_str());
        indx += tell1s[i].size();
        m_tell1names[indx] = 0;
        indx++;
      }
      if (dns)
      {
        dns->autoStartOn();
        m_DimService = new DimService(dns,servname.c_str(), "C", m_tell1names, m_names_size);
      }
      else
      {
        DimServer::autoStartOn();
        m_DimService = new DimService(servname.c_str(), "C", m_tell1names, m_names_size);
      }
    };
    void Update()
    {;
      m_DimService->updateService();
    }

};
class Tell1Stats
{
  public:
    float *m_LossRate;
    unsigned int *m_LossCount;
    int m_nents;
    DimService *m_CSvc;
    std::string m_CName;
    DimService *m_RSvc;
    std::string m_RName;
    std::vector<std::string> compList;
    Tell1Stats(std::vector<std::string> &TellList);
    void fillBuffers(MonMap *mmap);
    void makeServices(std::string prefix,DimServerDns *dns=0);
    void Update();
};
class MEPSvc : public PubSvc
{
public:
  MEPSvc(const std::string& name, ISvcLocator* sl);
  virtual ~MEPSvc();
  //IInterface pure virtual member functions
  StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);
  TellMap_T m_TellMap;
  TellService *m_Tell1Service;
  std::vector<std::string> m_TellNames;
  std::string m_TellStrings;
  DetMap_T<long long> m_DetMap;
  DetMap_T<long long> m_DetMap_old;
  DetMap_T<long long> m_DetMap_diff;
  DetMap_T<double> m_DetMap_rate;
  DimService *m_LHCbDataRate;
  DimService *m_MEPTotRateSvc;
  DimService *m_MEPDefRateSvc;
  Tell1Stats *m_Tell1Stats;
  float m_DataRate;
  float m_MEPRate;
  float m_MEPDeferred;
  bool m_enableTrending;
  std::string m_TrendPrefix;
  StatusCode initialize();
  StatusCode start();
//  DetData_T m_DetData;
//  DetData_T m_DetData_diff;
  void fillTellMap ();
  void analyze(void *buff, int siz,MonMap* mmap);
  void filldata(const std::string &cnam,MonMap *mmap);
  void filldata(const std::string &prefix,const std::string &cnam,MonMap *mmap);
  void fillrate(const std::string &prefix,const std::string &cnam,MonMap *mmap);
  void dump();
  void fillsums();
  StatusCode finalize();
  std::vector<std::string> m_tell1List;
  unsigned long long m_prevupdate;
//  ISimpleTrendWriter* m_trender;
};
#endif // ONLINE_GAUCHO_MEPSVC_H
