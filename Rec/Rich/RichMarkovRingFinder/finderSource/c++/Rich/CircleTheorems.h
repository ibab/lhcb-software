
#ifndef LESTER_CIRCLE_THEOREMS_H
#define LESTER_CIRCLE_THEOREMS_H

#include "Small2Vector.fwd"
#include "Small3Vector.fwd"

namespace Lester {

  class CircleTheorems {
  public:
    class PointAtInfinity {
    };
    class RadiusIsInfinite {
    };

    /// Returns would-be z-cpt of cross product if the 2-vecs
    /// were extended to 3-vecs
    static double twoVectorCross(const Small2Vector & a,
                                 const Small2Vector & b);
    static Small3Vector dumbCentreOfCircleThrough(const Small3Vector & a,
                                                const Small3Vector & b,
                                                const Small3Vector & c);
    static Small2Vector centreOfCircleThrough(const Small2Vector & a,
                                            const Small2Vector & b,
                                            const Small2Vector & c);
    static double radiusOfCircleThrough(const Small2Vector & a,
                                        const Small2Vector & b,
                                        const Small2Vector & c);
  private:
    /// Thanks to Robert (Bob) Yunken (Pen.State.U / U.Western.Australia / Perth) for lots of help with this routine!
    static double fNumerator(const Small2Vector & a,
                             const Small2Vector & b,
                             const Small2Vector & c,
                             const double aSq,
                             const double bSq,
                             const double cSq);
    /// Thanks to Robert (Bob) Yunken (Pen.State.U / U.Western.Australia / Perth) for lots of help with this routine!
    static double fDenominator(const Small2Vector & a,
                               const Small2Vector & b,
                               const Small2Vector & c);
    /// Thanks to Robert (Bob) Yunken (Pen.State.U / U.Western.Australia / Perth) for lots of help with this routine!
    static double fDumb(const Small3Vector & a,
                        const Small3Vector & b,
                        const Small3Vector & c);
  };

}

#endif
