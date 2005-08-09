#ifndef LESTER_STREAMCANVASTOOLS_LINE_H
#define LESTER_STREAMCANVASTOOLS_LINE_H

#include "opengl/Drawable.h"

namespace Lester {
  namespace StreamCanvasTools {

  class Line : public Drawable {
  public:
    Line(double x1, double y1,
	 double x2, double y2) :
      m_x1(x1), m_y1(y1), m_z1(0),
      m_x2(x2), m_y2(y2), m_z2(0) {
    };
    Line(double x1, double y1, double z1,
	 double x2, double y2, double z2) :
      m_x1(x1), m_y1(y1), m_z1(z1),
      m_x2(x2), m_y2(y2), m_z2(z2) {
    };
    Line(const Hep3Vector & from,
	 const Hep3Vector & to) :
           m_x1(from.x()), m_y1(from.y()), m_z1(from.z()),
	   m_x2(  to.x()), m_y2(  to.y()), m_z2(  to.z()) {
    };
  private:
    const double m_x1,m_y1,m_z1,m_x2,m_y2,m_z2;
  public:
    void drawOn(Lester::Canvas * canvas) const {
      canvas->line(m_x1,m_y1,m_z1,
		   m_x2,m_y2,m_z2);
    };
  };

    
  };
};

#endif
