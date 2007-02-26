// $Id: Kinematics.h,v 1.11 2007-02-26 13:13:08 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.11 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.10  2006/11/25 19:12:55  ibelyaev
//  improve Doxygen
//
// Revision 1.9  2006/05/02 14:29:09  ibelyaev
//  censored
//
// Revision 1.8  2006/03/08 21:32:34  ibelyaev
//  add WrongMass.h(.cpp)
//
// ============================================================================
#ifndef LOKI_KINEMATICS_H 
#define LOKI_KINEMATICS_H 1
// ============================================================================
// Include files
// ============================================================================
// Math Definitions 
// ============================================================================
#include "GaudiKernel/Vector4DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Point3DTypes.h"
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
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ============================================================================

namespace LoKi
{
  // Lorentz vector 
  typedef Gaudi::LorentzVector LorentzVector ;
  // Lorentz vector 
  typedef LorentzVector        Vector4D      ;
  // 3D vector 
  typedef Gaudi::XYZVector     ThreeVector   ;
  // 3D vector 
  typedef ThreeVector          Vector3D      ;
  // 3D point
  typedef Gaudi::XYZPoint      Point3D       ;
  
  /** @namespace LoKi::Kinematics 
   *  collection of few trivial functions for kinematical evaluations
   *
   *  @todo uncomment the implementation of 
   *  "transversity angle" and "forward-backward angle"
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-01-17
   */
  namespace Kinematics 
  {
    /** simple function for evaluation of the euclidiam norm
     *  for LorentzVectors 
     *  (E**2+Px**2+Py**2+Pz**2)
     *  @param vct the vector
     *  @return euclidian norm squared   
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    double euclidianNorm2 
    ( const LoKi::LorentzVector& vct ) ;

    /** simple function for evaluation of the euclidiam norm
     *  for LorentzVectors 
     *  sqrt(E**2+Px**2+Py**2+Pz**2)
     *  @param vct the vector
     *  @return euclidian norm    
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    double euclidianNorm
    ( const LoKi::LorentzVector& vct ) ;

    /** simple function for evaluation of the square of 
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
    
    /** simple function which evaluates the transverse 
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
    
    /** trivial function to evaluate the mass of 4-vector 
     *  @param mom lorenz vector
     *  @return invariant mass 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    inline double mass 
    ( const LoKi::LorentzVector& mom ) { return mom.M() ; }
    
    /** trivial function to evaluate the mass of 4-vectors  
     *  @param v1  the first lorenz vector 
     *  @param v2  the second lorenz vector
     *  @return invariant mass 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    inline double mass 
    ( const LoKi::LorentzVector& v1 , 
      const LoKi::LorentzVector& v2 ) { return mass ( v1 + v2 ) ; }
    
    /** trivial function to evaluate the mass of 4-vectors  
     *  @param v1  the first  lorenz vector 
     *  @param v2  the second lorenz vector
     *  @param v3  the third  lorenz vector
     *  @return invariant mass 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    inline double mass 
    ( const LoKi::LorentzVector& v1 , 
      const LoKi::LorentzVector& v2 ,
      const LoKi::LorentzVector& v3 ) 
    { 
      LoKi::LorentzVector v0( v1 ) ;
      v0 += v2 ;
      v0 += v3 ;      
      return mass ( v0 ) ; 
    } ;
    
    /** trivial function to evaluate the mass of 4-vectors  
     *  @param v1  the first  lorenz vector 
     *  @param v2  the second lorenz vector
     *  @param v3  the third  lorenz vector
     *  @param v4  the fourth  lorenz vector
     *  @return invariant mass 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    inline double mass 
    ( const LoKi::LorentzVector& v1 , 
      const LoKi::LorentzVector& v2 ,
      const LoKi::LorentzVector& v3 , 
      const LoKi::LorentzVector& v4 ) 
    {
      LoKi::LorentzVector v0( v1 ) ;
      v0 += v2 ;
      v0 += v3 ;      
      v0 += v4 ;      
      return mass ( v0 ) ; 
    } ;

    /** the simple function which adds a 4-momenta of all (MC)Particles
     *  from a sequence [begin,end).
     *  The actual type of elements is irrelevant, 
     *  The function relies only on existence of mehtod "momentum()"
     *  with a return-value convertible to LoKi::LorentzVector
     *
     *  e.g. eny sequence of objects, convertible to 
     *  "const LHCb::Particle*","const LHCb::MCParticle*" or 
     *  "const HepMC::GenParticle*" could be used.
     *
     *  @code 
     *  const LHCb::Vertex*             vertex = ... ;
     *  const LHCb::MCParticles*        mcps   = ... ; 
     *  const LHCb::Particles*          ps     = ... ; 
     *  std::vector<LHCb::MCParticle*>  mcv    = ... ;
     *  Range                           pr     = ... ;
     *  MCRange                         mcr    = ... ;
     *
     *  // get the 4-mometum of all daughter particle at  vertex:
     *  LoKi::LorentzVector sum1 = 
     *     LoKi::Adder::addMomenta( vertex->products().begin() , 
     *                              vertex->products().end()   ) ;
     *  
     *  // get the 4-momentum of all MC particles 
     *  LoKi::LorentzVector sum2 = 
     *     LoKi::Adder::addMomenta( mcps->begin() , mcps->end()   ) ;
     *  
     *  // get the 4-momentum of all particles 
     *  LoKi::LorentzVector sum3 = 
     *     LoKi::Adder::addMomenta( ps->begin() , ps->end()   ) ;
     *  
     *  // get the 4-momentum of all particles 
     *  LoKi::LorentzVector sum4 = 
     *     LoKi::Adder::addMomenta( pv.begin() , pv.end()   ) ;
     *
     *  // get the 4-momentum of all MC particles 
     *  LoKi::LorentzVector sum5 = 
     *     LoKi::Adder::addMomenta( mcv.begin() , mcv.end()   ) ;
     *
     *  // get the 4-momentum of all particles 
     *  LoKi::LorentzVector sum6 = 
     *     LoKi::Adder::addMomenta( pr.begin() , pr.end()   ) ;
     *
     *  // get the 4-momentum of all MC particles 
     *  LoKi::LorentzVector sum7 = 
     *     LoKi::Adder::addMomenta( mcr.begin() , mcr.end()   ) ;
     *
     *  @endcode 
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr 
     *  @date   2005-02-02
     *
     *  @see LoKi::LorentzVector 
     *
     *  @param  begin "begin"-iterator for the sequence of objects
     *  @param  end   "end"-iterator for the sequence of objects
     *  @return sum of 4-momenta of all particles 
     */
    template <class OBJECT>
    inline LoKi::LorentzVector 
    addMomenta
    ( OBJECT              begin                          , 
      OBJECT              end                            , 
      LoKi::LorentzVector result = LoKi::LorentzVector() ) 
    { 
      for ( ; begin != end ; ++begin ) 
      { if ( 0 != (*begin) ) { result += (*begin)->momentum() ; } }
      return result ;
    };
    
