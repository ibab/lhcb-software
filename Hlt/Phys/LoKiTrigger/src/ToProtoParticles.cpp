// $Id: ToProtoParticles.cpp 180655 2014-11-25 10:38:48Z mkenzie $
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
#include "LoKi/ToProtoParticles.h"
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
 *  Implementation file for class LoKi::Hlt1::ToProtoParticles
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
LoKi::Hlt1::ToProtoParticles::ToProtoParticles
( const std::string& location )
  : LoKi::AuxFunBase ( std::tie ( location ) )
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe ()
  , LoKi::Hlt1::HelperTool ( 1 )
  , m_sink ( location )
{}
// ============================================================================
// virtual destructor
// ============================================================================
LoKi::Hlt1::ToProtoParticles::~ToProtoParticles () {}
// ============================================================================
// clone method ("virtual constructor")
// ============================================================================
LoKi::Hlt1::ToProtoParticles* LoKi::Hlt1::ToProtoParticles::clone() const
{ return new LoKi::Hlt1::ToProtoParticles { *this } ; }
// ============================================================================
// the only one important method
// ============================================================================
LoKi::Hlt1::ToProtoParticles::result_type
LoKi::Hlt1::ToProtoParticles::operator()
  ( LoKi::Hlt1::ToProtoParticles::argument a ) const
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

    // get the track
    const LHCb::Track* track = cand1->get<LHCb::Track> () ;
    if ( !track ) { continue ; }

    // create ProtoParticle
    LHCb::ProtoParticle* proto = newProtoParticle();
    if ( !proto ) { continue ; }
    proto->setTrack(track);

    // add new candidate
    Hlt::Candidate *candidate = newCandidate();
    candidate->addToWorkers ( alg() ) ;
    // add new stage
    Hlt::Stage*  stage = newStage () ;
    candidate->addToStages( stage ) ;
    Hlt::Stage::Lock lock { stage, alg() } ;
    stage->set( proto ) ;
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
std::ostream& LoKi::Hlt1::ToProtoParticles::fillStream ( std::ostream& s ) const
{
  return
    s << "TC_TOPROTOPARTICLES( '' )" ;
}
// ============================================================================
