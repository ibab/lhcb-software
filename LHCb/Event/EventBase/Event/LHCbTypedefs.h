#ifndef PhysEvent_LHCbTypeDefs_H
#define PhysEvent_LHCbTypeDefs_H 1

#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Matrix.h"
#include "MathCore/LorentzVectorfwd.h"
#include "MathCore/LorentzVector.h"
#include "MathCore/Point3D.h"

/** @file LHCbTypeDefs.h
 *
 * Typedefs of typical LHCb quantities
 *
 * @author Patrick Koppenburg
 * created Mon Aug 29 13:01:46 2005
 *
 */                                                                                          
/**
 *
 * @todo Replace all these typedefs by the corresponding MathCore classes
 *
 */                                                                                          
namespace LHCb {

  typedef ROOT::Math::XYZPoint Point3D ;      ///< 3D Point
  typedef ROOT::Math::XYZTVector Point4D  ;   ///< 4D point (space-time) @todo Ask for a XYZTPoint
  typedef ROOT::Math::XYZVector Vector3D  ;   ///< 3D vector
  typedef ROOT::Math::XYZTVector Lorentz ;    ///< 4D (Lorentz) vector
  typedef CLHEP::HepSymMatrix SymMatrix ;             ///< Covariance matrix @todo Revise TMatrixDSym usage
  typedef CLHEP::HepMatrix Matrix ;               ///< Covariance matrix @todo Revise TMatrixDBase usage
}

#endif
