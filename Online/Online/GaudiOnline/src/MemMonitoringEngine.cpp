#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiOnline/MemMonitorSvc.h"
#include "RTL/GlobalSection.h"
using namespace LHCb;
using namespace std;

/*
 *   LHCb namespace declaration
 */
namespace LHCb {

  /** @class MemMonitoringEngine MemMonitoringEngine.cpp GaudiOnline/MemMonitoringEngine.cpp
    *
    *  MonitoringEngine implementation based on allocated memory.
    *  This trivial implementation just uses memory from the heap
    *  to store monitoring information.
    *
    *  @author Markus Frank
    */
  class MemMonitoringEngine : public MonitoringEngine {
  protected:
    struct Slot  {
      char        inuse;
      char        name[255];
      char        description[255];
      char        path[255];
      int         type;
      const void* value;
    };
    Slot* m_data;
    int   m_numSlots;
  public:
    /// Service constructor
    MemMonitoringEngine(CSTR nam, ISvcLocator* svc)
    : MonitoringEngine(nam,svc), m_data(0) 
    {
      declareProperty("NumberOfSlots",m_numSlots=200);
    }
    /// Standard destructor
    virtual ~MemMonitoringEngine()  {}
    /// Publish single monitoring item identified by owner and name
    virtual void publishItem(CSTR owner_name, CSTR nam, CSTR dsc, int typ, const void* var);
    /// Unpublish single monitoring item identified by owner and name
    virtual void revokeItem(CSTR owner_name, CSTR nam);
    /// Allocate and initialize publishing area
    virtual StatusCode allocatePubarea();
    /// Deallocate and release publishing area
    virtual StatusCode deallocatePubarea();
  };
  class SharedMemMonitoringEngine : public MemMonitoringEngine {
    RTL::GlobalSection* m_gbl;
  public:
    /// Service constructor
    SharedMemMonitoringEngine(CSTR nam, ISvcLocator* svc)
    : MemMonitoringEngine(nam,svc), m_gbl(0) {
    }
    /// Standard destructor
    virtual ~SharedMemMonitoringEngine()  {}
    /// Allocate and initialize publishing area
    virtual StatusCode allocatePubarea();
    /// Deallocate and release publishing area
    virtual StatusCode deallocatePubarea();
  };
}

/// Publish single monitoring item identified by owner and name
void MemMonitoringEngine::publishItem(CSTR owner_name, CSTR nam, CSTR dsc, int typ, const void* var)  {
  DataPoint p(var);
  Slot* s = m_data;
  if ( owner_name.length()+nam.length()+1 > sizeof(s->name) ) {
    MsgStream log(msgSvc(),name());
    log << MSG::ERROR << "Cannot allocate monitoring slot. Name too long." << endmsg;
    return;
  }
  for(s = m_data; s<m_data+m_numSlots; ++s)  {
    if ( 0 == s->inuse )  {
      string n = owner_name+"/"+nam;
      s->inuse = 1;
      s->type = typ;
      s->value = var;
      ::strcpy(s->name,n.c_str());
      if ( typ == HISTOGRAM )  {
        DataObject* obj = dynamic_cast<DataObject*>((Service*)s->value);
        IRegistry*  reg = obj ? obj->registry() : 0;
        std::string path = reg ? reg->identifier() : std::string();
        ::strncpy(s->path,path.c_str(),sizeof(s->path)-1);
        s->path[sizeof(s->path)-1] = 0;
        
      }
      ::strncpy(s->description,dsc.c_str(),sizeof(s->description)-1);
      s->description[sizeof(s->description)-1] = 0;
      return;
    }
  }
  MsgStream err(msgSvc(),name());
  err << MSG::ERROR << "Number of monitoring slots exhausted. "
      << "Increase value of MonitoringSvc.NumberOfSlots ["
      << m_numSlots << "]" << endmsg;
}

/// Unpublish single monitoring item identified by owner and name
void MemMonitoringEngine::revokeItem(CSTR owner_name, CSTR nam) {
  Slot* s = m_data;
  if ( owner_name.length()+nam.length()+1 > sizeof(s->name) ) {
    MsgStream log(msgSvc(),name());
    log << MSG::ERROR << "Cannot revoke monitoring slot. Name too long." << endmsg;
    return;
  }
  string n = owner_name+"/"+nam;
  for(s = m_data; s<m_data+m_numSlots; ++s)  {
    if ( 1 == s->inuse )  {
      if ( ::strcmp(n.c_str(),s->name) == 0 )  {
        s->inuse = 0;
        return;
      }
    }
  }
  MsgStream err(msgSvc(),name());
  err << MSG::ERROR << "Cannot revoke monitoring slot. [Slot not found]" << endmsg;
}

StatusCode MemMonitoringEngine::allocatePubarea()  {
  MsgStream log(msgSvc(),name());
  if ( m_numSlots > 0 )  {
    m_data = new Slot[m_numSlots];
    ::memset(m_data,0,sizeof(Slot)*m_numSlots);
    return StatusCode::SUCCESS;
  }
  log << MSG::ERROR << "Cannot allocate " << m_numSlots << " Slots. Bad option." << endmsg;
  return StatusCode::FAILURE;
}

StatusCode MemMonitoringEngine::deallocatePubarea()  {
  if ( m_data ) delete [] m_data;
  m_data = 0;
  return StatusCode::SUCCESS;
}

/// Allocate and initialize publishing area
StatusCode SharedMemMonitoringEngine::allocatePubarea()   {
  MsgStream log(msgSvc(),name());
  if ( m_numSlots > 0 )  {
    std::string gbl_name = "Monitoring_"+RTL::processName();
    std::auto_ptr<RTL::GlobalSection> gbl(new RTL::GlobalSection(gbl_name,sizeof(Slot)*m_numSlots,true));
    if ( *gbl.get() )  {
      m_gbl = gbl.release();
      m_data = (Slot*)gbl->buffer();
      ::memset(m_data,0,sizeof(Slot)*m_numSlots);
      return StatusCode::SUCCESS;
    }
    log << MSG::ERROR << "Cannot create global section:" << gbl_name << endmsg;
    return StatusCode::FAILURE;
  }
  log << MSG::ERROR << "Cannot allocate " << m_numSlots << " Slots. Bad option." << endmsg;
  return StatusCode::FAILURE;
}

/// Deallocate and release publishing area
StatusCode SharedMemMonitoringEngine::deallocatePubarea()   {
  if ( m_gbl ) delete m_gbl;
  m_gbl = 0;
  m_data = 0;
  return StatusCode::SUCCESS;
}

// Factory for instantiation of service objects
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,MemMonitoringEngine);
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,SharedMemMonitoringEngine);
