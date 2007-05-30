// @(#)root/mathcore:$Name: not supported by cvs2svn $:$Id: 3DConversions.cpp,v 1.1 2007-05-30 14:49:42 jpalac Exp $
// Authors: W. Brown, M. Fischler, L. Moneta    2005  

 /**********************************************************************
  *                                                                    *
  * Copyright (c) 2005, LCG ROOT FNAL MathLib Team                    *
  *                                                                    *
  *                                                                    *
  **********************************************************************/

// Source file for something else
//
// Created by: Mark Fischler and Walter Brown Thurs July 7, 2005
//
// Last update: $Id: 3DConversions.cpp,v 1.1 2007-05-30 14:49:42 jpalac Exp $
//

// TODO - For now, all conversions are grouped in this one compilation unit.
//        The intention is to seraparte them into a few .cpp files instead,
//        so that users needing one form need not incorporate code for them all.


//#include "Math/Math.h"

#include "3DConversions.h"

// #include "Math/GenVector/Rotation3D.h"
// #include "Math/GenVector/AxisAngle.h"
// #include "Math/GenVector/EulerAngles.h"
// #include "Math/GenVector/Quaternion.h"
#include "RotationZYX.h"
// #include "Math/GenVector/RotationX.h"
// #include "Math/GenVector/RotationY.h"
// #include "Math/GenVector/RotationZ.h"

#include <cmath>
#include <limits>

