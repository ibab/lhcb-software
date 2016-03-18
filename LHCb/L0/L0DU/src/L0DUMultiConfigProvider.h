// $Id: L0DUMultiConfigProvider.h,v 1.2 2010-01-20 16:30:58 odescham Exp $
#ifndef L0DUMULTICONFIGPROVIDER_H 
#define L0DUMULTICONFIGPROVIDER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
// from LHCb
#include "L0Interfaces/IL0DUConfigProvider.h"            // Interface
#include "L0DUConfigProvider.h" // single Config provider tool


/** @class L0DUMultiConfigProvider L0DUMultiConfigProvider.h
 *  
 *  Provide  **ANY** predefined L0DU configuration according to 
 *  the Trigger Configuration Key
 *  Configuration described in options file
 *
 *  @author Olivier Deschamps
 *  @date   2008-01-26
 */
class L0DUMultiConfigProvider : public GaudiTool, virtual public IL0DUConfigProvider {
public: 
  /// Standard constructor
  L0DUMultiConfigProvider( const std::string& type, 
                           const std::string& name,
                           const IInterface* parent);

  virtual ~L0DUMultiConfigProvider( ); ///< Destructor

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  LHCb::L0DUConfig*  config(long tck = LHCb::L0DUTemplateConfig::TCKValue,std::string slot="T0");
  LHCb::L0DUConfigs*  configs(std::string slot="T0"){return m_configs[slot];};

  
protected:

private:
  // private method
  LHCb::L0DUConfig* loadConfig( std::string tck ,std::string slot="T0");

  // attributes
  bool m_preload;
  std::vector<std::string> m_list;

  //
  std::map<std::string,LHCb::L0DUConfigs*> m_configs;
  IL0DUConfigProvider* m_provider;
  std::map<std::string,LHCb::L0DUConfig*> m_template;

};
#endif // L0DUMULTICONFIGPROVIDER_H
