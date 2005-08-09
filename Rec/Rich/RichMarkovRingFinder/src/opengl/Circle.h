#ifndef LESTER_STREAMCANVASTOOLS_CIRCLE_H
#define LESTER_STREAMCANVASTOOLS_CIRCLE_H

#include "opengl/Drawable.h"

namespace Lester {
  namespace StreamCanvasTools {

    class Circle : public Drawable {
    public:
      Circle(double x1, double y1,
	     double r, unsigned int facets) :
	m_x1(x1), m_y1(y1), m_z1(0),
	m_r(r),m_facets(facets) {
      };
    private:
      const double m_x1,m_y1,m_z1,m_r;
      const unsigned int m_facets;
      void drawOn(Lester::Canvas * canvas) const {
	canvas->circle(m_x1,m_y1,m_r,m_facets);
      };
    };
    
  };
};

#endif
