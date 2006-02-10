// $Id: Kinematics.cpp,v 1.2 2006-02-10 17:23:05 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $
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
  


// // ============================================================================
// /** @fn decayAngle 
//  *  This routine returns the cosine angle theta 
//  *  The decay angle calculated  is that between 
//  *  the flight direction of the daughter neson, "D",
//  *  in the rest frame of "Q" (the parent of "D"), 
//  *  with respect to "Q"'s fligth direction in "P"'s
//  *  (the parent of "Q") rest frame
//  * 
//  *  it is a EvtDecayAngle(P,Q,D) routine form EvtGen package
//  *  
//  *  @param D 4-momentum of the daughetr particle 
//  *  @param Q 4-momentum of mother particle 
//  *  @param P "rest frame system"
//  *  @return cosine of decay angle 
//  * 
//  *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
//  *  @date 2004-12-03
//  */
// // ============================================================================
// double LoKi::Kinematics::decayAngle
// ( const LoKi::LorentzVector& P , 
//   const LoKi::LorentzVector& Q ,
//   const LoKi::LorentzVector& D ) 
// {
//   const double pd  = P * D    ;
//   const double pq  = P * Q    ;
//   const double qd  = Q * D    ;
//   const double mq2 = Q.mag2() ;
//   const double mp2 = P.mag2() ;
//   const double md2 = D.mag2() ;
  
//   const double value = 
//     ( pq * pq - mq2 * mp2 ) * ( qd * qd - mq2 * md2 ) ;
  
//   if ( 0 > value ) 
//   {
//     LoKi::Report::Error 
//       ( "LoKi::Kinematics::decayAngle():: invalid 4-momenta " );
//     return LoKi::Constants::InvalidAngle ;
//   }
//   //
//   return ( pd * mq2 - pq * qd ) / sqrt( value ) ;
// };
// // ============================================================================

// // ============================================================================
// /** @fn transversityAngle 
//  *  This routine evaluates the cosine of "transversity angle", 
//  *  useful e.g. to disantangle the different partiasl vaves in 
//  *  0 -> 1 + 1 decay (e.g. Bs -> J/psi Phi 
//  *  
//  *  The code is kindly provided by Gerhard Raven 
//  * 
//  *  @param l1 4-vector of the first  particle, e.g. mu+
//  *  @param l2 4-vector of the second particle, e.g. mu- 
//  *  @param p1 4-vector of the third  particle, e.g. K+
//  *  @param p2 4-vector of the fourth particle, e.g. K- 
//  *  @return the cosine of transversity angle 
//  * 
//  *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
//  *  @date 2004-12-03
//  */ 
// // ============================================================================
// double LoKi::Kinematics::transversityAngle 
// ( const LoKi::LorentzVector& l1 , 
//   const LoKi::LorentzVector& l2 , 
//   const LoKi::LorentzVector& p1 , 
//   const LoKi::LorentzVector& p2 ) 
// {
//   const LoKi::LorentzRotation 
//     bst ( - ( l1 + l2 ).boostVector() ) ;
//   return ( ( bst * p1 ).vect() ).cross( ( bst * p2 ).vect() ).cosTheta( ( bst * l1 ).vect() ) ;
// };
// // ============================================================================

// // ============================================================================
// /** @fn forwardBackwardAngle
//  *  This routine evaluated the angle theta_FB
//  *  used e.g. for evaluation of forward-backward 
//  *  asymmetry for decay B -> K* mu+ mu- 
//  *  The angle calculated is that 
//  *  between between the mu+ and K^*0 momenta 
//  *  in the di-muon rest frame
//  *  
//  *  The code is kindly provided by Helder Lopes 
//  *
//  *  @param  K  4-momenutm of   "K*"
//  *  @param  l1 4-momentum of the first  lepton
//  *  @param  l2 4-momentum of the second lepton
//  *  @return the cosine of theta_FB 
//  * 
//  *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
//  *  @date 2004-12-03
//  */
// // ============================================================================
// double LoKi::Kinematics::forwardBackwardAngle
// ( const LoKi::LorentzVector& K  , 
//   const LoKi::LorentzVector& l1 , 
//   const LoKi::LorentzVector& l2 )
// {
//   // 
//   const LoKi::LorentzRotation bst( - ( l1 + l2 ).boostVector() ) ;
  
//   return ( ( bst * K ).vect() ).cosTheta( ( bst * l1 ).vect() ) ;
// };
// // ============================================================================



// ============================================================================
// The END 
// ============================================================================

