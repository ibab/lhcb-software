// $Id: GenParticleCuts.h,v 1.9 2007-03-04 16:41:18 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.9 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.8  2006/08/26 11:28:01  ibelyaev
//  add GSTATUS functor: HepMC::GenParticle::status()
//
// Revision 1.7  2006/05/26 17:32:11  ibelyaev
//  update to allow HepMCParticleMaker to be OK
//
// Revision 1.6  2006/05/02 14:30:27  ibelyaev
//  censored
//
// ============================================================================
#ifndef LOKI_GENPARTICLECUTS_H 
#define LOKI_GENPARTICLECUTS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/GenTypes.h"
#include "LoKi/GenParticles.h"
#include "LoKi/GenPIDOperators.h"
// ============================================================================

// ============================================================================
/** @file
 *
 *  Collection of useful functions/predicates to deal with 
 *  HepMC generator information
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
 *  @date 2006-01-23 
 */
// ============================================================================


namespace LoKi 
{
  namespace Cuts 
  {
    /** @var GTRUE 
     *  primitive predicate, "always true"
     *  @see LoKi::BooleanConstant
     *  @see LoKi::Cuts::PTRUE 
     *  @see LoKi::Cuts::VTRUE 
     *  @see LoKi::Cuts::MCTRUE 
     *  @see LoKi::Cuts::MCVTRUE 
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr 
     *  @date 2005-03-27
     */
    const LoKi::BooleanConstant<const HepMC::GenParticle*> GTRUE  ( true  ) ;

    /** @var GFALSE
     *  primitive predicate, "always false"
     *  @see LoKi::BooleanConstant
     *  @see LoKi::GenTypes::GTRUE 
     *  @see LoKi::Cuts::PFALSE 
     *  @see LoKi::Cuts::VFALSE 
     *  @see LoKi::Cuts::MCFALSE 
     *  @see LoKi::Cuts::MCVFALSE 
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr 
     *  @date 2005-03-27
     */
    const LoKi::BooleanConstant<const HepMC::GenParticle*> GFALSE ( false ) ;
    
    /** @var GALL
     *  primitive predicate, "always true"
     *  @see LoKi::BooleanConstant
     *  @see LoKi::GenTypes::GTRUE
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr 
     *  @date 2005-03-27
     */
    const LoKi::BooleanConstant<const HepMC::GenParticle*> GALL  = GTRUE    ;
    
    /** @var GNONE
     *  primitive predicate, "always false"
     *  @see LoKi::BooleanConstant
     *  @see LoKi::GenTypes::GFALSE
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr 
     *  @date 2005-03-27
     */
    const LoKi::BooleanConstant<const HepMC::GenParticle*> GNONE = GFALSE   ;
    
    /** @var GONE
     *  primitive function, "always 1"
     *  @see LoKi::Constant
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr 
     *  @date 2005-03-27
     */
    const LoKi::Constant       <const HepMC::GenParticle*> GONE   ( 1.0   ) ;
    
    /** @var GZERO
     *  primitive function, "always 0"
     *  @see LoKi::Constant
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr 
     *  @date 2005-03-27
     */
    const LoKi::Constant       <const HepMC::GenParticle*> GZERO  ( 0.0   ) ;    


    /** Minimum from 2 functions 
     *  
     *  @code 
     *  
     *  GFun f1 = ... ;
     *  GFun f2 = ... ;
     *  
     *  GFun f = GMIN( f1 , f2 ) ;
     *  
     *  @endcode 
     *  
     *  @see LoKi::Min
     */
    typedef LoKi::Min<const HepMC::GenParticle*>                      GMIN;

    /** Maxumum from 2 functions 
     *  
     *  @code 
     *  
     *  GFun f1 = ... ;
     *  MCFun f2 = ... ;
     *  
     *  MCFun f = MCMAX( f1 , f2 ) ;
     *  
     *  @endcode 
     *  
     *  @see LoKi::Max
     */
    typedef LoKi::Max<const HepMC::GenParticle*>                     GMAX;
    
