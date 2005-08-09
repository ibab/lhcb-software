
#ifndef LESTER_STREAMCANVASTOOLS_DOUBLEBUFFERINTERFACE_H
#define LESTER_STREAMCANVASTOOLS_DOUBLEBUFFERINTERFACE_H

#include "opengl/BufferInterface.h"

namespace Lester {
  namespace StreamCanvasTools {

    class DoubleBufferInterface : virtual public BufferInterface {
    public:
      virtual void approve() = 0;
      virtual void approveAndClear() = 0;
      /// Upon exit, the argument should point to a Buffer that is GUARANTEED NOT TO CHANGE until the next time one of these calls are made.  One way this could be implemented is if a compete buffer copy was made internally and the pointer left pointing at this.  However, clever implementations might want to create something more clever to avoid the need to copy.
      virtual void putAStableApprovedBufferInto(boost::shared_ptr<BufferInterface> & buffer) = 0;
      virtual ~DoubleBufferInterface() {};
    };

  };
};

#endif
