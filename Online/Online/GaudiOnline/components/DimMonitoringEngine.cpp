#include "GaudiKernel/Service.h"
#define NO_LONGLONG_TYPEDEF
#include "dis.hxx"
#include "RTL/rtl.h"
#include "GaudiOnline/MemMonitorSvc.h"

using namespace LHCb;

namespace LHCb {
  class DimMonitoringEngine : public MonitoringEngine {
    typedef std::pair<int,DimService*> Item;
    typedef std::map<std::string,Item> Services;
    Services                           m_services;
    void revoke(Services::iterator i);
  protected:
    /// Publish single monitoring item identified by owner and name
    virtual void i_publishItem(CSTR owner_name,CSTR nam,CSTR dsc,int typ,const void* var);
  public:
    /// Service constructor
    DimMonitoringEngine(CSTR nam,ISvcLocator* svc) : MonitoringEngine(nam,svc) {}
    /// Standard destructor
    virtual ~DimMonitoringEngine()  {}
    /// Unpublish single monitoring item identified by owner and name
    virtual void revokeItem(CSTR owner_name,CSTR nam);
  };
  class DimHistogram : public ::DimService, public MonitoringEngine::Histogram  {
  protected:
    ::DimService* m_service;
  public:
    DimHistogram(const std::string& nam,IMessageSvc* msg,AIDA::IHistogram* h);
    virtual ~DimHistogram()         {      delete m_service;    }
    virtual void serviceHandler();
  };
}

DimHistogram::DimHistogram(const std::string& nam,IMessageSvc* msg,AIDA::IHistogram* h)
: DimService(), Histogram(nam,msg,h), m_service(0)
{
  m_service = new DimService((char*)nam.c_str(),(char*)"F",&m_data[0],m_data.size()*sizeof(Buffer::value_type));
}

void DimHistogram::serviceHandler()  {
  filldata();
  setData(&m_data.front(),m_data.size()*sizeof(Buffer::value_type));
}

void DimMonitoringEngine::revoke(Services::iterator i)  {
  if ( i != m_services.end() )  {
    if ( (*i).second.second ) delete (*i).second.second;
    m_services.erase(i);
  }
}

void DimMonitoringEngine::i_publishItem(CSTR owner_name, CSTR nam, CSTR dsc, int typ, const void* var)  {
  DataPoint p(var);
  std::string m;
  std::string n = owner_name+"/"+nam;
  std::string nn = RTL::processName()+"/"+ n;
  std::string d = n+"/gauchocomment";
  std::string dd = RTL::processName()+"/"+ d;
  m_services[d] = Item(INTEGER,new DimService(dd.c_str(),(char*)dsc.c_str()));
  switch(typ)  {
    case BOOLEAN:
      m_services[n] = Item(typ,new DimService(nn.c_str(),(char*)"C:1",(void*)var,sizeof(bool)));
      return;
    case INTEGER:
      m_services[n] = Item(typ,new DimService(nn.c_str(),*p.INT));
      return;
    case LONG_INTEGER:
      m_services[n] = Item(typ,new DimService(nn.c_str(),*p.INT));
      return;
    case FLOAT:
      m_services[n] = Item(typ,new DimService(nn.c_str(),*p.FLOAT));
      return;
    case DOUBLE:
      m_services[n] = Item(typ,new DimService(nn.c_str(),*p.DOUBLE));
      return;
    case NTSTRING:
      m_services[n] = Item(typ,new DimService(nn.c_str(),p.NTSTRING));
      return;
    case STRING:
      m_services[n] = Item(typ,new DimService(nn.c_str(),(char*)p.STRING->c_str()));
      return;
    case DOUBLE_PAIR:
      m_services[n] = Item(typ,0);
      m = nn+"/1";
      m_services[n+"/1"] = Item(typ,new DimService(m.c_str(),p.PAIR->first));
      m = nn+"/2";
      m_services[n+"/1"] = Item(typ,new DimService(m.c_str(),p.PAIR->second));
      return;
    case HISTOGRAM:  // Don't know (yet) what to do
      m_services[n] = Item(typ,new DimHistogram(nn.c_str(),msgSvc(),p.HIST));
      return;
    default:
      return;
  }
}

void DimMonitoringEngine::revokeItem(CSTR owner_name, CSTR nam)  {
  std::string n = owner_name+"/"+nam;
  revoke(m_services.find(n+"/gauchocomment"));
  Services::iterator i = m_services.find(n);
  if ( i != m_services.end() )  {
    switch((*i).second.first)  {
      case DOUBLE_PAIR:
        revoke(m_services.find(n+"/1"));
        revoke(m_services.find(n+"/2"));
      case BOOLEAN:
      case INTEGER:
      case LONG_INTEGER:
      case FLOAT:
      case DOUBLE:
      case NTSTRING:
      case STRING:
        revoke(i);
        return;
      case HISTOGRAM:  // Don't know (yet) what to do
        return;
      default:
        return;
    }
  }
}

#include "GaudiKernel/SvcFactory.h"
// Factory for instantiation of service objects
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,DimMonitoringEngine)