    /** Simple "switch"
     *  The function evaluated the predicate and returns 
     *  one of the predefined values, depending on result on 
     *  predicate evaluation.
     *  In some sense it is a conversion of "predicate" to "function"
     * 
     *  @code
     *
     *   const HepMC::GenParticle* mcp = ...  ;
     *
     *   GFun fun = GSSWITCH( GHADRON , 1 , -1 ) 
     *
     *   const double value = fun( p ) ;
     *
     *  @endcode 
     *
     *  For this example function returns 1 if  the particle is hadron and 
     *   particle and -1 otherwise 
     *
     *  @see LoKi::SipleSwitch 
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     */
    typedef LoKi::SimpleSwitch<const HepMC::GenParticle*>           GSSWITCH;
    
    /** switch"
     *  The function evaluates the predicate and returns 
     *  the values of one of the predefined functions, 
     *  depending on result on predicate evaluation.
     * 
     *  @code
     *
     *   const HepMC::Particle* mcp = ...  ;
     *
     *   GFun fun = GSWITCH( HADRON , GP , GE ) ;
     *
     *   const double value = fun( p ) ;
     *
     *  @endcode 
     *
     *  For this example function returns the value of 
     *  P for hadrons and E for other particles 
     *
     *  @see LoKi::Switch 
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     */
    typedef LoKi::Switch<const HepMC::GenParticle*>                  GSWITCH;

    /** @var GBAR
     *  the most primitive function - it return the "barcode" 
     *  of HepMC::GenParticle object
     *
     *  @see LoKi::Cuts::BarCode
     *  @see HepMC::GenParticle
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date   2005-03-26
     */
    const   LoKi::GenParticles::BarCode                     GBAR     ;

    /** @var GBARCODE
     *  the most primitive function - it return the "barcode" 
     *  of HepMC::GenParticle object
     *
     *  @see LoKi::Cuts::BarCode
     *  @see HepMC::GenParticle
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date   2005-03-26
     */
    const   LoKi::GenParticles::BarCode                     GBARCODE ;
    
    /** @var GID 
     *  primitive evaluator of ParticleID for HepMC::GenParticle
     *
     *  The schematic algorithm:
     *
     *   HepMC::GenParticle::pdg_id() -> 
     *                     ParticleID -> ParticleID::pid() 
     *
     *   @code 
     *   
     *    const HepMC::GenParticle* p = ... ; 
     *   
     *    const  long pid = (long) GID( p ) ;
     *
     *   @endcode 
     *
     *  @see HepMC::GenParticle
     *  @see LoKi::GenParticles::Identifier 
     * 
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-03-26
     */
    const   LoKi::GenParticles::Identifier                  GID      ;
    
    /** @var GABSID
     *  primitive evaluator of absolute value for 
     *      ParticleID for HepMC::GenParticle
     *
     *  The schematic algorithm:
     *
     *   HepMC::GenParticle::pdg_id() -> 
     *                     ParticleID -> ParticleID::abspid() 
     *
     *   @code 
     *   
     *    const HepMC::GenParticle* p = ... ; 
     *   
     *    const  long pid = (long) GABSID( p ) ;
     *
     *   @endcode 
     *
     *  @see HepMC::GenParticle
     *  @see LoKi::GenParticles::Identifier 
     * 
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-03-26
     */
    const   LoKi::GenParticles::AbsIdentifier               GABSID   ;
    

    /** @var GSTATUS 
     *  the primitive evaluator of HepMC::Particle::status()
     *
     *  @code 
     * 
     *  const HepMC::GenParticle* p = ... ;
     *  const int status = (int) GSTATUS( p ) ;
     * 
     *  @endcode 
     *
     *  @see HepMC::GenParticle
     *  @see LoKi::GenParticles::Status     
     * 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-08-26
     */
    const   LoKi::GenParticles::Status                      GSTATUS  ;

    /** @var GETA
     *  trivial evaluator of pseudorapitidy for HepMC::GenParticle
     *
     *  @see HepMC::GenParticle
     *  @see LoKi::GenParticles::PseudoRapidity
     * 
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-03-26
     */
    const   LoKi::GenParticles::PseudoRapidity              GETA     ;
    
