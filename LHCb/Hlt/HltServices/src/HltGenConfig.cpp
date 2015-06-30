// $Id: HltGenConfig.cpp,v 1.18 2010-05-05 21:07:43 graven Exp $
// Include files
#include <algorithm>

// boost
#include <boost/algorithm/string.hpp>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IAuditor.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/System.h"

#include "GaudiKernel/TypeNameString.h"

// local
#include "HltGenConfig.h"
#include "Kernel/PropertyConfig.h"

namespace {
   using namespace std;
   namespace ba = boost::algorithm;
}

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
      m_accessSvc{ nullptr }, m_configSvc{ nullptr }
{
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
  declareProperty("EnvironmentVariables", m_envVars = {"PARAMFILESROOT"});
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

  // Save the environment variables
  for (const auto& var : m_envVars) {
     auto val = System::getEnv(var.c_str());
     if (val.empty()) {
        throw GaudiException(string{"Failed to obtain environment variable "} + var,
                             name(), StatusCode::FAILURE);
     }
     m_envVarValues[val] = var;
  }
  return sc;
}

vector<PropertyConfig::digest_type>
HltGenConfig::gatherDependencies(const INamedInterface &obj) const {
  vector<PropertyConfig::digest_type> depRefs;
  INamedInterface *ini = const_cast<INamedInterface *>(&obj); // we do treat obj logically const,
                                                              // even if we call code which seems
                                                              // wants non-const version of obj
  // in case of Algorithms, do some extra work...
  SmartIF<IAlgorithm> ia(ini);
  if (ia.isValid()) {
    auto subs = dynamic_cast<const Algorithm &>(*ia).subAlgorithms();
    std::transform( std::begin(*subs), std::end(*subs),
                    std::back_inserter(depRefs),
                    [&](const Algorithm*  dep) {
      debug() << "adding sub-algorithm " << dep->name() << " as dependant to " << obj.name() << endmsg;
      auto digest = generateConfig(*dep);
      if (digest.invalid()) error() << "problem creating dependant configuration for "
                                    << dep->name() << endmsg;
      return digest;
    } );
  }

  // check whether this obj uses any private tools -- if so, add them as
  // dependencies
  auto tools = m_toolmap.equal_range(obj.name());
  std::transform( tools.first, tools.second,
                  std::back_inserter(depRefs),
                  [&](const std::pair<const std::string, const IAlgTool*>& i) {
    debug() << "adding tool " << i.second->name() << " as dependency of " << obj.name() << endmsg;
    auto digest = generateConfig(*i.second);
    if (digest.invalid()) {
      error() << "problem creating dependant configuration for "
              << i.second->name() << endmsg;
    }
    return digest;
  });
  return depRefs;
}

ConfigTreeNode::digest_type
HltGenConfig::generateConfig(const INamedInterface &obj) const {
  // iterate over dependants, create config for them first (as we depend on
  // them)
  // ..
  // i.e. must go 'depth first', no choice here...
  auto depRefs = gatherDependencies(obj);

  // create and write the leaf object
  auto currentConfig = m_configSvc->currentConfiguration(obj);
  // check whether there is a modification request for this component...
  auto overrule = m_overrule.find(obj.name());
  if (overrule != std::end(m_overrule)) {
    warning() << " applying overrule to " << obj.name() << " : " << *overrule
              << endmsg;
    currentConfig = currentConfig.copyAndModify(std::begin(overrule->second),
                                                std::end(overrule->second));
    if (!currentConfig.digest().valid()) {
      error() << " overruling of " << obj.name() << " : " << *overrule
              << " failed" << endmsg;
      return ConfigTreeNode::digest_type::createInvalid();
    }
  }

  // Find known environment variables in options and replace them with the environment variable again.
  using iter_t = ba::find_iterator<string::const_iterator>;
  std::vector<string> over;
  for (const auto& valVar : m_envVarValues) {
    for (const auto& prop : currentConfig.properties()) {
      auto it = ba::make_find_iterator(prop.second, ba::first_finder(valVar.first, ba::is_iequal()));
      for(;it != iter_t(); ++it) {
        over.push_back(prop.first + ":" + ba::replace_range_copy(prop.second, *it, string{"$"} + valVar.second));
      }
    }
  }
  // If some overrules were found, apply them
  if (!over.empty()) {
    warning() << " applying environment variable overrule to " << obj.name() << " : " << over
              << endmsg;
    currentConfig = currentConfig.copyAndModify(std::begin(over), std::end(over));
    if (!currentConfig.digest().valid()) {
      error() << " overruling of " << obj.name() << " : " << over
              << " failed" << endmsg;
      return ConfigTreeNode::digest_type::createInvalid();
    }
  }

  // Write the actual config
  auto propRef = m_accessSvc->writePropertyConfig(currentConfig);
  if (propRef.invalid()) {
    error() << "problem writing PropertyConfig" << endmsg;
    return ConfigTreeNode::digest_type::createInvalid();
  }

  // create the tree node for this leaf object, and its dependencies, and write
  // it
  auto nodeRef = m_accessSvc->writeConfigTreeNode({ propRef, depRefs });
  if (nodeRef.invalid()) {
    error() << "problem writing ConfigTreeNode" << endmsg;
  }
  return nodeRef;
}

template <typename I, typename INS, typename R>
StatusCode HltGenConfig::getDependencies(I i, I end, INS inserter, R resolver) const
{
  for (; i != end; ++i) {
    info() << "Generating config for " << *i << endmsg;
    auto comp = resolver(*i);
    if (!comp) {
      error() << "Unable to get " << System::typeinfoName(typeid(comp)) << " "
              << *i << endmsg;
      return StatusCode::FAILURE;
    }
    auto digest = generateConfig(*comp);
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
  StatusCode sc = getDependencies( std::begin(m_svcConfig), std::end(m_svcConfig),
                                   std::back_inserter(depRefs),
                                   [&](const std::string &name) {
        IService *svc { nullptr };
        return sl->getService(Gaudi::Utils::TypeNameString(name), svc, false).isSuccess() ? svc : nullptr;
   } );
  if (sc.isFailure()) return sc;
  sc = getDependencies( std::begin(m_topConfig), std::end(m_topConfig),
                        std::back_inserter(depRefs),
                        [&](const std::string &name) {
          IAlgorithm *alg { nullptr };
          return m_appMgr->getAlgorithm(name, alg).isSuccess() ? alg : nullptr;
  }) ;
  if (sc.isFailure()) return sc;

  ConfigTreeNode topConfig(ConfigTreeNode::digest_type::createInvalid(),
                           depRefs, m_label);
  auto topDigest = m_accessSvc->writeConfigTreeNode(topConfig);
  if (topDigest.invalid()) {
    error() << "problem writing config " << endmsg;
    return StatusCode::FAILURE;
  }
  info() << " top id for this config: " << topDigest << endmsg;

  // create top level alias for this id
  auto topAlias = m_accessSvc->writeConfigTreeNodeAlias(
          ConfigTreeNodeAlias::createTopLevel(m_release, m_hltType, topConfig));
  if (topAlias.invalid()) {
    error() << "problem writing alias " << endmsg;
    return StatusCode::FAILURE;
  }
  return sc;
}

void HltGenConfig::onCreate(const IAlgTool *tool) {
  assert(tool);
  auto name = tool->name();
  auto i = m_toolmap.emplace(name.substr(0, name.rfind('.')), tool);
  debug() << "adding " << i->first << " -> " << name << endmsg;
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
