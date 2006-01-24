// $Id: Kinematics.h,v 1.1.1.1 2006-01-24 09:39:42 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.1.1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef LOKI_KINEMATICS_H 
#define LOKI_KINEMATICS_H 1
// ============================================================================
// Include files
// ============================================================================
// LHCbDefinitions 
// ============================================================================
#include "Kernel/Vector4DTypes.h"
#include "Kernel/Vector3DTypes.h"
#include "Kernel/Point3DTypes.h"
// ============================================================================

// ============================================================================
/** @file
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot at al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ============================================================================

namespace LoKi
{
  /// Lorentz vector 
  typedef Gaudi::LorentzVector LorentzVector ;
  /// 3D vector 
  typedef Gaudi::XYZVector     ThreeVector   ;
  /// 3D point
  typedef Gaudi::XYZVector     Point3D       ;


  /** @namespace Kinematics 
   *  collection of few trivial functions for kinematical evaluations
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-01-17
   */
  namespace Kinematics 
  {
    
    /** @fn euclidianNorm2 
     *  simple function for evaluation of the euclidiam norm
     *  for LorentzVectors 
     *  (E**2+Px**2+Py**2+Pz**2)
     *  @param vct the vector
     *  @return euclidian norm squared   
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    double euclidianNorm2 
    ( const LoKi::LorentzVector& vct ) ;

    /** @fn euclidianNorm 
     *  simple function for evaluation of the euclidiam norm
     *  for LorentzVectors 
     *  sqrt(E**2+Px**2+Py**2+Pz**2)
     *  @param vct the vector
     *  @return euclidian norm    
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    double euclidianNorm
    ( const LoKi::LorentzVector& vct ) ;

    /** @fn delta2euclidian 
     *  simple function for evaluation of the square of 
     *  the euclidian distance inbetwee 2 LorentzVectors 
     *  (DeltaE**2+DeltaPx**2+DeltaPy**2+DeltaPz**2)
     *  @param vct1 the first vector
     *  @param vct2 the second vector
     *  @return the square of euclidian distance 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    double delta2euclidian 
    ( const LoKi::LorentzVector& vct1 , 
      const LoKi::LorentzVector& vct2 ) ;
    
    /** @fn transverseMomentumDir 
     *  simple function which evaluates the transverse 
     *  momentum with respect a certain 3D-direction
     *  @param mom the momentum
     *  @param dir the direction
     *  @return the transverse moementum with respect to the direction
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    double transverseMomentumDir 
    ( const LoKi::LorentzVector& mom , 
      const LoKi::ThreeVector&   dir ) ;
    
    /** @fn mass 
     *  trivial function to evaluate the mass of 4-vector 
     *  @param mom lorenz vector
     *  @return invariant mass 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    double mass 
    ( const LoKi::LorentzVector& mom ) ;
    
    /** @fn mass 
     *  trivial function to evaluate the mass of 4-vectors  
     *  @param v1  the first lorenz vector 
     *  @param v2  the second lorenz vector
     *  @return invariant mass 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    double mass 
    ( const LoKi::LorentzVector& v1 , 
      const LoKi::LorentzVector& v2 ) ;

    /** @fn mass 
     *  trivial function to evaluate the mass of 4-vectors  
     *  @param v1  the first  lorenz vector 
     *  @param v2  the second lorenz vector
     *  @param v3  the third  lorenz vector
     *  @return invariant mass 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    double mass 
    ( const LoKi::LorentzVector& v1 , 
      const LoKi::LorentzVector& v2 ,
      const LoKi::LorentzVector& v3 ) ;

    /** @fn mass 
     *  trivial function to evaluate the mass of 4-vectors  
     *  @param v1  the first  lorenz vector 
     *  @param v2  the second lorenz vector
     *  @param v3  the third  lorenz vector
     *  @param v4  the fourth  lorenz vector
     *  @return invariant mass 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    double mass 
    ( const LoKi::LorentzVector& v1 , 
      const LoKi::LorentzVector& v2 ,
      const LoKi::LorentzVector& v3 , 
      const LoKi::LorentzVector& v4 ) ;
    

  } ; // end of namespace Kinematics  
}; // end of namespace LoKi

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_KINEMATICS_H
// ============================================================================
