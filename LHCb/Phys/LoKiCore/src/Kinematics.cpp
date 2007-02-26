// $Id: Kinematics.cpp,v 1.7 2007-02-26 13:13:09 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.7 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2006/05/02 14:29:11  ibelyaev
//  censored
//
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <climits>
#include <limits.h>
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Kinematics.h"
#include "LoKi/Constants.h"
#include "LoKi/Report.h"
// ============================================================================
// ROOT 
// ============================================================================
#include "Math/Boost.h"
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
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ============================================================================

// ============================================================================
/** simple function for evaluation of the euclidiam norm
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
/** simple function for evaluation of the euclidiam norm
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
/** simple function for evaluation of the square of 
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
/** simple function which evaluates the transverse 
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
/** This routine returns the cosine angle theta 
 *  The decay angle calculated  is that between 
 *  the flight direction of the daughter neson, "D",
 *  in the rest frame of "M" (the parent of "D"), 
 *  with respect to the boost direction from 
 *  "M"'s rest frame 
 *  
 *  @param D 4-momentum of the daughetr particle 
 *  @param M 4-momentum of mother particle 
 *  @return cosine of decay angle 
 *
 *  @see LoKi::LorentzVector.h
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2004-12-03
 */
// ============================================================================
double LoKi::Kinematics::decayAngle
( const LoKi::LorentzVector& D , 
  const LoKi::LorentzVector& M ) 
{
  
  // boost the daugter particles to mother center of mass 
  ROOT::Math::Boost   boost ( M.BoostToCM() ) ;
  const LoKi::LorentzVector pD = boost( D ) ;
  
  // diretcion of 3-momentum of daughter in this rest frame 
  const LoKi::ThreeVector v3D =  LoKi::ThreeVector(pD).Unit() ;
  
  // mother direction
  const LoKi::ThreeVector v3M =  LoKi::ThreeVector( M).Unit()  ;  
  
  // cosine 
  return v3D.Dot( v3M ) ;
} ;
// ============================================================================

// ============================================================================
/** This routine returns the cosine angle theta 
 *  The decay angle calculated  is that between 
 *  the flight direction of the daughter neson, "D",
 *  in the rest frame of "Q" (the parent of "D"), 
 *  with respect to "Q"'s fligth direction in "P"'s
 *  (the parent of "Q") rest frame
 * 
 *  it is a EvtDecayAngle(P,Q,D) routine form EvtGen package
 *  
 *  @param D 4-momentum of the daughetr particle 
 *  @param Q 4-momentum of mother particle 
 *  @param P "rest frame system"
 *  @return cosine of decay angle 
 * 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2004-12-03
 */
// ============================================================================
double LoKi::Kinematics::decayAngle
( const LoKi::LorentzVector& P , 
  const LoKi::LorentzVector& Q ,
  const LoKi::LorentzVector& D ) 
{
  const double pd  = P.Dot  ( D ) ;
  const double pq  = P.Dot  ( Q ) ;
  const double qd  = Q.Dot  ( D ) ;
  const double mq2 = Q.M2   () ;
  const double mp2 = P.M2   () ;
  const double md2 = D.M2   () ;
  
  const double value = 
    ( pq * pq - mq2 * mp2 ) * ( qd * qd - mq2 * md2 ) ;
  
  if ( 0 > value ) 
  {
    LoKi::Report::Error 
      ( "LoKi::Kinematics::decayAngle():: invalid 4-momenta " );
    return LoKi::Constants::InvalidAngle ;
  }
  //
  return ( pd * mq2 - pq * qd ) / sqrt( value ) ;
};
// ============================================================================

// // ============================================================================
// /** This routine evaluates the cosine of "transversity angle", 
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
// /** This routine evaluated the angle theta_FB
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

