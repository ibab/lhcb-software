// $Id: HltGenConfig.cpp,v 1.4 2008-03-13 15:45:39 graven Exp $
// Include files 
#include <algorithm>
#include "boost/assign/list_of.hpp"
#include <boost/type_traits/remove_const.hpp>
#include <boost/type_traits/is_const.hpp>  


// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IAuditor.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/Service.h"

// local
#include "HltGenConfig.h"
#include "HltBase/PropertyConfig.h"


using namespace std;
using boost::assign::list_of;


//-----------------------------------------------------------------------------
// Implementation file for class : HltGenConfig
//
// 2007-10-24 : Gerhard Raven
//-----------------------------------------------------------------------------


// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltGenConfig );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltGenConfig::HltGenConfig( const string& name, ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_appMgr(0)
  , m_accessSvc(0)
{
  declareProperty("ConfigTop", m_topConfig = list_of( string("Hlt") ) );
  declareProperty("ConfigSvc", m_svcConfig = list_of( string("ToolSvc") )
                                                    ( string("HltSvc") ) );
  declareProperty("ConfigAccessSvc", s_accessSvc = "ConfigFileAccessSvc");
}
//=============================================================================
// Destructor
//=============================================================================
HltGenConfig::~HltGenConfig() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltGenConfig::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  m_appMgr          = svc<IAlgManager>("ApplicationMgr");
  m_accessSvc       = svc<IConfigAccessSvc>(s_accessSvc,true);
  IToolSvc* toolSvc = svc<IToolSvc>("ToolSvc",true);
  toolSvc->registerObserver(this);
  // FIXME: need to unregister at some point!!!
  return sc;
}


ConfigTreeNode::digest_type
HltGenConfig::generateConfig(const INamedInterface& obj) const
{
  INamedInterface *ini = const_cast<INamedInterface*>(&obj); // we do treat obj logically const, 
                                                             // even if we call code which seems
                                                             // wants non-const version of obj

    //iterate over dependants, create config for them first (as we depend on them) ..
    // i.e. must go 'depth first', no choice here...
  vector<PropertyConfig::digest_type> depRefs; 

  // in case of Algorithms, do some extra work...
  SmartIF<IAlgorithm> ia(ini);
  if (ia.isValid()) {
      vector<Algorithm*> *subs = dynamic_cast<const Algorithm&>(*ia).subAlgorithms();
      for (vector<Algorithm*>::const_iterator dep = subs->begin(); dep!=subs->end(); ++dep) {
         debug() << "adding sub-algorithm " << (*dep)->name() << " as dependant to " << obj.name() << endmsg;
         ConfigTreeNode::digest_type digest = generateConfig(**dep);
         if (digest.invalid()) {
            error() << "problem creating dependant configuration for " << (*dep)->name() << endmsg;
         }
         depRefs.push_back(digest);
      }
  }

  // check whether this obj uses any private tools -- if so, add them as dependencies
  pair<Map_t::const_iterator,Map_t::const_iterator> range = findTools( obj.name() );
  for (Map_t::const_iterator i= range.first;i!=range.second;++i) {
       debug() << "adding tool " << i->second->name() << " as dependency of " << obj.name() << endmsg;
       ConfigTreeNode::digest_type  digest = generateConfig(*i->second);
       if (digest.invalid()) {
          error() << "problem creating dependant configuration for " << i->second->name() << endmsg;
       }
       depRefs.push_back(digest);
  }

  //TODO: replace the following by the usage of PropertyConfigSvc...

  // figure out whether we have a Service, Tool, Algorithm or Auditor...
  string kind = "Unknown";
  if      (SmartIF<IAlgorithm>(ini).isValid()) kind = "IAlgorithm";
  else if (SmartIF<IService>(ini).isValid())   kind = "IService";
  else if (SmartIF<IAlgTool>(ini).isValid())   kind = "IAlgTool";
  else if (SmartIF<IAuditor>(ini).isValid())   kind = "IAuditor";

  SmartIF<IProperty> ip(ini);
  PropertyConfig::digest_type propRef = m_accessSvc->writePropertyConfig( PropertyConfig( obj.name(), *ip, kind) );
  if (propRef.invalid()){  
        error() << "problem writing PropertyConfig" << endmsg;
        return MD5::createInvalidDigest();
  }

  ConfigTreeNode::digest_type nodeRef = m_accessSvc->writeConfigTreeNode( ConfigTreeNode( propRef, depRefs )  );
  if (nodeRef.invalid()){  
        error() << "problem writing ConfigTreeNode" << endmsg;
  }
  return nodeRef;
}


StatusCode HltGenConfig::generateConfig( ) const {
  vector<ConfigTreeNode::digest_type> depRefs; 

  // services...
  for (vector<string>::const_iterator i= m_svcConfig.begin(); i!= m_svcConfig.end(); ++i) {
      info() << "Generating config for " << *i << endmsg;
      IService *isvc(0); 
      if (!serviceLocator()->getService( *i, isvc ).isSuccess()) {
          return Error( string("Unable get IService ")+*i, StatusCode::FAILURE); 
      }
      ConfigTreeNode::digest_type svcDigest = generateConfig(*isvc);
      info() << " id for this : " << svcDigest << endmsg;
      depRefs.push_back(svcDigest);
  }
 
  // algorithms...
  for (vector<string>::const_iterator i= m_topConfig.begin(); i!= m_topConfig.end(); ++i) {
      info() << "Generating config starting from " << *i << endmsg;
      IAlgorithm* ialgo(0);
      StatusCode sc = m_appMgr->getAlgorithm( *i, ialgo );
      if (sc.isFailure()) {
          return Error( string("Unable get algorithm ")+*i, StatusCode::FAILURE); 
      }
      ConfigTreeNode::digest_type digest = generateConfig(*ialgo);
      info() << " id for this config: " << digest << endmsg;
      depRefs.push_back(digest);
  }
  
  ConfigTreeNode topConfig( MD5::createInvalidDigest(),depRefs,"head of ConfigTree generated by HltGenConfig");
  ConfigTreeNode::digest_type topDigest = m_accessSvc->writeConfigTreeNode(topConfig);
  if (topDigest.invalid()){  
    error() << "problem creating config file " << endmsg;
  }
  info() << " top id for this config: " << topDigest << endmsg;
  return StatusCode::SUCCESS;
}

std::pair<HltGenConfig::Map_t::const_iterator,HltGenConfig::Map_t::const_iterator> 
HltGenConfig::findTools(const std::string& parent) const {
  return m_toolmap.equal_range(parent);
}

void HltGenConfig::onCreate(const IAlgTool* tool) {
  assert(tool!=0);
  string name = tool->name();
  string key = name.substr(0,name.rfind('.'));
  debug() << "adding " << key << " -> " << name << endmsg;
  m_toolmap.insert( make_pair(key,tool) );
}
//=============================================================================
//=============================================================================
StatusCode HltGenConfig::execute() {
  return StatusCode::SUCCESS;
}

StatusCode HltGenConfig::finalize() {
  static bool first(true);
  if (first) {
     first = false;
     generateConfig();
  }
  return GaudiAlgorithm::finalize();
}
