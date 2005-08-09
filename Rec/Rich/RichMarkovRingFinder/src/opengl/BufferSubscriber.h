#ifndef LESTER_STREAMCANVASTOOLS_BUFFERSUBSCRIBER_H
#define LESTER_STREAMCANVASTOOLS_BUFFERSUBSCRIBER_H

#include "opengl/DoubleBuffer.h"

namespace Lester {
  namespace StreamCanvasTools {

    /// A class wanting to know about buffer changes should inherit from this interface and implement the methods herein, which will be called by a BufferProvider when the relevant changes occur.
    class BufferSubscriber {
    public:
      /// Should really be called requestSimpleRedraw
      virtual void notifyOfAlterationToScratchBufferIn(boost::shared_ptr<DoubleBuffer> buffer) = 0;
      virtual void notifyOfNewApprovedBufferIn(boost::shared_ptr<DoubleBuffer> buffer) = 0;
      // Should really remove this method altogether and create a new classes called "ViewSubscriber"/"ViewProvider" to sed/receive update suggestions
      //virtual void notifyOfBufferSuggestion(std::string & suggestion) = 0;
    };
  };
};

#endif
