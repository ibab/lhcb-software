// $Id: HltGenConfig.cpp,v 1.18 2010-05-05 21:07:43 graven Exp $
// Include files
#include <algorithm>

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

//-----------------------------------------------------------------------------
// Implementation file for class : HltGenConfig
//
// 2007-10-24 : Gerhard Raven
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(HltGenConfig)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltGenConfig::HltGenConfig(const string &name, ISvcLocator *pSvcLocator)
    : GaudiAlgorithm(name, pSvcLocator), m_appMgr{ nullptr },
      m_accessSvc{ nullptr }, m_configSvc{ nullptr } {

  declareProperty("ConfigTop", m_topConfig = { { "Hlt" } });
  declareProperty("ConfigSvc",
                  m_svcConfig = { {"ToolSvc"}, {"HltANNSvc"},
                                  {"Hlt::Service"} });
  declareProperty("ConfigAccessSvc", s_accessSvc = "ConfigTarFileAccessSvc");
  declareProperty("PropertyConfigSvc", s_configSvc = "PropertyConfigSvc");
  declareProperty("HltType", m_hltType);
  declareProperty("MooreRelease", m_release);
  declareProperty("Label", m_label);
  declareProperty("Overrule", m_overrule);
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltGenConfig::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  m_appMgr = svc<IAlgManager>("ApplicationMgr");
  m_accessSvc = svc<IConfigAccessSvc>(s_accessSvc, true);
  m_configSvc = svc<IPropertyConfigSvc>("PropertyConfigSvc", true);
  IToolSvc *toolSvc = svc<IToolSvc>("ToolSvc", true);
  toolSvc->registerObserver(this);
  // FIXME: need to unregister at some point!!!
  if (m_hltType.empty()) {
    error() << "You must specify the hltType label for a configuration..."
            << endmsg;
    return StatusCode::FAILURE;
  }
  if (m_label.empty()) {
    error()
        << "You must specify short, descriptive label for a configuration..."
        << endmsg;
    return StatusCode::FAILURE;
  }
  return sc;
}

vector<PropertyConfig::digest_type>
HltGenConfig::gatherDependencies(const INamedInterface &obj) const {
  vector<PropertyConfig::digest_type> depRefs;
  INamedInterface *ini =
      const_cast<INamedInterface *>(&obj); // we do treat obj logically const,
                                           // even if we call code which seems
                                           // wants non-const version of obj
  // in case of Algorithms, do some extra work...
  SmartIF<IAlgorithm> ia(ini);
  if (ia.isValid()) {
    vector<Algorithm *> *subs =
        dynamic_cast<const Algorithm &>(*ia).subAlgorithms();
    for (const Algorithm *dep : *subs) {
      debug() << "adding sub-algorithm " << dep->name() << " as dependant to "
              << obj.name() << endmsg;
      ConfigTreeNode::digest_type digest = generateConfig(*dep);
      if (digest.invalid()) {
        error() << "problem creating dependant configuration for "
                << dep->name() << endmsg;
      }
      depRefs.push_back(digest);
    }
  }

  // check whether this obj uses any private tools -- if so, add them as
  // dependencies
  auto tools = m_toolmap.equal_range(obj.name());
  for (auto i = tools.first; i != tools.second; ++i) {
    debug() << "adding tool " << i->second->name() << " as dependency of "
            << obj.name() << endmsg;
    ConfigTreeNode::digest_type digest = generateConfig(*i->second);
    if (digest.invalid()) {
      error() << "problem creating dependant configuration for "
              << i->second->name() << endmsg;
    }
    depRefs.push_back(digest);
  }
  return depRefs;
}

