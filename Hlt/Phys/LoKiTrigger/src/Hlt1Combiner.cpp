#ifdef __INTEL_COMPILER
#pragma warning(disable:1572) // non-pointer conversion ... may lose significant bits
#pragma warning(push)
#endif

#include "HltBase/HltUtils.h"

#include "GaudiAlg/GaudiAlgorithm.h"
#include "Kernel/IParticleCombiner.h"
#include "Kernel/DaVinciAlgorithm.h"
#include "Kernel/IDecodeSimpleDecayString.h"

#include "LoKi/Hlt1Combiner.h"
#include "LoKi/ToParticles.h"
#include "LoKi/Hlt1.h"
#include "LoKi/Constants.h"
#include "LoKi/ParticleProperties.h"
#include "LoKi/CompareParticles.h"
#include  "LTTools.h"

#ifdef __INTEL_COMPILER
#pragma warning(pop)
#endif

LoKi::Hlt1::Hlt1Combiner::Hlt1Combiner
( std::string                                                decay      ,
  std::string                                                output     ,
  const LoKi::Hlt1::Hlt1Combiner::Source&                    particles1 ,
  const LoKi::Hlt1::Hlt1Combiner::Source&                    particles2 ,
  const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut    ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&      mothcut    )
  : LoKi::BasicFunctors<const Hlt::Candidate*>::Source ()
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

LoKi::Hlt1::Hlt1Combiner::Hlt1Combiner
( std::string                                                decay      ,
  std::string                                                output     ,
  std::string                                                particles1 ,
  std::string                                                particles2 ,
  const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut    ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&      mothcut    )
  : LoKi::BasicFunctors<const Hlt::Candidate*>::Source ()
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

LoKi::Hlt1::Hlt1Combiner::~Hlt1Combiner () {}

LoKi::Hlt1::Hlt1Combiner* LoKi::Hlt1::Hlt1Combiner::clone() const
{ return new LoKi::Hlt1::Hlt1Combiner { *this } ; }

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

  std::cout << " == STARTING TO FILL DAUGHTERS HERE ! == " << std::endl;

  // get the daughters
  Selected daughters;
  // loop over two selections and add daughters
  for ( ITERATOR icand1 = arg1->begin(); icand1 != arg1->end(); icand1++ )
  {
    const Hlt::Candidate* cand1 = *icand1 ;
    if ( !cand1 ) { continue ; }

    const LHCb::Particle* particle1 = cand1->get<LHCb::Particle> () ;
    if ( !particle1 ) { continue ; }

    daughters.add ( LoKi::Particles::nameFromPID(particle1->particleID()) , particle1 ) ;
  }

  for ( ITERATOR icand2 = arg2->begin(); icand2 != arg2->end(); icand2++ )
  {

    const Hlt::Candidate* cand2 = *icand2 ;
    if ( !cand2 ) { continue ; }

    const LHCb::Particle* particle2 = cand2->get<LHCb::Particle> () ;
    if ( !particle2 ) { continue ; }

    daughters.add ( LoKi::Particles::nameFromPID(particle2->particleID()) , particle2 ) ;
  }

  std::cout << " == STARTING COMBINATION LOOPS HERE ! == " << std::endl;

  // loop over all decays
  for ( std::vector<Decays::Decay>::const_iterator idecay = m_decays.begin(); idecay != m_decays.end(); idecay++)
  {

    std::cout << "In decay loop for decay: "; idecay->fillStream(std::cout); std::cout << std::endl;

    Combiner loop ;
    const Decays::Decay::Items& dec_items = idecay->children() ;
    for ( Decays::Decay::Items::const_iterator child = dec_items.begin() ; child != dec_items.end(); child++)
    {
      std::cout << "In child loop: "; child->fillStream(std::cout); std::cout << std::endl;
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
        if ( ! m_acut ( combination ) ) { continue ; }

        LHCb::Vertex     vertex ;
        LHCb::Particle   *mother = newParticle( location() );
        mother->setParticleID( idecay->mother().pid() );
        combiner -> combine ( combination , *mother , vertex ) ;

        // apply mother cuts
        if ( ! m_cut ( mother ) ) { continue ; }

        //std::cout << "====================================================================================" << std::endl;
        //std::cout << " CREATED PARTICLE: " << std::endl;
        //mother.fillStream(std::cout);
        //std::cout << std::endl;
        //std::cout << "====================================================================================" << std::endl;

        //
        // start new candidate
        //
        Hlt::Candidate *candidate = newCandidate();
        candidate->addToWorkers ( alg() ) ;
        // add new stage
        Hlt::Stage* stage = newStage () ;
        candidate->addToStages( stage ) ;
        Hlt::Stage::Lock lock { stage, alg() } ;
        stage->set( mother ) ;
        output.push_back ( candidate ) ;
    } // loop over combinations
  } // loop over decays
  return !m_sink ? output : m_sink ( output ) ;
}

