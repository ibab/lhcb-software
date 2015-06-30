// $Id: HltGenConfig.h,v 1.12 2010-05-05 21:07:43 graven Exp $
#ifndef HLTGENCONFIG_H
#define HLTGENCONFIG_H 1

// Include files
#include <string>
#include <vector>
#include <unordered_map>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Kernel/IConfigAccessSvc.h"
#include "Kernel/IPropertyConfigSvc.h"

/** @class HltGenConfig HltGenConfig.h
 *
 *  functionality:
 *         generate configurations by interrogating
 *         a Gaudi job...
 *
 *  @author Gerhard Raven
 *  @date   2007-10-24
 */
class HltGenConfig : public GaudiAlgorithm, public IToolSvc::Observer
{
  public:
    HltGenConfig( const std::string& name, ISvcLocator* pSvcLocator );

    ~HltGenConfig() override = default; ///< Destructor

    StatusCode initialize() override; ///< Algorithm initialization
    StatusCode execute() override;    ///< Algorithm execution
    StatusCode finalize() override;   ///< Algorithm finalization

    void onCreate( const IAlgTool* tool ) override;

  private:
    typedef unsigned int TCK_t;

    IAlgManager* m_appMgr;
    std::string s_accessSvc;
    IConfigAccessSvc* m_accessSvc;
    std::string s_configSvc;
    IPropertyConfigSvc* m_configSvc;
    std::vector<std::string> m_topConfig; ///< where to start when configuring
    std::vector<std::string> m_svcConfig; ///< Which services to configure
    std::string m_hltType; ///< what runtype do we label this config as?
    std::string m_release; ///< what is the Moore release we use as a label?
    std::string m_label;   ///< what is the label?
    std::map<std::string, std::vector<std::string>> m_overrule;
    std::vector<std::string> m_envVars; ///< Which environment variables do we put back in.
    std::unordered_map<std::string, std::string> m_envVarValues;

    StatusCode generateConfig() const;

    ConfigTreeNode::digest_type generateConfig( const INamedInterface& obj ) const;
    template <typename I, typename INS, typename R>
    StatusCode getDependencies( I begin, I end, INS inserter , R resolver) const;

    // keep track of whom uses which tools
    std::multimap<std::string, const IAlgTool*>  m_toolmap;

    // obtain dependencies
    std::vector<PropertyConfig::digest_type>
    gatherDependencies( const INamedInterface& obj ) const;
};
#endif // HLTGENCONFIG_H
