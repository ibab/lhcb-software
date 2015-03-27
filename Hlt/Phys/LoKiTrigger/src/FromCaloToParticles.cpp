// $Id: FromCaloToParticles.cpp 180655 2015-03-19 apuignav $
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
#include "LoKi/FromCaloToParticles.h"
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
 *  Implementation file for class LoKi::Hlt1::FromCaloToParticles
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
LoKi::Hlt1::FromCaloToParticles::FromCaloToParticles
( const std::string& pid         ,
  const std::string& location    ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&   cut )
  : LoKi::AuxFunBase ( std::tie ( pid , location , cut ) )
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe ()
  , LoKi::Hlt1::HelperTool ( 1 )
  , m_sink ( location )
  , m_pp   ( LoKi::Particles::ppFromName(pid) )
  , m_cut  ( cut      )
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
LoKi::Hlt1::FromCaloToParticles::~FromCaloToParticles () {}
// ============================================================================
// clone method ("virtual constructor")
// ============================================================================
LoKi::Hlt1::FromCaloToParticles* LoKi::Hlt1::FromCaloToParticles::clone() const
{ return new LoKi::Hlt1::FromCaloToParticles { *this } ; }
// ============================================================================
// the only one important method
// ============================================================================
LoKi::Hlt1::FromCaloToParticles::result_type
LoKi::Hlt1::FromCaloToParticles::operator()
  ( LoKi::Hlt1::FromCaloToParticles::argument a ) const
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
    LHCb::Particle* particle = caloRecoChain( calo, m_pp->particleID() ) ;

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
// Create a full LHCb particle from L0CaloCandidate ============================================================================
LHCb::Particle* LoKi::Hlt1::FromCaloToParticles::caloRecoChain( const LHCb::L0CaloCandidate* cand,
                                                                const LHCb::ParticleID particleID ) const
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
    p->setParticleID( particleID );
    p->setProto( proto );
    LHCb::CaloParticle calopart( p );
    calopart.updateParticle();
    return p ;
}
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream& LoKi::Hlt1::FromCaloToParticles::fillStream ( std::ostream& s ) const
{
  return
    s << "TC_FROMCALOTOPARTICLES("
      <<        m_pp->name() << ","
      << "'" << location()   << "')" ;
}
// ============================================================================
