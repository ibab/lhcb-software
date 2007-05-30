// @(#)root/mathcore:$Name: not supported by cvs2svn $:$Id: 3DConversions.h,v 1.1 2007-05-30 14:49:42 jpalac Exp $
// Authors: W. Brown, M. Fischler, L. Moneta    2005  

 /**********************************************************************
  *                                                                    *
  * Copyright (c) 2005, LCG ROOT FNAL MathLib Team                    *
  *                                                                    *
  *                                                                    *
  **********************************************************************/

// Header file for class Rotation in 3 dimensions, represented by 3x3 matrix
//
// Created by: Mark Fischler and Walter Brown Thurs July 7, 2005
//
// Last update: Wed Thurs July 7, 2005
//
#ifndef Gaudi_Math_GenVector_3DConversions 
#define Gaudi_Math_GenVector_3DConversions  1

//#include "Math/GenVector/Rotation3Dfwd.h"
//#include "Math/GenVector/AxisAnglefwd.h"
//#include "Math/GenVector/EulerAnglesfwd.h"
//#include "Math/GenVector/Quaternionfwd.h"
//#include "Math/GenVector/RotationXfwd.h"
//#include "Math/GenVector/RotationYfwd.h"
//#include "Math/GenVector/RotationZfwd.h"
//#include "Math/GenVector/RotationZYXfwd.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/Transform3DTypes.h"
#include "RotationZYXfwd.h"


namespace Gaudi {
namespace Math {


// flag a compile time error when a wrong conversion is instantiated
struct ERROR_This_Rotation_Conversion_is_NOT_Supported;
template<class R1, class R2> 
void convert( R1 const &, R2 const) {
   ERROR_This_Rotation_Conversion_is_NOT_Supported();
}
// ----------------------------------------------------------------------
void convert( Gaudi::Rotation3D  const & from, Gaudi::RotationZYX & to);
void convert( Gaudi::AxisAngle   const & from, Gaudi::RotationZYX & to);
void convert( Gaudi::EulerAngles const & from, Gaudi::RotationZYX & to);
void convert( Gaudi::Quaternion  const & from, Gaudi::RotationZYX & to);
// ----------------------------------------------------------------------
void convert( Gaudi::RotationZYX const & from, Gaudi::Rotation3D  & to);
void convert( Gaudi::RotationZYX const & from, Gaudi::AxisAngle   & to);
void convert( Gaudi::RotationZYX const & from, Gaudi::EulerAngles & to);
void convert( Gaudi::RotationZYX const & from, Gaudi::Quaternion  & to);
// ----------------------------------------------------------------------
void convert( Gaudi::RotationX const & from, Gaudi::RotationZYX & to);
void convert( Gaudi::RotationY const & from, Gaudi::RotationZYX & to);
void convert( Gaudi::RotationZ const & from, Gaudi::RotationZYX & to);
// ----------------------------------------------------------------------


} //namespace Math
} //namespace Gaudi

#endif // Gaudi_Math_GenVector_3DConversions 
