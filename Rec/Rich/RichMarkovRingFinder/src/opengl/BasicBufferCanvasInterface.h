#ifndef LESTER_STREAMCANVASTOOLS_BASICBUFFERCANVASINTERFACE_H
#define LESTER_STREAMCANVASTOOLS_BASICBUFFERCANVASINTERFACE_H

#include "opengl/BasicCanvasInterface.h"

namespace Lester {
  namespace StreamCanvasTools {
    
    class BasicBufferCanvasInterface : virtual public BasicCanvasInterface {
    public:
      virtual void approve() = 0;
      virtual void approveAndClear() = 0;
    };

  };
};

#endif
