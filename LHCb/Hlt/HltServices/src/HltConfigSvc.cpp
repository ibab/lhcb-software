// $Id: HltConfigSvc.cpp,v 1.2 2010-06-10 13:52:00 cattanem Exp $
// Include files 

#include <algorithm>

#include "boost/regex.hpp"

// from Gaudi
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/GaudiException.h"

#include "Event/ODIN.h"
#include "Event/HltDecReports.h"

// local
#include "HltConfigSvc.h"

//TODO: check for IEEE 1003.1 compliance instead of Linux...
#ifndef _WIN32
#include <netdb.h>
#include <unistd.h>
#endif

#ifdef _WIN32
#pragma warning(disable : 4355) // 'this' used in base member initializer list
#endif

static const ConfigTreeNodeAlias::alias_type TCK_{ std::string("TCK/") };
static const std::string TOPLEVEL{ "TOPLEVEL/" };

//-----------------------------------------------------------------------------
// Implementation file for class : HltConfigSvc
//
// 2007-10-24 : Gerhard Raven
//-----------------------------------------------------------------------------

// Declaration of the Service Factory
DECLARE_COMPONENT( HltConfigSvc )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltConfigSvc::HltConfigSvc( const std::string& name, ISvcLocator* pSvcLocator)
  : base_class( name , pSvcLocator )
  , m_decodeOdin("ODINDecodeTool",this)
{
  //TODO: template this pattern of property + 'transformer' -> thing_I_really_want with callback support
  //TODO: Already done -- called propertyhandler...
  declareProperty("TCK2ConfigMap", m_tck2config_)->declareUpdateHandler( &HltConfigSvc::updateMap, this);
  declareProperty("initialTCK", m_initialTCK_ )->declareUpdateHandler( &HltConfigSvc::updateInitial, this); 
  declareProperty("prefetchDir", m_prefetchDir);
  declareProperty("checkOdin", m_checkOdin = true);
  declareProperty("maskL0TCK", m_maskL0TCK = false);
  declareProperty("HltDecReportsLocations", m_outputContainerName = {"/Event/Hlt1/DecReports","/Event/Hlt2/DecReports"} );
  declareProperty("Hlt2Mode", m_hlt2mode = false);
}

void HltConfigSvc::updateMap(Property&) {
  m_tck2config.clear();
  std::transform( std::begin(m_tck2config_), std::end(m_tck2config_),
                  std::inserter( m_tck2config, std::end(m_tck2config) ),
                  []( const std::pair<std::string,std::string>& i) { 
            return std::make_pair( TCK{i.first}, i.second ); 
  } );
}

void HltConfigSvc::updateInitial(Property&) {
    m_initialTCK = TCK{m_initialTCK_};
}


