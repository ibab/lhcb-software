

#ifndef LESTER_HIT_H
#define LESTER_HIT_H

#include "CLHEP/Vector/TwoVector.h"
#include "opengl/BasicCanvasInterface.fwd"
#include "GenericRingFinder/GenericHit.fwd"

namespace Lester {

  class Hit : public Hep2Vector {
  public:
    Hit(const double x,
	const double y);
    Hit(const GenRingF::GenericHit & hit);
#ifdef LESTER_USE_GRAPHICS
    /// scaleFactor allows user to modify default size
    void draw(BasicCanvasInterface & canvas, const double scaleFactor=1.0) const;
#endif
  };

}

#endif
