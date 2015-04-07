// $Id: L0CaloToParticles.cpp 180655 2015-03-19 apuignav $
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
#include "LoKi/L0CaloToParticles.h"
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
 *  Implementation file for class LoKi::Hlt1::L0CaloToParticles
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
//  consructor from the pid hypothesis, name and LoKi basic functor
// ============================================================================
LoKi::Hlt1::L0CaloToParticles::L0CaloToParticles
( const std::string& pid         ,
  const std::string& location    ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&   cut )
  : LoKi::AuxFunBase ( std::tie ( pid , location , cut ) )
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe ()
  , LoKi::Hlt1::CaloHelperTool ( 1 )
  , m_sink ( location )
  , m_pp   ( LoKi::Particles::ppFromName(pid) )
  , m_cut  ( cut      )
{
 LoKi::ILoKiSvc* svc = lokiSvc() ;
 SmartIF<IToolSvc> tsvc ( svc ) ;
 StatusCode sc = tsvc->retrieveTool("CaloDataProvider", "EcalReadoutTool", m_calo);
}
// ============================================================================
// virtual destructor
// ============================================================================
LoKi::Hlt1::L0CaloToParticles::~L0CaloToParticles () {}
// ============================================================================
// clone method ("virtual constructor")
// ============================================================================
LoKi::Hlt1::L0CaloToParticles* LoKi::Hlt1::L0CaloToParticles::clone() const
{ return new LoKi::Hlt1::L0CaloToParticles { *this } ; }
// ============================================================================
// the only one important method
// ============================================================================
LoKi::Hlt1::L0CaloToParticles::result_type
LoKi::Hlt1::L0CaloToParticles::operator()
  ( LoKi::Hlt1::L0CaloToParticles::argument a ) const
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

    // get the proto
    const LHCb::L0CaloCandidate* calo = cand1->get<LHCb::L0CaloCandidate> () ;
    if ( !calo ) { continue ; }

    // Filter calo candidates for photon
    if ( 22 == m_pp->particleID().pid() && !isPhotonCand(calo) ) { continue ; }

    // create Particle
    LHCb::Particle* particle = caloCandidateToParticle( calo, m_pp->particleID() ) ;

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
std::ostream& LoKi::Hlt1::L0CaloToParticles::fillStream ( std::ostream& s ) const
{
  return
    s << "TC_L0CALOTOPARTICLES("
      << "'" << m_pp->name() << "',"
      << "'" << location()   << "')" ;
}
// ============================================================================
