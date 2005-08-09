
#ifndef LESTER_STREAMCANVASTOOLS_OSTREAMCANVAS_H
#define LESTER_STREAMCANVASTOOLS_OSTREAMCANVAS_H

#include <ostream>
#include "opengl/StreamCanvasTokens.h"
#include "opengl/BasicCanvasInterface.h"

namespace Lester {
  namespace StreamCanvasTools {
    
    class OstreamCanvas : virtual public BasicCanvasInterface {
    private:
      std::ostream & m_os;
    protected:
      std::ostream & os() {
	return m_os;
      };
    public:
      OstreamCanvas(std::ostream & os) : 
	m_os(os) {
      };
    public:
      // implement BasicCanvasInterface:
      typedef BasicCanvasInterface::GLfloat GLfloat;
      virtual void colour(const Lester::Colour & col) {
	os() << Tokens::colour << Tokens::separator
	     << col.getRed() << Tokens::separator
	     << col.getGreen() << Tokens::separator
	     << col.getBlue() << Tokens::terminator;
      };
      virtual void circle(const GLfloat x1, const GLfloat y1,
			const GLfloat radius, const int facets = 50) {
	os() << Tokens::circle << Tokens::separator 
	     << x1 << Tokens::separator
	     << y1 << Tokens::separator
	     << radius << Tokens::separator
	     << facets << Tokens::terminator;
      };
      virtual void line(const GLfloat x1, const GLfloat y1,
			const GLfloat x2, const GLfloat y2) {
	os() << Tokens::line2d << Tokens::separator 
	     << x1 << Tokens::separator
	     << y1 << Tokens::separator
	     << x2 << Tokens::separator
	     << y2 << Tokens::terminator;
      };
      virtual void line(const GLfloat x1, const GLfloat y1, const GLfloat z1,
			const GLfloat x2, const GLfloat y2, const GLfloat z2) {
	os() << Tokens::line3d << Tokens::separator 
	     << x1 << Tokens::separator
	     << y1 << Tokens::separator
	     << z1 << Tokens::separator
	     << x2 << Tokens::separator
	     << y2 << Tokens::separator
	     << z2 << Tokens::terminator;
      };
      virtual void clear() {
	os() << Tokens::clear << Tokens::terminator;
      };
      virtual void update() {
	os() << std::flush;
      };
    };

  };
};

#endif
