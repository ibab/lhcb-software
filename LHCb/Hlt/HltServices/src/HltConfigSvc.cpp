// $Id: HltConfigSvc.cpp,v 1.2 2010-06-10 13:52:00 cattanem Exp $
// Include files 

#include <algorithm>

#include "boost/lexical_cast.hpp"
#include "boost/foreach.hpp"
#include "boost/lambda/lambda.hpp"
#include "boost/lambda/bind.hpp"
#include "boost/regex.hpp"
namespace bl=boost::lambda;

// from Gaudi
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/GaudiException.h"

#include "Event/ODIN.h"
#include "Event/HltDecReports.h"

// local
#include "HltConfigSvc.h"

//TODO: check for IEEE 1003.1 compliance instead of Linux...
#if linux
#include <netdb.h>
#include <unistd.h>
#endif

#ifdef _WIN32
#pragma warning(disable : 4355) // 'this' used in base member initializer list
#endif

using namespace std;
using boost::lexical_cast;


//-----------------------------------------------------------------------------
// Implementation file for class : HltConfigSvc
//
// 2007-10-24 : Gerhard Raven
//-----------------------------------------------------------------------------

// Declaration of the Service Factory
DECLARE_SERVICE_FACTORY( HltConfigSvc )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltConfigSvc::HltConfigSvc( const string& name, ISvcLocator* pSvcLocator)
  : PropertyConfigSvc( name , pSvcLocator )
  , m_configuredTCK(0)
  , m_evtSvc(0)
  , m_incidentSvc(0)
  , m_decodeOdin("ODINDecodeTool",this)
  , m_id(~0u)
{
  //TODO: template this pattern of property + 'transformer' -> thing_I_really_want with callback support
  //TODO: Already done -- called propertyhandler...
  declareProperty("TCK2ConfigMap", m_tck2config_)->declareUpdateHandler( &HltConfigSvc::updateMap, this);
  declareProperty("initialTCK", m_initialTCK_ )->declareUpdateHandler( &HltConfigSvc::updateInitial, this); 
  declareProperty("prefetchDir", m_prefetchDir);
  declareProperty("checkOdin", m_checkOdin = true);
  declareProperty("maskL0TCK", m_maskL0TCK = false);
  declareProperty( "HltDecReportsLocation", m_outputContainerName = std::string("/Event/")+LHCb::HltDecReportsLocation::Default );
}

void HltConfigSvc::updateMap(Property&) {
  m_tck2config.clear();
  typedef std::pair<std::string,std::string> val_t;
  BOOST_FOREACH( const val_t& i, m_tck2config_ ) {
    m_tck2config.insert( make_pair( TCK( i.first ), i.second)  );
  }
}

void HltConfigSvc::updateInitial(Property&) {
    m_initialTCK = TCK(m_initialTCK_);
}

//=============================================================================
// Destructor
//=============================================================================
HltConfigSvc::~HltConfigSvc() {
   // if not released already by now, then do it now...
} 


