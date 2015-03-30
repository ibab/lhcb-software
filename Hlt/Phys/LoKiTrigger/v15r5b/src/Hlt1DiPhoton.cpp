// $Id: Hlt1DiPhoton.cpp 180655 2015-03-19 apuignav $
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
#include "LoKi/Hlt1DiPhoton.h"
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
 *  Implementation file for class LoKi::Hlt1::Hlt1DiPhoton
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
LoKi::Hlt1::Hlt1DiPhoton::Hlt1DiPhoton
( const std::string& pid          ,
  const std::string& location     ,
  const double photonEtMin        ,
  const double photonEtSumMin     ,
  const double photonEtAsymMax    ,
  const double diphotonMassMin    ,
  const double diphotonEtMin      )
  : LoKi::AuxFunBase ( std::tie ( photonEtMin, photonEtSumMin,
                                  photonEtAsymMax, diphotonMassMin, diphotonEtMin ) )
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe ()
  , LoKi::Hlt1::HelperTool ( 1 )
  , m_sink ( location )
  , m_pp ( LoKi::Particles::ppFromName(pid) )
  , m_photonEtMin (photonEtMin)
  , m_photonEtSumMin ( photonEtSumMin )
  , m_photonEtAsymMax ( photonEtAsymMax )
  , m_diPhotonMassSqMin ( diphotonMassMin*diphotonMassMin )
  , m_diphotonEtMin (diphotonEtMin)
  , m_z (12300.0)
  , m_calo (NULL)
{
 LoKi::ILoKiSvc* svc = lokiSvc() ;
 SmartIF<IToolSvc> tsvc ( svc ) ;
 StatusCode sc = tsvc->retrieveTool("CaloDataProvider", "EcalReadoutTool", m_calo);
}
// ============================================================================
// virtual destructor
// ============================================================================
LoKi::Hlt1::Hlt1DiPhoton::~Hlt1DiPhoton () {}
// ============================================================================
// clone method ("virtual constructor")
// ============================================================================
LoKi::Hlt1::Hlt1DiPhoton* LoKi::Hlt1::Hlt1DiPhoton::clone() const
{ return new LoKi::Hlt1::Hlt1DiPhoton { *this } ; }
// ============================================================================
// the only one important method
// ============================================================================
LoKi::Hlt1::Hlt1DiPhoton::result_type
LoKi::Hlt1::Hlt1DiPhoton::operator()
  ( LoKi::Hlt1::Hlt1DiPhoton::argument a ) const
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

        // Asymmetry cut
        if ( m_photonEtAsymMax < fabs(et1-et2)/(et1+et2) ) { continue ; }

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
        if ( m_diPhotonMassSqMin > M2 ) { continue ; }
        //if ( 8000*8000 < M2 ) { continue ; }
        //std::cout << std::sqrt(M2) << std::endl;

        // Create proper photons
        LHCb::Particle *gamma1 = caloRecoChain( calo1 ) ;
        LHCb::Particle *gamma2 = caloRecoChain( calo2  ) ;

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
// Create a full LHCb particle from L0CaloCandidate ============================================================================
LHCb::Particle* LoKi::Hlt1::Hlt1DiPhoton::caloRecoChain( const LHCb::L0CaloCandidate* cand ) const
{
    const Gaudi::XYZPoint pos = cand->position() ;
    double x = pos.x() ;
    double y = pos.y() ;
    double r = pos.rho() ;
    const LHCb::CaloCellID id = cand->id() ;

    // Make CaloCluster with some minimal energy/position
    LHCb::CaloCluster* cluster = new LHCb::CaloCluster() ;
    cluster->position().setZ(m_z) ;
    cluster->setSeed(id) ;
    cluster->position().parameters()(LHCb::CaloPosition::E) = cand->et()*m_z/r ;
    cluster->position().parameters()(LHCb::CaloPosition::X) = x ;
    cluster->position().parameters()(LHCb::CaloPosition::Y) = y ;

    // Make CaloDigits and populate cluster entries if needed
    LHCb::CaloDigitStatus::Status used = LHCb::CaloDigitStatus::UseForEnergy ;
    LHCb::CaloCellID id1(id.calo(),id.area(),id.row()  , id.col()+1) ;
    LHCb::CaloCellID id2(id.calo(),id.area(),id.row()+1, id.col()  ) ;
    LHCb::CaloCellID id3(id.calo(),id.area(),id.row()+1, id.col()+1) ;

    LHCb::CaloDigit *d  = new LHCb::CaloDigit( id  , m_calo->digit( id ,  0.) ) ;
    LHCb::CaloDigit *d1 = new LHCb::CaloDigit( id1 , m_calo->digit( id1 , 0.) ) ;
    LHCb::CaloDigit *d2 = new LHCb::CaloDigit( id2 , m_calo->digit( id2 , 0.) ) ;
    LHCb::CaloDigit *d3 = new LHCb::CaloDigit( id3 , m_calo->digit( id3 , 0.) ) ;

    LHCb::CaloCluster::Entries& entries = cluster->entries() ;
    entries.push_back(LHCb::CaloClusterEntry(d, LHCb::CaloDigitStatus::SeedCell | used )) ;
    entries.push_back(LHCb::CaloClusterEntry(d1, used)) ;
    entries.push_back(LHCb::CaloClusterEntry(d2, used)) ;
    entries.push_back(LHCb::CaloClusterEntry(d3, used)) ;

    // Make CaloHypo
    LHCb::CaloHypo *hypo = new LHCb::CaloHypo() ;
    hypo->setHypothesis( LHCb::CaloHypo::Photon ) ;
    hypo->addToClusters( cluster ) ;
    hypo->setPosition( new LHCb::CaloPosition(cluster->position()) ) ;

    // Make ProtoParticle
    LHCb::ProtoParticle* proto = new LHCb::ProtoParticle() ;
    proto->addToCalo( hypo ) ;

    // Make Particle
    LHCb::Particle* p = new LHCb::Particle();
    p->setParticleID( LHCb::ParticleID(22) );
    p->setProto( proto );
    LHCb::CaloParticle calopart( p );
    calopart.updateParticle();
    return p ;
}
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream& LoKi::Hlt1::Hlt1DiPhoton::fillStream ( std::ostream& s ) const
{
  return s << "TC_HLT1DIPHOTON("
           << "'" << m_pp->name() << "',"
           << "'" << location()   << "',"
                  << m_photonEtMin << ","
                  << m_photonEtSumMin << ","
                  << std::sqrt(m_diPhotonMassSqMin) << ","
                  << m_diphotonEtMin << ")" ;
}
// ============================================================================
