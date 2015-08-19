// $Id: ConfigTreeEditor.h,v 1.7 2010-05-05 21:05:05 graven Exp $
#ifndef CONFIGTREEEDITOR_H
#define CONFIGTREEEDITOR_H 1
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IConfigTreeEditor.h"
#include "Kernel/IPropertyConfigSvc.h"
#include "Kernel/IConfigAccessSvc.h"
#include <vector>
#include <map>
#include <string>

// Include files
// from Gaudi

/** @class ConfigTreeEditor ConfigTreeEditor.h
 *
 * functionality:
 *
 *  @author Gerhard Raven
 *  @date   2008-02-21
 */
class ConfigTreeEditor : public extends1<GaudiTool, IConfigTreeEditor>
{
  public:
    ConfigTreeEditor( std::string type, std::string name, const IInterface* parent );
    ~ConfigTreeEditor() override = default;

    StatusCode initialize() override;

    // multimap of 'component' -> 'key','value'
    ConfigTreeNode::digest_type
    updateAndWrite( const ConfigTreeNode::digest_type& in,
                    const std::multimap<
                        std::string, std::pair<std::string, std::string>>& updates,
                    const std::string& label ) const override;

  private:
    std::string s_propertyConfigSvc, s_configAccessSvc;
    IPropertyConfigSvc* m_propertyConfigSvc;
    IConfigAccessSvc* m_configAccessSvc;
};

#endif
