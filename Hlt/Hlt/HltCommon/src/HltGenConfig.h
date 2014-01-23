// $Id: HltGenConfig.h,v 1.12 2010-05-05 21:07:43 graven Exp $
#ifndef HLTGENCONFIG_H
#define HLTGENCONFIG_H 1

// Include files
#include <string>
#include <vector>
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

    virtual ~HltGenConfig(); ///< Destructor

    virtual StatusCode initialize(); ///< Algorithm initialization
    virtual StatusCode execute();    ///< Algorithm execution
    virtual StatusCode finalize();   ///< Algorithm finalization

    virtual void onCreate( const IAlgTool* tool );

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
    std::string m_label;   ///< what is the Moore release we use as a label?
    std::map<std::string, std::vector<std::string>> m_overrule;

    StatusCode generateConfig() const;

    ConfigTreeNode::digest_type generateConfig( const INamedInterface& obj ) const;
    template <typename COMP, typename I, typename R, typename INS>
    StatusCode getDependencies( I begin, I end, R resolver, INS inserter ) const;

    // keep track of whom uses which tools
    typedef std::multimap<std::string, const IAlgTool*> Map_t;
    Map_t m_toolmap;
    std::pair<Map_t::const_iterator, Map_t::const_iterator>
    findTools( const std::string& parent ) const;

    // obtain dependencies
    std::vector<PropertyConfig::digest_type>
    gatherDependencies( const INamedInterface& obj ) const;
};
#endif // HLTGENCONFIG_H
