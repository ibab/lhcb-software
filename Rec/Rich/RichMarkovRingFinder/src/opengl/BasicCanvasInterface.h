#ifndef LESTER_OPENGL_BASICCANVASINTERFACE_H
#define LESTER_OPENGL_BASICCANVASINTERFACE_H

#include "opengl/Colour.fwd"

namespace Lester {

  class BasicCanvasInterface {
  public:
    typedef float GLfloat;
  public:
    virtual void line(const GLfloat x1, const GLfloat y1,
		      const GLfloat x2, const GLfloat y2) = 0;
    virtual void line(const GLfloat x1, const GLfloat y1, const GLfloat z1,
		      const GLfloat x2, const GLfloat y2, const GLfloat z2) = 0;
    virtual void clear() = 0;
    virtual void update() = 0;
    virtual void colour(const Lester::Colour & col) = 0;
    virtual void circle(const GLfloat x1, const GLfloat y1, const GLfloat radius, const int facets = 50) = 0;
  };

};

#endif
