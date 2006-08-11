
#ifndef LESTER_RICH_H
#define LESTER_RICH_H

namespace Lester {
  
  class Rich;
  class EventDescription;
  class Data;

};

#include "CircleParams.h"
#include "Small2Vector.fwd"
#include "GraphicsObjects.h"

namespace Lester {

  class Rich {
  public:
#ifdef LESTER_USE_GRAPHICS
    static void draw(BasicCanvasInterface & canvas, const EventDescription & rp, const Data & d, const bool showTruth=false);
#endif
  };

};

#endif
