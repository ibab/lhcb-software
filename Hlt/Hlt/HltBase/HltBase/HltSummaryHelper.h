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

namespace Hlt {
  
  namespace SummaryHelper {
    
    size_t ncandidates(const LHCb::HltSummary& sum, int id) {
      std::string key = " [retrieve] ";
      if (!sum.hasSelectionSummary(id)) throw zen::invalid_key(key);
      const LHCb::HltSelectionSummary& sel = sum.selectionSummary(id);
      return sel.data().size();
    }
    
    template <class T>
    std::vector<T*> retrieve(const LHCb::HltSummary& sum,int id) {
      std::vector<T*> cont;
      std::string key = " [retrieve] ";
      if (!sum.hasSelectionSummary(id)) throw zen::invalid_key(key);
      const LHCb::HltSelectionSummary& sel = sum.selectionSummary(id);
      const std::vector<ContainedObject*>& data =sel.data();
      for (std::vector<ContainedObject*>::const_iterator it = data.begin();
           it != data.end(); ++it) {
        ContainedObject* obj = (ContainedObject*) (*it);
        T* t = dynamic_cast<T*>(obj);
        if (t) cont.push_back(t);
      }
      return cont;
    }
    
  };
};

#endif

