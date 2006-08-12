

#include "EventDescription.h"

#include "GenericRingFinder/GenericResults.h"
//#include "GenericRingFinder/GenericRingIndex.h"
#include "Rich2Inferrer.h"
#include "NimTypeRichModel.h"

std::ostream & operator<<(std::ostream & os, const Lester::EventDescription & rp) {
  return rp.printMeTo(os);
}

namespace Lester {
  
  /// The fill method fills the "GenRingF::GenericResults" structure with the rings which were found during the fitting procedure.
  void EventDescription::fill(GenRingF::GenericResults & results, const GenRingF::GenericInput & input, boost::shared_ptr<NimTypeRichModel> ntrm) const {
    results.rings.clear();
    unsigned int i=0;
    for (Circs::const_iterator it = circs.begin();
	 it!=circs.end();
	 ++it) {
      results.rings.push_back(GenRingF::GenericRing(GenRingF::GenericRingIndex(i++), it->centre().x(), it->centre().y(), it->radius()));
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

  /* NOT NEEDED AT PRESENT ...
  bool EventDescription::operator==(const EventDescription & other) const {
    return 
      circs==other.circs && 
      meanBackground == other.meanBackground;
  }
  */

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

  /*
  EventDescription EventDescription::jitterSymm1() const {
    // Jitter ALL circles
    EventDescription rp;
    for (Circs::const_iterator it=circs.begin();
	 it!=circs.end();
	 ++it) {
      rp.circs.push_back(it->jitterSymm1());
    };
    return rp;
  }
  */

  //
  //inline double suggestJitteredBg() const {
  //assert(backgroundIsVariable); // it makes no sense to have called this routine otherwise!
  //const double trialnewbg = (meanBackground+0.3*RandGauss::shoot());
  //const double ans = ((trialnewbg>0)?trialnewbg:meanBackground);
  //return ans;
  //};
  //

  /* Not needed now:
  EventDescription EventDescription::jitterSymm2() const {
    // Jitter SOME circles and or the bakckground
    const int siz = circs.size();
    const int sizPlusOptionalOne = siz; // the 1 is for the background!
    if (sizPlusOptionalOne>0) {
      const int numToJitter = lestersRandomNumberToJitter1(sizPlusOptionalOne);
      EventDescription rp=*this;
      const std::vector<int> toJitter 
	= getSetOfMDifferentIntsFromN(sizPlusOptionalOne, numToJitter);
      //std::cerr<<"j[" << numToJitter<<"]";
      for (int i=0; i<numToJitter; ++i) {
	const int toJit = toJitter[i];
	// toJit ALWAYS represents a circle
	assert(toJit>=0 && toJit<siz);
	rp.circs[toJit]=(rp.circs[toJit].jitterSymm1());	    
      };      
      return rp;
    } else {
      return *this; // LESTER STRANGE
    };
  } */

#ifdef LESTER_USE_GRAPHICS
  void EventDescription::draw(BasicCanvasInterface & canvas) const {
    for (Circs::const_iterator it=circs.begin();
	 it!=circs.end();
	 ++it) {
      it->draw(canvas);
    };
  }
#endif


}


