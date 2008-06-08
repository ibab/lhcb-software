
#ifndef LESTER_EVENT_DESCRIPTION_H
#define LESTER_EVENT_DESCRIPTION_H

#include <iostream>
#include <vector>
#include "CircleParams.h"
#include "GenericRingFinder/GenericResults.fwd"
#include "GenericRingFinder/GenericInput.fwd"
#include "NimTypeRichModel.fwd"
#include <boost/shared_ptr.hpp>

namespace Lester {
  
  class EventDescription 
  {
  public: // ?? not private ??
    typedef std::vector<CircleParams> Circs;
    Circs circs;
    //static const bool backgroundIsVariable = false;
    //double meanBackground;
  public:
    EventDescription(); /// warning the default constructor will probably be pathological.
    EventDescription(const GenRingF::GenericResults & results);
    /// The fill method fills the "GenRingF::GenericResults" structure with the rings which were found during the fitting procedure.
    void fill(GenRingF::GenericResults & results,
	      const GenRingF::GenericInput & input,
	      boost::shared_ptr<NimTypeRichModel> ntrm) const;
    std::ostream & printMeTo(std::ostream & os) const;
  };
    
}

std::ostream & operator<<(std::ostream & os, const Lester::EventDescription & rp);

#endif
