// $Id: HltGenConfig.cpp,v 1.18 2010-05-05 21:07:43 graven Exp $
// Include files 
#include <algorithm>
#include "boost/assign/list_of.hpp"
// #include <boost/type_traits/remove_const.hpp>
// #include <boost/type_traits/is_const.hpp>  
#include "boost/lambda/lambda.hpp"
#include "boost/lambda/bind.hpp"
#include "boost/lambda/construct.hpp"


// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IAuditor.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/Service.h"

#include "GaudiKernel/TypeNameString.h"

// local
#include "HltGenConfig.h"
#include "Kernel/PropertyConfig.h"


using namespace std;
using boost::assign::list_of;
namespace bl = boost::lambda;


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
  , m_configSvc(0)
{
    std::vector<std::string> tmp=list_of( string("ToolSvc") )( string("HltANNSvc") ) ( string("Hlt::Service") );
  
    declareProperty("ConfigTop", m_topConfig = std::vector<std::string>(1,string("Hlt")) );
    declareProperty("ConfigSvc", m_svcConfig = tmp );
    declareProperty("ConfigAccessSvc",   s_accessSvc = "ConfigTarFileAccessSvc");
    declareProperty("PropertyConfigSvc", s_configSvc = "PropertyConfigSvc");
    declareProperty("HltType", m_hltType );
    declareProperty("MooreRelease", m_release );
    declareProperty("Label", m_label );
    declareProperty("Overrule", m_overrule);
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
    m_configSvc       = svc<IPropertyConfigSvc>("PropertyConfigSvc",true);
    IToolSvc* toolSvc = svc<IToolSvc>("ToolSvc",true);
    toolSvc->registerObserver(this);
    // FIXME: need to unregister at some point!!!
    if (m_hltType.empty()) {
        error() << "You must specify the hltType label for a configuration..." << endmsg;
        return StatusCode::FAILURE;
    }
    if (m_label.empty()) {
        error() << "You must specify short, descriptive label for a configuration..." << endmsg;
        return StatusCode::FAILURE;
    }
    return sc;
}

vector<PropertyConfig::digest_type> 
HltGenConfig::gatherDependencies(const INamedInterface& obj) const
{
    vector<PropertyConfig::digest_type> depRefs; 
    INamedInterface *ini = const_cast<INamedInterface*>(&obj); // we do treat obj logically const, 
                                                               // even if we call code which seems
                                                               // wants non-const version of obj
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
    return depRefs;
}

ConfigTreeNode::digest_type
HltGenConfig::generateConfig(const INamedInterface& obj) const
{
    //iterate over dependants, create config for them first (as we depend on them) ..
    // i.e. must go 'depth first', no choice here...
    vector<PropertyConfig::digest_type> depRefs = gatherDependencies(obj);

    //create and write the leaf object
    PropertyConfig currentConfig = m_configSvc->currentConfiguration(obj);
    // check whether there is a modification request for this component...
    std::map<std::string, std::vector<std::string> >::const_iterator overrule = m_overrule.find( obj.name() ) ;
    if (overrule != m_overrule.end()) {
        warning() << " applying overrule to " << obj.name() << " : " << *overrule << endmsg;
        currentConfig = currentConfig.copyAndModify(overrule->second.begin(), overrule->second.end());
        if (!currentConfig.digest().valid()) {
            error() << " overruling of " << obj.name() << " : " << *overrule << " failed" << endmsg;
            return ConfigTreeNode::digest_type::createInvalid();
        }
    }
    PropertyConfig::digest_type propRef = m_accessSvc->writePropertyConfig( currentConfig );
    if (propRef.invalid()){  
        error() << "problem writing PropertyConfig" << endmsg;
        return ConfigTreeNode::digest_type::createInvalid();
    }

    //create the tree node for this leaf object, and its dependencies, and write it
    ConfigTreeNode::digest_type nodeRef = m_accessSvc->writeConfigTreeNode( ConfigTreeNode( propRef, depRefs )  );
    if (nodeRef.invalid()){  
        error() << "problem writing ConfigTreeNode" << endmsg;
    }
    return nodeRef;
}


template <typename COMP, typename I, typename R, typename T>
StatusCode HltGenConfig::getDependencies(I i, I end, R resolver,T inserter) const {
    for (; i!= end; ++i) {
        info() << "Generating config for " << *i << endmsg;
        COMP *comp(0); 
        if (!resolver( *i, comp ).isSuccess()) {
            error() << "Unable to get " << System::typeinfoName(typeid(COMP)) << " "  << *i << endmsg;
            return StatusCode::FAILURE;
        }
        ConfigTreeNode::digest_type digest = generateConfig(*comp);
        info() << " id for this : " << digest << endmsg;
        if (!digest.valid()) {
            error() << "got invalid digest for " << *i << endmsg;
            return StatusCode::FAILURE;
        }
        *inserter++ = digest; 
    }
    return StatusCode::SUCCESS;
}

StatusCode HltGenConfig::generateConfig() const {
    vector<ConfigTreeNode::digest_type> depRefs; 
  
    typedef StatusCode (ISvcLocator::*getService_t)(const Gaudi::Utils::TypeNameString&,IService*&,bool);

    getService_t getService = &ISvcLocator::getService;

    std::vector<Gaudi::Utils::TypeNameString> x;
    std::transform( m_svcConfig.begin(),m_svcConfig.end(), 
                    std::back_inserter(x),
                    bl::constructor<Gaudi::Utils::TypeNameString>()
                  );
    
    StatusCode 
    sc = getDependencies<IService>( x.begin(), x.end(), 
                                    bl::bind( getService, serviceLocator().get(), bl::_1, bl::_2, false ),
                                    std::back_inserter(depRefs));
    if (sc.isFailure()) return sc;
    sc = getDependencies<IAlgorithm>( m_topConfig.begin(), m_topConfig.end(), 
                                    bl::bind( &IAlgManager::getAlgorithm, m_appMgr, bl::_1, bl::_2 ),
                                    std::back_inserter(depRefs));
    if (sc.isFailure()) return sc;

    ConfigTreeNode topConfig( ConfigTreeNode::digest_type::createInvalid(),depRefs, m_label);
    ConfigTreeNode::digest_type topDigest = m_accessSvc->writeConfigTreeNode(topConfig);
    if (topDigest.invalid()){  
        error() << "problem writing config " << endmsg;
        return StatusCode::FAILURE;
    }
    info() << " top id for this config: " << topDigest << endmsg;

    // create top level alias for this id
    ConfigTreeNodeAlias::alias_type topAlias = m_accessSvc->writeConfigTreeNodeAlias( ConfigTreeNodeAlias::createTopLevel(m_release,m_hltType,topConfig) );
    if (topAlias.invalid()) {
        error() << "problem writing alias " << endmsg;
        return StatusCode::FAILURE;
    }
    return sc;
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
StatusCode HltGenConfig::execute() {
    return StatusCode::SUCCESS;
}

//=============================================================================
StatusCode HltGenConfig::finalize() {
    static bool first(true);
    if (first) {
        first = false;
        generateConfig();
    }
    return GaudiAlgorithm::finalize();
}
