#ifndef GAUDIONLINE_DUMMYMONITORSVC_H
#define GAUDIONLINE_DUMMYMONITORSVC_H

// Framework include files
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IMonitorSvc.h"
#include <map>

// Forward declarations
class StatEntity;
namespace AIDA {
  class IHistogram;
  class IBaseHistogram;
}

/*
 *   LHCb namespace declaration
 */
namespace LHCb  {

  /** @class DummyMonitorSvc DummyMonitorSvc.h GaudiOnline/DummyMonitorSvc.h
    *
    *  This class implements the IMonitorSvc interface, and publishes Gaudi variables
    *  into a memory area.
    *
    *  @author Markus Frank
    */
  class DummyMonitorSvc : public Service, virtual public IMonitorSvc {
    typedef const std::string& CSTR;
    typedef const IInterface*  Client;
    struct Items : public std::map<std::string,std::string> {
      std::string owner;
      Items() {}
      Items(CSTR o) : std::map<std::string,std::string>(), owner(o) {}
      Items(const Items& c) : std::map<std::string,std::string>(c), owner(c.owner) {}
    };
    typedef std::map<Client,Items*> ClientMap;
    typedef std::pair<ClientMap::iterator,Items::iterator> RegInfo;

    /// Internal helper to access the client name
    std::string clientName(Client owner);
    /// Internal helper to register new clients (or return existing ones)
    ClientMap::iterator regClient(Client c);
    /// Internal helper to declare monitoring items
    void regItem(CSTR item_name,const void* ptr, CSTR dscription,Client owner);

  public:
    /// Service constructor
    DummyMonitorSvc(CSTR name, ISvcLocator* svcLoc) : Service(name, svcLoc) {}
    /// Standard destructor
    virtual ~DummyMonitorSvc() {}
    /// IInterface pure virtual member functions
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);
    /// Initialize service
    virtual StatusCode initialize();
    /// Finalize service
    virtual StatusCode finalize();
    
#define _DECL(t)  virtual void declareInfo(CSTR n,t& v,CSTR d,Client c) {regItem(n,&v,d,c); }
#define _DECLP(t) virtual void declareInfo(CSTR n,t  v,CSTR d,Client c) {regItem(n,v,d,c);  }
    typedef std::pair<double,double> Rate;
    _DECL(bool)
      _DECL(const bool)
      _DECL(const int)
      _DECL(const long)
      _DECL(const double)
      _DECL(const std::string)
      _DECL(const Rate)
      _DECL(int)
      _DECL(long)
      _DECL(double)
      _DECL(Rate)
      _DECLP(AIDA::IHistogram*)
      _DECLP(const AIDA::IBaseHistogram*)
      _DECL(const StatEntity)

      virtual void declareInfo(CSTR n,CSTR /*format*/,const void* v,int /*size*/,CSTR d,Client c) {regItem(n,v,d,c);}

    /** Undeclare monitoring information
      * @param name Monitoring information name knwon to the external system
      * @param owner Owner identifier of the monitoring information
      */
    virtual void undeclareInfo(CSTR nam, Client owner);
    
    /** Undeclare monitoring information
      * @param owner Owner identifier of the monitoring information
      */
    virtual void undeclareAll(Client owner) ;
    
    /** Get the names for all declared monitoring informations for a given
      * owner. If the owner is NULL, then it returns for all owners
      */
    virtual std::set<std::string>* getInfos(Client owner = 0);

  private:
    /// Map associating to each algorithm name a set with the info items
    ClientMap         m_clients;
  };
}      // End namespace gaudi
#endif // GAUDIONLINE_DUMMYMONITORSVC_H

// Include files
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"

using namespace LHCb;
using namespace std;

// Factory for instantiation of service objects
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,DummyMonitorSvc)

StatusCode DummyMonitorSvc::queryInterface(const InterfaceID& riid, void** ppvIF) {
  if(IMonitorSvc::interfaceID() == riid) {
    *ppvIF = dynamic_cast<IMonitorSvc*> (this);
    addRef();
    return StatusCode::SUCCESS;
  } 
  return Service::queryInterface(riid, ppvIF);
}

StatusCode DummyMonitorSvc::initialize() {
  StatusCode sc = Service::initialize(); 
  MsgStream msg(msgSvc(),name());
  if ( !sc.isSuccess() )  {
    msg << MSG::ERROR << "Cannot initialize base class." << endmsg;
    return sc;
  }
  undeclareAll(0);
  msg << MSG::INFO << " MonitorSvc of type DummyMonitorSvc initialized." << endmsg;
  return sc; 
}

StatusCode DummyMonitorSvc::finalize() {
  undeclareAll(0);
  return Service::finalize();
}

DummyMonitorSvc::ClientMap::iterator DummyMonitorSvc::regClient(Client c) {
  ClientMap::iterator i = m_clients.find(c);
  return (i != m_clients.end()) ? i : m_clients.insert(make_pair(c,new Items(clientName(c)))).first;
}

void DummyMonitorSvc::regItem(CSTR nam, const void*, CSTR dsc, Client c) {
  ClientMap::iterator i = regClient(c);
  if ( i != m_clients.end() )  {
    (*i).second->insert(make_pair(nam,dsc));
  }
}

void DummyMonitorSvc::undeclareInfo(CSTR nam, Client owner)  {
  ClientMap::iterator i = m_clients.find(owner);
  if( i != m_clients.end() )  {
    Items::iterator j = (*i).second->find(nam);
    if ( j != (*i).second->end() )  {
      (*i).second->erase(j);
    }
  }
}

void DummyMonitorSvc::undeclareAll(Client c)    {
  ClientMap::iterator i = (0==c) ? m_clients.begin() : m_clients.find(c);
  if ( c && i != m_clients.end() )  {
    delete (*i).second;
    m_clients.erase(i);
    return;
  }
  for( ; i != m_clients.end(); ++i)
    delete (*i).second;
  m_clients.clear();
}

set<string>* DummyMonitorSvc::getInfos(Client owner)  {
  static set<string> infos;
  infos.clear();
  ClientMap::iterator i = m_clients.find(owner);
  if ( i != m_clients.end() )  {
    for(Items::const_iterator j=(*i).second->begin(); j != (*i).second->end(); ++j)
      infos.insert((*j).first);
  }
  return &infos;
}

string DummyMonitorSvc::clientName(Client owner)  {
  const INamedInterface* c = dynamic_cast<const INamedInterface*>(owner);
  return ( c ) ? c->name() : string("");
}