    /** @var GPHI
     *  trivial evaluator of 'phi' for HepMC::GenParticle
     *
     *  @see HepMC::GenParticle
     *  @see LoKi::GenParticles::Phi
     * 
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-03-26
     */
    const   LoKi::GenParticles::Phi                         GPHI     ;

    /** @var GTHETA
     *  trivial evaluator of 'theta' for HepMC::GenParticle
     *
     *  @see HepMC::GenParticle
     *  @see LoKi::GenParticles::Theta
     * 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2005-03-26
     */
    const   LoKi::GenParticles::Theta                       GTHETA   ;

    /** @var GVEV
     *  primitive predicate to check teh validity of "end_vertex" 
     *  for HepMC::GenPArticle object
     *
     *  The schematic algorithm:
     *
     *    0 != HepMC::GenParticle::end_vertex() 
     *
     *   @code 
     *   
     *    const HepMC::GenParticle* p = ... ; 
     *   
     *    const bool valid  = GVEV ( p ) ;
     *
     *   @endcode 
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-03-26
     */
    const   LoKi::GenParticles::ValidEndVertex              GVEV     ;

    /** the evaluator of euclidian distance with 
     *  respect to the given 4-momentum, useful 
     *  e.g. for trivial "kinematical matching". 
     *  One can find 
     *  the particle with "closest" 4-momentum 
     *  to the given one:
     *
     *  @code 
     *
     *  SEQUENCE            mcparticles = ... ;
     *  LoKi::LorentzVector vct         = ... ; 
     *
     *  // find "best match" 
     *  SEQUENCE::const_iterator imin = 
     *    LoKi::select_min( genparticles.begin () , 
     *                      genparticles.end   () , 
     *                      GMOMDIST ( vct )    ) ;
     *
     *  @endcode 
     *
     *  E.g. it could be used in a combination with 
     *  LoKi::Extract::genParticles to find a "best matched"
     *  generator particle:
     * 
     *  @code 
     * 
     *  const Particle* particle = ... 
     *  // or 
     *  // const MCParticle* particle = ... ;
     *
     *  // get 4-momentum and ID from "original" particle:
     *  const LoKi::LorentzVector& vct = particle->momentum()  ;
     *  const ParticleID&          pid = particle->particleID() ;
     * 
     *  // get all HepMCParticles with the same ID :
     *  typedef std::vector<const HepMC::GenParticle*> GenParticles ;
     *  
     *  GenParticles particles ; 
     *  // get HepMC graphs from Gaudi TES: 
     *  const HepMCEvents* events = 
     *      get<HepMCEvents>( HepMCEventLocation::Default ) ;
     *  LoKi::Extract::genParticles 
     *      ( events                          ,    // source    
     *        std::back_inserter( particles ) ,    // target 
     *        pid == GID                      ) ;  // criteria 
     *  
     *  // find the best match:
     *  GenParticles::const_iterator ibest = 
     *    LoKi::select_min ( particles.begin     () , 
     *                       particles.end       () , 
     *                       GMOMDIST ( vct / GeV ) ) ;
     *  
     *  if ( particles.end() != ibest ) 
     *  {
     *    // the best 4-momentum match !!
     *    const HepMC::GenParticle* best = *ibest ;
     *  }
     *
     *  @endcode 
     *  
     *  @see LoKi::Extract::genParticles
     *  @see LoKi::select_min 
     *  @see HepMC::GenParticle 
     *  @see LoKi::LorentzVector 
     *  @see LoKi::GenParticles::MomentumDistance 
     *  
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-03-23 
     */
    typedef LoKi::GenParticles::MomentumDistance            GMOMDIST ;

    /** simple evaluator of transverse momentum 
     *  relative to some 3-vector (e.g. jet axis) 
     * 
     *  @code 
     *
     *  // vector (e.g. jet axis)
     *  const LoKi::ThreeVector& vct = ... ;
     *  
     *  // particle 
     *  const HepMC::GenParticle* particle = ... ;
     * 
     *  // create function 
     *  GFun ptDir = GPTDIR( vct ) ;
     * 
     *  // use the function:
     *  const double pt = ptDir( particle ) ;
     *
     *  @endcode 
     *
     *  @see HepMC::GenParticle 
     *  @see LoKi::ThreeVector 
     *  @see LoKi::GenParticles::TransverseMomentumRel
     *  
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-03-23 
     */
    typedef LoKi::GenParticles::TransverseMomentumRel       GPTDIR   ;
    
