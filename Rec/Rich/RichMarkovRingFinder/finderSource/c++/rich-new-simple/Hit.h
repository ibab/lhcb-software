

#ifndef LESTER_HIT_H
#define LESTER_HIT_H

#include "Constants.h"

#include "CLHEP/Vector/TwoVector.h"
#include "GraphicsObjects.h"
#include "GenericRingFinder/GenericHit.h"

namespace Lester {

  class Hit : public Hep2Vector {
  public:
    Hit(const double x,
	const double y) :
      Hep2Vector(x,y) {
    };
    Hit(const GenRingF::GenericHit & hit) :
	Hep2Vector(hit.x(), hit.y()) {
    };
#ifdef LESTER_USE_GRAPHICS
    /// scaleFactor allows user to modify default size
    void draw(BasicCanvasInterface & canvas, const double scaleFactor=1.0) const {
      const double dotRad=0.005*Constants::viewRangeParameter*scaleFactor;
      canvas.circle(x(),y(),dotRad);
     };
#endif
  };
};

#endif
