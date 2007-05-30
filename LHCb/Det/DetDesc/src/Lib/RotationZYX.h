// @(#)root/mathcore:$Name: not supported by cvs2svn $:$Id: RotationZYX.h,v 1.1 2007-05-30 14:49:05 jpalac Exp $
// Authors: J. Palacios, L. Moneta    2007  

 /**********************************************************************
  *                                                                    *
  * Copyright (c) 2007 , LCG ROOT MathLib Team                         *
  *                                                                    *
  *                                                                    *
  **********************************************************************/

// Header file for class Rotation in 3 dimensions, described by 3 Z-Y-X  Euler angles 
// representing a rotation along Z, Y and X
//
// Created by: Lorenzo Moneta, Wed. May 22, 2007
//
// Last update: $Id: RotationZYX.h,v 1.1 2007-05-30 14:49:05 jpalac Exp $
//
#ifndef Gaudi_Math_GenVector_RotationZYX 
#define Gaudi_Math_GenVector_RotationZYX  1

//#ifndef ROOT_Math_Math
//#include "Math/Math.h"
//#endif

#ifndef ROOT_Math_GenVector_Rotation3D 
#include "Math/GenVector/Rotation3D.h"
#endif


#ifndef ROOT_Math_GenVector_DisplacementVector3D
#include "Math/GenVector/DisplacementVector3D.h"
#endif

#ifndef ROOT_Math_GenVector_PositionVector3D
#include "Math/GenVector/PositionVector3D.h"
#endif

#ifndef ROOT_Math_GenVector_LorentzVector
#include "Math/GenVector/LorentzVector.h"
#endif

#ifndef Gaudi_Math_GenVector_3DConversions
#include "3DConversions.h"
#endif

#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Vector4DTypes.h"
#include "GaudiKernel/PhysicalConstants.h"

#include <algorithm>
#include <cassert>
#include <iostream>


namespace Gaudi {

  /**
     Rotation class with the (3D) rotation represented by
     angles describing first a rotation of 
     an angle phi (yaw) about the  Z axis, 
     followed by a rotation of an angle theta (pitch) about the new Y' axis, 
     folowed by a third rotation of an angle psi (roll) about the final X'' axis. 
     This is  sometimes referred to as the Euler 321 sequence.
     It has not to be confused with the typical Goldstein definition of the Euler Angles 
     (Z-X-Z or 313 sequence) which is used by the ROOT::Math::EulerAngles class.  


     @ingroup GenVector
  */

class RotationZYX {

public:

  typedef double Scalar;


  // ========== Constructors and Assignment =====================

  /**
     Default constructor
  */
  RotationZYX() : fPhi(0.0), fTheta(0.0), fPsi(0.0) { }

   /**
      Constructor from phi, theta and psi
   */
   RotationZYX( Scalar phi, Scalar theta, Scalar psi ) :
      fPhi(phi), fTheta(theta), fPsi(psi)
   {Rectify();}			// Added 27 Jan. 06   JMM

   /**
      Construct given a pair of pointers or iterators defining the
      beginning and end of an array of three Scalars, to be treated as
      the angles phi, theta and psi.
   */
   template<class IT>
   RotationZYX(IT begin, IT end) { SetComponents(begin,end); }

   // The compiler-generated copy ctor, copy assignment, and dtor are OK.

   /**
      Re-adjust components place angles in canonical ranges
   */
   void Rectify();


   // ======== Construction and Assignment From other Rotation Forms ==================

   /**
      Construct from another supported rotation type (see gv_detail::convert )
   */
   template <class OtherRotation> 
   explicit RotationZYX(const OtherRotation & r) {Gaudi::Math::convert(r,*this);}


   /**
      Assign from another supported rotation type (see gv_detail::convert )
   */
   template <class OtherRotation> 
   RotationZYX & operator=( OtherRotation const  & r ) { 
      Gaudi::Math::convert(r,*this);
      return *this;
   }


   // ======== Components ==============

   /**
      Set the three Euler angles given a pair of pointers or iterators
      defining the beginning and end of an array of three Scalars.
   */
   template<class IT>
   void SetComponents(IT begin, IT end) {
      fPhi   = *begin++;
      fTheta = *begin++;
      fPsi   = *begin++;
      assert(begin == end); 
      Rectify();		
   }

   /**
      Get the axis and then the angle into data specified by an iterator begin
      and another to the end of the desired data (4 past start).
   */
   template<class IT>
   void GetComponents(IT begin, IT end) const {
      *begin++ = fPhi;
      *begin++ = fTheta;
      *begin++ = fPsi;
      assert(begin == end); 
   }

   /**
      Get the axis and then the angle into data specified by an iterator begin
   */
   template<class IT>
   void GetComponents(IT begin) const {
      *begin++ = fPhi;
      *begin++ = fTheta;
      *begin   = fPsi;
   }

   /**
      Set the components phi, theta, psi based on three Scalars.
   */
   void SetComponents(Scalar phi, Scalar theta, Scalar psi) {
      fPhi=phi; fTheta=theta; fPsi=psi; 
      Rectify();			
   }

   /**
      Get the components phi, theta, psi into three Scalars.
   */
   void GetComponents(Scalar & phi, Scalar & theta, Scalar & psi) const {
      phi=fPhi; theta=fTheta; psi=fPsi;
   }

   /**
      Set Phi angle (Z rotation angle) 		
   */
   void SetPhi(Scalar phi) { fPhi=phi; Rectify(); }

