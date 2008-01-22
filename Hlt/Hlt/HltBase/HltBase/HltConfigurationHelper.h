#ifndef HLT_HLTCONFIGURATIONHELPER_H 
#define HLT_HLTCONFIGURATIONHELPER_H 1

#include "HltBase/HltTypes.h"

// Include files

/** @class HltConfigurationHelper HltConfigurationHelper.h 
 *  @author Jose Angel Hernando Morata
 *  @date   2007-06-22
 */

namespace Hlt 
{
  namespace ConfigurationHelper {
    
    int getID(Hlt::Configuration& conf,
              const std::string& root, const std::string& key);
    
    std::string getName(Hlt::Configuration& conf,
                        const std::string& root, int id);  
    
    bool validID(Hlt::Configuration& conf,
                 const std::string& root, const std::string& key);
  }
}

#endif