//=============================================================================
// Finalization
//=============================================================================
StatusCode HltConfigSvc::finalize() {
  m_evtSvc->release();      m_evtSvc=0;
  if (m_incidentSvc!=0) { m_incidentSvc->release(); m_incidentSvc=0; }
  return PropertyConfigSvc::finalize();
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltConfigSvc::initialize() {
  // see if we're running online... HLTXYY_ZZZZ_#
  // HLTC0101_GauchoJob_1
  std::string taskName( System::argv()[0] );
  static boost::regex expr("^HLT.*_(.*)_([0-9]+)");
  boost::smatch what;
  if (boost::regex_match(taskName,what,expr)) {
        m_id = boost::lexical_cast<unsigned int>( what[2] );
  }
  bool found = false;
#if linux
  char name[HOST_NAME_MAX]; size_t len=0;
  if (!gethostname(name,len)) {
      struct hostent *x = gethostbyname(name);
      if (x) {
          unsigned char *addr = (unsigned char*)(x->h_addr+x->h_length-2);
          m_id = m_id << 8 | *addr++;
          m_id = m_id << 8 | *addr++;
          found = true;
      }
  } 
#endif  
  if (!found) m_id = m_id <<16;


  StatusCode status = PropertyConfigSvc::initialize();
  if (!status.isSuccess() ) return status;
  if (!service( "EventDataSvc", m_evtSvc).isSuccess()) return StatusCode::FAILURE;
  if (!m_decodeOdin.retrieve().isSuccess()) return StatusCode::FAILURE;


  if (!service( "IncidentSvc", m_incidentSvc).isSuccess()) return StatusCode::FAILURE;
  // add listener to be triggered by first BeginEvent with low priority
  // so it gets called first
  bool rethrow = false;
  bool oneShot = false;
  m_incidentSvc->addListener(this,IncidentType::BeginEvent,
                             std::numeric_limits<long>::min(),rethrow,oneShot);

  
  // load all TCKs... (brute force, but OK for now...)
  std::vector<ConfigTreeNodeAlias> tcks = cas()->configTreeNodeAliases(ConfigTreeNodeAlias::alias_type( std::string("TCK/") ));
  BOOST_FOREACH( const ConfigTreeNodeAlias& tck, tcks ) tck2id( TCK( tck.alias().str().substr(4) ) );

  // find the ID of the initial TCK
  ConfigTreeNode::digest_type initialID = tck2id( TCK( m_initialTCK ) );

  if (std::count(m_prefetchDir.begin(),m_prefetchDir.end(),'/')!=0) {
    error() << " prefetch directory "  << m_prefetchDir << " is too specific" << endmsg;
    return StatusCode::FAILURE;
  }

  // load all TOPLEVEL aliases for specified release
  std::vector<ConfigTreeNodeAlias> tops = cas()->configTreeNodeAliases(ConfigTreeNodeAlias::alias_type( std::string("TOPLEVEL/")+m_prefetchDir+'/' ));
  std::vector<ConfigTreeNodeAlias>::const_iterator initTop = std::find_if( tops.begin(), tops.end(), bl::bind(&ConfigTreeNodeAlias::ref,bl::_1)==initialID );
  if (initTop == tops.end() ) {
    error() << " initial TCK ( " << m_initialTCK << " -> " << initialID << " ) not amongst entries in prefetch directory "  << m_prefetchDir << endmsg;
    return StatusCode::FAILURE;
  }

  // get pointer into initTop->alias(), to the end of TOPLEVEL/+prefetch, to pick up which type this is...
  std::string::size_type pos = initTop->alias().str().find('/', std::string("TOPLEVEL/"+m_prefetchDir+'/').size() );
  if (pos == std::string::npos) {
    error() << " could not determine type from " << initTop->alias() << " with prefetchdir = " << m_prefetchDir << endmsg;
    return StatusCode::FAILURE;
  }
  ++pos;
  // and fetch them, and preload their configurations...
  info() << " prefetching  from " << initTop->alias().str().substr(0,pos) << endmsg;
  std::vector<ConfigTreeNodeAlias> sameTypes = cas()->configTreeNodeAliases(ConfigTreeNodeAlias::alias_type( initTop->alias().str().substr(0,pos) ));

  for (std::vector<ConfigTreeNodeAlias>::const_iterator i = sameTypes.begin(); i!=sameTypes.end(); ++i ) {
     debug() << " considering " << i->alias().str() << endmsg; 
     if ( i->alias().str().substr(0,pos) != initTop->alias().str().substr(0,pos) ) continue;
     debug() << " loading config " << i->alias().str() << " -> " << i->ref() << endmsg; 
     if ( !loadConfig( i->ref() ) ) {
        error() << " failed to load config " << *i << endmsg; 
        return StatusCode::FAILURE;
     }
  }

  // configure everyone from the a-priori specified TCK
        
  status = configure( initialID, false );
  if (status.isSuccess()) m_configuredTCK = TCK( m_initialTCK );
  return status;
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
    ConfigTreeNode::digest_type id = ConfigTreeNode::digest_type::createInvalid();
    TCKMap_t::const_iterator i = m_tck2config.find( tck );
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
        ConfigTreeNodeAlias::alias_type alias( std::string("TCK/") +  tck.str()  );
        boost::optional<ConfigTreeNode> n = cas()->readConfigTreeNodeAlias( alias );
        if (!n) {
            error() << "Could not resolve TCK " <<  tck  << " : no alias '" << alias << "' found " << endmsg;
            return id;
        }
        id = n->digest(); // need a digest, not an object itself...
        // add to cache...
        m_tck2configCache.insert( make_pair(  tck , id.str() ) );
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
               << " ********************************************\n" << endl;
      ++currentTCK;
  }
  if (m_configuredTCK != currentTCK) {
      info() << "updating configuration from TCK " << m_configuredTCK << " to TCK " << currentTCK << endl;
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
    unsigned int tck = odin->triggerConfigurationKey();

    if ( m_configuredTCK != tck ) {
        TCK TCKr(tck);
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
    std::auto_ptr<LHCb::HltDecReports> hdr( new LHCb::HltDecReports() );
    hdr->setConfiguredTCK(m_configuredTCK.uint());
    hdr->setTaskID(m_id);
    m_evtSvc->registerObject(m_outputContainerName,hdr.release());
}

void HltConfigSvc::handle(const Incident& /*incident*/) {
    //dummyCheckOdin();
    if (m_checkOdin) checkOdin();
    createHltDecReports();
}
