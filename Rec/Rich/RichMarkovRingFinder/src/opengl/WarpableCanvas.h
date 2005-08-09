
#ifndef LESTER_WARPABLECANVAS_H
#define LESTER_WARPABLECANVAS_H

// fwd dec
#include "opengl/WarpableCanvas.fwd" 

// includes
#include <iostream>
#include "opengl/BasicCanvasInterface.h"
#include "Utils/MathsConstants.h"

// declaration
class WarpableCanvas : public Lester::BasicCanvasInterface {
private:
  struct XY {
    GLfloat x;
    GLfloat y;
    XY(const GLfloat xx, const GLfloat yy) : x(xx), y(yy) {
    };
  };
  // returned WARPED coords from FLAT ones
  XY wp(const XY & flt) const {
    if (m_warp) {
      const GLfloat r = sqrt(flt.x*flt.x+flt.y*flt.y);
      const double gamma = m_cs * tanh(r/m_cs)/r;
      return XY(flt.x*gamma, flt.y*gamma);
    } else {
      return flt;
    };
  };
// WarpableCanvas() {};
// WarpableCanvas(const WarpableCanvas & other) {};
// WarpableCanvas & operator=(const WarpableCanvas & other) { return *this; };
// virtual ~WarpableCanvas() {};
  double m_cs;
  bool m_warp;
  BasicCanvasInterface & m_other;
public:
  WarpableCanvas(BasicCanvasInterface & other, const double characteristicSize=1) :
    m_cs(characteristicSize),
    m_warp(true),
    m_other(other) {
  };
  void setWarp(const bool toState=true) {
    m_warp=toState;
  };

  std::ostream & printMeTo(std::ostream & os) const {
    os << "WarpableCanvas[]";
    return os;
  };


  //// Methods of base class:
  void update() {
    m_other.update();
  };
  void clear() {
    m_other.clear();
  };
  void line(const GLfloat x1, const GLfloat y1,
	    const GLfloat x2, const GLfloat y2) {
    m_other.line(x1,y1,x2,y2);
  };
  void line(const GLfloat x1, const GLfloat y1, const GLfloat z1,
		    const GLfloat x2, const GLfloat y2, const GLfloat z2) {
    m_other.line(x1,y1,z1,  x2,y2,z2);
  };
  void colour(const Lester::Colour & col) {
    m_other.colour(col);
  };
  void circle(const GLfloat x1, const GLfloat y1, const GLfloat radius, const int facets = 50) {
    XY last=this->wp(XY(x1+radius, y1));
    const double dth = MathsConstants::twoPi / facets;
    for (double theta=dth; theta<=MathsConstants::twoPi+0.0001; theta+=dth) {
      XY nw = this->wp(XY(x1+radius*cos(theta), y1+radius*sin(theta)));
      m_other.line(last.x,last.y,  nw.x,nw.y);
      last=nw;
    };
  };

};

inline std::ostream & operator<<(std::ostream & os, const WarpableCanvas & obj) {
  return obj.printMeTo(os);
};

#endif

