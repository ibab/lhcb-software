// $Id: Hlt1DiPhotonMaker.cpp 180655 2015-03-19 apuignav $
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
#include "LoKi/Hlt1DiPhotonMaker.h"
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
 *  Implementation file for class LoKi::Hlt1::Hlt1DiPhotonMaker
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
//  constructor from the pid hypothesis, name and LoKi basic functor
// ============================================================================
LoKi::Hlt1::Hlt1DiPhotonMaker::Hlt1DiPhotonMaker
( const std::string& pid          ,
  const std::string& location     ,
  const double photonEtMin        ,
  const double photonEtSumMin     ,
  const double diphotonMassMin    ,
  const double diphotonMassMax    ,
  const double diphotonEtMin      )
  : LoKi::AuxFunBase ( std::tie ( pid, location, photonEtMin, photonEtSumMin,
                                  diphotonMassMin, diphotonMassMax, diphotonEtMin ) )
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe ()
  , LoKi::Hlt1::CaloHelperTool ( 1 )
  , m_sink ( location )
  , m_pp ( LoKi::Particles::ppFromName(pid) )
  , m_photonEtMin (photonEtMin)
  , m_photonEtSumMin ( photonEtSumMin )
  , m_diPhotonMassSqMin ( diphotonMassMin*diphotonMassMin )
  , m_diPhotonMassSqMax ( diphotonMassMax*diphotonMassMax )
  , m_diphotonEtMin ( diphotonEtMin )
{}
// ============================================================================
// virtual destructor
// ============================================================================
LoKi::Hlt1::Hlt1DiPhotonMaker::~Hlt1DiPhotonMaker () {}
// ============================================================================
// clone method ("virtual constructor")
// ============================================================================
LoKi::Hlt1::Hlt1DiPhotonMaker* LoKi::Hlt1::Hlt1DiPhotonMaker::clone() const
{ return new LoKi::Hlt1::Hlt1DiPhotonMaker { *this } ; }
// ============================================================================
// the only one important method
// ============================================================================
LoKi::Hlt1::Hlt1DiPhotonMaker::result_type
LoKi::Hlt1::Hlt1DiPhotonMaker::operator()
  ( LoKi::Hlt1::Hlt1DiPhotonMaker::argument a ) const
{
  Assert ( alg() , "Invalid setup!" );

  typedef result_type           CANDIDATES ;

  if ( a.empty() ) { return result_type () ; }

  const CANDIDATES* arg1 = &a;

  CANDIDATES   output;

  // loop candidates
  for ( auto icand1 = arg1->begin() ; arg1->end() != icand1 ; ++icand1 )
  {
    const Hlt::Candidate* cand1 = *icand1 ;
    if ( !cand1 ) { continue ; }                              // CONTINUE

    // get the L0CaloCandidate
    const LHCb::L0CaloCandidate* calo1 = cand1->get<LHCb::L0CaloCandidate> () ;
    if ( !calo1 ) { continue ; }

    // check the candidate type
    if ( !isPhotonCand(calo1) ) { continue ; }

    // Et cut
    const double et1 = calo1->et() ;
    if ( m_photonEtMin > et1 ) { continue ; }

    for ( auto icand2 = icand1+1 ; arg1->end() != icand2; ++icand2 )
    {
        const Hlt::Candidate* cand2 = *icand2 ;
        if ( !cand2 ) { continue ; }

        // get the L0CaloCandidate
        const LHCb::L0CaloCandidate* calo2 = cand2->get<LHCb::L0CaloCandidate> () ;
        if ( !calo2 ) { continue ; }

        // check the candidate type
        if ( !isPhotonCand(calo2) ) { continue ; }

        // Et cut
        const double et2 = calo2->et() ;
        if ( m_photonEtMin > et2 ) { continue ; }

        // Sum Et cut
        if ( m_photonEtSumMin > (et1+et2) ) { continue ; }

        // Mass cut
        const Gaudi::XYZPoint pos1 = calo1 -> position() ;
        const Gaudi::XYZPoint pos2 = calo2 -> position() ;

        const double x1  = pos1.x() ;
        const double y1  = pos1.y() ;
        const double x2  = pos2.x() ;
        const double y2  = pos2.y() ;

        // Can we improve this with pos.rho?
        // I cannot do the diff between the GaudiXYZ points
        const double M2 = et1 * et2 * (((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2))/std::sqrt(x1*x1+y1*y1)/std::sqrt(x2*x2+y2*y2)) ;
        if ( m_diPhotonMassSqMax < M2 ) { continue ; }
        if ( m_diPhotonMassSqMin > M2 ) { continue ; }


        // Create proper photons
        LHCb::Particle *gamma1 = caloCandidateToParticle( calo1, LHCb::ParticleID(22) ) ;
        LHCb::Particle *gamma2 = caloCandidateToParticle( calo2, LHCb::ParticleID(22) ) ;

        // Build the CaloParticle and cut on Et
        LHCb::CaloParticle diphoton( gamma1 ) ;
        diphoton.addCaloPosition( gamma2 ) ;
        if( m_diphotonEtMin > diphoton.momentum().pt() ) { continue ; }

        // Build the diphoton particle
        LHCb::Particle* particle = new LHCb::Particle();
        particle->setMeasuredMass( diphoton.mass() );
        particle->setMeasuredMassErr( diphoton.emass() );
        particle->setMomentum( diphoton.momentum() ) ;
        particle->setParticleID( m_pp->particleID() ) ;
        particle->addToDaughters( gamma1 ) ;
        particle->addToDaughters( gamma2 ) ;

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
    }
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
std::ostream& LoKi::Hlt1::Hlt1DiPhotonMaker::fillStream ( std::ostream& s ) const
{
  return s << "TC_HLT1DIPHOTONMAKER("
           << "'" << m_pp->name() << "',"
           << "'" << location()   << "',"
                  << m_photonEtMin << ","
                  << m_photonEtSumMin << ","
                  << std::sqrt(m_diPhotonMassSqMin) << ","
                  << std::sqrt(m_diPhotonMassSqMax) << ","
                  << m_diphotonEtMin << ")" ;
}
// ============================================================================