   /**
      Return Phi angle (Z rotation angle) 		
   */
   Scalar Phi() const { return fPhi; }

   /**
      Set Theta angle (Y' rotation angle) 				
   */
   void SetTheta(Scalar theta) { fTheta=theta; Rectify(); }

   /**
      Return Theta angle (Y' rotation angle) 				
   */
   Scalar Theta() const { return fTheta; }

   /**
      Set Psi angle (X'' rotation angle)	    
   */
   void SetPsi(Scalar psi) { fPsi=psi; Rectify(); }

   /**
      Return Psi angle (X'' rotation angle)
   */
   Scalar Psi() const { return fPsi; }

   // =========== operations ==============


   /**
      Rotation operation on a displacement vector in any coordinate system and tag
   */
   template <class CoordSystem, class U>
   ROOT::Math::DisplacementVector3D<CoordSystem,U>
   operator() (const ROOT::Math::DisplacementVector3D<CoordSystem,U> & v) const {
      return Gaudi::Rotation3D(*this) ( v );
   }

   /**
      Rotation operation on a position vector in any coordinate system
   */
   template <class CoordSystem, class U>
   ROOT::Math::PositionVector3D<CoordSystem, U>
   operator() (const ROOT::Math::PositionVector3D<CoordSystem,U> & v) const {
      Gaudi::XYZVector xyz(v);
      Gaudi::XYZVector rxyz = operator()(xyz);
      return ROOT::Math::PositionVector3D<CoordSystem,U> ( rxyz );
   }

   /**
      Rotation operation on a Lorentz vector in any 4D coordinate system
   */
   template <class CoordSystem>
   ROOT::Math::LorentzVector<CoordSystem>
   operator() (const ROOT::Math::LorentzVector<CoordSystem> & v) const {
      Gaudi::XYZVector xyz(v.Vect());
      xyz = operator()(xyz);
      Gaudi::LorentzVector xyzt (xyz.X(), xyz.Y(), xyz.Z(), v.E());
      return ROOT::Math::LorentzVector<CoordSystem> ( xyzt );
   }

   /**
      Rotation operation on an arbitrary vector v.
      Preconditions:  v must implement methods x(), y(), and z()
      and the arbitrary vector type must have a constructor taking (x,y,z)
   */
   template <class ForeignVector>
   ForeignVector
   operator() (const  ForeignVector & v) const {
      Gaudi::XYZVector xyz(v);
      Gaudi::XYZVector rxyz = operator()(xyz);
      return ForeignVector ( rxyz.X(), rxyz.Y(), rxyz.Z() );
   }

   /**
      Overload operator * for rotation on a vector
   */
   template <class AVector>
   inline
   AVector operator* (const AVector & v) const
   {
      return operator()(v);
   }

   /**
      Invert a rotation in place
   */
   void Invert(); 

   /**
      Return inverse of a rotation
   */
   RotationZYX Inverse() const { 
      RotationZYX r(*this); 
      r.Invert(); 
      return r;
   }


   // ========= Multi-Rotation Operations ===============

   /**
      Multiply (combine) two rotations
   */
   RotationZYX operator * (const RotationZYX & e) const;
   RotationZYX operator * (const Gaudi::Rotation3D  & r) const;
   RotationZYX operator * (const Gaudi::AxisAngle   & a) const;
   RotationZYX operator * (const Gaudi::Quaternion  & q) const;
   RotationZYX operator * (const Gaudi::EulerAngles & q) const;
   RotationZYX operator * (const Gaudi::RotationX  & rx) const;
   RotationZYX operator * (const Gaudi::RotationY  & ry) const;
   RotationZYX operator * (const Gaudi::RotationZ  & rz) const;

   /**
      Post-Multiply (on right) by another rotation :  T = T*R
   */
   template <class R>
   RotationZYX & operator *= (const R & r) { return *this = (*this)*r; }

   /**
      Distance between two rotations
   */
  //   template <class R>
  //   Scalar Distance ( const R & r ) const {return gv_detail::dist(*this,r);}

   /**
      Equality/inequality operators
   */
   bool operator == (const RotationZYX & rhs) const {
      if( fPhi   != rhs.fPhi   ) return false;
      if( fTheta != rhs.fTheta ) return false;
      if( fPsi   != rhs.fPsi   ) return false;
      return true;
   }
   bool operator != (const RotationZYX & rhs) const {
      return ! operator==(rhs);
   }

private:

   double fPhi;
   double fTheta;
   double fPsi;

   static double Pi() { return Gaudi::Units::pi; }

};  // RotationZYX

/**
   Distance between two rotations
 */
//template <class R>
//inline
//typename RotationZYX::Scalar
//Distance ( const RotationZYX& r1, const R & r2) {return Gaudi::Math::dist(r1,r2);}

/**
   Multiplication of an axial rotation by an AxisAngle
 */
RotationZYX operator* (Gaudi::RotationX const & r1, RotationZYX const & r2);
RotationZYX operator* (Gaudi::RotationY const & r1, RotationZYX const & r2);
RotationZYX operator* (Gaudi::RotationZ const & r1, RotationZYX const & r2);

/**
   Stream Output and Input
 */
  // TODO - I/O should be put in the manipulator form 

std::ostream & operator<< (std::ostream & os, const RotationZYX & e);
 
  

} // namespace Gaudi

#endif // ROOT_Math_GenVector_RotationZYX 
