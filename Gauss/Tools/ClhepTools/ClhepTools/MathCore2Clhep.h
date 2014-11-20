// $Id: MathCore2Clhep.h,v 1.2 2007-06-22 16:22:13 gcorti Exp $
#ifndef CLHEPTOOLS_MATHCORE2CLHEP_H 
#define CLHEPTOOLS_MATHCORE2CLHEP_H 1

// Include files
// MathLib
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Transform3DTypes.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Transform3D.h"

/** @namespace math2clhep MathCore2Clhep.h ClhepTools/MathCore2Clhep.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2005-12-13
 */
namespace LHCb {

  namespace math2clhep 
  {
    static HepGeom::Transform3D transform3D(const Gaudi::Transform3D& tr) {
        using namespace CLHEP;
        double xx, xy, xz, dx, yx, yy, yz, dy, zx, zy, zz, dz;
        
        tr.GetComponents(xx, xy, xz, dx, yx, yy, yz, dy, zx, zy, zz, dz );

        return HepGeom::Transform3D(HepRotation( HepRep3x3(xx, xy, xz,
                                                           yx, yy, yz,
                                                           zx, zy, zz) ),
                                    Hep3Vector(dx, dy, dz) );
    }

    template<class aVector>
    static HepGeom::Vector3D<double> vector3D(const aVector& v) {
      return HepGeom::Vector3D<double>( v.x(), v.y(), v.z() );
    }

    template<class aPoint>
    static HepGeom::Point3D<double> point3D(const aPoint& p) {
      return HepGeom::Point3D<double>( p.x(), p.y(), p.z() );
    }
    
  }
  

}


#endif // CLHEPTOOLS_MATHCORE2CLHEP_H