    /** simple evaluator of number of particles in the tree 
     *  which satisfy a certain predicate 
     *
     *  @warning current implementation is valid only for 
     *       HepMC::parents, HepMC::children, 
     *       HepMC::ancestors and HepMC::descendants 
     *
     *  E.g. could be used to extract all b/anti-b quarks 
     *  which comes from decay of Higgs 
     * 
     *  @code
     *
     *  // get Generator information
     *  const HepMCEvents* events = 
     *          get<HepMCEvents>( HepMCEventLocation::Default ) ;
     *
     *  typedef std::vector<const HepMC::GenParticle*> GenParticles ;
     *
     *  // select b(and antib) quarks from decay of higgs 
     *  GenParticles bquarks ;
     *  LoKi::Extract::genParticles
     *   ( events                         , 
     *     std::back_inserter( bquarks )  , 
     *     ( "b" == GABSID ) && 
     *    1 == GNINTREE( "H_10" == GABSID , HepMC::parents ) ) ;
     * 
     *  @endcode 
     *
     *  @see HepMC::IteratorRange 
     *  @see HepMC::parents 
     *  @see HepMC::GenParticle 
     *  @see HepMC::GenVertex 
     *  @see LoKi::Cuts::GABSID 
     *  @see LoKi::Extract::getParticles 
     *  @see LoKi::GenParticles::NInTree
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-03-23 
     */
    typedef LoKi::GenParticles::NInTree                     GNINTREE ;

    /** trivial function which evaluates for true for all 
     *  particles whcih belongs to "descendents" of given particle/vertex 
     * 
     *  @code 
     *  
     *   const HepMC::GenParticle bquark = ... ;
     *
     *   // get all particles from b-quark 
     *  typedef std::vector<const HepMC::GenParticle*> GPs ;
     *  GPs gps ;
     *  const HepMCEvents* events = ... ;
     *  LoKi::Extract::genParticles 
     *                 (  events                     ,   // source  
     *                    std::back_inserter ( gps ) ,   // target 
     *                    GFROMTREE ( bquark )       ) ; // predicate    
     * 
     *  @endcode 
     *  
     *  @see LoKi::GenParticles::FromHepMCTree 
     *  
     *  @author Vanya BELYAEV  Ivan.Belyaev@lapp.in2p3.fr
     *  @date   2005-05-16
     */
    typedef LoKi::GenParticles::FromHepMCTree                  GFROMTREE ;
    
    /** trivial function which evaluates for true for all 
     *  'ancestors' of given particle
     *
     *  @code 
     *  
     *   const HepMC::GenParticle bquark = ... ;
     *
     *   // get all particles from b-quark 
     *  typedef std::vector<const HepMC::GenParticle*> GPs ;
     *  GPs gps ;
     *  const HepMCEvents* events = ... ;
     *  LoKi::Extract::genParticles 
     *                 (  events                     ,   // source  
     *                    std::back_inserter ( gps ) ,   // target 
     *                    GANCESTOR ( bquark )      ) ; // predicate    
     * 
     *  @endcode 
     *  
     *  @see LoKi::GenParticles::IsAnAncestor
     *  
     *  @author Vanya BELYAEV  Ivan.Belyaev@lapp.in2p3.fr
     *  @date   2005-05-16
     */
    typedef LoKi::GenParticles::IsAnAncestor                  GANCESTOR ;


