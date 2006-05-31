// $Id: Line.h,v 1.1 2006-05-31 13:38:31 jpalac Exp $
#ifndef KERNEL_LINE_H 
#define KERNEL_LINE_H 1

// Include files

#include <iostream>

// #include "Kernel/Point3DTypes.h"
// #include "Kernel/Vector3DTypes.h"

/** @class Line Line.h Kernel/Line.h
 *  
 *  A very simple class to describe a 3D line.
 *  Based on line equation P = P0 + mu*V0 where V0 is the vector P1-P0,
 *  P0 and P1 being two points on the line.
 *  
 *  The class can be used as a very general abstraction for a 3D line.
 *  However, as there is only const access to the first point given in the 
 *  constructor, this can take the meaning of the origin of a ray. The
 *  direction vector also only has const access. It can therefore be safely
 *  used to define the scale of steps along the line. Users requiring a 
 *  normalised direction vector should construct the line using one.
 *
 *  @author Juan PALACIOS
 *  @date   2006-04-19
 */

namespace Gaudi{

  template<typename aPoint, typename aVector>
  class Line {
  public: 

    typedef aPoint  Point;
    typedef aVector Vector;
      
    Line() 
    {
    }
      

    Line(const aPoint& p0, const aVector& v0)
      :
      m_p0(p0),
      m_v0(v0)
    {
    }
      
    Line(const aPoint& p0, const aPoint& p1) 
      :
      m_p0(p0),
      m_v0(p1-p0)
    {
    }

    /// Return the point of origin
    const aPoint&  beginPoint()    const { return m_p0; }
    /// Return the direction vector of the line
    const aVector& direction() const { return m_v0; }

    /// Return a point on the line tick direction vectors away
    /// from point of origin
    aPoint position(const double tick) const {
      return beginPoint() + direction()*tick;
    }

    inline std::ostream& fillStream(std::ostream& os) const {
      os << "\np0 (" 
         << m_p0.x() << " " << m_p0.y() << " " << m_p0.z()
         << ") direction (" 
         << m_v0.x()<< " " << m_v0.y() << " " << m_v0.z() << ")\n"
         << std::endl;
      return os;
    }

  private:

    aPoint m_p0;
    aVector m_v0;

  }; // Line class
  template<typename aPoint, typename aVector>
  inline std::ostream& operator <<(std::ostream& os, const Line<aPoint,
                                   aVector>& rhs) {
    return rhs.fillStream(os);
  }
    


}; // Gaudi namespace
#endif // KERNEL_LINE_H
