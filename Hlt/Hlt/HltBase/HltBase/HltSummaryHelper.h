#ifndef HLT_HLTSUMMARYHELPER_H 
#define HLT_HLTSUMMARYHELPER_H 1

#include "HltBase/HltTypes.h"
#include "Event/HltSummary.h"

// Include files

/** @class HltSummaryHelper HltSummaryHelper.h 
 *  @author Jose Angel Hernando Morata
 *  @date   2007-06-22
 */

namespace HltSummaryHelper {

  //! exception class: not valid key
  class invalid_key : public std::logic_error {
  public:
    invalid_key(const std::string& key):logic_error(key) {
      std::cout << "--ERROR: no selection in summary " << key << std::endl;
    }
  };

  //! exception class: not container
  class invalid_container : public std::logic_error {
  public:
    invalid_container(const std::string& key):logic_error(key) {
      std::cout << "--ERROR: no container in selection " << key << std::endl;
    }
  };

  template <class CONT>
  const CONT& retrieve(const LHCb::HltSummary& sum,int id) {
    std::string key = " [retrieve] ";
    if (!sum.hasSelectionSummary(id)) throw invalid_key(key);
    const LHCb::HltSelectionSummary& sel = sum.selectionSummary(id);
    if (sel.data().size()<=0) throw invalid_container(key);
    ContainedObject* obj = sel.data().front();
    Hlt::DataSizeHolder< CONT >* holder = 
      dynamic_cast< Hlt::DataSizeHolder<CONT>*>(obj);
    if (!holder) throw invalid_container(key);
    return holder->object();
  }

  size_t ncandidates(const LHCb::HltSummary& sum, int id);
  

 //  template <class CONT>
//   bool register(LHCb::HltSummary& sum, int id, CONT& con) {
//     Hlt::DataHolder<CONT>* holder = new Hlt::DataHolder<CONT>(*con);
//     if (!sum.hasSelectionSummary(id)) return false;
//     LHCb::HltSelectionSummary& sel = sum.selectionSummary(id);    
//     sel.addData(*holder); 
//     return true;
//   }
  
};

#endif

