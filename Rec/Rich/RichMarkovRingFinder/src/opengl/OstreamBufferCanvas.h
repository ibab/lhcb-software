
#ifndef LESTER_STREAMCANVASTOOLS_OSTREAMBUFFERCANVAS_H
#define LESTER_STREAMCANVASTOOLS_OSTREAMBUFFERCANVAS_H

#include "opengl/StreamCanvasTokens.h"
#include "opengl/BasicBufferCanvasInterface.h"
#include "opengl/OstreamCanvas.h"

namespace Lester {
  namespace StreamCanvasTools {
    
    class OstreamBufferCanvas : 
      public OstreamCanvas,
      virtual public BasicBufferCanvasInterface {
    public:
      OstreamBufferCanvas(std::ostream & os) : 
	OstreamCanvas(os) {
      };
    public:
      // implement BasicBufferCanvasInterface:
      virtual void approve() {
	os() << Tokens::approve << Tokens::terminator;
      };
      virtual void approveAndClear() {
	os() << Tokens::approveAndClear << Tokens::terminator;
      };
    };

  };
};

#endif
