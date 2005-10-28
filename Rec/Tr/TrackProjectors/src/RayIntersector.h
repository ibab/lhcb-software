// $Id: RayIntersector.h,v 1.1 2005-10-28 16:02:30 hernando Exp $
#ifndef RAYINTERSECTOR_H 
#define RAYINTERSECTOR_H 1

#include "CLHEP/Geometry/Vector3D.h"

// Include files

/** @class RayIntersector RayIntersector.h
 *
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2005-10-27
 */

typedef HepVector3D EVector;

//! RayIntersector compute the distance between 2 straight lines == rays
//! A ray it is defined with two vectors: a point and a direction
//! and the derivate of the distance respect to the initial variation of the 
//! ray parameters
class RayIntersector {
public:
  
  /// Standard constructor
  RayIntersector()
  {}

  //! Constructor with the reference ray
  RayIntersector(const EVector& x, const EVector& w) 
  {setReferenceRay(x,w);}
  
  //! destructor
  virtual ~RayIntersector( );

  //! set rhe refenrence ray
  void setReferenceRay(const EVector& x, const EVector& w)
  {m_x2 = x;m_w = w;}  

  //! return the distance between 2 rays
  const EVector& distance(const EVector& x1, const EVector& v1,
                          const EVector& x, const EVector& w) {
    setReferenceRay(x,w);
    return distance(x1,v1);
  }
  

  //! return the distance of this ray to respect the reference one
  const EVector& distance(const EVector& x1, const EVector& v1);
  
  //! return the derivative of the distance with respect the derivation
  //! of the parameters of the ray 
  double ddistance(const EVector& dx1, const EVector& dv1) {
    return ddistance(dx1,dv1,EVector(0.,0.,0.),EVector(0.,0.,0.));
  }

  //! return the derivative of the distance with respect the derivation
  //! of the parameters of the ray and the reference ray
  double ddistance(const EVector& dx1, const EVector& dv1,
                   const EVector& dx, const EVector& dw);

protected:

  //! return the distance of this ray to respect the reference one
  double ddistance(const EVector& dx, const EVector& dv1,
                   const EVector& dv2);
  
protected:

  //! parameters of the ray
  EVector m_x1;
  EVector m_v1;

  //! parameters of the reference ray
  EVector m_x2;
  EVector m_w;

  //! paramters of relation 1-2
  EVector m_x;
  EVector m_v2;
  
  //! internal parameters
  double m_v10;
  double m_v20;
  double m_v12;
  double m_v11;
  double m_v22;
  
  double m_D;
  double m_phi1;
  double m_phi2;

  EVector m_distance;

};
#endif // RAYINTERSECTOR_H
