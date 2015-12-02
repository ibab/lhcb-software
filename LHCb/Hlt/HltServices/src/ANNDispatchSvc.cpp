#include "Kernel/IANNSvc.h"
#include "Kernel/IPropertyConfigSvc.h"

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h" 

#include "Event/RawEvent.h"

#include "Kernel/TCK.h"

using std::map;
using std::vector;
using std::string;
using boost::optional;

using namespace LHCb;

class ANNDispatchSvc : public extends<Service, IANNSvc, IIncidentListener> {

public:
  ANNDispatchSvc( const std::string& name, ISvcLocator* pSvcLocator);
  ~ANNDispatchSvc() = default ; 
  
  StatusCode initialize(  ) override;
  StatusCode finalize() override;

  // IncidentListener interface
  void handle(const Incident& incident) override;

  // IANNSvc interface: delegate to child...

  boost::optional<minor_value_type>  value(const major_key_type& major, const std::string& minor) const override
  { verify(); return m_child->value(major,minor); }
  boost::optional<minor_value_type>  value(const major_key_type& major, int minor) const override
  { verify(); return m_child->value(major,minor); }

  bool                           hasMajor(const major_key_type& major) const override
  { verify(); return m_child->hasMajor(major); }

  std::vector<minor_key_type>    keys(const major_key_type& major) const override
  { verify(); return m_child->keys(major); }
  std::vector<minor_value_type>  items(const major_key_type& major) const override
  { verify(); return m_child->items(major); }
  GaudiUtils::VectorMap< minor_value_type::first_type, minor_value_type::second_type > item_map(const major_key_type& major) const override
  { verify(); return m_child->item_map(major); }
  
  std::vector<major_key_type>    majors() const override
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
  std::string       m_inputRawEventLocation;
  std::vector<std::string> m_rawEventLocations;
  mutable unsigned int      m_currentTCK;
  mutable PropertyConfig::digest_type m_currentDigest;
};

DECLARE_COMPONENT( ANNDispatchSvc )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ANNDispatchSvc::ANNDispatchSvc( const string& name, ISvcLocator* pSvcLocator)
  : base_class( name , pSvcLocator )
  , m_evtSvc{nullptr}
  , m_child{nullptr}
  , m_incidentSvc{nullptr}
  , m_propertyConfigSvc{nullptr}
  , m_uptodate(false)
  , m_inputRawEventLocation("")
  , m_currentTCK(0)
{
  declareProperty("IANNSvcInstance", m_instanceName = "HltANNSvc");
  declareProperty("IPropertyConfigSvcInstance", m_propertyConfigSvcName = "PropertyConfigSvc");
  declareProperty("RawEventLocation", m_inputRawEventLocation); 
}

StatusCode
ANNDispatchSvc::initialize(  )
{
  StatusCode sc = Service::initialize();
  if (sc.isFailure()) {
    fatal() << "Service::initialize() failed!!!"<< endmsg;
    return sc;
  }

  if( msgLevel(MSG::VERBOSE) ) verbose() << "==> Initialize" << endmsg;

  m_rawEventLocations.clear();
  if( m_inputRawEventLocation != "" )m_rawEventLocations.push_back(m_inputRawEventLocation);
  m_rawEventLocations.push_back(LHCb::RawEventLocation::Trigger);
  m_rawEventLocations.push_back(LHCb::RawEventLocation::Copied);
  m_rawEventLocations.push_back(LHCb::RawEventLocation::Default);

  if (!service("EventDataSvc", m_evtSvc).isSuccess()) { 
    fatal() << "ANNDispatchSvc failed to get the EventDataSvc." << endmsg;
    return StatusCode::FAILURE;
  }
  // grab IncidentSvc
  if (!service( "IncidentSvc", m_incidentSvc).isSuccess()) {
    fatal() << "ANNDispatchSvc failed to get the IncidentSvc." << endmsg;
    return StatusCode::FAILURE;
  }
  // grab IConfigAccessSvc
  if (!service( m_propertyConfigSvcName, m_propertyConfigSvc).isSuccess()) {
    fatal() << "ANNDispatchSvc failed to get the IConfigAccessSvc." << endmsg;
    return StatusCode::FAILURE;
  }
  // grab IANNSvc
  if (!service( m_instanceName, m_child).isSuccess()) {
    fatal() << "ANNDispatchSvc failed to get the IANNSvc." << endmsg;
    return StatusCode::FAILURE;
  }

  // add listener for beginEvent... (should be beginRun, but that doesn't seem to be fired...)
  bool rethrow = false;
  bool oneShot = false;
  m_incidentSvc->addListener(this,IncidentType::BeginEvent,int(0),rethrow,oneShot);
  // incidentSvc->addListener(this,IncidentType::BeginRun,priority,rethrow,oneShot);
  m_uptodate = false;
  return sc;
}

