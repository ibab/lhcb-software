// Include files 

#include <iomanip>

// local
#include "RateFromTCK.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RateFromTCK
//
// 2011-08-10 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( RateFromTCK )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RateFromTCK::RateFromTCK( const std::string& type,
                          const std::string& name,
                          const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_propertyConfigSvc(0),
    m_condTrigger(0),
    m_triggerTCK(0)
{
  declareInterface<IRateFromTCK>(this);

  declareProperty( "IPropertyConfigSvcInstance", m_propertyConfigSvcName = "PropertyConfigSvc");
}
//=============================================================================
// Initialize
//=============================================================================
StatusCode RateFromTCK::initialize(){
  StatusCode sc = GaudiTool::initialize();
  if (!sc) return sc;
  if (UNLIKELY( msgLevel(MSG::VERBOSE))) verbose() << "RateFromTCK::initialize()" << endmsg;
  if (!service( m_propertyConfigSvcName, m_propertyConfigSvc).isSuccess()) {
    fatal() << "RateFromTCK failed to get the IConfigAccessSvc." << endmsg;
    return StatusCode::FAILURE;
  }
  if (this->existDet<Condition>("Conditions/Online/LHCb/RunInfo/Trigger")) {
    registerCondition("Conditions/Online/LHCb/RunInfo/Trigger",
                      m_condTrigger, &RateFromTCK::i_cacheTriggerData);
    if (UNLIKELY( msgLevel(MSG::DEBUG))) debug() << "Conditions/Online/LHCb/RunInfo/Trigger found" << endmsg;
  } else {
    warning() << "Conditions/Online/LHCb/RunInfo/Trigger not found." << endmsg;
  }
  return StatusCode::SUCCESS ;
  
}
//=============================================================================
// get rate - Moved from Jaap Panman's RateFromTCK
//=============================================================================
double RateFromTCK::parameterFromTCK(std::string instanceName, std::string code, std::string sub) const {
  // get rate from TCK
  if (UNLIKELY( msgLevel(MSG::DEBUG))) debug() << "==> rate from TCK with " 
                                               << "``" << instanceName << "'' " 
                                               << "``" << code << "'' " 
                                               << "``" << sub << "'' " 
                                               << endmsg;
  double rate = 0;
  unsigned int tck = getTCK();

  // Decode the raw event to get the TCK from the raw Hlt DecReports
  if (tck == 0) {
    // if there is no TCK, do nothing
    if (UNLIKELY( msgLevel(MSG::VERBOSE))) verbose() 
      << "No TCK was found, we will get the rate from the CondDB" << endmsg;
    return rate;
  }
  else {
    TCK _tck(tck); 
    _tck.normalize();
    ConfigTreeNodeAlias::alias_type alias( std::string("TCK/") +  _tck.str()  );
    // grab properties of child from config database...
    const ConfigTreeNode* tree = m_propertyConfigSvc->resolveConfigTreeNode(alias);
    if (!tree) {
      // if we could not resolve the (non-zero) TCK we have a problem
      if (UNLIKELY( msgLevel(MSG::VERBOSE))) verbose() << "Obtained TCK " << std::hex
                                            << _tck << " from the database which could not be resolved" << endmsg;
      return rate;
    } else { 
      PropertyConfig::digest_type child = m_propertyConfigSvc->findInTree(tree->digest(), instanceName);
      const PropertyConfig *config = m_propertyConfigSvc->resolvePropertyConfig(child);
      if ( config == 0 ) {
        warning() << "could not find property " << instanceName << endmsg;
        return rate;
      }
      if (UNLIKELY( msgLevel(MSG::VERBOSE))) verbose() << "TCK data found: " << tck << " for instance " << instanceName 
                                            << " " << config->properties().size() << " properties " << endmsg;
      // list properties
      for (PropertyConfig::Properties::const_iterator i =  config->properties().begin(); 
           i!= config->properties().end(); ++i ) {
        if (UNLIKELY( msgLevel(MSG::VERBOSE))) verbose() << i->first << " : " << i->second << endmsg;
        if ( i->first == code ) {
          try {
            std::size_t pos = (i->second).find(sub);
            std::string str2 = (i->second).substr(pos + sub.size()); 
            rate = std::strtod(str2.c_str(), 0);
          }
          catch (const std::exception&) {
            warning() << "could not find substring " << sub << " in " << i->first << endmsg;
          }
        }
      }
    }
  }
  return rate;
}
//=========================================================================
//  Extract data from Trigger
//=========================================================================
StatusCode RateFromTCK::i_cacheTriggerData() {
  if (UNLIKELY( msgLevel(MSG::DEBUG))) debug() << "callback Trigger:" << endmsg;
  m_triggerTCK = (unsigned int) m_condTrigger->param<int>("TCK");
  return StatusCode::SUCCESS;
}

//=============================================================================
StatusCode RateFromTCK::runUpdate(){
  return GaudiTool::runUpdate() ;
}
