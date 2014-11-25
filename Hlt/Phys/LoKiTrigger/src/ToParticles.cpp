#ifdef __INTEL_COMPILER
#pragma warning(disable:1572) // non-pointer conversion ... may lose significant bits
#pragma warning(push)
#endif

#include "HltBase/HltUtils.h"

#include "GaudiAlg/GaudiAlgorithm.h"

#include "LoKi/Combiner.h"
#include "LoKi/ToParticles.h"
#include "LoKi/Hlt1.h"
#include "LoKi/Constants.h"
#include "LoKi/ParticleProperties.h"
#include "LoKi/ILoKiSvc.h"
#include "LTTools.h"

#ifdef __INTEL_COMPILER
#pragma warning(pop)
#endif

LoKi::Hlt1::ToParticles::ToParticles
( std::string               pid         ,
  std::string               location    ,
  double                    ptcut       )
  : LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe ()
  , LoKi::Hlt1::HelperTool ( 1 )
  , m_sink { std::move(location) }
  , m_pid( LoKi::Particles::pidFromName(pid) )
  , m_ptcut { ptcut }
{
}

LoKi::Hlt1::ToParticles::~ToParticles () {}

LoKi::Hlt1::ToParticles* LoKi::Hlt1::ToParticles::clone() const
{ return new LoKi::Hlt1::ToParticles { *this } ; }

LoKi::Hlt1::ToParticles::result_type
LoKi::Hlt1::ToParticles::operator()
  ( LoKi::Hlt1::ToParticles::argument a ) const
{
  Assert ( alg() , "Invalid setup!" );

  typedef result_type           CANDIDATES ;

  LoKi::ILoKiSvc* svc = lokiSvc() ;
  SmartIF<IToolSvc> tsvc ( svc );

  IParticle2State* m_p2s = 0;
  StatusCode sc = tsvc->retrieveTool("Particle2State",m_p2s);

  if ( a.empty() ) { return result_type () ; }

  const CANDIDATES* arg1 = &a;

  CANDIDATES   output;

  // loop candidates
  for ( auto  icand1 = arg1->begin() ; arg1->end() != icand1 ; ++icand1 )
  {
    const Hlt::Candidate* cand1 = *icand1 ;
    if ( !cand1 ) { continue ; }                              // CONTINUE

    // get the track
    const LHCb::Track* track = cand1->get<LHCb::Track> () ;
    if ( !track ) { continue ; }

    // create ProtoParticle
    LHCb::ProtoParticle* proto = newProtoParticle();
    if ( !proto ) { continue ; }
    proto->setTrack(track);

    // create Particle
    LHCb::Particle* particle = newParticle( location() );
    if ( !particle ) { continue ; }
    particle->setProto(proto);

    // which state should be used here ?!?!
    const LHCb::State* state = 0;
    // try closest to beam
    if ( !state ) { state = track->stateAt ( LHCb::State::ClosestToBeam )    ; }
    // try first measurement
    if ( !state ) { state = track->stateAt ( LHCb::State::FirstMeasurement ) ; }
    // try first state
    if ( !state ) { state = &track->firstState()                             ; }
    // continue
    if ( !state ) { continue ; }

    // fill particle from state
    const StatusCode sc = m_p2s->state2Particle( *state, *particle ) ;
    if ( sc.isFailure() ) { continue ; }

    // set PID
    particle->setParticleID(m_pid);

    // cuts here
    if ( !particle->pt()>ptcut() ) { continue ; }

    //std::cout << "Used track: " << std::endl;
    //track->fillStream(std::cout);
    //std::cout << "Used state: " << std::endl;
    //state->fillStream(std::cout);
    //std::cout << "Used proto: " << std::endl;
    //proto->fillStream(std::cout);
    //std::cout << "====================================================================================" << std::endl;
    //std::cout << "Used particle: " << std::endl;
    //particle->fillStream(std::cout);
    //std::cout << std::endl;
    //std::cout << "====================================================================================" << std::endl;
    //
    // start new candidate
    //
    Hlt::Candidate *candidate = newCandidate();
    candidate->addToWorkers ( alg() ) ;
    // add new stage
    Hlt::Stage*  stage = newStage () ;
    candidate->addToStages( stage ) ;
    Hlt::Stage::Lock lock { stage, alg() } ;
    stage->set( particle ) ;
    //
    /*
    {
      //
      // 1. "initiator" stage - copy the first candidate
      //
      /// get new stage
      Hlt::Stage*     stage   = newStage  () ;
      candidate -> addToStages ( stage ) ;
      /// lock new stage:
      Hlt::Stage::Lock lock { stage , alg() } ;
      //
      lock.addToHistory ( cand1->workers() ) ;
      // lock.addToHistory ( myName()         ) ;
      stage -> set ( cand1->currentStage() ) ;
    }
    //
    {
      //
      // 2. "regular" stage : particle
      //
      /// get new stage
      Hlt::Stage*     stage   = newStage () ;
      candidate -> addToStages ( stage ) ;
      /// lock new stage:
      Hlt::Stage::Lock lock { stage , alg() } ;
      // lock.addToHistory ( myName()         ) ;
      stage    -> set ( particle ) ;
    }
    */
    // add new candidate to the output:
    output.push_back ( candidate ) ;
  }
  return !m_sink ? output : m_sink ( output ) ;
}

std::ostream& LoKi::Hlt1::ToParticles::fillStream ( std::ostream& s ) const
{
  return
    s << "TC_TOPARTICLES("
      <<        m_pid    << ","
      << "'" << location() << "')" ;
}
