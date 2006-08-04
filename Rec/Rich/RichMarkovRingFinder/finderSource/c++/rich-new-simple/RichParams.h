
#ifndef LESTER_RICH_PARAMS_H
#define LESTER_RICH_PARAMS_H

#include <iostream>
#include <vector>
#include "CircleParams.h"
#include "GenericRingFinder/GenericResults.fwd"
#include "GenericRingFinder/GenericInput.fwd"

namespace Lester {
  
  class RichParams {
  public: // ?? not private ??
    typedef std::vector<CircleParams> Circs;
    Circs circs;
    //static const bool backgroundIsVariable = false;
    double meanBackground;
  public:
    RichParams();
    RichParams(const GenRingF::GenericResults & results);
    /// The fill method fills the "GenRingF::GenericResults" structure with the rings which were found during the fitting procedure.
    void fill(GenRingF::GenericResults & results, const GenRingF::GenericInput & input) const;
    double priorProbability() const;
    bool operator==(const RichParams & other) const;
    std::ostream & printMeTo(std::ostream & os) const;
    double sigmaMu() const;
    double prob(const Hep2Vector & p) const;
    void jokeSetRandom /* deprecated! */ ();
    RichParams jitterSymm1() const;
    RichParams jitterSymm2() const;
#ifdef LESTER_USE_GRAPHICS
    void draw(BasicCanvasInterface & canvas) const;
#endif
  };
    
}

std::ostream & operator<<(std::ostream & os, const Lester::RichParams & rp);

#endif
