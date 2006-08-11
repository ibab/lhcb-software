

#ifndef LESTER_HIT_H
#define LESTER_HIT_H

#include "Small2Vector.h"
// Note to CGL : I don't want to put ANYTHING in your opengl dir into CVS, hence the need for this ifdef
#ifdef LESTER_USE_GRAPHICS
#include "opengl/BasicCanvasInterface.fwd"
#endif
#include "GenericRingFinder/GenericHit.fwd"

namespace Lester {

  class Hit : public Small2Vector {
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
