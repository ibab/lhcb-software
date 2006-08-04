
#ifndef LESTER_CIRCLE_THEOREMS_H
#define LESTER_CIRCLE_THEOREMS_H

class Hep2Vector; //#include "CLHEP/Vector/TwoVector.h"
class Hep3Vector; //#include "CLHEP/Vector/ThreeVector.h"

namespace Lester {
  
  class CircleTheorems {
  public:
    class PointAtInfinity {
    };
    class RadiusIsInfinite {
    };
    /// Returns would-be z-cpt of cross product if the 2-vecs
    /// were extended to 3-vecs
    static double twoVectorCross(const Hep2Vector & a,
				 const Hep2Vector & b);
    static Hep3Vector dumbCentreOfCircleThrough(const Hep3Vector & a,
						const Hep3Vector & b,
						const Hep3Vector & c);
    static Hep2Vector centreOfCircleThrough(const Hep2Vector & a,
					    const Hep2Vector & b,
					    const Hep2Vector & c);
    static double radiusOfCircleThrough(const Hep2Vector & a,
					const Hep2Vector & b,
					const Hep2Vector & c);
  private:
    /// Thanks to Robert (Bob) Yunken (Pen.State.U / U.Western.Australia / Perth) for lots of help with this routine!
    static double fNumerator(const Hep2Vector & a,
			     const Hep2Vector & b,
			     const Hep2Vector & c,
			     const double aSq,
			     const double bSq,
			     const double cSq);
    /// Thanks to Robert (Bob) Yunken (Pen.State.U / U.Western.Australia / Perth) for lots of help with this routine!
    static double fDenominator(const Hep2Vector & a,
			       const Hep2Vector & b,
			       const Hep2Vector & c);
    /// Thanks to Robert (Bob) Yunken (Pen.State.U / U.Western.Australia / Perth) for lots of help with this routine!
    static double fDumb(const Hep3Vector & a,
			const Hep3Vector & b,
			const Hep3Vector & c);
  };

}

#endif