    /** quark content 
     * 
     *  @code
     * 
     *   const HepMC::GenParticle* p  = ...         ;
     *   GCut cut = GQUARK( ParticleID::bottom )
     * 
     *   const bool beauty  = cut ( p ) ;
     *
     *  @endcode 
     *
     *  @see HepMC::GenParticle
     *  @see LoKi::GenParticles::HasQuark
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    typedef LoKi::GenParticles::HasQuark             GQUARK ;

    
    /** @var GBEAUTY
     *  Check for bottom quark 
     *
     *  @code
     * 
     *   const HepMC::GenParticle* p  = ...         ;
     *
     *   const bool beauty  = GBEAUTY ( p ) ;
     *
     *  @endcode 
     *
     *  @see HepMC::GenParticle
     *  @see LoKi::GenParticles::HasQuark
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    const  LoKi::GenParticles::HasQuark GBEAUTY( LHCb::ParticleID::bottom ) ;    
    
    /** @var GCHARM
     *  Check for charm quark 
     *
     *  @code
     * 
     *   const HepMC::GenParticle* p  = ...         ;
     *
     *   const bool charm  = GCHARM ( p ) ;
     *
     *  @endcode 
     *
     *  @see MCParticle
     *  @see LoKi::GenParticles::HasQuark
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    const  LoKi::GenParticles::HasQuark GCHARM ( LHCb::ParticleID::charm  ) ;

    /** @var GSTRANGE
     *  Check for strange quark 
     *
     *  @code
     * 
     *   const HepMC::GenParticle* p  = ...         ;
     *
     *   const bool strange = GSTRANGE ( p ) ;
     *
     *  @endcode 
     *
     *  @see HepMC::GenParticle
     *  @see LoKi::GenParticles::HasQuark
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    const  LoKi::GenParticles::HasQuark GSTRANGE ( LHCb::ParticleID::strange ) ;
    
    /** @var GTOP
     *  Check for TOP  quark 
     *
     *  @code
     * 
     *   const HepMC::GenParticle* p  = ...         ;
     *
     *   const bool top  = GTOP( p ) ;
     *
     *  @endcode 
     *
     *  @see HepMC::GenParticle
     *  @see LoKi::GenParticles::HasQuark
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    const  LoKi::GenParticles::HasQuark GTOP  ( LHCb::ParticleID::top     ) ;
    
    /** @var GCHARGED
     *  Check for particle charge 
     *
     *  @code
     * 
     *   const HepMC::Particle* p  = ...         ;
     *
     *   const bool charged = GCHARGED ( p ) ;
     *
     *  @endcode 
     *
     *  @see HepMC::GenParticle
     *  @see LoKi::GenParticles::IsCharged
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    const  LoKi::GenParticles::IsCharged GCHARGED ;
    
    /** @var GNEUTRAL
     *  Check for particle charge 
     *
     *  @code
     * 
     *   const HepMC::GenParticle* p  = ...         ;
     *
     *   const bool neutral = GNEUTRAL ( p ) ;
     *
     *  @endcode 
     *
     *  @see HepMC::GenParticle
     *  @see LoKi::GenParticles::IsNeutral
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    const  LoKi::GenParticles::IsNeutral  GNEUTRAL ;
    
    /** @var GLEPTON 
     *  Check for particle type   
     *
     *  @code
     * 
     *   const HepMC::GenParticle* p  = ...         ;
     *
     *   const bool lepton  = GLEPTON  ( p ) ;
     *
     *  @endcode 
     *
     *  @see HepMC::GenParticle
     *  @see LoKi::GenParticles::IsLepton 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    const  LoKi::GenParticles::IsLepton  GLEPTON  ;

    /** @var GMESON  
     *  Check for particle type   
     *
     *  @code
     * 
     *   const HepMC::GenParticle* p  = ...         ;
     *
     *   const bool meson   = GMESON   ( p ) ;
     *
     *  @endcode 
     *
     *  @see HepMC::GenParticle
     *  @see LoKi::GenParticles::IsMeson  
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    const  LoKi::GenParticles::IsMeson GMESON   ;

    /** @var GBARYON 
     *  Check for particle type   
     *
     *  @code
     * 
     *   const HepMC::GenParticle* p  = ...         ;
     *
     *   const bool baryon  = GBARYON  ( p ) ;
     *
     *  @endcode 
     *
     *  @see HepMC::GenParticle
     *  @see LoKi::GenParticles::IsBaryon 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    const  LoKi::GenParticles::IsBaryon GBARYON  ;

    /** @var GHADRON 
     *  Check for particle type   
     *
     *  @code
     * 
     *   const HepMC::GenParticle* p  = ...         ;
     *
     *   const bool hadron  = GHADRON  ( p ) ;
     *
     *  @endcode 
     *
     *  @see HepMC::GenParticle
     *  @see LoKi::GenParticles::IsHadron 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    const  LoKi::GenParticles::IsHadron GHADRON  ;

   /** @var GNUCLEUS
     *  Check for particle type   
     *
     *  @code
     * 
     *   const HepMC::GenParticle* p  = ...         ;
     *
     *   const bool nucleus = GNUCLEUS ( p ) ;
     *
     *  @endcode 
     *
     *  @see HepMC::GenParticle
     *  @see LoKi::GenParticles::IsNucleus
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    const  LoKi::GenParticles::IsNucleus GNUCLEUS ;

    /** @var GP 
     *  evaluator of particle momentum (in HepMC units)
     *
     *
     *  @see LoKi::GenParticles::Momentum
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    const LoKi::GenParticles::Momentum GP ;

    /** @var GPT 
     *  evaluator of particle transverse momentum (in HepMC units)
     *
     *
     *  @see LoKi::GenParticles::TransverseMomentum
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    const LoKi::GenParticles::TransverseMomentum GPT ;

    /** @var GE
     *  evaluator of particle energy (in HepMC units)
     *
     *
     *  @see LoKi::GenParticles::Energy
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    const LoKi::GenParticles::Energy      GE ;

    /** @var GM
     *  evaluator of particle mass (in HepMC units)
     *
     *
     *  @see LoKi::GenParticles::Mass
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    const LoKi::GenParticles::Mass GM ;


    /** @var GPX 
     *  evaluator of particle momentum (in HepMC units)
     *
     *  @see LoKi::GenParticles::MomentumX
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    const LoKi::GenParticles::MomentumX GPX ;

    /** @var GPY
     *  evaluator of particle momentum (in HepMC units)
     *
     *  @see LoKi::GenParticles::MomentumY
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    const LoKi::GenParticles::MomentumY GPY ;

    /** @var GPZ
     *  evaluator of particle momentum (in HepMC units)
     *
     *  @see LoKi::GenParticles::MomentumZ
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    const LoKi::GenParticles::MomentumZ GPZ ;

    /** @var GTIME 
     *  evaluator of particle proper lifetime c*tau (in HepMC units)
     *
     *  @see LoKi::GenParticles::ProperLifeTime 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    const LoKi::GenParticles::ProperLifeTime GTIME  ;

    /** evaluator of particle proper lifetime c*tau (in HepMC units)
     *
     *  @see LoKi::GenParticles::ProperLifeTime 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    typedef LoKi::GenParticles::ProperLifeTime GCTAU ;
    
    /** evaluator of particle NOMINAL proper lifetime c*tau (in HepMC units)
     *
     *  @see LoKi::GenParticles::NominalLifeTime 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    const LoKi::GenParticles::NominalLifeTime GNLT ;
    
    /** helper adapter which delegates the evaluation of the 
     *  "vertex" function to production vertex of the particle
     *
     *  Extract all particles, which are produces at |z|<10 
     *
     *  @code 
     * 
     *  const LHCb::HepMCEvents* events 
     *    get<LHCb::HepMCEvents>( LHCb::HepMCEventLocation::Default ) ;
     *  
     *  typedef std::vector<HepMC::GenParticle*> PARTICLES ;
     *  
     *  PARTICLES parts ;
     * 
     *  // create the predicate:
     *  GCut cut = GFAPVX( abs( GVZ  ) , 0 )  < 10 ;
     * 
     *  LoKi::Extract::genParticles 
     *    ( events , std::back_inserter ( parts )  , cut ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::GVZ 
     *  @see HepMC::GenParticle 
     *  @see HepMC::GenVertex 
     *  @see LoKi::Extract::genParticles 
     *  @see LoKi::GenParticles::AdapterToProductionVertex
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-08
     */
    typedef LoKi::GenParticles::AdapterToProductionVertex  GFAPVX ;

