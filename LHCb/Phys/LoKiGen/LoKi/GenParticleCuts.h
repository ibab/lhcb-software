// $Id$
// ============================================================================
#ifndef LOKI_GENPARTICLECUTS_H
#define LOKI_GENPARTICLECUTS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/MoreFunctions.h"
#include "LoKi/Monitoring.h"
#include "LoKi/GenTypes.h"
#include "LoKi/GenParticles.h"
#include "LoKi/GenPIDOperators.h"
#include "LoKi/GenSources.h"
#include "LoKi/GenParticles2.h"
#include "LoKi/GenParticles3.h"
#include "LoKi/GenParticles4.h"
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
 *  By usage of this code one clearly states the disagreement
 *  with the smear campaign of Dr. O.Callot et al.:
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-01-23
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Cuts
  {
    // ========================================================================
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
    const LoKi::GenParticles::ThreeCharge                                 G3Q ;
    // ========================================================================
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
    const   LoKi::GenParticles::AbsIdentifier                          GABSID ;
    // ========================================================================
    /** @var GALL
     *  primitive predicate, "always true"
     *  @see LoKi::BooleanConstant
     *  @see LoKi::GenTypes::GTRUE
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-03-27
     */
    const LoKi::BasicFunctors<const HepMC::GenParticle*>::BooleanConstant GALL ( true ) ;
    // ========================================================================
    /** @typedef GANCESTOR
     *  trivial function which evaluates for true for all
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
    typedef LoKi::GenParticles::IsAnAncestor                        GANCESTOR ;
    // ========================================================================
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
    const   LoKi::GenParticles::BarCode                                  GBAR ;
    // ========================================================================
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
    const   LoKi::GenParticles::BarCode                              GBARCODE ;
    // ========================================================================
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
    const  LoKi::GenParticles::IsBaryon                              GBARYON  ;
    // ========================================================================
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
    const LoKi::GenParticles::HasQuark   GBEAUTY ( LHCb::ParticleID::bottom ) ;
    // ========================================================================
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
    const  LoKi::GenParticles::IsCharged                             GCHARGED ;
    // ========================================================================
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
    const LoKi::GenParticles::HasQuark     GCHARM ( LHCb::ParticleID::charm ) ;
    // ========================================================================
    /** @typedef GCHILD
     *  Simple adapter that delegates the function to certain child particle
     *  @see LoKi::GenParticles::ChildFun
     *  @see LoKi::GenChild::Selector
     *  @see LoKi::Cuts::GCHILDFUN
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2011-12-13
     */
    typedef LoKi::GenParticles::ChildFun                               GCHILD ;
    // ========================================================================
    /** @typedef GCHILDCUT
     *  Simple adapter that delegates the predicate to certain child particle
     *  @see LoKi::GenParticles::ChildCut
     *  @see LoKi::GenChild::Selector
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2011-12-13
     */
    typedef LoKi::GenParticles::ChildCut                            GCHILDCUT ;
    // ========================================================================
    /** @typedef GCHILDFUN
     *  Simple adapter that delegates the function to certain child particle
     *  @see LoKi::GenParticles::ChildFun
     *  @see LoKi::GenChild::Selector
     *  @see LoKi::Cuts::GCHILD
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2011-12-13
     */
    typedef LoKi::GenParticles::ChildFun                            GCHILDFUN ;
    // ========================================================================
    /** @typedef GCOUNTER
     *  Simple monitoring counter for predicates
     *
     *  @code
     *
     *  // some predicate to be monitored
     *  GCut cut = ... ;
     *
     *  // Create the monitored predicate using the generic counter
     *  // either local counter, or the counter from IStatSvc or ICounterSvc
     *  StatEntity* counter = ... ; ///< the generic counter
     *  GCut mon = COUNTER ( cut , counter ) ;
     *
     *  for ( ... )
     *    {
     *      ...
     *      const HepMC::GenParticle* p = ... ;
     *      const bool result = mon( p ) ; ///< use the predicate
     *      ...
     *    }
     *
     *  ...
     *  info ()
     *     << " Monitoring results : "                       << endreq
     *     << " NEntries:  #" << counter->entries ()         << endreq
     *     << " Efficiency:(" << counter->eff     ()
     *     << "+="            << counter->effErr  () << ")%" << endreq ;
     *
     *  @endcode
     *
     *  The alternative (recommended) way to create the monitored predicate:
     *  @code
     *
     *  GCut cut = ... ;
     *
     *  GCut mon = monitor ( cut , counter("SomeEffCounter") ) ;
     *
     *  @endcode
     *
     *  The substitution of the predicate by monitored predicate
     *  cound be done "on-flight" without the disturbing of the
     *  actual processing:
     *  @code
     *
     *  // some predicate to be monitored
     *  GCut cut = ... ;
     *
     *  if ( monitoring )
     *    {
     *       cut = monitor ( cut , "Efficiency1" ) ;
     *    }
     *
     *  @endcode
     *
     *  @attention The string representation of the object
     *             is delegated to the underlying prediate,
     *             therefore the object is NOT recontructable
     *             from its string representations. It is done
     *             on purpose to avoid the disturbing of ids.
     *
     *  @see LoKi::Monitoring::Counter
     *  @see LoKi::monitor
     *  @see StatEntity
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-03-03
     */
    typedef LoKi::Monitoring::Counter<const HepMC::GenParticle*,bool> GCOUNTER ;
    // ========================================================================
    /** @var GCTAU
     *  evaluator of particle proper lifetime c*tau (in HepMC units)
     *
     *  @see LoKi::GenParticles::ProperLifeTime
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    const LoKi::GenParticles::ProperLifeTime                            GCTAU ;
    // ========================================================================
    /** @typedef GDECNODE
     *  the trivial predicate whcih acts on ParticleID
     *
     *
     *  @code
     *
     *  // construct some node:
     *  Decays::Node node = Decays::Nodes::Lepton() && Decays::Nodes::Positive() ;
     *
     *  const HepMC::GenParticle* p = .... ;
     *
     *  // create the predicate:
     *  GCut good = GDECNODE( node  ) ;
     *
     *  // use the predicate:
     *  const ok = good ( p ) ;
     *
     *  @endcode
     *
     *  @see LHCb::ParticleID
     *  @see LHCb::ParticleID
     *  @see Decays::iNode
     *  @see Decays::Node
     *  @see Decays::Nodes
     *  @author Vanya BELYAEV Ivane.Belyaev@nikhef.nl
     *  @date 2008-12-17
     */
    typedef LoKi::GenParticles::DecNode                              GDECNODE ;
    // ========================================================================
    /** @typedef GDECTREE
     *  the trivial predicate which acts on decay structure
     *
     *  @code
     *
     *  // construct some tree
     *  Decays::Tree_<const HepMC::GenParticle*> tree =  ... ;
     *
     *  const HepMC::GenParticle* p = .... ;
     *
     *  // create the predicate:
     *  GCut good = GDECTREE ( tree  ) ;
     *
     *  // use the predicate:
     *  const ok = good ( p ) ;
     *
     *  @endcode
     *
     *  @see LHCb::ParticleID
     *  @see Decays::iTree_
     *  @see Decays::Tree_
     *  @see Decays::Trees
     *  @author Vanya BELYAEV Ivane.Belyaev@nikhef.nl
     *  @date 2008-12-17
     */
    typedef LoKi::GenParticles::DecTree                              GDECTREE ;
    // ========================================================================
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
    const LoKi::BasicFunctors<const HepMC::GenParticle*>::BooleanConstant GFALSE ( false ) ;
    // ========================================================================
    /** @typedef GFAEVX
     *  helper adapter which delegates the evaluation of the
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
    typedef LoKi::GenParticles::AdapterToEndVertex                     GFAEVX ;
    // ========================================================================
    /** @typedef GFAPVX
     *  helper adapter which delegates the evaluation of the
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
    typedef LoKi::GenParticles::AdapterToProductionVertex              GFAPVX ;
    // ========================================================================
    /** @typedef GFROMTREE
     *  trivial function which evaluates for true for all
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
    typedef LoKi::GenParticles::FromHepMCTree                       GFROMTREE ;
    // ========================================================================
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
    const  LoKi::GenParticles::IsHadron                               GHADRON ;
    // ========================================================================
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
    const   LoKi::GenParticles::Identifier                                GID ;
    // ========================================================================
    /** @typedef GINTREE
     *  simple meta function to check the presence of the particle in the
     *  decay tree of the particle:
     *
     *  @code
     *   // check for semiletonic decays of B-meson:
     *   const HepMC::GenParticle* B = ... ;
     *   // create the predicate:
     *   GCut hasEorMu = GINTREE ( "e+" == GABSID || "mu+" == GABSID ) ;
     *   // use the predicate!
     *   const bool semileptonic = hasEorMu ( B ) ;
     *
     *  @endcode
     *  @see LoKi::GenAlgs::found
     *  @see LoKi::GenParticles::InTree
     *  @author Vanya BELYAEV ibelyaev@physic.syr.edu
     *  @date 2007-06-03
     */
    typedef LoKi::GenParticles::InTree                                GINTREE ;
    // ========================================================================
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
    const  LoKi::GenParticles::IsLepton                               GLEPTON ;
    // ========================================================================
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
    const  LoKi::GenParticles::IsMeson                                 GMESON ;
    // ========================================================================
    /** @typedef GMOMDIST
     *  The evaluator of euclidian distance with
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
    typedef LoKi::GenParticles::MomentumDistance                     GMOMDIST ;
    // ========================================================================
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
    const  LoKi::GenParticles::IsNeutral                             GNEUTRAL ;
    // ========================================================================
    /** @typedef GNINTREE
     *  simple evaluator of number of particles in the tree
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
    typedef LoKi::GenParticles::NInTree                              GNINTREE ;
    // ========================================================================
    /** @var GNLT
     *  evaluator of particle NOMINAL proper lifetime c*tau (in HepMC units)
     *
     *  @see LoKi::GenParticles::NominalLifeTime
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    const LoKi::GenParticles::NominalLifeTime                            GNLT ;
    // ========================================================================
    /** @var GNONE
     *  primitive predicate, "always false"
     *  @see LoKi::BooleanConstant
     *  @see LoKi::GenTypes::GFALSE
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-03-27
     */
    const LoKi::BasicFunctors<const HepMC::GenParticle*>::BooleanConstant GNONE ( false ) ;
    // ========================================================================
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
    const  LoKi::GenParticles::IsNucleus                             GNUCLEUS ;
    // ========================================================================
    /** @var GONE
     *  primitive function, "always 1"
     *  @see LoKi::Constant
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-03-27
     */
    const LoKi::BasicFunctors<const HepMC::GenParticle*>::Constant GONE ( 1 ) ;
    // ========================================================================
    /** @var GOSCILLATED
     *  Check if the particle oscillated.
     *
     *  Note: the concept of oscillation in EvtGen/Pythia/HepMC is a bit
     *  mislleading
     *
     *  if one has
     *  @\f[  \mathrm{B}^0 \to \bar{\mathrm{}}^0 \to \mathrm {X} $\f]
     *  in this case both @\f[\mathrm{B}^0$\f] and  $\f[ \bar{\mathrm{}}^0 @\f]
     *  are considered as "oscillated"
     *
     *  @see LoKi::GenParticles::Oscillated
     *  @see LoKi::GenParticles::oscillated1
     *  @see LoKi::GenParticles::oscillated2
     *
     *  @code
     *  @endcode
     *  @author Vanay BELYAEV Ivan.Belyaev@nikhef.nl
     *  @author 2008-07-03
     */
    const LoKi::GenParticles::Oscillated                          GOSCILLATED ;
    // ========================================================================
    /** @var GOSCILLATED1
     *  Check if the particle oscillated.
     *
     *  @see LoKi::GenParticles::Oscillated1
     *  @see LoKi::GenParticles::oscillated1
     *
     *  @author Vanay BELYAEV Ivan.Belyaev@nikhef.nl
     *  @author 2008-07-03
     */
    const LoKi::GenParticles::Oscillated1                        GOSCILLATED1 ;
    // ========================================================================
    /** @var GOSCILLATED2
     *  Check if the particle oscillated.
     *
     *  @see LoKi::GenParticles::Oscillated2
     *  @see LoKi::GenParticles::oscillated2
     *
     *  @author Vanay BELYAEV Ivan.Belyaev@nikhef.nl
     *  @author 2008-07-03
     */
    const LoKi::GenParticles::Oscillated2                        GOSCILLATED2 ;
    // ========================================================================
    /** @typedef GPLOT
     *  Simple monitoring histogram for the functions
     *
     *  @code
     *
     *  // some function to be monitored
     *  GFun fun = ... ;
     *
     *  // Create the monitored function using the histogram:
     *  AIDA::IHistogram1D* histo = ... ;
     *  GFun mon = PLOT ( fun , histo ) ;
     *
     *  for ( ... )
     *    {
     *      ...
     *      const LHCb::Particle* p = ... ;
     *      const double result = mon( p ) ; ///< use the function
     *      ...
     *    }
     *
     *  @endcode
     *
     *  The alternative (recommended) way to create the monitored function
     *  @code
     *
     *  // some function to be monitored
     *  GFun fun = ... ;
     *
     *  AIDA::IHistogram1D* histo = ... ;
     *  GFun mon = monitor ( fun , histo ) ;
     *
     *  @endcode
     *
     *  The substitution of the function by monitored function
     *  cound be done "on-flight" without the disturbing of the
     *  actual processing:
     *  @code
     *
     *  // some function to be monitored
     *  GFun fun = ... ;
     *
     *  if ( monitoring )
     *    {
     *       AIDA::IHistogram1D* histo = ... ;
     *       fun = monitor ( fun , histo ) ;
     *    }
     *
     *  @endcode
     *
     *  @attention The string representation of the object
     *             is delegated to the underlying function,
     *             therefore the object is NOT recontructable
     *             from its string representations. It is done
     *             on purpose to avoid the disturbing of ids.
     *
     *  @see LoKi::Monitoring::Plot
     *  @see LoKi::monitor
     *  @see AIDA::IHistogram1D
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-03-03
     */
    typedef LoKi::Monitoring::Plot<const HepMC::GenParticle*,double> GPLOT ;
    // ========================================================================
    /** @typedef GPTDIR
     *  simple evaluator of transverse momentum
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
    typedef LoKi::GenParticles::TransverseMomentumRel                  GPTDIR ;
    // ========================================================================
    /** @typedef GPTREL
     *  simple evaluator of transverse momentum
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
     *  GFun ptDir = GPTREL ( vct ) ;
     *
     *  // use the function:
     *  const double pt = ptDir( particle ) ;
     *
     *  @endcode
     *
     *  @see HepMC::GenParticle
     *  @see LoKi::ThreeVector
     *  @see LoKi::GenParticles::TransverseMomentumRel
     *  @see LoKi::Cut::GPTDIR
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-03-23
     */
    typedef LoKi::GenParticles::TransverseMomentumRel                  GPTREL ;
    // ========================================================================
    /** @typedef GQUARK
     *  quark content
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
    typedef LoKi::GenParticles::HasQuark                               GQUARK ;
    // ========================================================================
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
    typedef LoKi::SimpleSwitch<const HepMC::GenParticle*>            GSSWITCH ;
    // ========================================================================
    /** @typedef GSTAT
     *  Simple monitoring counter for the functions
     *
     *  @code
     *
     *  // some function to be monitored
     *  GFun fun = ... ;
     *
     *  // Create the monitored function using the generic counter
     *  // either local counter, or the counter from IStatSvc or ICounterSvc
     *  StatEntity* counter = ... ; ///< the generic counter
     *  GFun mon = STAT ( fun , counter ) ;
     *
     *  for ( ... )
     *    {
     *      ...
     *      const HepMC::GenParticle* p = ... ;
     *      const double result = mon( p ) ; ///< use the function
     *      ...
     *    }
     *
     *  ...
     *  info ()
     *     << " Monitoring results : "                 << endreq
     *     << " NEntries:  #" << counter->entries  ()  << endreq
     *     << " TotalSum:   " << counter->flag     ()  << endreq
     *     << " Mean+-RMS:  " << counter->flagMean ()
     *     << "+="            << counter->flagRMS  ()  << endreq
     *     << " Min/Max:    " << counter->flagMin  ()
     *     << "/"             << counter->flagMax  ()  << endreq ;
     *
     *  @endcode
     *
     *  The alternative (recommended) way to create the monitored function
     *  @code
     *
     *  // some function to be monitored
     *  GFun fun = ... ;
     *
     *  GFun mon = monitor ( fun , counter("SomeCounter") ) ;
     *
     *  @endcode
     *
     *  The substitution of the function by monitored function
     *  cound be done "on-flight" without the disturbing of the
     *  actual processing:
     *  @code
     *
     *  // some function to be monitored
     *  GFun fun = ... ;
     *
     *  if ( monitoring )
     *    {
     *       fun = monitor ( fun , "MonitoredFun" ) ;
     *    }
     *
     *  @endcode
     *
     *  @attention The string representation of the object
     *             is delegated to the underlying function,
     *             therefore the object is NOT recontructable
     *             from its string representations. It is done
     *             on purpose to avoid the disturbing of ids.
     *
     *  @see LoKi::Monitoring::Counter
     *  @see LoKi::monitor
     *  @see StatEntity
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-03-03
     */
    typedef LoKi::Monitoring::Counter<const HepMC::GenParticle*,double> GSTAT ;
    // ========================================================================
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
    const   LoKi::GenParticles::Status                                GSTATUS ;
    // ========================================================================
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
    const LoKi::GenParticles::HasQuark GSTRANGE ( LHCb::ParticleID::strange ) ;
    // ========================================================================
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
    typedef LoKi::Switch<const HepMC::GenParticle*>                   GSWITCH ;
    // ========================================================================
    /** @var GTIME
     *  evaluator of particle proper lifetime c*tau (in HepMC units)
     *
     *  @see LoKi::GenParticles::ProperLifeTime
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    const LoKi::GenParticles::ProperLifeTime                            GTIME ;
    // ========================================================================
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
    const LoKi::GenParticles::HasQuark         GTOP ( LHCb::ParticleID::top ) ;
    // ========================================================================
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
    const LoKi::BasicFunctors<const HepMC::GenParticle*>::BooleanConstant GTRUE ( true  ) ;
    // ========================================================================
    /** @var GVALID
     *  Simple predicate which checks the ildty of the pointer
     *  @author Vanya BELYAEV belyaev@physics.syr.edu
     *  @date 2007-07-22
     */
    const LoKi::Valid<const HepMC::GenParticle*>                       GVALID ;
    // ========================================================================
    /** @var GVEV
     *  primitive predicate to check the validity of "end_vertex"
     *  for HepMC::GenParticle object
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
    const   LoKi::GenParticles::ValidEndVertex                           GVEV ;
    // ========================================================================
    /** @var GZERO
     *  primitive function, "always 0"
     *  @see LoKi::Constant
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-03-27
     */
    const LoKi::BasicFunctors<const HepMC::GenParticle*>::Constant GZERO ( 0 ) ;
    // ========================================================================
  } //                                              end of namespace LoKi::Cuts
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The END
// ============================================================================
#endif // LOKI_GENPARTICLECUTS_H
// ============================================================================

