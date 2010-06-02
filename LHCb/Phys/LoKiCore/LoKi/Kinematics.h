// $Id: Kinematics.h,v 1.21 2010-06-02 15:40:31 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_KINEMATICS_H 
#define LOKI_KINEMATICS_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/SymmetricMatrixTypes.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/KinTypes.h"
#include "LoKi/Constants.h"
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
  // ===========================================================================
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
    // ========================================================================
    /** simple function for evaluation of the euclidiam norm
     *  for LorentzVectors:
     * 
     *  \f$  \left| v \right|_{euclid}^2 = 
     *    e^2 + p_x^2 + p_y^2 + p_z^2 \f$ 
     *  
     *  @attention this value is <b>NOT</b> Lorentz Invariant!
     *
     *  @param vct the vector
     *  @return euclidian norm squared   
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    GAUDI_API
    double euclidianNorm2 ( const LoKi::LorentzVector& vct ) ;
    // ========================================================================
    /** simple function for evaluation of the euclidiam norm
     *  for LorentzVectors 
     *
     *  \f$  \left| v \right|_{euclid} = 
     *      \sqrt{ e^2 + p_x^2 + p_y^2 + p_z^2 } \f$ 
     *
     *  @attention this value is <b>NOT</b> Lorentz Invariant!
     *
     *  @param vct the vector
     *  @return euclidian norm    
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    GAUDI_API
    double euclidianNorm
    ( const LoKi::LorentzVector& vct ) ;
    // ========================================================================
    /** simple function for evaluation of the square of 
     *  the euclidian distance in between two Lorentz-vectors:
     *  
     *  \f$ \delta^2_{euclid}\left( v_1 , v_2 \right) = 
     *   \left| v_1 - v_2 \right|^2_{euclid} = 
     *    (e_1-e_2)^2 + (p_{x1}-p_{x2})^2 
     *    + (p_{y1}-p_{y2})^2 
     *    + (p_{z1}-p_{z2})^2 \f$ 
     *   
     *  @attention this value is <b>NOT</b> Lorentz Invariant!
     *
     *  @param vct1 the first vector
     *  @param vct2 the second vector
     *  @return the square of euclidian distance 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    GAUDI_API
    double delta2euclidian 
    ( const LoKi::LorentzVector& vct1 , 
      const LoKi::LorentzVector& vct2 ) ;
    // ========================================================================    
    /** simple function which evaluates the transverse 
     *  momentum with respect a certain 3D-direction:
     * 
     * \f$
     *  r_T = \left| \vec{r} \right| = 
     *  = \left| \vec{v} - 
     * \vec{d}\frac{\left(\vec{v}\vec{d}\right)}
     *  { \left| \vec{d} \right|^2 } \right|
     *  \f$
     *  
     *  @param mom the momentum
     *  @param dir the direction
     *  @return the transverse moementum with respect to the direction
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    GAUDI_API
    double transverseMomentumDir 
    ( const LoKi::ThreeVector& mom , 
      const LoKi::ThreeVector& dir ) ;
    /** simple function which evaluates the transverse 
     *  momentum with respect a certain 3D-direction:
     * 
     * \f$
     *  r_T = \left| \vec{r} \right| = 
     *  = \left| \vec{v} - 
     * \vec{d}\frac{\left(\vec{v}\vec{d}\right)}
     *  { \left| \vec{d} \right|^2 } \right|
     *  \f$
     *  
     *  @param mom the momentum
     *  @param dir the direction
     *  @return the transverse moementum with respect to the direction
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    inline double transverseMomentumDir 
    ( const LoKi::LorentzVector& mom , 
      const LoKi::ThreeVector&   dir ) 
    { return transverseMomentumDir ( mom.Vect() , dir ) ; }
    // ========================================================================    
    /** trivial function to evaluate the mass of 4-vector 
     *  @param mom lorenz vector
     *  @return invariant mass 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    inline double mass 
    ( const LoKi::LorentzVector& mom ) { return mom.M() ; }
    // ========================================================================    
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
    // ========================================================================    
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
    } 
    // ========================================================================
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
    } 
    // ========================================================================
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
     *  @param  result the initial value 
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
    }
    // ========================================================================
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
     *  @param  result  the initial value for result 
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
    }
    // ========================================================================
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
    { return addMomenta ( seq.begin() , seq.end() , result ) ; }
    // ========================================================================
    /** This routine returns the cosine angle theta 
     *  The decay angle calculated  is that between 
     *  the flight direction of the daughter neson, "D",
     *  in the rest frame of "Q" (the parent of "D"), 
     *  with respect to "Q"'s flight direction in "P"'s
     *  (the parent of "Q") rest frame
     * 
     *  \f$ 
     *  \cos \theta = \frac
     *  { \left(P \cdot D\right)Q^2 - 
     *    \left(P \cdot Q\right)\left(D \cdot Q \right) }
     *  {\sqrt{ \left[ \left( P \cdot Q \right)^2 - Q^2 P^2 \right] 
     *          \left[ \left( D \cdot Q \right)^2 - Q^2 D^2 \right] } } 
     *  \f$ 
     *  
     *  Note that the expression has the symmetry: \f$ P \leftrightarrow D \f$ 
     *  
     *  Essentially it is a rewritten <c>EvtDecayAngle(P,Q,D)</c> 
     *  routine from EvtGen package
     *  
     *  @param D 4-momentum of the daughter particle 
     *  @param Q 4-momentum of mother particle 
     *  @param P "rest frame system"
     *  @return cosine of decay angle 
     *
     *  @see LoKi::LorentzVector
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2004-12-03
     */
    GAUDI_API
    double decayAngle
    ( const LoKi::LorentzVector& P , 
      const LoKi::LorentzVector& Q ,
      const LoKi::LorentzVector& D ) ;
    // ========================================================================
    /** This routine returns the cosine angle theta 
     *  The decay angle calculated  is that between 
     *  the flight direction of the daughter neson, "D",
     *  in the rest frame of "Q" (the parent of "D"), 
     *  with respect to "Q"'s flight direction in "P"'s
     *  (the parent of "Q") rest frame
     * 
     *  \f$ 
     *  \cos \theta = \frac
     *  { \left(P \cdot D\right)Q^2 - 
     *    \left(P \cdot Q\right)\left(D \cdot Q \right) }
     *  {\sqrt{ \left[ \left( P \cdot Q \right)^2 - Q^2 P^2 \right] 
     *          \left[ \left( D \cdot Q \right)^2 - Q^2 D^2 \right] } } 
     *  \f$ 
     *  
     *  Note that the expression has the symmetry: \f$ P \leftrightarrow D \f$ 
     *  
     *  Essentially it is a rewritten <c>EvtDecayAngle(P,Q,D)</c> 
     *  routine from EvtGen package
     *  
     *  @param D 4-momentum of the daughter particle 
     *  @param Q 4-momentum of mother particle 
     *  @param P "rest frame system"
     *  @return cosine of decay angle 
     *
     *  @see LoKi::LorentzVector
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2004-12-03
     */    
    inline double cosDecayAngle
    ( const LoKi::LorentzVector& P , 
      const LoKi::LorentzVector& Q ,
      const LoKi::LorentzVector& D ) { return decayAngle ( P , Q, D ) ; }
    // ========================================================================
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
     *  Clearly it is a variant of 3-argument with the 
     *  P-argument to be of type (0,0,0,E) 
     *  (=="laborator frame") 
     *
     *  @see LoKi::LorentzVector
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2004-12-03
     */
    inline double decayAngle
    ( const LoKi::LorentzVector& D , 
      const LoKi::LorentzVector& M ) 
    {
      return decayAngle ( LoKi::LorentzVector (0,0,0,2*M.E()) , M , D ) ;
    }
    // ========================================================================
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
     *  Clearly it is a variant of 3-argument with the 
     *  P-argument to be of type (0,0,0,E) 
     *  (=="laborator frame") 
     *
     *  @see LoKi::LorentzVector
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2004-12-03
     */    
    inline double cosDecayAngle
    ( const LoKi::LorentzVector& D , 
      const LoKi::LorentzVector& M ) { return decayAngle ( D , M ) ; }
    // ========================================================================
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
     *  @see LoKi::LorentzVector
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2004-12-03
     */
    double decayAngle_
    ( const LoKi::LorentzVector& D , 
      const LoKi::LorentzVector& M ) ;
    // ========================================================================
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
    //      *  @see LoKi::LorentzVector
    //      *
    //      *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
    //      *  @date 2004-12-03
    //      */
    //     double forwardBackwardAngle
    //     ( const LoKi::LorentzVector& K  , 
    //       const LoKi::LorentzVector& l1 , 
    //       const LoKi::LorentzVector& l2 ) ;
    // ========================================================================
    /** simple function which evaluates the magnitude of 3-momentum 
     *  of particle "v" in the rest system of particle "M" 
     *
     *  \f$ \left|\vec{p}\right| = 
     *     \sqrt{  \frac{\left(v\cdot M\right)^2}{M^2} -v^2} \f$
     * 
     *  Note that this is clear Lorentz invarinat expresssion. 
     * 
     *  @attention particle M must be time-like particle: M^2 > 0 !
     *
     *  @param v the vector to be checked 
     *  @param M the defintion of "rest"-system
     *  @return the magnitude of 3D-momentum of v in rest-frame of M 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-07-27
     */
    GAUDI_API
    double restMomentum 
    ( const LoKi::LorentzVector& v , 
      const LoKi::LorentzVector& M ) ;
    // ========================================================================
    /** simple function which evaluates the energy
     *  of particle "v" in the rest system of particle "M" 
     * 
     *  \f$ e = \frac{ v \cdot M }{\sqrt{ M^2 } } \f$
     *  
     *  Note that this is clear Lorentz invarinat expresssion. 
     * 
     *  @attention particle M must be time-like particle: M^2 > 0 !
     *
     *  @param v the vector to be checked 
     *  @param M the defintion of "rest"-system
     *  @return the energy of v in rest-frame of M 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-07-27
     */
    GAUDI_API
    double restEnergy 
    ( const LoKi::LorentzVector& v , 
      const LoKi::LorentzVector& M ) ;
    // ========================================================================
    /** simple function to evaluate the cosine angle between 
     *  two directions (v1 and v2) in the rest system of M 
     *
     * \f$ 
     * \cos\theta = 
     * \frac{\vec{p}_1\vec{p}_2}{\left|\vec{p}_1\right|
     * \left|\vec{p}_2\right|} = 
     * \frac{1}{\left|\vec{p}_1\right|\left|\vec{p}_2\right|} 
     * \left( E_1E_2 -\frac{1}{2}
     * \left(\left(v_1+v_2\right)^2-v_1^2-v_2^2 \right) \right), 
     * \f$ 
     *  where 
     *  \f$ 
     *  E_1 E_2 = \frac{ \left ( v_1 \cdot M\right) \left (v_2 \cdot M \right ) }{M^2}
     *  \f$ 
     *  and 
     *  \f$ 
     * \left|\vec{p}_1\right|\left|\vec{p}_2\right| = 
     * \sqrt{ 
     * \left( \frac{\left(v_1\cdot M\right)^2}{M^2}-v_1^2 \right)   
     *      \left( \frac{\left(v_2\cdot M\right)^2}{M^2}-v_2^2 \right) }
     * \f$ 
     *
     *  Note that the expressions are clear Lorentz invariant
     * 
     *  @attention the particle M must be time-like particle: M^2 > 0 ! 
     *  @param v1 the first vector 
     *  @param v2 the last vector 
     *  @param M  the defintion of rest-system
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-07-27
     */
    GAUDI_API
    double cosThetaRest 
    ( const LoKi::LorentzVector& v1 ,
      const LoKi::LorentzVector& v2 , 
      const LoKi::LorentzVector& M  ) ; 
    // ========================================================================
    /** evaluate the angle \f$\chi\f$ 
     *  beween two decay planes, 
     *  formed by particles v1&v2 and h1&h2 correspondingly. 
     *  The angle is evaluated in the rest frame 
     *  of "mother" particles (defined as v1+v2+h1+h2) 
     *      
     *  The sign is set according to Thomas Blake's code from
     *  P2VVAngleCalculator tool
     *  @see P2VVAngleCalculator 
     *
     *  @see LoKi::Kinematics::cosDecayAngleChi 
     *  @see LoKi::Kinematics::sinDecayAngleChi 
     *
     *  @param d1 the 1st daughter 
     *  @param d2 the 2nd daughter 
     *  @param h1 the 3rd daughter 
     *  @param h2 the 4th daughter 
     *  @return angle chi 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-07-27
     */
    GAUDI_API
    double decayAngleChi 
    ( const LoKi::LorentzVector& d1 , 
      const LoKi::LorentzVector& d2 , 
      const LoKi::LorentzVector& h1 , 
      const LoKi::LorentzVector& h2 ) ;  
    // ========================================================================
    /** evaluate \f$\cos \chi\f$, where \f$\chi\f$ if the angle  
     *  beween two decay planes, formed by particles d1&d2 
     *  and h1&h2 correspondingly. 
     *
     *  The angle is evaluated in the rest frame 
     *  of "mother" particles (defined as d1+d2+h1+h2) 
     *
     *  The angle is evaluated using the explicit 
     *  Lorenzt-invariant expression:
     *  \f[
     *  \cos \chi =      
     *   - \frac{ L_D^{\mu} L_H^{\mu} }
     *     { \sqrt{ \left[ -L_D^2 \right]\left[ -L_H^2 \right] }},
     &   =
     *   - \frac{ 
	   *     \epsilon_{ijkl}d_1^{j}d_2^{k}\left(h_1+h_2\right)^l
     *     \epsilon_{imnp}h_1^{m}h_2^{n}\left(d_1+d_2\right)^p }
     *     { \sqrt{ \left[ -L_D^2 \right]\left[ -L_H^2 \right] }},
     *  \f] 
     *  where "4-normales" are defined as:
     *  \f$ 
     *   L_D^{\mu} = \epsilon_{\mu\nu\lambda\kappa}
     *                d_1^{\nu}d_2^{\lambda}\left(h_1+h_2\right)^{\kappa} 
     *  \f$ 
     *   and 
     *  \f$ 
     *   L_H^{\mu} = \epsilon_{\mu\lambda\delta\rho}
     *                h_1^{\lambda}h_2^{\delta}\left(d_1+d_2\right)^{\rho} 
     *   \f$.
     *
     *  @param d1 the 1st daughter 
     *  @param d2 the 2nd daughter 
     *  @param h1 the 3rd daughter 
     *  @param h2 the 4th daughter 
     *  @return cos(chi) 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-07-27
     */
    GAUDI_API
     double cosDecayAngleChi 
     ( const LoKi::LorentzVector& d1 , 
       const LoKi::LorentzVector& d2 , 
       const LoKi::LorentzVector& h1 , 
       const LoKi::LorentzVector& h2 ) ; 
    // ========================================================================
    /** evaluate \f$\sin\chi\f$, where \f$\chi\f$ is the angle 
     *  beween two decay planes, 
     *  formed by particles v1&v2 and h1&h2 correspondingly. 
     *  The angle is evaluated in the rest frame 
     *  of "mother" particles (defined as v1+v2+h1+h2) 
     *      
     *  The angle is  calculated using the explicit 
     *   Lorentz-invariant expression:
     *  \f[ 
     *   \sin \chi = 
     *   \frac  { 
     *   \epsilon_{\mu\nu\lambda\delta}
     *   L_D^{\mu}L_H^{\nu}H^{\lambda}M^{\delta} }
     *   { \sqrt{ 
	   *   \left[ -L_D^2 \right]\left[ -L_H^2 \right] 
     *   \left[ \left( H\ cdot M\right)^2-H^2M^2 \right] 
     *   }} = \frac { 
     *   \epsilon_{\mu\nu\lambda\delta}
     *   d_1^{\mu}d_2^{\nu}h_1^{\lambda}h_2^{\delta}
     *   \left( \left( D \cdot H \right)^2 - D^2H^2 \right) }
     *   { \sqrt{ 
	   *   \left[ -L_D^2 \right]\left[ -L_H^2    \right] 
     *   \left[ \left(H\cdot M\right)^2-H^2M^2 \right] 
	   *   }},
     *  \f] 
     *  where "4-normales" are defined as:
     *  \f$
     *  L_D^{\mu} = \epsilon_{\mu\nu\lambda\kappa}
     *                d_1^{\nu}d_2^{\lambda}\left(h_1+h_2\right)^{\kappa} 
     *  \f$, 
     *  \f$ 
     *  L_H^{\mu} = \epsilon_{\mu\lambda\delta\rho}
     *  h_1^{\lambda}h_2^{\delta}\left(d_1+d_2\right)^{\rho} 
     *  \f$
     *  and   \f$ D = d_1 + d_2 \f$, 
     *        \f$ H = h_1 + h_2 \f$, 
     *        \f$ M = D + H = d_1 + d_2 + h_1+h_2 \f$. 
     *
     *  The sign for <c>sin</c> is set according to 
     *  Thomas Blake's code from
     *  P2VVAngleCalculator tool
     *  @see P2VVAngleCalculator 
     *
     *  @param d1 the 1st daughter 
     *  @param d2 the 2nd daughter 
     *  @param h1 the 3rd daughter 
     *  @param h2 the 4th daughter 
     *  @return angle chi 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-07-27
     */ 
    GAUDI_API
    double sinDecayAngleChi 
    ( const LoKi::LorentzVector& d1 , 
      const LoKi::LorentzVector& d2 , 
      const LoKi::LorentzVector& h1 , 
      const LoKi::LorentzVector& h2 ) ;
    // ========================================================================
    /** evaluate the Armenteros-Podolanski variable \f$\mathbf{\alpha}\f$, 
     *  defined as:
     *  \f[
     *  \mathbf{\alpha} = \dfrac
     *  { \mathrm{p}^{\mathrm{L},1} - \mathrm{p}^{\mathrm{L},1} }
     *  { \mathrm{p}^{\mathrm{L},1} + \mathrm{p}^{\mathrm{L},1} },
     *  \f]
     *  where 
     *   \f$ \mathrm{p}^{\mathrm{L},1}\f$ and 
     *   \f$ \mathrm{p}^{\mathrm{L},2}\f$ are longitudinal momentum
     *   components for the first and the seco ddaughter particles 
     *   with respect to the total momentum direction. 
     *
     *  Clearly this expression could be rewritten in an equivalent 
     *  form which however much more easier for calculation:
     *  \f[
     *  \mathbf{\alpha} = \dfrac
     *  { \vec{\mathbf{p}}_1^2 - \vec{\mathbf{p}}_2^2 }  
     *  { \left( \vec{\mathbf{p}}_1 + \vec{\mathbf{p}}_2 \right)^2 }  
     *  \f]
     * 
     *  @attention instead of 
     *     2D \f$\left(\mathrm{p_T},\mathbf{\alpha}\right)\f$ diagram, 
     *     in the case of twobody decays at LHCb it is much better to 
     *     use 2D diagram \f$\left(\cos \theta, \mathrm{m} \right)\f$
     *     diagram, where \f$\cos\theta\f$-is the decay 
     *     angle,see the variable LV01, and \f$\mathrm{m}\f$ is an 
     *     invariant evalauted for some (fixed) mass prescription, 
     *     e.g. \f$\pi^+\pi^-\f$.  
     *
     *  @param d1  three momentum of the first  daughter particle 
     *  @param d2  three momentum of the second daughter particle 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-21 
     */
    GAUDI_API
    double armenterosPodolanskiX 
    ( const LoKi::ThreeVector& d1 , 
      const LoKi::ThreeVector& d2 ) ;
    // ========================================================================
    /** evaluate the Armenteros-Podolanski variable \f$\mathbf{\alpha}\f$, 
     *  defined as:
     *  \f[
     *  \mathbf{\alpha} = \dfrac
     *  { \mathrm{p}^{\mathrm{L},1} - \mathrm{p}^{\mathrm{L},1} }
     *  { \mathrm{p}^{\mathrm{L},1} + \mathrm{p}^{\mathrm{L},1} },
     *  \f]
     *  where 
     *   \f$ \mathrm{p}^{\mathrm{L},1}\f$ and 
     *   \f$ \mathrm{p}^{\mathrm{L},2}\f$ are longitudinal momentum
     *   components for the first and the seco ddaughter particles 
     *   with respect to the total momentum direction. 
     *
     *  Clearly this expression could be rewritten in an equivalent 
     *  form which however much more easier for calculation:
     *  \f[
     *  \mathbf{\alpha} = \dfrac
     *  { \vec{\mathbf{p}}_1^2 - \vec{\mathbf{p}}_2^2 }  
     *  { \left( \vec{\mathbf{p}}_1 + \vec{\mathbf{p}}_2 \right)^2 }  
     *  \f]
     * 
     *  @attention instead of 
     *     2D \f$\left(\mathrm{p_T},\mathbf{\alpha}\right)\f$ diagram, 
     *     in the case of twobody decays at LHCb it is much better to 
     *     use 2D diagram \f$\left(\cos \theta, \mathrm{m} \right)\f$
     *     diagram, where \f$\cos\theta\f$-is the decay 
     *     angle,see the variable LV01, and \f$\mathrm{m}\f$ is an 
     *     invariant evalauted for some (fixed) mass prescription, 
     *     e.g. \f$\pi^+\pi^-\f$.  
     *
     *  @param d1  four momentum of the first  daughter particle 
     *  @param d2  four momentum of the second daughter particle 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-21 
     */
    inline double armenterosPodolanskiX 
    ( const LoKi::LorentzVector& d1 , 
      const LoKi::LorentzVector& d2 ) 
    { 
      return armenterosPodolanskiX ( d1.Vect () , d2. Vect() ) ; 
    }
    // ========================================================================
    /** evaluate the chi2 of the mass  \f$\chi^2_{M}\f$.
     * 
     *  \f$\chi^2_{M}\f$ is evaluated as 
     *  \f$\chi^2_{M}=\chi^2_{M^2}\f$, where 
     *  \f$\chi^2_{M^2} = \mathrm{V} \left( \Delta M^2 \right)^2 \f$ and 
     *  \f$ \mathrm{V} = \left( \mathrm{P^T}\mathrm{C}\mathrm{P}\right)^{-1}\f$,
     *  and \f$C\f$ is covariance matrix, and 
     *  \f$\mathrm{P} = 2\cdot\left(  - p_x , -p_y , -p_z , e \right)^T\f$.
     * 
     *  Essentially it is just 1-step of the mass-fit
     *
     *  @param mass the nominal mass 
     *  @param mom  four momentum 
     *  @param cov  four momentum covariance matrix 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-23 
     */
    GAUDI_API
    double chi2mass 
    ( const double               mass , 
      const LoKi::LorentzVector& mom  , 
      const Gaudi::SymMatrix4x4& cov  ) ;
    // ========================================================================
    /** @evaluate the (cosine) of tansversity angle,
     *  \f$ \cos \theta_{\mathrm{tr}}\f$, 
     *  e.g. for decay  
     *  \f$ \mathrm{B}^0_{\mathrm{s}} \to 
     *       \left( \mathrm{J}/\psi \to \mu^+ \mu^-               \right)
     *       \left( \phi            \to \mathrm{K}^+ \mathrm{K}^- \right) \$
     * 
     *  The evaluation is performed using the explicit Lorentz-invariant 
     *  expression:
     *  \f[
     *   \cos \theta_{\mathrm{tr}} = 
     *    \frac{ \epsilon_{\mu\nu\lambda\kappa}
     *          d_1^{\mu}h_1^{\nu}h_2^{\lambda}L_H^{\kappa} }
     *    {
     *     \sqrt{  \left( d_1 \cdot D  \right) / D^2 - d_1^2 } 
     *     \sqrt{  - L_H^2 }      
     *    },     
     *  \f]
     * where 4-normal \f$ L_H^{\mu}\f$ is defined as  
     *  \f$ 
     *  L_H^{\mu} = \epsilon_{\mu\lambda\delta\rho}
     *  h_1^{\lambda}h_2^{\delta}\left(d_1+d_2\right)^{\rho} 
     *  \f$, and \f$ D = d_1 + d_2 \f$. 
     *
     *  @param d1 (INPUT) the 4-momentum of positive lepton 
     *  @param d2 (INPUT) the 4-momentum of negative lepton 
     *  @param h1 (INPUT) the 4-momentum of positive kaon 
     *  @param h2 (INPUT) the 4-momentum of negative kaon  
     *  @return the cosine of the transversity angle theta 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-06-02
     */
    GAUDI_API
    double cosThetaTr 
    ( const LoKi::LorentzVector& d1 , 
      const LoKi::LorentzVector& d2 , 
      const LoKi::LorentzVector& h1 , 
      const LoKi::LorentzVector& h2 ) ;
    // ========================================================================
    /** evaluate the cosine of transverse angle phi, 
     *  \f$ \cos \phi_{\mathrm{tr}}\f$, e.g. for decay  
     *  \f$ \mathrm{B}^0_{\mathrm{s}} \to 
     *       \left( \mathrm{J}/\psi \to \mu^+ \mu^-               \right)
     *       \left( \phi            \to \mathrm{K}^+ \mathrm{K}^- \right) \$
     *    
     *  The evaluation is performed using the explicit Lorentz-invariant 
     *  expression as angle between the ``in-plane'' vector \f$q\f$, and 
     *  vector \f$H\f$ in rest frame of \f$D\f$, where 
     *  \f$  q = d_1 - \frac{ d_1 \cdot L_H}{L_H^2}L_H \f$, 
     *  the ``4-normal'' is defiend as 
     *  \f$  L_H^{\mu} = \epsilon_{\mu\lambda\delta\rho}
     *  h_1^{\lambda}h_2^{\delta}\left(d_1+d_2\right)^{\rho} 
     *  \f$, \f$ D = d_1 + d_2 \f$, \f$ H = h_1 + h_2 \f$.
     *
     *  @see LoKi::Kinematics::cosThetaRest 
     *  @param d1 (INPUT) the 4-momentum of positive lepton 
     *  @param d2 (INPUT) the 4-momentum of negative lepton 
     *  @param h1 (INPUT) the 4-momentum of positive kaon 
     *  @param h2 (INPUT) the 4-momentum of negative kaon  
     *  @return the cosine of the transversity angle theta 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-06-02
     */
    GAUDI_API
    double cosPhiTr 
    ( const LoKi::LorentzVector& d1 , 
      const LoKi::LorentzVector& d2 , 
      const LoKi::LorentzVector& h1 , 
      const LoKi::LorentzVector& h2 ) ;
    // ========================================================================
    /** evaluate the sine of transverse angle phi, \f$ \sin \phi_{\mathrm{tr}}\f$, 
     *  e.g. for decay  
     *  \f$ \mathrm{B}^0_{\mathrm{s}} \to 
     *       \left( \mathrm{J}/\psi \to \mu^+ \mu^-               \right)
     *       \left( \phi            \to \mathrm{K}^+ \mathrm{K}^- \right) \$
     *    
     *  The evaluation is performed using the explicit Lorentz-invariant 
     *  expression:
     *  \f[
     *   \sin \phi_{\mathrm{tr}} = 
     *    - frac { 
     *      \epsilon_{\mu\nu\lambda\kappa}
     *       d_1^{\mu}L_H^{\mu}D^{\lambda}H^{\kappa}
     *     }{
     *     \sqrt{ -L^2 }
     *     \sqrt{  D^2 }
     *     \sqrt{ \left( q \cdot D  \right) / D^2 - q^2 } 
     *     \sqrt{ \left( M \cdot D  \right) / D^2 - M^2 } 
     *     }
     *  \f] 
     * where 4-normal \f$ L_H^{\mu}\f$ is defined as  
     *  \f$ 
     *  L_H^{\mu} = \epsilon_{\mu\lambda\delta\rho}
     *  h_1^{\lambda}h_2^{\delta}\left(d_1+d_2\right)^{\rho} 
     *  \f$,
     * \f$ D = d_1 + d_2 \f$,  
     * \f$ H = h_1 + h_2 \f$,  
     * \f$ M = D+ H \f$ and ``in-plane'' 4-vector \f$q\f$ is defined as 
     *  \f$  q = d_1 - \frac{ d_1 \cdot L_H}{L_H^2}L_H \f$.
     *
     *  @param d1 (INPUT) the 4-momentum of positive lepton 
     *  @param d2 (INPUT) the 4-momentum of negative lepton 
     *  @param h1 (INPUT) the 4-momentum of positive kaon 
     *  @param h2 (INPUT) the 4-momentum of negative kaon  
     *  @return the cosine of the transversity angle theta 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-06-02
     */
    GAUDI_API
    double sinPhiTr 
    ( const LoKi::LorentzVector& d1 , 
      const LoKi::LorentzVector& d2 , 
      const LoKi::LorentzVector& h1 , 
      const LoKi::LorentzVector& h2 ) ;
    // ========================================================================
    /** evaluate the transverse angle phi, \f$ \phi_{\mathrm{tr}}\f$, 
     *  e.g. for decay  
     *  \f$ \mathrm{B}^0_{\mathrm{s}} \to 
     *       \left( \mathrm{J}/\psi \to \mu^+ \mu^-               \right)
     *       \left( \phi            \to \mathrm{K}^+ \mathrm{K}^- \right) \$
     *    
     *  The evaluation is performed using the explicit Lorentz-invariant 
     *  expression for \f$ \sin \phi_{\mathrm{tr}} \f$ and 
     *   \f$ \cos \phi_{\mathrm{tr}} \f$ and 
     * 
     *  @see LoKi::Kinematics::cosPhiTr 
     *  @see LoKi::Kinematics::sinPhiTr 
     *
     *  @param d1 (INPUT) the 4-momentum of positive lepton 
     *  @param d2 (INPUT) the 4-momentum of negative lepton 
     *  @param h1 (INPUT) the 4-momentum of positive kaon 
     *  @param h2 (INPUT) the 4-momentum of negative kaon  
     *  @return the cosine of the transversity angle theta 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-06-02
     */
    GAUDI_API
    double anglePhiTr 
    ( const LoKi::LorentzVector& d1 , 
      const LoKi::LorentzVector& d2 , 
      const LoKi::LorentzVector& h1 , 
      const LoKi::LorentzVector& h2 ) ;
    // ========================================================================
  } //                                        end of namespace LoKi::Kinematics  
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_KINEMATICS_H
// ============================================================================
