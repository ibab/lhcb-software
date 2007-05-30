// @(#)root/mathcore:$Name: not supported by cvs2svn $:$Id: RotationZYX.cpp,v 1.1 2007-05-30 14:49:05 jpalac Exp $
// Authors: W. Brown, M. Fischler, L. Moneta    2005  

 /**********************************************************************
  *                                                                    *
  * Copyright (c) 2005 , LCG ROOT FNAL MathLib Team                    *
  *                                                                    *
  *                                                                    *
  **********************************************************************/

// Implementation file for rotation in 3 dimensions, represented by RotationZYX
//
// Created by: Lorenzo Moneta, May 23 2007
//
// Last update: $Id: RotationZYX.cpp,v 1.1 2007-05-30 14:49:05 jpalac Exp $
//
#include "RotationZYX.h"

#include <cmath>

//#include "Math/GenVector/Cartesian3D.h"
//#include "Math/GenVector/DisplacementVector3D.h"
#include "RotationZYX.h"
//#include "Math/GenVector/Rotation3D.h"
//#include "Math/GenVector/Quaternion.h"
//#include "Math/GenVector/RotationX.h"
//#include "Math/GenVector/RotationY.h"
//#include "Math/GenVector/RotationZ.h"

//#include "Math/GenVector/AxisAnglefwd.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/Transform3DTypes.h"

namespace Gaudi {


// ========== Constructors and Assignment =====================



// ========== Operations =====================

// DisplacementVector3D< Cartesian3D<double> >
// RotationZYX::
// operator() (const DisplacementVector3D< Cartesian3D<double> > & v) const
// {
//   return Rotation3D(*this)(v);
// }


RotationZYX RotationZYX::operator * (const Gaudi::Rotation3D  & r) const {
   // combine with a Rotation3D
   Gaudi::Rotation3D tmp;
   Gaudi::Math::convert(*this, tmp);
   return RotationZYX ( tmp * r );
}

RotationZYX RotationZYX::operator * (const Gaudi::AxisAngle   & a) const {
   // combine with a AxisAngle
   Gaudi::Quaternion tmp;
   Gaudi::Math::convert(*this, tmp);
   return RotationZYX ( tmp * Gaudi::Quaternion(a) );
}

RotationZYX RotationZYX::operator * (const Gaudi::EulerAngles   & e) const {
   // combine with EulerAngles
   Gaudi::Quaternion tmp;
   Gaudi::Math::convert(*this, tmp);
   return RotationZYX ( tmp * Gaudi::Quaternion(e) );
}

RotationZYX RotationZYX::operator * (const RotationZYX & e) const {
   // combine with a RotationZYX
   //return RotationZYX ( Quaternion(*this) * Quaternion(e) );
   Gaudi::Rotation3D tmp;
   Gaudi::Math::convert(*this, tmp);
   Gaudi::Rotation3D e_tmp;
   Gaudi::Math::convert(e, e_tmp);
   return RotationZYX ( tmp * e_tmp );
}
RotationZYX RotationZYX::operator * (const Gaudi::Quaternion & q) const {
   // combination with a Quaternion
   Gaudi::Quaternion tmp;
   Gaudi::Math::convert(*this, tmp);
   return RotationZYX ( tmp * q );
}

RotationZYX RotationZYX::operator * (const Gaudi::RotationX  & r) const {
   // combine with a RotationX
   Gaudi::Quaternion tmp;
   Gaudi::Math::convert(*this, tmp);
   return RotationZYX ( tmp * r );
}

RotationZYX RotationZYX::operator * (const Gaudi::RotationY  & r) const {
   // combine with a RotationY
   Gaudi::Quaternion tmp;
   Gaudi::Math::convert(*this, tmp);
   return RotationZYX ( tmp * r );
}

RotationZYX RotationZYX::operator * (const Gaudi::RotationZ  & r) const {
   // combine with a RotationZ
   // TODO -- this can be made much faster because it merely adds
   //         the r.Angle() to phi.
   Scalar newPhi = fPhi + r.Angle();
   if ( newPhi <= -Pi()|| newPhi > Pi() ) {
      newPhi = newPhi - std::floor( newPhi/(2*Pi()) +.5 ) * 2*Pi();
   }
   return RotationZYX ( newPhi, fTheta, fPsi );
}

RotationZYX operator * ( Gaudi::RotationX const & r, RotationZYX const & e )  {
   return RotationZYX(r) * e;  // TODO: improve performance
}

RotationZYX operator * ( Gaudi::RotationY const & r, RotationZYX const & e )  {
   return RotationZYX(r) * e;  // TODO: improve performance
}

RotationZYX
operator * ( Gaudi::RotationZ const & r, RotationZYX const & e )  {
   return RotationZYX(r) * e;  // TODO: improve performance
}

void RotationZYX::Rectify()
{
   // rectify . The angle theta must be defined between [-PI/2,PI.2]
   //  same as Euler- Angles, just here Theta is shifted by PI/2 with respect to 
   // the theta of the EulerAngles class

   Scalar theta2 = fTheta + Gaudi::Units::halfpi;
   if ( theta2 < 0 || theta2 > Pi() ) {
      Scalar t = theta2 - std::floor( theta2/(2*Pi() ) ) * 2*Pi();
      if ( t <= Pi() ) {
         theta2 = t;
      } else {
         theta2 = 2*Pi() - t;
         fPhi =  fPhi + Pi();
         fPsi =  fPsi + Pi();
      }
      // ftheta is shifted of PI/2 w.r.t theta2
      fTheta = theta2 - Gaudi::Units::halfpi; 
   }
   
   if ( fPhi <= -Pi()|| fPhi > Pi() ) {
      fPhi = fPhi - std::floor( fPhi/(2*Pi()) +.5 ) * 2*Pi();
   }
   
   if ( fPsi <= -Pi()|| fPsi > Pi() ) {
      fPsi = fPsi - std::floor( fPsi/(2*Pi()) +.5 ) * 2*Pi();
   }
   
} // Rectify()

void RotationZYX::Invert()
{
   // invert this rotation. 
   // use Rotation3D. TO Do :have algorithm to invert it directly
   Gaudi::Rotation3D r;
   Gaudi::Math::convert(*this, r);
   r.Invert();
   *this = r;
}

// ========== I/O =====================

std::ostream & operator<< (std::ostream & os, const RotationZYX & e) {
   // TODO - this will need changing for machine-readable issues
   //        and even the human readable form may need formatiing improvements
   os << "\n{phi(Z angle): " << e.Phi() << "   theta(Y angle): " << e.Theta() 
   << "   psi(X angle): " << e.Psi() << "}\n"; 
   return os;
}


} //namespace Gaudi