    /** helper adapter which delegates the evaluation of the 
     *  "vertex" function to end vertex of the particle
     *
     *  Extract all particles, which are decayed after z > 1000 
     *
     *  @code 
     * 
     *  const LHCb::HepMCEvents* events 
     *    get<LHCb::HepMCEvents>( LHCb::HepMCEventLocation::Default ) ;
     *  
     *  typedef std::vector<HepMC::GenParticle*> PARTICLES ;
     *  
     *  PARTICLES parts ;
     * 
     *  // create the predicate:
     *  GCut cut = GFAEVX( GVZ  , 10000  ) >  1000 ;
     * 
     *  LoKi::Extract::genParticles 
     *    ( events , std::back_inserter ( parts )  , cut ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::GVZ
     *  @see HepMC::GenParticle 
     *  @see HepMC::GenVertex 
     *  @see LoKi::Extract::genParticles 
     *  @see LoKi::GenParticles::AdapterToEndVertex
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-08
     */
    typedef LoKi::GenParticles::AdapterToEndVertex         GFAEVX ;    
    
    /** @var G3Q
     *  The trivial evaluator of 3*chareg of the particle 
     *
     *  @code 
     *
     *  const HepMC::GenParticle* p = ... ;
     *  
     *  const bool positive = 0 < G3Q( p ) ;
     *
     *  @endcode 
     * 
     *  Algorithm: 
     *
     *   HepMC::GenParticle:pdg_id -> LHCb::ParticleID -> threeCharge 
     *
     *  @author Vanya BELYAVE ibelyaev@physics.syr.edu
     *  @date 2006-03-07
     */
    const LoKi::GenParticles::ThreeCharge                  G3Q    ;    
    
