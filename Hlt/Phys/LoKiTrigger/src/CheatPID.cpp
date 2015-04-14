// $Id: CheatPID.cpp 180655 2015-03-19 apuignav $
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
#include "LoKi/CheatPID.h"
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
 *  Implementation file for class LoKi::Hlt1::CheatPID
 *  This file is part of LoKi project:
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
 *  contributions and advices from G.Raven, J.van Tilburg,
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Albert Puig (albert.puig@cern.ch)
 *  @date   2015-03-19
 */
// ============================================================================
//  consructor from the pid hypothesis
// ============================================================================
LoKi::Hlt1::CheatPID::CheatPID
( const std::string& pid         ,
  const std::string& location    )
  : LoKi::AuxFunBase ( std::tie ( pid , location ) )
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe ()
  , LoKi::Hlt1::HelperTool ( 1 )
  , m_sink ( location )
  , m_pp   ( LoKi::Particles::ppFromName(pid) )
{}
// ============================================================================
// virtual destructor
// ============================================================================
LoKi::Hlt1::CheatPID::~CheatPID () {}
// ============================================================================
// clone method ("virtual constructor")
// ============================================================================
LoKi::Hlt1::CheatPID* LoKi::Hlt1::CheatPID::clone() const
{ return new LoKi::Hlt1::CheatPID { *this } ; }
// ============================================================================
// the only one important method
// ============================================================================
LoKi::Hlt1::CheatPID::result_type
LoKi::Hlt1::CheatPID::operator()
  ( LoKi::Hlt1::CheatPID::argument a ) const
{
  Assert ( alg() , "Invalid setup!" );

  typedef result_type           CANDIDATES ;

  if ( a.empty() ) { return result_type () ; }

  const CANDIDATES* arg1 = &a;

  CANDIDATES   output;

  // loop candidates
  for ( auto  icand1 = arg1->begin() ; arg1->end() != icand1 ; ++icand1 )
  {
    const Hlt::Candidate* cand1 = *icand1 ;
    if ( !cand1 ) { continue ; }                              // CONTINUE

    const LHCb::Particle* part = cand1->get<LHCb::Particle> () ;

    if ( !part ) { continue ; }

    if ( part->isBasicParticle() ) { continue ; }

    LHCb::Particle* particle = part->clone() ;
    particle->setParticleID(m_pp->particleID()) ;

    LHCb::Vertex* vertex = part->endVertex()->clone() ;
	if ( vertex )
	{
		particle->setEndVertex( vertex ) ;
		_storeVertex( vertex ) ;
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
std::ostream& LoKi::Hlt1::CheatPID::fillStream ( std::ostream& s ) const
{
  return
    s << "TC_CHEATPID("
      << "'" << m_pp->name() << "',"
      << "'" << location()   << "')" ;
}
// ============================================================================