ConfigTreeNode::digest_type
HltGenConfig::generateConfig(const INamedInterface &obj) const {
  // iterate over dependants, create config for them first (as we depend on
  // them)
  // ..
  // i.e. must go 'depth first', no choice here...
  vector<PropertyConfig::digest_type> depRefs = gatherDependencies(obj);

  // create and write the leaf object
  PropertyConfig currentConfig = m_configSvc->currentConfiguration(obj);
  // check whether there is a modification request for this component...
  auto overrule = m_overrule.find(obj.name());
  if (overrule != std::end(m_overrule)) {
    warning() << " applying overrule to " << obj.name() << " : " << *overrule
              << endmsg;
    currentConfig = currentConfig.copyAndModify(overrule->second.begin(),
                                                overrule->second.end());
    if (!currentConfig.digest().valid()) {
      error() << " overruling of " << obj.name() << " : " << *overrule
              << " failed" << endmsg;
      return ConfigTreeNode::digest_type::createInvalid();
    }
  }
  PropertyConfig::digest_type propRef =
      m_accessSvc->writePropertyConfig(currentConfig);
  if (propRef.invalid()) {
    error() << "problem writing PropertyConfig" << endmsg;
    return ConfigTreeNode::digest_type::createInvalid();
  }

  // create the tree node for this leaf object, and its dependencies, and write
  // it
  ConfigTreeNode::digest_type nodeRef =
      m_accessSvc->writeConfigTreeNode({ propRef, depRefs });
  if (nodeRef.invalid()) {
    error() << "problem writing ConfigTreeNode" << endmsg;
  }
  return nodeRef;
}

template <typename COMP, typename I, typename R, typename T>
StatusCode HltGenConfig::getDependencies(I i, I end, R resolver,
                                         T inserter) const {
  for (; i != end; ++i) {
    info() << "Generating config for " << *i << endmsg;
    COMP *comp{ nullptr };
    if (!resolver(*i, comp).isSuccess()) {
      error() << "Unable to get " << System::typeinfoName(typeid(COMP)) << " "
              << *i << endmsg;
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

  auto sl = serviceLocator().get();
  StatusCode sc = getDependencies<IService>(
      std::begin(m_svcConfig), std::end(m_svcConfig),
      [&](const std::string &name, IService *&svc) {
        return sl->getService(Gaudi::Utils::TypeNameString(name), svc, false);
      },
      std::back_inserter(depRefs));
  if (sc.isFailure())
    return sc;
  sc = getDependencies<IAlgorithm>(
      std::begin(m_topConfig), std::end(m_topConfig),
      [&](const std::string &name,
          IAlgorithm *&alg) { return m_appMgr->getAlgorithm(name, alg); },
      std::back_inserter(depRefs));
  if (sc.isFailure())
    return sc;

  ConfigTreeNode topConfig(ConfigTreeNode::digest_type::createInvalid(),
                           depRefs, m_label);
  ConfigTreeNode::digest_type topDigest =
      m_accessSvc->writeConfigTreeNode(topConfig);
  if (topDigest.invalid()) {
    error() << "problem writing config " << endmsg;
    return StatusCode::FAILURE;
  }
  info() << " top id for this config: " << topDigest << endmsg;

  // create top level alias for this id
  ConfigTreeNodeAlias::alias_type topAlias =
      m_accessSvc->writeConfigTreeNodeAlias(
          ConfigTreeNodeAlias::createTopLevel(m_release, m_hltType, topConfig));
  if (topAlias.invalid()) {
    error() << "problem writing alias " << endmsg;
    return StatusCode::FAILURE;
  }
  return sc;
}

void HltGenConfig::onCreate(const IAlgTool *tool) {
  assert(tool);
  string name = tool->name();
  string key = name.substr(0, name.rfind('.'));
  debug() << "adding " << key << " -> " << name << endmsg;
  m_toolmap.emplace(key, tool);
}
//=============================================================================
StatusCode HltGenConfig::execute() { return StatusCode::SUCCESS; }

//=============================================================================
StatusCode HltGenConfig::finalize() {
  static bool first(true);
  if (first) {
    first = false;
    generateConfig();
  }
  return GaudiAlgorithm::finalize();
}
