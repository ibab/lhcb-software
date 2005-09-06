#include "MathCore/LorentzVectorfwd.h"
#include "MathCore/LorentzVector.h"
#include "MathCore/Point3D.h"
#include "TMatrixDSym.h"

/** @file LHCbTypeDefs.h
 *
 * Typedefs of typical LHCb quantities
 *
 * @author Patrick Koppenburg
 * created Mon Aug 29 13:01:46 2005
 *
 */                                                                                          
namespace LHCb {

  typedef ROOT::Math::XYZPoint Point3D ;      ///< 3D Point
  typedef ROOT::Math::XYZTVector Point4D  ;   ///< 4D point (space-time) @todo Ask for a XYZTPoint
  typedef ROOT::Math::XYZTVector Momentum ;   ///< Lorentz vector
  typedef TMatrixDSym SymMatrix ;             ///< Covariance matrix @todo Revise TMatrixDSym usage
  typedef TMatrixDBase Matrix ;               ///< Covariance matrix @todo Revise TMatrixDBase usage
}
