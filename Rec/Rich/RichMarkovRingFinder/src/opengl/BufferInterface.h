
#ifndef LESTER_STREAMCANVASTOOLS_BUFFERINTERFACE_H
#define LESTER_STREAMCANVASTOOLS_BUFFERINTERFACE_H

#include "opengl/Drawable.h"
#include "opengl/Canvas.h"

namespace Lester {
  namespace StreamCanvasTools {

    class BufferInterface {
    public:
      virtual void clear() = 0;
      virtual void add(boost::shared_ptr<Drawable> d) = 0;
      virtual void flushDrawablesToCanvas(Lester::Canvas * canvas) const = 0;
      virtual ~BufferInterface() {};
    };

  };
};

#endif
