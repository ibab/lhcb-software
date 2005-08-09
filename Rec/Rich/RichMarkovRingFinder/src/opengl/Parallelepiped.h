#ifndef LESTER_STREAMCANVASTOOLS_PARALLELEPIPED_H
#define LESTER_STREAMCANVASTOOLS_PARALLELEPIPED_H

#include "opengl/Drawable.h"

namespace Lester {
  namespace StreamCanvasTools {

    class Parallelepiped : public Drawable {
    public:
      Parallelepiped(const Hep3Vector & corner,
		     const Hep3Vector & edge1,
		     const Hep3Vector & edge2,
		     const Hep3Vector & edge3) :
	m_corner(corner),
	m_edge1(edge1),
	m_edge2(edge2),
	m_edge3(edge3) {
      };
    private:
      void drawOn(Lester::Canvas * canvas) const {
	
	Line(m_corner, m_corner+m_edge1).drawOn(canvas);
	Line(m_corner, m_corner+m_edge2).drawOn(canvas);
	Line(m_corner, m_corner+m_edge3).drawOn(canvas);
	
	Line(m_corner+m_edge1,m_corner+m_edge1+m_edge2).drawOn(canvas);
	Line(m_corner+m_edge1,m_corner+m_edge1+m_edge3).drawOn(canvas);
	Line(m_corner+m_edge2,m_corner+m_edge2+m_edge1).drawOn(canvas);
	Line(m_corner+m_edge2,m_corner+m_edge2+m_edge3).drawOn(canvas);
	Line(m_corner+m_edge3,m_corner+m_edge3+m_edge1).drawOn(canvas);
	Line(m_corner+m_edge3,m_corner+m_edge3+m_edge2).drawOn(canvas);
	
	const Hep3Vector farCorner = m_corner+m_edge1+m_edge2+m_edge3;
	
	Line(farCorner, farCorner-m_edge1).drawOn(canvas);
	Line(farCorner, farCorner-m_edge2).drawOn(canvas);
	Line(farCorner, farCorner-m_edge3).drawOn(canvas);
      };
    private:
      const Hep3Vector m_corner;
      const Hep3Vector m_edge1;
      const Hep3Vector m_edge2;
      const Hep3Vector m_edge3;
    };
    
  };
};

#endif
