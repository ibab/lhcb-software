// $Id: ToParticles.cpp 180655 2014-11-25 10:38:48Z mkenzie $
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
// PartProp
// ============================================================================
#include "Kernel/ParticleProperty.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Combiner.h"
#include "LoKi/ToParticles.h"
#include "LoKi/Hlt1.h"
#include "LoKi/Constants.h"
#include "LoKi/ParticleProperties.h"
#include "LoKi/ILoKiSvc.h"
#include "LTTools.h"
// ============================================================================
#ifdef __INTEL_COMPILER
#pragma warning(pop)
#endif
// ============================================================================
/** @file
 *
 *  Implementation file for class LoKi::Hlt1::ToParticles
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
//  consructor from the pid hypothesis, name and simple pt cut
// ============================================================================
LoKi::Hlt1::ToParticles::ToParticles
( const std::string& pid         ,
  const std::string& location    ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&   cut )
  : LoKi::AuxFunBase ( std::tie ( pid , location , cut ) ) 
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe ()
  , LoKi::Hlt1::HelperTool ( 1 )
  , m_sink ( location ) 
  , m_pid       ( pid ) 
  , m_pid_plus  (     ) 
  , m_pid_minus (     ) 
  , m_mass      ( 0   ) 
  , m_cut       ( cut )    
{
  const LHCb::ParticleProperty* pp_1 = LoKi::Particles::ppFromName ( m_pid ) ;
  Assert ( 0 != pp_1 , "Invalid PID is specified  '" + m_pid + "'" ) ;
  const LHCb::ParticleProperty* pp_2 = pp_1->anti()  ;
  Assert ( 0 != pp_2 , "Invalid anti-particle for '" + m_pid + "'" ) ;
  //
  if ( 0 < pp_1->charge() ) { m_pid_plus  = pp_1 ->particleID() ; }
  else                      { m_pid_minus = pp_1 ->particleID() ; }
  //
  if ( 0 < pp_2->charge() ) { m_pid_plus  = pp_2 ->particleID() ; }
  else                      { m_pid_minus = pp_2 ->particleID() ; }
  //
  Assert ( 0 != m_pid_plus .pid() , "Invalid PID+ for '" + m_pid + "'" ) ;
  Assert ( 0 != m_pid_minus.pid() , "Invalid PID- for '" + m_pid + "'" ) ;
  //
  m_mass = pp_1 -> mass() ;
  Assert ( 0 < m_mass  , "Invalid mass for '" + m_pid + "'" ) ;
}
// ============================================================================
// virtual destructor
// ============================================================================
LoKi::Hlt1::ToParticles::~ToParticles () {}
// ============================================================================
// clone method ("virtual constructor")
// ============================================================================
LoKi::Hlt1::ToParticles* LoKi::Hlt1::ToParticles::clone() const
{ return new LoKi::Hlt1::ToParticles { *this } ; }
// ============================================================================
// the only one important method
// ============================================================================
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
    LHCb::Particle* particle = new LHCb::Particle();
    if ( !particle ) { continue ; }
    particle->setProto(proto);
    
    // set PID
    0 < track->charge() ?
      particle->setParticleID    ( m_pid_plus  ) : 
      particle->setParticleID    ( m_pid_minus ) ;
    
    // set mass
    particle->setMeasuredMass    ( m_mass );
    particle->setMeasuredMassErr ( 0 );
    
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

    // cuts here
    if ( ! m_cut ( particle ) ) {
      delete particle;
      continue ;
    }

    // store in TES
    _storeParticle( particle ) ;

    // add new candidate
    Hlt::Candidate *candidate = newCandidate();
    candidate->addToWorkers ( alg() ) ;
    // add new stage
    Hlt::Stage*  stage = newStage () ;
    candidate->addToStages( stage ) ;
    Hlt::Stage::Lock lock { stage, alg() } ;
    stage->set( particle ) ;
    output.push_back ( candidate ) ;
    // ========================================================================
  } //                                           end of loop over input data
  // ==========================================================================
  // register the selection in Hlt Data Service
  return !m_sink ? output : m_sink ( output ) ;
  // ==========================================================================
}
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream& LoKi::Hlt1::ToParticles::fillStream ( std::ostream& s ) const
{
  return
    s << "TC_TOPARTICLES("
      <<        m_pid      << " ,"
      << "'" << location() << "'," 
      <<        m_cut      << ")" ;
}
// ============================================================================
