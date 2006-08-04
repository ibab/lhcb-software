
#include "Hit.h"

#include "GenericRingFinder/GenericHit.h"
// Note to CGL : I don't want to put ANYTHING in your opengl dir into CVS, hence the need for this ifdef
#ifdef LESTER_USE_GRAPHICS
#include "opengl/BasicCanvasInterface.h"
#endif
#include "Constants.h"

namespace Lester {

  Hit::Hit(const double x,
           const double y) :
    Hep2Vector(x,y) {
  }

  Hit::Hit(const GenRingF::GenericHit & hit) :
    Hep2Vector(hit.x(), hit.y()) {
  }

#ifdef LESTER_USE_GRAPHICS
  /// scaleFactor allows user to modify default size
  void Hit::draw(BasicCanvasInterface & canvas, const double scaleFactor) const {
    const double dotRad=0.005*Constants::viewRangeParameter*scaleFactor;
    canvas.circle(x(),y(),dotRad);
  }
#endif

}
