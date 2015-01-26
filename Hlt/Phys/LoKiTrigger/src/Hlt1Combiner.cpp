// $Id: Hlt1Combiner.cpp 180655 2014-11-25 10:38:48Z mkenzie $
// ============================================================================
// Include files
// ============================================================================
#ifdef __INTEL_COMPILER
#pragma warning(disable:1572) // non-pointer conversion ... may lose significant bits
#pragma warning(push)
#endif
// ============================================================================
// HltBase
// ============================================================================
#include "HltBase/HltUtils.h"
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
// Kernel
// ============================================================================
#include "Kernel/IParticleCombiner.h"
#include "Kernel/DaVinciAlgorithm.h"
#include "Kernel/IDecodeSimpleDecayString.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Hlt1Combiner.h"
#include "LoKi/ToParticles.h"
#include "LoKi/Hlt1.h"
#include "LoKi/Constants.h"
#include "LoKi/ParticleProperties.h"
#include "LoKi/CompareParticles.h"
#include  "LTTools.h"
// ============================================================================
#ifdef __INTEL_COMPILER
#pragma warning(pop)
#endif
// ============================================================================
/** @file
 *
 *  Implementation file for class LoKi::Hlt1::Hlt1Combiner
 *  This file is part of LoKi project:
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
 *  contributions and advices from G.Raven, J.van Tilburg,
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Matthew KENZIE matthew.kenzie@cern.ch
 *  @date   2014-11-27
 *
 *                    $Revision: 180655 $
 *  Last Modification $Date: 2014-11-25 11:38:48 +0100 (Tue, 25 Nov 2014) $
 *                 by $Author: mkenzie $
 */
