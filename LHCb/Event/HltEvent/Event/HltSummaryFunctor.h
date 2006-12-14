// $Id: HltSummaryFunctor.h,v 1.1 2006-12-14 11:18:42 hernando Exp $
#ifndef EVENT_HLTSUMMARYFUNCTOR_H 
#define EVENT_HLTSUMMARYFUNCTOR_H 1

#include "Event/HltSummary.h"

/** @class HltSummaryFunctor HltSummaryFunctor.h Event/HltSummaryFunctor.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2006-12-12
 */

namespace HltSummaryFunctor 
{

  template <class T>
  void retrieve(const LHCb::HltSelectionSummary& sel, std::vector<T*>& cont) {
    const std::vector<ContainedObject*>& dobjs = sel.data();
    for (std::vector<ContainedObject*>::const_iterator it = dobjs.begin();
         it != dobjs.end(); ++it) {
      ContainedObject* obj = (ContainedObject*) (*it);
      if ((obj)->clID() == T::classID()) {
        T* t = dynamic_cast<T*>(obj);
        cont.push_back(t);
      }
    }
  }
  
  template <class T>
  void retrieve(const LHCb::HltSummary& sum, int idselection, 
                std::vector<T*>& cont) {
    if (!sum.hasSelectionSummary(idselection)) return;
    const LHCb::HltSelectionSummary& sel = sum.selectionSummary(idselection);
    retrieve(sel,cont);
  }

};
#endif // EVENT_HLTSUMMARYFUNCTOR_H
