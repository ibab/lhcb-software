#ifndef HLT_HLTCONFIGURATIONHELPER_H 
#define HLT_HLTCONFIGURATIONHELPER_H 1

#include "HltBase/HltTypes.h"

// Include files

/** @class HltConfigurationHelper HltConfigurationHelper.h 
 *  @author Jose Angel Hernando Morata
 *  @date   2007-06-22
 */

namespace HltConfigurationHelper {
  
  int getID(Hlt::Configuration& conf,
            const std::string& root, const std::string& name);
  
  std::string getName(Hlt::Configuration& conf,
                      const std::string& root, int id);  
};

#endif