    /** the simple function which adds a 4-momenta of all (MC)Particles
     *  from a sequence [begin,end).
     *  The actual type of elements is irrelevant, 
     *  The function relies only on existence of mehtod "momentum()"
     *  with a return-value convertible to LoKi::LorentzVector
     *
     *  e.g. eny sequence of objects, convertible to 
     *  "const LHCb::Particle*","const LHCb::MCParticle*" or 
     *  "const HepMC::GenParticle*" could be used.
     *
     *  @code 
     *  const LHCb::Vertex*             vertex = ... ;
     *
     *  // get the 4-mometum of all charged daughter particle at  vertex:
     *  LoKi::LorentzVector sum1 = 
     *     LoKi::Adder::addMomenta
     *       ( vertex->products().begin() , 
     *         vertex->products().end()   , 0 != Q ) ;
     *  
     *  @endcode 
     *
     *  @see LoKi::Cuts::Q 
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr 
     *  @date   2005-02-02
     *
     *  @see LoKi::LorentzVector 
     *
     *  @param  first  "begin"-iterator for the sequence of objects
     *  @param  last   "end"-iterator for the sequence of objects
     *  @param  cut     the predicate 
     *  @return sum of 4-momenta of all particles 
     */
    template <class OBJECT, class PREDICATE>
    inline LoKi::LorentzVector 
    addMomenta
    ( OBJECT              first                          , 
      OBJECT              last                           , 
      PREDICATE           cut                            , 
      LoKi::LorentzVector result = LoKi::LorentzVector() )
    { 
      for ( ; first != last ; ++first ) 
      { 
        if ( 0 != (*first) && predicate ( *first ) ) 
        { result += (*first)->momentum() ; } 
      }
      return result ;
    };
    
