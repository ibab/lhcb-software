#ifndef GAUDIONLINE_MEMMONITORSVC_H
#define GAUDIONLINE_MEMMONITORSVC_H

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IMonitorSvc.h"
#include <map>
#include <set>

// Forward declarations
class MsgStream;
class StatEntity;
namespace AIDA {
  class IHistogram;
  class IBaseHistogram;
  class IHistogram1D;
  class IHistogram2D;
  class IHistogram3D;
}

// Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID IID_IMonitoringEngine("IMonitoringEngine", 2, 0);

/*
 *   LHCb namespace declaration
 */
namespace LHCb  {
    
  /** @class MonitoringEngine MemMonitorSvc.h GaudiOnline/MemMonitorSvc.h
    *
    *  Helper class to easy the implementation of various monitoring
    *  implementations.
    *
    *  @author Markus Frank
    */
  class MonitoringEngine : public Service {
  protected:
    typedef const std::string& CSTR;
  public:
    enum DataTypes {
      BOOLEAN,
      INTEGER,
      LONG_INTEGER,
      FLOAT,
      DOUBLE,
      NTSTRING,
      STRING,
      DOUBLE_PAIR,
      HISTOGRAM,
      AUXILIARY
    };
    union DataPoint {
      const void*               ptr;
      bool*                     BOOL;
      int*                      INT;
      long*                     LONG;
      float*                    FLOAT;
      double*                   DOUBLE;
      char*                     NTSTRING;
      std::string*              STRING;
      std::pair<double,double>* PAIR;
      AIDA::IHistogram*         HIST;
      AIDA::IHistogram1D*       h1;
      AIDA::IHistogram2D*       h2;
      AIDA::IHistogram3D*       h3;
      DataPoint(const void* p) { ptr = p; }
    };
    class Histogram  {
    protected:
      typedef std::vector<float> Buffer;
      IMessageSvc* m_messageSvc;
      DataPoint    m_hist;
      Buffer       m_data;
      void filldata();
      void fill_1d();
      void fill_2d();
      
