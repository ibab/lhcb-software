// $Id: Kinematics.cpp,v 1.1.1.1 2006-01-24 09:39:41 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1.1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Kinematics.h"
#include "LoKi/Constants.h"
#include "LoKi/Report.h"
// ============================================================================

// ============================================================================
/** @file
 *
 *  Implementation file for functions from namespace LoKi::Kinematics
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

// ============================================================================
/** @fn euclidianNorm2 
 *  simple function for evaluation of the euclidiam norm
 *  for LorentzVectors 
 *  (E**2+Px**2+Py**2+Pz**2)
 *  @param vct the vector
 *  @return euclidian norm squared   
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-01-17
 */
// ============================================================================
double LoKi::Kinematics::euclidianNorm2 
( const LoKi::LorentzVector& vct ) 
{
  return 
    vct.e() * vct.e() + 
    vct.x() * vct.x() + 
    vct.y() * vct.y() + 
    vct.z() * vct.z() ;
} ;
// ============================================================================

// ============================================================================
/** @fn euclidianNorm 
 *  simple function for evaluation of the euclidiam norm
 *  for LorentzVectors 
 *  sqrt(E**2+Px**2+Py**2+Pz**2)
 *  @param vct the vector
 *  @return euclidian norm    
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-01-17
 */
// ============================================================================
double LoKi::Kinematics::euclidianNorm
( const LoKi::LorentzVector& vct ) 
{ return sqrt ( euclidianNorm2 ( vct )  ) ; } ;
// ============================================================================

// ============================================================================
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
// ============================================================================
double LoKi::Kinematics::delta2euclidian 
( const LoKi::LorentzVector& vct1 , 
  const LoKi::LorentzVector& vct2 ) 
{ return euclidianNorm2 ( vct1 - vct2 ) ; } ;
// ============================================================================

// ============================================================================
/** @fn transverseMomentumDir 
 *  simple function which evaluates the transverse 
 *  momentum with respect a certain 3D-direction
 *  @param mom the momentum
 *  @param dir the direction
 *  @return the transverse moementum with respect to the direction
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-01-17
 */
// ============================================================================
double LoKi::Kinematics::transverseMomentumDir 
( const LoKi::LorentzVector& mom , 
  const LoKi::ThreeVector&   dir ) 
{
  const double dmag2 = dir.Mag2() ;
  if ( 0 == dmag2 ) { return mom.P() ; }
  const LoKi::ThreeVector v( mom ) ;
  const LoKi::ThreeVector perp = v - dir * ( v.Dot( dir ) / dmag2 ) ;
  return perp.R() ;
} ;
// ============================================================================

// ============================================================================
/** @fn mass 
 *  trivial function to evaluate the mass of 4-vector 
 *  @param mom lorenz vector
 *  @return invariant mass 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-01-17
 */
// ============================================================================
double LoKi::Kinematics::mass 
( const LoKi::LorentzVector& mom ) { return mom.M() ; }
// ============================================================================

// ============================================================================
/** @fn mass 
 *  trivial function to evaluate the mass of 4-vectors  
 *  @param v1  the first lorenz vector 
 *  @param v2  the second lorenz vector
 *  @return invariant mass 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-01-17
 */
// ============================================================================
double LoKi::Kinematics::mass 
( const LoKi::LorentzVector& v1 , 
  const LoKi::LorentzVector& v2 ) { return (v1+v2).M(); } ;
// ============================================================================

// ============================================================================
/** @fn mass 
 *  trivial function to evaluate the mass of 4-vectors  
 *  @param v1  the first  lorenz vector 
 *  @param v2  the second lorenz vector
 *  @param v3  the third  lorenz vector
 *  @return invariant mass 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-01-17
 */
// ============================================================================
double LoKi::Kinematics::mass 
( const LoKi::LorentzVector& v1 , 
  const LoKi::LorentzVector& v2 ,
  const LoKi::LorentzVector& v3 ) 
{
  LoKi::LorentzVector v ( v1 )  ;
  v += v2 ;
  v += v3 ;
  return v.M() ;
} ;
// ============================================================================

// ============================================================================
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
// ============================================================================
double LoKi::Kinematics::mass 
( const LoKi::LorentzVector& v1 , 
  const LoKi::LorentzVector& v2 ,
  const LoKi::LorentzVector& v3 , 
  const LoKi::LorentzVector& v4 ) 
{
  LoKi::LorentzVector v ( v1 ) ;
  v += v2 ;
  v += v3 ;
  v += v4 ;
  return v.M() ;
} ;
// ============================================================================
  

// ============================================================================
// The END 
// ============================================================================

