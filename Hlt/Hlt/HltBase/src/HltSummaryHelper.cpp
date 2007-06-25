#include "HltBase/HltSummaryHelper.h"

// Include files

/** @class HltSelectionHelper HltSelectionHelper.h 
 *  @author Jose Angel Hernando Morata
 *  @date   2007-06-22
 */


size_t HltSummaryHelper::ncandidates(const LHCb::HltSummary& sum, int id) {
  if (!sum.hasSelectionSummary(id)) return 0;
  const LHCb::HltSelectionSummary& sel = sum.selectionSummary(id);
  if (sel.data().size()<=0) return 0;
  ContainedObject* iobj = sel.data().front();
  Hlt::ISizeHolder* isize = dynamic_cast<Hlt::ISizeHolder*>(iobj);
  if (!isize) return 0;
  return isize->size();
}



