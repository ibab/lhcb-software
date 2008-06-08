

#include "EventDescription.h"

#include "GenericRingFinder/GenericResults.h"
#include "Rich2Inferrer.h"
#include "NimTypeRichModel.h"

std::ostream & operator<<(std::ostream & os, const Lester::EventDescription & rp) {
  return rp.printMeTo(os);
}

namespace Lester {

  /// The fill method fills the "GenRingF::GenericResults" structure with the rings which were found during the fitting procedure.
  void EventDescription::fill(GenRingF::GenericResults & results, 
                              const GenRingF::GenericInput & input, 
                              boost::shared_ptr<NimTypeRichModel> ntrm) const 
  {
    results.rings.clear();
    unsigned int i=0;
    for (Circs::const_iterator it = circs.begin();
         it!=circs.end();
         ++it) {
      results.rings.push_back(GenRingF::GenericRing(GenRingF::GenericRingIndex(i++), 
                                                    it->centre().x(), 
                                                    it->centre().y(), 
                                                    it->radius()));
    };
    results.inferrer = boost::shared_ptr<GenRingF::GenericInferrer>(new Lester::Rich2Inferrer(input, ntrm, results.rings));
  }

  EventDescription::EventDescription(const GenRingF::GenericResults & results) {
    for (GenRingF::GenericResults::GenericRings::const_iterator it = results.rings.begin();
         it!=results.rings.end();
         ++it) {
      circs.push_back(CircleParams(Small2Vector(it->x(), it->y()), it->radius()));
    };
  }

  EventDescription::EventDescription() {
    //Beware! This default description is pathalogical! (partly on purpose)
  }

  std::ostream & EventDescription::printMeTo(std::ostream & os) const {
    int i=0;
    os << "EventDescription[" << std::endl;
    for(Circs::const_iterator it=circs.begin();
        it!=circs.end();
        ++it) {
      os << "circ "<<i<<"="<<*it<<std::endl;
      ++i;
    };
    os << "]";

    return os;
  }

}


