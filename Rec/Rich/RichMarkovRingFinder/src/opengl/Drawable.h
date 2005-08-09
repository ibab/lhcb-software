#ifndef LESTER_STREAMCANVASTOOLS_DRAWABLE_H
#define LESTER_STREAMCANVASTOOLS_DRAWABLE_H

#include "opengl/Canvas.fwd"

namespace Lester {
  namespace StreamCanvasTools {

    class Drawable {
    public:
      virtual void drawOn(Lester::Canvas * canvas) const = 0;
      virtual ~Drawable() {};
    };
    
  };
};

#endif
