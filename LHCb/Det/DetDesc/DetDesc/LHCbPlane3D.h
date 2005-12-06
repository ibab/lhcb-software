// -*- C++ -*-
// $Id: LHCbPlane3D.h,v 1.2 2005-12-06 18:51:27 jpalac Exp $
// ---------------------------------------------------------------------------
//
/** @class Plane3D LHCbPlane3D.h DetDesc/LHCbPlane3D.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2005-12-05
 *
 * Modification of CLHEP/Geometry/Plane3D.h to work with ROOT::MathCore Vector, 
 * Point and Transformation classes. Meant to be a temporary solution.
 * while ROOT::Math::Plane3D is implemented
 * Original comments follow below:
 */
//
//
// This file is a part of the CLHEP - a Class Library for High Energy Physics.
//
// History:
// 22.09.96 E.Chernyaev - initial version
// 19.10.96 J.Allison - added == and <<.
// 15.04.03 E.Chernyaev - CLHEP-1.9: template version

#ifndef LHCB_PLANE3D_H
#define LHCB_PLANE3D_H

#include <iosfwd>
#include "Kernel/Point3DTypes.h"
#include "Kernel/Vector3DTypes.h"
#include "Kernel/Transform3DTypes.h"

namespace DetDesc {

  /**
   * Template class for geometrical plane in 3D.
   *
   * @author Evgeni Chernyaev <Evgueni.Tcherniaev@cern.ch>
   * @ingroup geometry
   */
  class Plane3D {
  protected:
    double a_, b_, c_, d_;
 
  public:
    /**
     *  Default constructor - creates plane z=0. */
    Plane3D() : a_(0.), b_(0.), c_(1.), d_(0.) {}

    /**
     * Constructor from four numbers - creates plane a*x+b*y+c*z+d=0. */
    Plane3D(double a, double b, double c, double d) : a_(a), b_(b), c_(c), d_(d) {}

    /**
     * Constructor from normal and point. */
    Plane3D(const Gaudi::XYZVector& n, const Gaudi::XYZPoint& p)
      : a_(n.x()), b_(n.y()), c_(n.z()), d_(-n.Dot(p) ) {}

    /**
     * Constructor from three points. */
    Plane3D(const Gaudi::XYZPoint& p1, 
            const Gaudi::XYZPoint& p2, 
            const Gaudi::XYZPoint& p3) {
      Gaudi::XYZVector n = (p2-p1).Cross(p3-p1);
      a_ = n.x(); b_ = n.y(); c_ = n.z(); d_ = -n.Dot(p1);
    }

    /** Copy constructor.
     * Plane3D<double> has two constructors:
     * from Plane3D<double> (provided by compiler) and
     * from Plane3D<float> (defined in this file).
     * Plane3D<float> has only the last one.
     */
    Plane3D(const Plane3D & p)
      : a_(p.a_), b_(p.b_), c_(p.c_), d_(p.d_) {}

    /**
     * Destructor. */
    ~Plane3D() {};

    /**
     * Assignment. */
    Plane3D & operator=(const Plane3D & p) {
      a_ = p.a_; b_ = p.b_; c_ = p.c_; d_ = p.d_; return *this;
    }

    /**
     * Returns the a-coefficient in the plane equation: a*x+b*y+c*z+d=0. */
    double a() const { return a_; }
    /**
     * Returns the b-coefficient in the plane equation: a*x+b*y+c*z+d=0. */
    double b() const { return b_; }
    /**
     * Returns the c-coefficient in the plane equation: a*x+b*y+c*z+d=0. */
    double c() const { return c_; }
    /**
     * Returns the free member of the plane equation: a*x+b*y+c*z+d=0. */
    double d() const { return d_; }

    /**
     * Returns normal. */
    Gaudi::XYZVector normal() const { 
      return Gaudi::XYZVector(a_,b_,c_); }

    /**
     * Normalization. */
    Plane3D & normalize() {
      double ll = sqrt(a_*a_ + b_*b_ + c_*c_);
      if (ll > 0.) { a_ /= ll; b_ /= ll; c_ /= ll, d_ /= ll; }
      return *this;
    }

    /**
     * Returns distance to the point. */
    double distance(const Gaudi::XYZPoint& p) const {
      return a()*p.x() + b()*p.y() + c()*p.z() + d();
    }

    /**
     * Returns projection of the point to the plane. */
    Gaudi::XYZPoint point(const Gaudi::XYZPoint& p) const {
      double k = distance(p)/(a()*a()+b()*b()+c()*c());
      return Gaudi::XYZPoint(p.x()-a()*k, p.y()-b()*k, p.z()-c()*k);
    }

    /**
     * Returns projection of the origin to the plane. */
    Gaudi::XYZPoint point() const {
      double k = -d()/(a()*a()+b()*b()+c()*c());
      return Gaudi::XYZPoint(a()*k, b()*k, c()*k);
    }

    /**
     * Test for equality. */
    bool operator == (const Plane3D & p) const {
      return a() == p.a() && b() == p.b() && c() == p.c() && d() == p.d();
    }

    /**
     * Test for inequality. */
    bool operator != (const Plane3D & p) const {
      return a() != p.a() || b() != p.b() || c() != p.c() || d() != p.d();
    }

    /**
     * Transformation by Transform3D. */
    Plane3D & transform(const Gaudi::Transform3D & m) {
      Gaudi::XYZVector n = m*normal();
      //      n.transform(m);
      d_ = -n.Dot( m*point() ); a_ = n.x(); b_ = n.y(); c_ = n.z();
      return *this;
    }
  };

  /**
   * Output to the stream.
   * @relates Plane3D
   */
  std::ostream & operator<<(std::ostream & os, const Plane3D & p);


} /* namespace Gaudi */

#endif /* LHCB_PLANE3D_H */