    public:
      /// Initializing constructor
      Histogram(const std::string& nam,IMessageSvc* msg,AIDA::IHistogram* h);
      /// Standard descructor
      virtual ~Histogram()         {                           }
      /// Size of data buffer in bytes
      size_t size()  const         {  return m_data.size()*sizeof(float); }
      /// Access internal data buffer (readonly, const)
      const Buffer& data()  const  {  return m_data;                      }
      /// Access internal data buffer (read/write)
      Buffer& data()               {  return m_data;                      }
    };
    /// Retrieve interface ID
    static const InterfaceID& interfaceID() { return IID_IMonitoringEngine; }
  protected:
    /// Publish single monitoring item identified by owner and name
    virtual void i_publishItem(CSTR owner_name, CSTR nam, CSTR dsc, int typ, const void* var) = 0;
    /// Allocate and initialize publishing area
    virtual StatusCode allocatePubarea()    {  return StatusCode::SUCCESS; }
    /// Deallocate and release publishing area
    virtual StatusCode deallocatePubarea()  {  return StatusCode::SUCCESS; }
  public:
    /// Service constructor
    MonitoringEngine(CSTR nam, ISvcLocator* svc) : Service(nam,svc) {}
    /// Standard destructor
    virtual ~MonitoringEngine()  {}
    /// Initialize service
    virtual StatusCode initialize();
    /// Finalize service
    virtual StatusCode finalize();
    // IInterface pure virtual member functions
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIF)  {
      if( interfaceID() == riid ) {
        *ppvIF = (MonitoringEngine*)this;
        addRef();
        return StatusCode::SUCCESS;
      } 
      return Service::queryInterface(riid, ppvIF);
    }
    virtual void publishItem(CSTR owner_name, CSTR nam, CSTR dsc, const bool& var)
    {  i_publishItem(owner_name,nam,dsc,BOOLEAN,&var);                         }
    virtual void publishItem(CSTR owner_name, CSTR nam, CSTR dsc, const int& var)
    {  i_publishItem(owner_name,nam,dsc,INTEGER,&var);                         }
    virtual void publishItem(CSTR owner_name, CSTR nam, CSTR dsc, const long& var)
    {  i_publishItem(owner_name,nam,dsc,LONG_INTEGER,&var);                    }
    virtual void publishItem(CSTR owner_name, CSTR nam, CSTR dsc, const float& var)
    {  i_publishItem(owner_name,nam,dsc,FLOAT,&var);                           }
    virtual void publishItem(CSTR owner_name, CSTR nam, CSTR dsc, const double& var)
    {  i_publishItem(owner_name,nam,dsc,DOUBLE,&var);                          }
    virtual void publishItem(CSTR owner_name, CSTR nam, CSTR dsc, const char* var)
    {  i_publishItem(owner_name,nam,dsc,NTSTRING,var);                         }
    virtual void publishItem(CSTR owner_name, CSTR nam, CSTR dsc, const std::string& var)
    {  i_publishItem(owner_name,nam,dsc,STRING,&var);                          }
    virtual void publishItem(CSTR owner_name, CSTR nam, CSTR dsc, const std::pair<double,double>& var)
    {  i_publishItem(owner_name,nam,dsc,DOUBLE_PAIR,&var);                     }
    virtual void publishItem(CSTR owner_name, CSTR nam, CSTR dsc, const AIDA::IHistogram* var)
    {  i_publishItem(owner_name,nam,dsc,HISTOGRAM,&var);                       }
    /// Unpublish single monitoring item identified by owner and name
    virtual void revokeItem(CSTR owner_name, CSTR nam) = 0;
  };

  /** @class MemMonitorSvc MemMonitorSvc.h GaudiOnline/MemMonitorSvc.h
    *
    *  This class implements the IMonitorSvc interface, and publishes Gaudi variables
    *  into a memory area.
    *
    *  @author Markus Frank
    */
  class MemMonitorSvc : public Service, virtual public IMonitorSvc {
    typedef const std::string& CSTR;
    typedef const IInterface* Client;
    typedef std::pair<std::string,std::string> Item;
    struct Items : public std::set<Item*> {
      std::string owner;
      Items() {}
      Items(CSTR o) : std::set<Item*>(), owner(o) {}
      Items(const Items& c) : std::set<Item*>(c), owner(c.owner) {}
      Items& operator=(const Items& c)  {
	if ( this != &c )   {
	  this->std::set<Item*>::operator=(c);
	  owner = c.owner;
	}
	return *this;
      }
    };
    typedef std::map<Client,Items> ClientMap;
    typedef std::pair<ClientMap::iterator,Items::iterator> RegInfo;

    /// Internal helper to declare itms to the engine
    template <typename T> void i_publish(RegInfo args,const T& var)  {
      if ( args.first != m_clients.end() )  {
        Item* itm = (*args.second);
        m_engine->publishItem((*args.first).second.owner,itm->first,itm->second,var);
      }
    }
    /// Internal helper to access the client name
    std::string clientName( Client owner );
    /// Internal helper to register new clients (or return existing ones)
    ClientMap::iterator regClient(Client c);
    /// Internal helper to declare monitoring items
    RegInfo regItem(CSTR item_name,CSTR dscription,Client owner);
    /// Internal helper to undeclare monitoring items
    void undeclareItem(MsgStream& log, CSTR owner, Item* itm);
    /// Internal helper to undeclare monitoring items
    void undeclareItems(MsgStream& log, Items& itms);

  public:
    /// Service constructor
    MemMonitorSvc(CSTR name, ISvcLocator* sl);
    /// Standard destructor
    virtual ~MemMonitorSvc() {}
    /// IInterface pure virtual member functions
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);
    /// Initialize service
    virtual StatusCode initialize();
    /// Finalize service
    virtual StatusCode finalize();
    
    /** Declare monitoring information
      * @param name Monitoring information name knwon to the external system
      * @param var Monitoring Listener address
      * @param desc Textual description
      * @param owner Owner identifier of the monitoring information 
      * (needed to peform clean up 
      */
    virtual void declareInfo(CSTR nam,const bool& var,  CSTR dsc,Client own)
    {  i_publish(regItem(nam,dsc,own),var);                   }
    virtual void declareInfo(CSTR nam,const int& var,   CSTR dsc,Client own)
    {  i_publish(regItem(nam,dsc,own),var);                   }
    virtual void declareInfo(CSTR nam,const long& var,  CSTR dsc,Client own)
    {  i_publish(regItem(nam,dsc,own),var);                   }
    virtual void declareInfo(CSTR nam,const double& var,CSTR dsc,Client own)
    {  i_publish(regItem(nam,dsc,own),var);                   }
    virtual void declareInfo(CSTR nam,CSTR var,         CSTR dsc,Client own)
    {  i_publish(regItem(nam,dsc,own),var);                   }
    virtual void declareInfo(CSTR nam,const std::pair<double,double>& var,CSTR dsc,Client own)
    {  i_publish(regItem(nam,dsc,own),var);                   }
    virtual void declareInfo(CSTR nam,const AIDA::IHistogram* var,  CSTR dsc,Client own)
    {  i_publish(regItem(nam,dsc,own),var);                   }
    /// Temp hack !
    virtual void declareInfo(CSTR nam, const AIDA::IBaseHistogram* var,CSTR dsc,Client own);
    virtual void declareInfo(CSTR /*nam*/,CSTR /*format*/,const void* /*var*/,int /*size*/,CSTR /*desc*/,Client /*own*/) 
    {}
    virtual void declareInfo(CSTR /*nam*/, const StatEntity& /*var*/, CSTR /*desc*/, Client /*own*/)
    {}
    /** Undeclare monitoring information
      * @param name Monitoring information name knwon to the external system
      * @param owner Owner identifier of the monitoring information
      */
    virtual void undeclareInfo(CSTR nam, Client owner);
    
    /** Undeclare monitoring information
      * @param owner Owner identifier of the monitoring information
      */
    virtual void undeclareAll( Client owner ) ;
    
    /** Get the names for all declared monitoring informations for a given
      * owner. If the owner is NULL, then it returns for all owners
      */
    virtual std::set<std::string>* getInfos(Client owner = 0);

  private:
    /// Map associating to each algorithm name a set with the info items
    ClientMap         m_clients;
    /// Property: monitoring engine type
    std::string       m_engineType;
    /// Reference to monitoring engine
    MonitoringEngine* m_engine;
  };
}      // End namespace gaudi
#endif // GAUDIONLINE_MEMMONITORSVC_H