StatusCode
ANNDispatchSvc::finalize(  )
{
  StatusCode status = Service::finalize();
  if (m_propertyConfigSvc) { m_propertyConfigSvc->release(); m_propertyConfigSvc=nullptr; }
  if (m_incidentSvc) { m_incidentSvc->release(); m_incidentSvc=nullptr; }
  if (m_evtSvc) { m_evtSvc->release(); m_evtSvc=nullptr; }
  if (m_child) { m_child->release(); m_child=nullptr; }
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

  //Get the Hlt DecReports
  //SmartDataPtr<LHCb::HltDecReports> decReports(m_evtSvc, LHCb::HltDecReportsLocation::Default );
  //Get the TCK from the DecReports
  //unsigned int TCK = decReports->configuredTCK();  
    
  //Decode the raw event to get the TCK from the raw Hlt DecReports
  unsigned int tck = 0;

  SmartDataPtr<LHCb::RawEvent> rawEvent(m_evtSvc, m_inputRawEventLocation); 
  auto iLoc = m_rawEventLocations.begin();
  for (; iLoc != m_rawEventLocations.end() && !rawEvent ; ++iLoc ) {
    rawEvent = SmartDataPtr<LHCb::RawEvent>(m_evtSvc, *iLoc); 
  }

  if (!rawEvent) {
    if( msgLevel(MSG::VERBOSE) ) 
      verbose() << " No RawEvent found! We will get ANN info from HltInit instead." << endmsg;
    m_uptodate = true;
    m_currentTCK = tck;
    return;    
  }
  //Now we get the TCK only from the raw event
  const std::vector<RawBank*> hltdecreportsRawBanks = rawEvent->banks( RawBank::HltDecReports );
  if( hltdecreportsRawBanks.size() == 0) {
    if( msgLevel(MSG::VERBOSE) ) 
      verbose() << " No HltDecReports RawBank in RawEvent. We will get ANN info from HltInit instead." << endmsg;
    m_uptodate = true;
    m_currentTCK = tck;
    return; 
  } else {
    const RawBank* hltdecreportsRawBank = hltdecreportsRawBanks.front();
    if (!hltdecreportsRawBank) {
      if( msgLevel(MSG::VERBOSE) ) 
        verbose() << "Corrupted HltDecReport in the RawBank, we will get ANN info from HltInit instead" << endmsg; 
      m_uptodate = true;
      m_currentTCK = tck;
      return; 
    } else {
      const unsigned int *content = hltdecreportsRawBank->begin<unsigned int>();
      // version 0 has only decreps, version 1 has TCK, taskID, then decreps...
      if (hltdecreportsRawBank->version() > 0 ) {
        tck = *content++ ;
      }
    }
  }
  if (tck == 0) {
    // if there is no TCK, do not dispatch
    if( msgLevel(MSG::VERBOSE) ) 
      verbose() << "No TCK was found in the RawBank, we will get ANN info from HltInit instead" << endmsg;
    m_uptodate = true;
    m_currentTCK = tck;
    return;
  }
  if (tck!=m_currentTCK || !m_currentDigest.valid()) {
     if( msgLevel(MSG::VERBOSE) ) verbose() << "Entering this loop" << endmsg;
    TCK _tck(tck); _tck.normalize();
    ConfigTreeNodeAlias::alias_type alias( std::string("TCK/") +  _tck.str()  );
    // grab properties of child from config database...
    const ConfigTreeNode* tree = m_propertyConfigSvc->resolveConfigTreeNode(alias);
    if (!tree) {
      //If we could not resolve the (non-zero) TCK we have a problem
       if( msgLevel(MSG::VERBOSE) ) verbose() << "Obtained TCK " << _tck << 
        " from the Hlt DecReports which could not be resolved. We will get ANN info from HltInit instead." << endmsg;
      m_uptodate = true;
      m_currentTCK = tck;
      return;
    } else { 
      PropertyConfig::digest_type child = m_propertyConfigSvc->findInTree(tree->digest(), m_instanceName);
      if (!m_currentDigest.valid() || m_currentDigest!=child) {
        const PropertyConfig *config = m_propertyConfigSvc->resolvePropertyConfig(child);
        assert(config!=0);
        // if ( config==0 ) return StatusCode::FAILURE;
        // push properties to child
        SmartIF<IProperty> iProp(m_child);
        for (const auto & elem : config->properties()) {
          iProp->setProperty( elem.first, elem.second  ).ignore();
        }
        m_currentDigest = child;
        // do not reinit for ANNSvc derived instances, as they have a proper updateHandler...
        // StatusCode sc = m_child->reinitialize();
      }
    }
  }
  m_uptodate = true;
  m_currentTCK = tck;
}
