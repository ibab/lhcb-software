// $Id: ConfigTreeEditor.h,v 1.3 2008-03-06 15:42:08 pkoppenb Exp $
#ifndef CONFIGTREEEDITOR_H 
#define CONFIGTREEEDITOR_H 1
#include "GaudiAlg/GaudiTool.h"
#include "HltBase/IConfigTreeEditor.h"
#include "HltBase/IPropertyConfigSvc.h" 
#include "HltBase/IConfigAccessSvc.h" 
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
class ConfigTreeEditor : public GaudiTool,
                         virtual public IConfigTreeEditor 
{
public:
   ConfigTreeEditor( const std::string& type,
                     const std::string& name,
                     const IInterface* parent );
   ~ConfigTreeEditor();

   StatusCode initialize();
   StatusCode finalize();

   void  printAlgorithms(const ConfigTreeNode::digest_type& in) const;
   void  printServices(const ConfigTreeNode::digest_type& in) const;
   void  printTools(const ConfigTreeNode::digest_type& in) const;
   void  printProperties(const ConfigTreeNode::digest_type& top, const std::string& component,
                                                                 const std::string& prop) const;
  
   // multimap of 'component' -> 'key','value'
   ConfigTreeNode::digest_type updateAndWrite(const ConfigTreeNode::digest_type& in,
                                              const std::multimap<std::string,
                                                                  std::pair<std::string,std::string> >& updates) const;

   ConfigTreeNode::digest_type updateAndWrite(const ConfigTreeNode::digest_type& in,
                                              const std::vector<std::string>& updates) const ;

private:
    std::string         s_propertyConfigSvc, s_configAccessSvc;
    IPropertyConfigSvc *m_propertyConfigSvc;
    IConfigAccessSvc   *m_configAccessSvc;
};

#endif