    /** the simple function which adds a 4-momenta of all (MC)Particles
     *  fron "container/sequence/range"
     *  The actual type of container is irrelevant it could be, e.g. 
     *    - std::vector<Particle*>
     *    - std::vector<const Particle*>
     *    - std::vector<MCParticle*>
     *    - std::vector<const MCParticle*>
     *    - Particles
     *    - MCParticles
     *    - SmartRefVectro<Particle>
     *    - SmartRefVectro<MCParticle>
     *    - Range 
     *    - MCRange 
     *
     *  @code
     *  
     *  const Vertex* vertex = ... ;
     *  
     *  // get the 4-momentum of all daughter particle at  vertex:
     *  const LoKi::LorentzVector sum = 
     *     LoKi::Adder::addMomenta( vertex->products() ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::LorentzVector 
     *  @see LoKi::Kinematics::addMomenta
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr 
     *  @date   2005-02-02
     *
     *  @param seq a container of input (MC)Particles 
     *  @return sum of 4-momenta of all particles 
     */
    template <class OBJECTS>
    inline LoKi::LorentzVector addMomenta
    ( const OBJECTS&             seq                            ,
      const LoKi::LorentzVector& result = LoKi::LorentzVector() ) 
    { return addMomenta ( seq.begin() , seq.end() , result ) ; };
    
    /** This routine returns the cosine angle theta 
     *  The decay angle calculated  is that between 
     *  the flight direction of the daughter neson, "D",
     *  in the rest frame of "M" (the parent of "D"), 
     *  with respect to the boost direction from 
     *  "M"'s rest frame 
     *  
     *  @param D 4-momentum of the daughter particle 
     *  @param M 4-momentum of mother particle 
     *  @return cosine of decay angle 
     *
     *  @see LoKi::LorentzVector.h
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2004-12-03
     */
    double decayAngle
    ( const LoKi::LorentzVector& D , 
      const LoKi::LorentzVector& M ) ;

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
     *  @see LoKi::LorentzVector.h
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2004-12-03
     */
    double decayAngle
    ( const LoKi::LorentzVector& P , 
      const LoKi::LorentzVector& Q ,
      const LoKi::LorentzVector& D ) ;
    
//     /** This routine evaluates the cosine of "transversity angle", 
//      *  useful e.g. to disantangle the different partial waves in 
//      *  0 -> 1 + 1 decay (e.g. Bs -> J/psi Phi) 
//      *  
//      *  The code is kindly provided by Gerhard Raven 
//      * 
//      *  @param l1 4-vector of the first  particle, e.g. mu+
//      *  @param l2 4-vector of the second particle, e.g. mu- 
//      *  @param p1 4-vector of the third  particle, e.g. K+
//      *  @param p2 4-vector of the fourth particle, e.g. K- 
//      *  @return the cosine of transversity angle 
//      * 
//      *  @see LoKi::LorentzVector.h
//      *
//      *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
//      *  @date 2004-12-03
//      */ 
//     double transversityAngle 
//     ( const LoKi::LorentzVector& l1 , 
//       const LoKi::LorentzVector& l2 , 
//       const LoKi::LorentzVector& p1 , 
//       const LoKi::LorentzVector& p2 ) ;
    
//     /** This routine evaluated the angle theta_FB
//      *  used e.g. for evaluation of forward-backward 
//      *  asymmetry for decay B -> K* mu+ mu- 
//      *  The angle calculated is that 
//      *  between between the mu+ and K^*0 momenta 
//      *  in the di-muon rest frame
//      *  
//      *  The code is kindly provided by Helder Lopes 
//      *
//      *  @param  K  4-momenutm of   "K*"
//      *  @param  l1 4-momentum of the first  lepton
//      *  @param  l2 4-momentum of the second lepton
//      *  @return the cosine of theta_FB 
//      * 
//      *  @see LoKi::LorentzVector.h
//      *
//      *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
//      *  @date 2004-12-03
//      */
//     double forwardBackwardAngle
//     ( const LoKi::LorentzVector& K  , 
//       const LoKi::LorentzVector& l1 , 
//       const LoKi::LorentzVector& l2 ) ;

  }  // end of namespace Kinematics  
} // end of namespace LoKi

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_KINEMATICS_H
// ============================================================================