namespace Gaudi {
namespace Math {

enum ERotation3DMatrixIndex
{ kXX = Rotation3D::kXX, kXY = Rotation3D::kXY, kXZ = Rotation3D::kXZ
, kYX = Rotation3D::kYX, kYY = Rotation3D::kYY, kYZ = Rotation3D::kYZ
, kZX = Rotation3D::kZX, kZY = Rotation3D::kZY, kZZ = Rotation3D::kZZ
};


// ----------------------------------------------------------------------
static void correctByPi ( double& psi, double& phi ) {
   static const double pi = Gaudi::Units::pi;
   if (psi > 0) {
      psi -= pi;
   } else {
      psi += pi;
   }
   if (phi > 0) {
      phi -= pi;
   } else {
     phi += pi;
   }
}
  
//------------------------------------------------------------------------
void convert( Gaudi::Rotation3D const & from, Gaudi::RotationZYX  & to)
{
   // conversion from Rotation3D to RotationZYX
   // same Math used as for EulerAngles apart from some different meaning of angles and 
   // matrix elements. But the basic algoprithms principles are the same described in 
   // http://www.cern.ch/mathlibs/documents/eulerAngleComputation.pdf

   // theta is assumed to be in range [-PI/2,PI/2]. 
   // this is guranteed by the Rectify function

   static const double pi_2 = Gaudi::Units::halfpi;

   double r[9]; 
   from.GetComponents(r,r+9);

   double phi,theta,psi = 0;

   // careful for numeical error make sin(theta) ourtside [-1,1]
   double sinTheta =  r[kXZ]; 
   if ( sinTheta < -1.0) sinTheta = -1.0;
   if ( sinTheta >  1.0) sinTheta =  1.0;
   theta = std::asin( sinTheta );

   // compute psi +/- phi
   // Depending on whether cosTheta is positive or negative and whether it
   // is less than 1 in absolute value, different mathematically equivalent
   // expressions are numerically stable.
   // algorithm from 
   // adapted for the case 3-2-1
   
   double psiPlusPhi = 0; 
   double psiMinusPhi = 0; 

   // valid if sinTheta not eq to -1 otherwise is zero
   if (sinTheta > - 1.0) 
      psiPlusPhi = atan2 ( r[kYX] + r[kZY], r[kYY] - r[kZX] );

   // valid if sinTheta not eq. to 1
   if (sinTheta < 1.0) 
      psiMinusPhi = atan2 ( r[kZY] - r[kYX] , r[kYY] + r[kZX] );

   psi = .5 * (psiPlusPhi + psiMinusPhi); 
   phi = .5 * (psiPlusPhi - psiMinusPhi); 

   // correction is not necessary if sinTheta = +/- 1
   //if (sinTheta == 1.0 || sinTheta == -1.0) return;

   // apply the corrections according to max of the other terms
   // I think is assumed convention that theta is between -PI/2,PI/2. 
   // OTHERWISE RESULT MIGHT BE DIFFERENT ???

   //since we determine phi+psi or phi-psi phi and psi can be both have a shift of +/- PI.
   // The shift must be applied on both (the sum (or difference) is knows to +/- 2PI ) 
   //This can be fixed looking at the other 4 matrix terms, which have terms in sin and cos of psi 
   // and phi. sin(psi+/-PI) = -sin(psi) and cos(psi+/-PI) = -cos(psi). 
   //Use then the biggest term for making the correction to minimize possible numerical errors   

   // set up w[i], all of which would be positive if sin and cosine of
   // psi and phi were positive:
   double w[4];
   w[0] = -r[kYZ]; w[1] = -r[kXY]; w[2] = r[kZZ]; w[3] = r[kXX];
   
   // find biggest relevant term, which is the best one to use in correcting.
   double maxw = std::fabs(w[0]); 
   int imax = 0;
   for (int i = 1; i < 4; ++i) {
      if (std::fabs(w[i]) > maxw) {
         maxw = std::fabs(w[i]);
         imax = i;
      }
   }

   // Determine if the correction needs to be applied:  The criteria are 
   // different depending on whether a sine or cosine was the determinor: 
   switch (imax) {
      case 0:
         if (w[0] > 0 && psi < 0)               correctByPi ( psi, phi );
         if (w[0] < 0 && psi > 0)               correctByPi ( psi, phi );
            break;
      case 1:
         if (w[1] > 0 && phi < 0)               correctByPi ( psi, phi );
         if (w[1] < 0 && phi > 0)               correctByPi ( psi, phi );
            break;
      case 2:
         if (w[2] > 0 && std::fabs(psi) > pi_2) correctByPi ( psi, phi );    
         if (w[2] < 0 && std::fabs(psi) < pi_2) correctByPi ( psi, phi );    
            break;
      case 3:
         if (w[3] > 0 && std::fabs(phi) > pi_2) correctByPi ( psi, phi );    
         if (w[3] < 0 && std::fabs(phi) < pi_2) correctByPi ( psi, phi );    
            break;
   }          

   to.SetComponents(phi, theta, psi);
      
} // convert to RotationZYX

// ----------------------------------------------------------------------

void convert( Gaudi::AxisAngle const & from , Gaudi::RotationZYX & to  )
{
   // conversion from AxisAngle to RotationZYX
   // TODO better : temporary make conversion using  Rotation3D   
   Gaudi::Rotation3D tmp(from); 
   convert(tmp,to);
}
// ----------------------------------------------------------------------

void convert( Gaudi::EulerAngles const & from , Gaudi::RotationZYX & to  )
{
   // conversion from EulerAngles to RotationZYX
   // TODO better : temporary make conversion using  Rotation3D   
   Gaudi::Rotation3D tmp(from); 
   convert(tmp,to);
}

void convert( Quaternion const & from , RotationZYX & to  )
{
   // conversion from Quaternion to RotationZYX
   // TODO better : temporary make conversion using  Rotation3D   
   Rotation3D tmp(from); 
   convert(tmp,to);
}

// ----------------------------------------------------------------------
// conversions from RotationZYX
void convert( RotationZYX const & from, Rotation3D  & to) { 
   // conversion to Rotation3D (matrix)

   double phi,theta,psi = 0;
   from.GetComponents(phi,theta,psi);
   to.SetComponents( std::cos(theta)*std::cos(phi), 
                      - std::cos(theta)*std::sin(phi), 
                      std::sin(theta), 
                      
                      std::cos(psi)*std::sin(phi) + std::sin(psi)*std::sin(theta)*std::cos(phi), 
                      std::cos(psi)*std::cos(phi) - std::sin(psi)*std::sin(theta)*std::sin(phi), 
                      -std::sin(psi)*std::cos(theta), 
                      
                      std::sin(psi)*std::sin(phi) - std::cos(psi)*std::sin(theta)*std::cos(phi), 
                      std::sin(psi)*std::cos(phi) + std::cos(psi)*std::sin(theta)*std::sin(phi), 
                      std::cos(psi)*std::cos(theta)
      );

}
void convert( RotationZYX const & from, AxisAngle   & to) { 
   // conversion to axis angle
   // TODO better : temporary make conversion using  Rotation3D   
   Rotation3D tmp;
   convert(from, tmp);
   to = tmp;
}
void convert( RotationZYX const & from, EulerAngles & to) {
   // conversion to Euler angle
   // TODO better : temporary make conversion using  Rotation3D   
   Rotation3D tmp; 
   convert(from, tmp);
   to = tmp;
}
void convert( RotationZYX const & from, Quaternion  & to) { 
   double phi,theta,psi = 0;
   from.GetComponents(phi,theta,psi);

   double sphi2   = std::sin(phi/2); 
   double cphi2   = std::cos(phi/2); 
   double stheta2 = std::sin(theta/2); 
   double ctheta2 = std::cos(theta/2); 
   double spsi2   = std::sin(psi/2); 
   double cpsi2   = std::cos(psi/2); 
   to.SetComponents(  cphi2 * cpsi2 * ctheta2 - sphi2 * spsi2 * stheta2, 
                      sphi2 * cpsi2 * stheta2 + cphi2 * spsi2 * ctheta2, 
                      cphi2 * cpsi2 * stheta2 - sphi2 * spsi2 * ctheta2, 
                      sphi2 * cpsi2 * ctheta2 + cphi2 * spsi2 * stheta2 
      );
}


// ----------------------------------------------------------------------

void convert( RotationX const & from , RotationZYX & to  )
{
   // conversion from RotationX to RotationZYX 
   to.SetComponents(0,0,from.Angle());
}


// ----------------------------------------------------------------------

void convert( RotationY const & from , RotationZYX & to  )
{
   // conversion from RotationY to RotationZYX 
   to.SetComponents(0,from.Angle(),0);
}

// ----------------------------------------------------------------------

void convert( RotationZ const & from , RotationZYX & to  )
{
   // conversion from RotationY to RotationZYX 
   to.SetComponents(from.Angle(),0,0);
}

} //namespace Math
} //namespace Gaudi