std::ostream& LoKi::Hlt1::Hlt1Combiner::fillStream ( std::ostream& s ) const
{
  return
    s << "TC_HLT1COMBINER("
      << "'" << location () << "')" ;
}

  /*
  std::cout << " NOW WE COMMENCE THE COMBINATIONS" << std::endl;
  // loop over all decays
  for ( std::vector<Decays::Decay>::const_iterator idecay = m_decays.begin(); idecay != m_decays.end(); idecay++){

    const LoKi::Particles::PidCompare compare = LoKi::Particles::PidCompare () ;

    Combiner loop ;
    const Decays::Decay::Items& dec_items = idecay->children() ;
    for ( Decays::Decay::Items::const_iterator child = dec_items.begin() ; child != dec_items.end(); child++)
    {
      loop.add( daughters ( child->name() ) ) ;
    }

    // now do actual loop of combinations
    for ( ; loop.valid() ; loop.next() )
    {
        if ( !loop.unique( compare ) ) { continue ; }

        // get current combination
        LHCb::Particle::ConstVector combination ( loop.dim() );
        loop.current ( combination.begin() ) ;

        LHCb::Vertex     vertex ;
        LHCb::Particle   mother ( idecay->mother().pid() ) ;
        combiner -> combine ( combination , mother , vertex ) ;

        // newCandiate() ....
        std::cout << " CREATED PARTICLE: " << std::endl;
        mother.fillStream(std::cout);
        // apply cuts
        //

        // store candidates
        // start new candidate
        //
        Hlt::Candidate *candidate = newCandidate();
        candidate->addToWorkers ( alg() ) ;
        // add new stage
        Hlt::Stage* stage = newStage () ;
        candidate->addToStages( stage ) ;
        Hlt::Stage::Lock lock { stage, alg() } ;
        stage->set( &mother ) ;
        //
        */
        /*
        {
          //
          // 1. "regular" stage : particle
          //
          /// get new stage
          Hlt::Stage*     stage   = newStage () ;
          candidate -> addToStages ( stage ) ;
          /// lock new stage:
          Hlt::Stage::Lock lock { stage , alg() } ;
          // lock.addToHistory ( myName()         ) ;
          stage    -> set ( &mother ) ;
        }
        //
        {
          // 2. "regular stage: vertex
          //
          // get new stage
          Hlt::Stage*     stage   = newStage () ;
          candidate -> addToStages ( stage ) ;
          /// lock new stage:
          Hlt::Stage::Lock lock { stage , alg() } ;
          // lock.addToHistory ( myName()         ) ;
          stage    -> set ( &vertex ) ;
        }
        */

        // add new candidate to the output:


/// TESTING STUFF:
/*
  for ( std::vector<Decays::Decay>::const_iterator idecay = m_decays.begin(); idecay != m_decays.end(); idecay++){
    const Decays::Decay::Items& dec_items = idecay->children() ;
    for ( Decays::Decay::Items::const_iterator child = dec_items.begin() ; child != dec_items.end(); child++)
    {
      //const LHCb::Particle::ConstVector particles;
      if ( child->name() == "K+"  || child->name() == "K-" )  { tes_location = "Kaons"; }
      if ( child->name() == "pi+" || child->name() == "pi-" ) { tes_location = "Pions"; }
      if ( child->name() == "mu+" || child->name() == "mu-" ) { tes_location = "Muons"; }

      // loop over candidates to get appropriate input candidates
      for ( auto icand1 = arg1->begin(); icand1 != arg1->end(); icand1 ++)
      {

        const Hlt::Candidate* cand1 = *icand1 ;
        if ( !cand1 ) { continue ; }
        // track
        const LHCb::Track* track = cand1->get<LHCb::Track> () ;
        if ( !track ) { continue ; }
        // proto
        LHCb::ProtoParticle* proto = newProtoParticle();
        if ( !proto ) { continue ; }
        proto->setTrack(track);
        // particle
        LHCb::Particle* particle = newParticle( tes_location );
        if ( !particle ) { continue ; }
        particle->setProto(proto);
        // state
        const LHCb::State* state = getState( track );
        if ( !state ) { continue ; }
        // fill particle from state
        const StatusCode sc = p2s()->state2Particle( *state, *particle ) ;
        if ( sc.isFailure() ) { continue ; }

        // set PID
        particle->setParticleID( child->pid() );

        std::cout << "Used particle: " << std::endl;
        particle->fillStream(std::cout);
        std::cout << std::endl;

        // add to daughters
        daughters.add( child->name(), particle );

        std::cout << "Added to daughters" << std::endl;
        //particles.push_back(particle);
      }
      //daughters.add( child->name(), particles );
    }
  }

  std::cout << " SHOULD HAVE FILLED DAUGHTERS BY NOW " << std::endl;

  // loop decays to figure out daughters
  // CAN INCORPORATE CUTS WITH THIS KIND OF THING:
  // loop cuts to filter particles
  //for ( MyCuts::const_iterator idau = m_cuts.begin(); idau != m_cuts.end(); idau++ )
  //{
  //  daughters.add
  //    ( idau->m_name      ,
  //      particles.begin() ,
  //      particles.end()   ,
  //      LoKi::Particles::Identifier() == idau->m_pid && idau->m_cut.func() ) ;
  //}

 */