    /** @typedef GDPHI
     *  simple evaluator of "delta phi" of the particle momenta
     * 
     *  @code 
     *
     *   double phi = ... ;
     * 
     *   // create the function: 
     *  GFun dphi = GDPHI(phi) ;
     * 
     *  const HepMC::GenParticle* p = ... ;
     *
     *  // use the function 
     *  const double result = dphi ( p ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::GenParticless::DeltaPhi 
     *  @see LoKi::GenParticless::Phi
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-03-03 
     */
    typedef LoKi::GenParticles::DeltaPhi                    GDPHI ;

    /** @typedef GDETA
     *  simple evaluator of "delta eta" of the particle momenta
     * 
     *  @code 
     *
     *   double eta = ... ;
     * 
     *   // create the function: 
     *  GFun deta = GDETA(eta) ;
     * 
     *  const HepMC::GenParticle* p = ... ;
     *
     *  // use the function 
     *  const double result = deta ( p ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::GenParticless::DeltaEta 
     *  @see LoKi::GenParticless::Eta
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-03-03 
     */
    typedef LoKi::GenParticles::DeltaEta                    GDETA ;

    /** @typedef GDR2 
     *  simple evaluator of "delta eta" of the particle momenta
     * 
     *  @code 
     *
     *   double et
     * 
     *   // create the function: 
     *  GFun r2 = GDR2( v ) ;
     * 
     *  const HepMC::GenParticle* p = ... ;
     *
     *  // use the function 
     *  const double result = r2 ( p ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::GenParticless::DeltaR2  
     *  @see LoKi::GenParticless::DeltaEta
     *  @see LoKi::GenParticless::DeltaPhi
     *  @see LoKi::Cuts::GDELTAR2 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-03-03 
     */
    typedef LoKi::GenParticles::DeltaR2                     GDR2     ;

    /** @typedef GDELTAR2 
     *  simple evaluator of "delta eta" of the particle momenta
     * 
     *  @code 
     *
     *   double et
     * 
     *   // create the function: 
     *  GFun r2 = GDELTAR2( v ) ;
     * 
     *  const HepMC::GenParticle* p = ... ;
     *
     *  // use the function 
     *  const double result = r2 ( p ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::GenParticless::DeltaR2  
     *  @see LoKi::GenParticless::DeltaEta
     *  @see LoKi::GenParticless::DeltaPhi
     *  @see LoKi::Cuts::GDR2 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-03-03 
     */
    typedef LoKi::GenParticles::DeltaR2                     GDELTAR2 ;
    
  } // end of namespace LoKi::Cuts 
  
} // end of namespace LoKi

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_GENPARTICLECUTS_H
// ============================================================================