//=============================================================================
// Finalization
//=============================================================================
StatusCode HltConfigSvc::finalize() {
  m_evtSvc.reset();
  m_incidentSvc.reset();
  return PropertyConfigSvc::finalize();
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltConfigSvc::initialize() {

  StatusCode status = PropertyConfigSvc::initialize();
  if (!status.isSuccess() ) return status;
  m_evtSvc = service( "EventDataSvc");
  if (!m_evtSvc) return StatusCode::FAILURE;
  if (!m_decodeOdin.retrieve().isSuccess()) return StatusCode::FAILURE;


  m_incidentSvc = service( "IncidentSvc");
  if (!m_incidentSvc) return StatusCode::FAILURE;
  // add listener to be triggered by first BeginEvent with low priority
  // so it gets called first
  bool rethrow = false;
  bool oneShot = false;
  m_incidentSvc->addListener(this,IncidentType::BeginEvent,
                             std::numeric_limits<long>::min(),rethrow,oneShot);

  // load the complete TCK -> ID mapping... (brute force, but OK for now...)
  for ( const auto& tck: cas()->configTreeNodeAliases( TCK_ ) )
     tck2id( TCK{ tck.alias().str().substr(4) } );

  // find the ID of the initial TCK
  auto initialID = tck2id( TCK{ m_initialTCK } );

  if (std::count(std::begin(m_prefetchDir),std::end(m_prefetchDir),'/')!=0) {
    error() << " prefetch directory "  << m_prefetchDir << " is too specific" << endmsg;
    return StatusCode::FAILURE;
  }

  // load all TOPLEVEL aliases for specified release
  auto tops = cas()->configTreeNodeAliases(ConfigTreeNodeAlias::alias_type{ TOPLEVEL+m_prefetchDir+'/' });
  auto initTop = std::find_if( std::begin(tops), std::end(tops), 
                               [=](const ConfigTreeNodeAlias& a) {  return a.ref()==initialID; } );
  if ( initTop == std::end(tops) ) {
    error() << " initial TCK ( " << m_initialTCK << " -> " << initialID << " ) not amongst entries in prefetch directory "  << m_prefetchDir << endmsg;
    return StatusCode::FAILURE;
  }

  // get pointer into initTop->alias(), to the end of TOPLEVEL/+prefetch, to pick up which type this is...
  auto pos = initTop->alias().str().find('/', TOPLEVEL.size() + m_prefetchDir.size() + 1 );
  if (pos == std::string::npos) {
    error() << " could not determine type from " << initTop->alias() << " with prefetchdir = " << m_prefetchDir << endmsg;
    return StatusCode::FAILURE;
  }
  ++pos;
  // and fetch them, and preload their configurations...
  info() << " prefetching  from " << initTop->alias().str().substr(0,pos) << endmsg;
  for (const auto& i : cas()->configTreeNodeAliases( { initTop->alias().str().substr(0,pos) } ) ) {
     debug() << " considering " << i.alias().str() << endmsg; 
     if ( i.alias().str().substr(0,pos) != initTop->alias().str().substr(0,pos) ) continue;
     debug() << " loading config " << i.alias().str() << " -> " << i.ref() << endmsg; 
     if ( !loadConfig( i.ref() ) ) {
        error() << " failed to load config " << i << endmsg; 
        return StatusCode::FAILURE;
     }
  }

  // configure everyone from the a-priori specified TCK
  status = configure( initialID, false );
  if (status.isSuccess()) m_configuredTCK = TCK{ m_initialTCK };
  return status;
}

StatusCode HltConfigSvc::start() {
  //TODO: verify whether this works in case of checkpointing,
  m_id = ~0u;
  // see if we're running online... HLTXYY_ZZZZ_#
  // HLTC0101_GauchoJob_1
  std::string taskName{ System::argv()[0] };
  static boost::regex expr("^HLT.*_([^_]*)_([0-9]+)");
  boost::smatch what;
  if (boost::regex_match(taskName,what,expr)) m_id = ( std::stoul( what[2] ) << 16 );
#ifndef _WIN32
  char name[_POSIX_HOST_NAME_MAX];
  if (!gethostname(name,sizeof(name))) {
      auto *x = gethostbyname(name);
      if (x) {
          unsigned char *addr = (unsigned char*)(x->h_addr+x->h_length-2);
          m_id |=  ( addr[0] << 8 ) | addr[1];
      }
  }
#endif  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Perform mapping from TCK to onfiguration ID
//=============================================================================
ConfigTreeNode::digest_type
HltConfigSvc::tck2id(TCK tck) const {
    tck.normalize();
    if ( m_maskL0TCK  ) {
        tck.maskL0();
        debug() << " masked L0 part of TCK -- now have " << tck << endmsg;
    }
    auto id = ConfigTreeNode::digest_type::createInvalid();
    auto i = m_tck2config.find( tck );
    if (i != m_tck2config.end()) {
        id = ConfigTreeNode::digest_type::createFromStringRep(i->second);
        warning() << " TCK " << tck << " mapped (by explicit option) to " << id << endmsg;
        return id;
    }

    // NOTE: we need to access to the IConfigAccessSvc of the parent to make
    // sure we are consistent...

    i = m_tck2configCache.find( tck );
    if ( i!=m_tck2configCache.end() )  {
        id = ConfigTreeNode::digest_type::createFromStringRep(i->second);
    } else {
        ConfigTreeNodeAlias::alias_type alias{ std::string("TCK/") +  tck.str() };
        auto n = cas()->readConfigTreeNodeAlias( alias );
        if (!n) {
            error() << "Could not resolve TCK " <<  tck  << " : no alias '" << alias << "' found " << endmsg;
            return id;
        }
        id = n->digest(); // need a digest, not an object itself...
        // add to cache...
        m_tck2configCache.emplace( tck , id.str() );
    }
    debug() << "mapping TCK" <<  tck  << " to configuration ID" << id << endmsg;
    return id;
}

//=============================================================================
// Check TCK on 'beginEvent' incident
//=============================================================================
void HltConfigSvc::dummyCheckOdin() {
  // check if TCK still the same -- if not, reconfigure... 
  TCK currentTCK = m_configuredTCK;
  static unsigned nEvent(0);
  debug() << "nEvent: " << nEvent << endmsg;
  if (++nEvent%100==0) { 
      always()   << "\n ********************************************\n"
               << " *********INCREASING TCK !!!!****************\n"
               << " ********************************************\n" << endmsg;
      ++currentTCK;
  }
  if (m_configuredTCK != currentTCK) {
      info() << "updating configuration from TCK " << m_configuredTCK << " to TCK " << currentTCK << endmsg;
      StatusCode sc = reconfigure( tck2id(currentTCK) );
      if (sc.isSuccess()) m_configuredTCK = currentTCK;
  }
}

void HltConfigSvc::checkOdin() {
    SmartDataPtr<LHCb::ODIN> odin( m_evtSvc , LHCb::ODINLocation::Default );
    if (!odin) m_decodeOdin->execute();
    if (!odin) {
        error() << " Could not locate ODIN... " << endmsg;
        m_incidentSvc->fireIncident(Incident(name(),IncidentType::AbortEvent));
        throw GaudiException("No ODIN present??","",StatusCode::FAILURE ); 
        return;
    }
    auto tck = odin->triggerConfigurationKey();

    if ( m_configuredTCK != tck ) {
        TCK TCKr{tck};
        info() << "requesting configuration update from TCK " << m_configuredTCK 
               << " to TCK " << TCKr << endmsg;
        StatusCode sc = reconfigure( tck2id(TCKr) );
        if (sc.isSuccess()) { 
            m_configuredTCK = TCKr;
        } else {
            error()   << "\n\n\n\n\n"
                      << "            ****************************************\n"
                      << "            ****************************************\n"
                      << "            ****************************************\n"
                      << "            ********                        ********\n"
                      << "            ********   RECONFIGURE FAILED   ********\n"
                      << "            ********                        ********\n"
                      << "            ****************************************\n"
                      << "            ****************************************\n"
                      << "            ****************************************\n"
                      << "\n\n\n\n\n"
                      << endmsg;
            m_incidentSvc->fireIncident(Incident(name(),IncidentType::AbortEvent));
            throw GaudiException("Reconfigure failed","",sc); 
            return;
        }
    }
}

void HltConfigSvc::createHltDecReports() {
    for ( const auto& location :  m_outputContainerName ) {
        std::unique_ptr<LHCb::HltDecReports> hdr( new LHCb::HltDecReports() );
        hdr->setConfiguredTCK(m_configuredTCK.uint());
        hdr->setTaskID(m_id);
        m_evtSvc->registerObject(location,hdr.release());
    }
}

void HltConfigSvc::handle(const Incident& /*incident*/) {
    //dummyCheckOdin();
    if (m_checkOdin) checkOdin();
    createHltDecReports();
}