// ============================================================================
//   constructor from decay string, output location, two Sources for daughters,
//   combination cut and mother cut
// ============================================================================
LoKi::Hlt1::Hlt1Combiner::Hlt1Combiner
( std::string                                                decay      ,
  std::string                                                output     ,
  const LoKi::Hlt1::Hlt1Combiner::Source&                    particles1 ,
  const LoKi::Hlt1::Hlt1Combiner::Source&                    particles2 ,
  const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut    ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&      mothcut    )
  : LoKi::AuxFunBase  ( std::tie ( decay      , output     , 
                                   particles1 , particles2 ,
                                   combcut    , mothcut    ) ) 
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Source ()
  , LoKi::Hlt1::HelperTool( 1 )
  , m_decstring ( decay )
  , m_sink { std::move(output) }
  , m_source1 ( particles1 )
  , m_source2 ( particles2 )
  , m_acut ( combcut )
  , m_cut  ( mothcut )
{
  setup();
}
// ============================================================================
//   constructor from decay string, output location, two Sources for daughters,
//   combination cut and mother cut
// ============================================================================
LoKi::Hlt1::Hlt1Combiner::Hlt1Combiner
( std::string                                                decay      ,
  std::string                                                output     ,
  std::string                                                particles1 ,
  std::string                                                particles2 ,
  const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut    ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&      mothcut    )
  : LoKi::AuxFunBase  ( std::tie ( decay      , output     , 
                                   particles1 , particles2 ,
                                   combcut    , mothcut    ) ) 
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Source ()
  , LoKi::Hlt1::HelperTool( 1 )
  , m_decstring ( decay )
  , m_sink { std::move(output) }
  , m_source1 { LoKi::Hlt1::Selection{ std::move(particles1) } }
  , m_source2 { LoKi::Hlt1::Selection{ std::move(particles2) } }
  , m_acut { combcut }
  , m_cut  { mothcut }
{
  setup();
}
// ============================================================================
// setup function (common call from constructors)
// ============================================================================
StatusCode LoKi::Hlt1::Hlt1Combiner::setup()
{
  // get required tools
  LoKi::ILoKiSvc* svc = lokiSvc() ;
  SmartIF<IToolSvc> tsvc ( svc ) ;
  StatusCode sc = tsvc->retrieveTool("Particle2State",m_p2s);
  sc = tsvc->retrieveTool("DecodeSimpleDecayString:PUBLIC", m_dsds);
  //sc = tsvc->retrieveTool("DaVinci::DefaultTools::ParticleCombiner", m_pc);
  sc = tsvc->retrieveTool("LoKi::FastVertexFitter:PUBLIC", m_pc);

  m_decays = Decays::decays( m_decstring , m_dsds ) ;
  Assert ( !m_decays.empty(), "Unable to decode DecayDescriptor" );

  return StatusCode::SUCCESS ;
}
// ============================================================================
// virtual destructor
// ============================================================================
LoKi::Hlt1::Hlt1Combiner::~Hlt1Combiner () {}
// ============================================================================
// clone method ("virtual constructor")
// ============================================================================
LoKi::Hlt1::Hlt1Combiner* LoKi::Hlt1::Hlt1Combiner::clone() const
{ return new LoKi::Hlt1::Hlt1Combiner { *this } ; }
// ============================================================================
// getState helper function
// ============================================================================
const LHCb::State*
LoKi::Hlt1::Hlt1Combiner::getState
  ( const LHCb::Track* &track ) const
{
  const LHCb::State* state = 0;
  // try closest to beam
  if ( !state ) { state = track->stateAt ( LHCb::State::ClosestToBeam )    ; }
  // try first measurement
  if ( !state ) { state = track->stateAt ( LHCb::State::FirstMeasurement ) ; }
  // try first state
  if ( !state ) { state = &track->firstState()                             ; }

  return state;
}
// ============================================================================
// the only one important method
// ============================================================================
LoKi::Hlt1::Hlt1Combiner::result_type
LoKi::Hlt1::Hlt1Combiner::operator()
  ( /* LoKi::Hlt1::Hlt1Combiner::argument a */ ) const
{
  Assert ( alg() , "Invalid setup!" );

  typedef result_type                CANDIDATES ;
  typedef CANDIDATES::const_iterator ITERATOR   ;

  const CANDIDATES a1 = source1() () ;
  if ( a1.empty() ) { return result_type () ; } // no action for EMPTY input

  const CANDIDATES a2 = source2() () ;
  if ( a2.empty() ) { return result_type () ; } // no action for EMPTY input

  // input selections:
  const CANDIDATES* arg1 = &a1;
  const CANDIDATES* arg2 = &a2;

  // the output selection
  CANDIDATES output;

  // the combination tools
  typedef LoKi::Combiner_<LHCb::Particle::ConstVector> Combiner ;
  const IParticleCombiner* combiner = pc() ;
  const LoKi::Particles::PidCompare compare = LoKi::Particles::PidCompare () ;

  // get the daughters
  Selected daughters;
  // loop over two selections and add daughters
  for ( ITERATOR icand1 = arg1->begin(); icand1 != arg1->end(); icand1++ )
  {
    const Hlt::Candidate* cand1 = *icand1 ;
    if ( !cand1 ) { continue ; }

    const LHCb::Particle* particle1 = cand1->get<LHCb::Particle> () ;
    if ( !particle1 ) { continue ; }

    //std::cout << "===================================== PARTICLE 1 ===================================" << std::endl;
    //particle1->fillStream(std::cout);
    //std::cout << std::endl;
    //std::cout << "====================================================================================" << std::endl;

    daughters.add ( LoKi::Particles::nameFromPID(particle1->particleID()) , particle1 ) ;
  }

  for ( ITERATOR icand2 = arg2->begin(); icand2 != arg2->end(); icand2++ )
  {

    const Hlt::Candidate* cand2 = *icand2 ;
    if ( !cand2 ) { continue ; }

    const LHCb::Particle* particle2 = cand2->get<LHCb::Particle> () ;
    if ( !particle2 ) { continue ; }

    //std::cout << "===================================== PARTICLE 2 ===================================" << std::endl;
    //particle2->fillStream(std::cout);
    //std::cout << std::endl;
    //std::cout << "====================================================================================" << std::endl;

    daughters.add ( LoKi::Particles::nameFromPID(particle2->particleID()) , particle2 ) ;
  }

  //std::cout << " == STARTING COMBINATION LOOPS HERE ! == " << std::endl;

  // loop over all decays
  for ( std::vector<Decays::Decay>::const_iterator idecay = m_decays.begin(); 
        idecay != m_decays.end(); idecay++)
  {

    // std::cout << "In decay loop for decay: "; 
    // idecay->fillStream(std::cout); std::cout << std::endl;

    Combiner loop ;
    const Decays::Decay::Items& dec_items = idecay->children() ;
    for ( Decays::Decay::Items::const_iterator child = dec_items.begin() ; 
          child != dec_items.end(); child++)
    {
      //std::cout << "In child loop: "; child->fillStream(std::cout); std::cout << std::endl;
      loop.add( daughters ( child->name() ) ) ;
    }

    // now do actual loop of combinations
    for ( ; loop.valid() ; loop.next() )
    {
        if ( !loop.unique( compare ) ) { continue ; }

        // get current combination
        LHCb::Particle::ConstVector combination ( loop.dim() );
        loop.current ( combination.begin() ) ;

        // apply comb cuts
        if ( ! m_acut.fun ( combination ) ) { continue ; }

        LHCb::Vertex*     vertex = new LHCb::Vertex();
        LHCb::Particle*   mother = new LHCb::Particle();
        mother->setParticleID( idecay->mother().pid() );

        // do combination
        combiner -> combine ( combination , *mother , *vertex ) ;

        // apply mother cuts
        if ( ! m_cut.fun ( mother ) ) {
          delete vertex;
          delete mother;
          continue ;
        }

        // store in TES
        _storeParticle( mother ) ;

        // add new candidate
        Hlt::Candidate *candidate = newCandidate();
        candidate->addToWorkers ( alg() ) ;
        // add new stage
        Hlt::Stage* stage = newStage () ;
        candidate->addToStages( stage ) ;
        Hlt::Stage::Lock lock { stage, alg() } ;
        stage->set( mother ) ;
        output.push_back ( candidate ) ;
        // ====================================================================
      } //                                         end loop over combinations
    // ========================================================================
    } //                                           end of loop over decays
  // ==========================================================================
  // register the selection in Hlt Data Service
  return !m_sink ? output : m_sink ( output ) ;
  // ==========================================================================
}
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream& LoKi::Hlt1::Hlt1Combiner::fillStream ( std::ostream& s ) const
{
  return
    s << "TC_HLT1COMBINER("
      << "'" << location () << "')" ;
}
// ============================================================================
