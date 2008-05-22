#ifndef HLT_HLTSUMMARYHELPER_H 
#define HLT_HLTSUMMARYHELPER_H 1

#include "HltBase/EAssertions.h"
#include "HltBase/HltTypes.h"
#include "Event/HltSummary.h"

// Include files

/** @class HltSummaryHelper HltSummaryHelper.h 
 *  @author Jose Angel Hernando Morata
 *  @date   2007-06-22
 */

namespace Hlt { namespace SummaryHelper {
      
    template <class T>
    std::vector<T*> retrieve(const LHCb::HltSummary& sum,stringKey id) {
      std::string key = " [retrieve] ";
      if (!sum.hasSelectionSummary(id.str())) throw zen::invalid_key(" [retrieve] ");
      const LHCb::HltSelectionSummary& sel = sum.selectionSummary(id.str());
      const std::vector<ContainedObject*>& data = sel.data();
      std::vector<T*> cont;
      for (std::vector<ContainedObject*>::const_iterator it = data.begin();
           it != data.end(); ++it) {
        T* t = dynamic_cast<T*>(*it);
        if (t) cont.push_back(t);
      }
      return cont;
    }
    
}; };

#endif

