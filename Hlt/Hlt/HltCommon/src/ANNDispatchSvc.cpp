
#include "Kernel/IANNSvc.h"
#include "HltBase/IPropertyConfigSvc.h"

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h" 

#include "Event/ODIN.h"

#include "TCKrep.h"

using std::map;
using std::vector;
using std::string;
using boost::optional;

class ANNDispatchSvc : public Service
                     , virtual public IANNSvc 
                     , virtual public IIncidentListener {

public:
  ANNDispatchSvc( const std::string& name, ISvcLocator* pSvcLocator);
  ~ANNDispatchSvc() {}
  
  StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);
  StatusCode initialize(  );
  StatusCode finalize();

  // IncidentListener interface
  void handle(const Incident& incident);

  // IANNSvc interface: delegate to child...

  boost::optional<minor_value_type>  value(const major_key_type& major, const std::string& minor) const
  { verify(); return m_child->value(major,minor); }
  boost::optional<minor_value_type>  value(const major_key_type& major, int minor) const
  { verify(); return m_child->value(major,minor); }

  bool                           hasMajor(const major_key_type& major) const
  { verify(); return m_child->hasMajor(major); }

  std::vector<minor_key_type>    keys(const major_key_type& major) const
  { verify(); return m_child->keys(major); }
  std::vector<minor_value_type>  items(const major_key_type& major) const
  { verify(); return m_child->items(major); }
  std::vector<major_key_type>    majors() const
  {  verify(); return m_child->majors(); }
private:
  void verify() const { if (!m_uptodate) faultHandler(); }
  void faultHandler() const ;


  mutable IDataProviderSvc* m_evtSvc;
  mutable IANNSvc         * m_child;
  mutable IIncidentSvc    * m_incidentSvc;
  mutable IPropertyConfigSvc * m_propertyConfigSvc;
  mutable bool m_uptodate;
  std::string       m_propertyConfigSvcName;
  std::string       m_instanceName;
  mutable unsigned int      m_currentTCK;
  mutable PropertyConfig::digest_type m_currentDigest;
};

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ANNDispatchSvc::ANNDispatchSvc( const string& name, ISvcLocator* pSvcLocator)
  : Service( name , pSvcLocator )
  , m_evtSvc(0)
  , m_child(0)
  , m_incidentSvc(0)
  , m_propertyConfigSvc(0)
  , m_uptodate(false)
{
   declareProperty("IANNSvcInstance",m_instanceName = "HltANNSvc");
}

StatusCode
ANNDispatchSvc::initialize(  )
{
  if (!service("EventDataSvc", m_evtSvc).isSuccess()) return StatusCode::FAILURE;
  // grab IncidentSvc
  if (!service( "IncidentSvc", m_incidentSvc).isSuccess()) return StatusCode::FAILURE;
  // grab IConfigAccessSvc
  if (!service( m_propertyConfigSvcName, m_propertyConfigSvc).isSuccess()) return StatusCode::FAILURE;
  // grab IANNSvc
  if (!service( m_instanceName, m_child).isSuccess()) return StatusCode::FAILURE;

  // add listener for beginEvent... (should be beginRun, but that doesn't seem to be fired...)
  bool rethrow = false;
  bool oneShot = false;
  m_incidentSvc->addListener(this,IncidentType::BeginEvent,int(0),rethrow,oneShot);
  // incidentSvc->addListener(this,IncidentType::BeginRun,priority,rethrow,oneShot);
  m_uptodate = false;
  return StatusCode::SUCCESS;
}

StatusCode
ANNDispatchSvc::finalize(  )
{
  StatusCode status = Service::finalize();
  if (m_propertyConfigSvc) { m_propertyConfigSvc->release(); m_propertyConfigSvc=0; }
  if (m_incidentSvc) { m_incidentSvc->release(); m_incidentSvc=0; }
  if (m_evtSvc) { m_evtSvc->release(); m_evtSvc=0; }
  if (m_child) { m_child->release(); m_child=0; }
  return status;
}
//=============================================================================
// Incident handler
//=============================================================================
void ANNDispatchSvc::handle(const Incident& /*incident*/) {
    m_uptodate = false;
}

//=============================================================================
// update child
//=============================================================================

void ANNDispatchSvc::faultHandler() const {

    SmartDataPtr<LHCb::ODIN> odin( m_evtSvc , LHCb::ODINLocation::Default );
    if (!odin) {
        error() << " Could not locate ODIN... " << endmsg;
        m_incidentSvc->fireIncident(Incident(name(),IncidentType::AbortEvent));
        return;
    }
    unsigned int TCK = odin->triggerConfigurationKey();
    if (TCK!=m_currentTCK || !m_currentDigest.valid()) {
        TCKrep tck(TCK);
        ConfigTreeNodeAlias::alias_type alias( std::string("TCK/") +  tck.str()  );
        // grab properties of child from config database...
        const ConfigTreeNode* tree = m_propertyConfigSvc->resolveConfigTreeNode(alias);
        PropertyConfig::digest_type child = m_propertyConfigSvc->findInTree(tree->digest(), m_instanceName);
        if (!m_currentDigest.valid() || m_currentDigest!=child) {
            const PropertyConfig *config = m_propertyConfigSvc->resolvePropertyConfig(child);
            assert(config!=0);
            // if ( config==0 ) return StatusCode::FAILURE;
            // push properties to child
            SmartIF<IProperty> iProp(m_child);
            for (PropertyConfig::Properties::const_iterator i =  config->properties().begin();i!= config->properties().end(); ++i ) {
                iProp->setProperty( i->first, i->second  );
            }
            m_currentDigest = child;
            // do not reinit for ANNSvc derived instances, as they have a proper updateHandler...
            // StatusCode sc = m_child->reinitialize();
        }
    }
    m_uptodate = true;
    m_currentTCK = TCK;

}



//=============================================================================
// queryInterface
//=============================================================================
StatusCode ANNDispatchSvc::queryInterface(const InterfaceID& riid,
                                  void** ppvUnknown) {
    if ( IANNSvc::interfaceID().versionMatch(riid) )   {
        *ppvUnknown = (IANNSvc*)this;
        addRef();
        return SUCCESS;
    }
    if ( IANSvc::interfaceID().versionMatch(riid) )   {
        *ppvUnknown = (IANSvc*)this;
        addRef();
        return SUCCESS;
    }
    return Service::queryInterface(riid,ppvUnknown);
}
