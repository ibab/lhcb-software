// $Id: CaloHelperTool.cpp 180824 2014-11-28 09:49:23Z mkenzie $
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <climits>
// ============================================================================
/// GaudiKernel
// ============================================================================
#include "GaudiKernel/Incident.h"
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
#ifdef __INTEL_COMPILER
#pragma warning( disable                                                            \
                 : 1572 ) // non-pointer conversion ... may lose significant bits
#pragma warning( push )
#endif
// ============================================================================
// HltBase
// ============================================================================
#include "HltBase/HltUtils.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Algs.h"
#include "LoKi/UpgradeTool.h"
#include "LoKi/Combiner.h"
#include "LoKi/CaloHelperTool.h"
// ============================================================================
// Local
// ============================================================================
#include "LTTools.h"
// ============================================================================
#ifdef __INTEL_COMPILER
#pragma warning( pop )
#endif
// ============================================================================
/** @file
 *  Implementation file for class LoKi::Hlt1::CaloHelperTool
 *  @author Albert Puig (albert.puig@epfl.ch)
 *  @date   2015-03-31
 */
// ============================================================================
// create the tool
// ============================================================================
LoKi::Hlt1::CaloHelperTool::CaloHelperTool( const int /* dummy */ )
    : LoKi::Hlt1::HelperTool(0)
    , m_z (12300.0)
    , m_calo (NULL)

{
  setup() ;
}
// ============================================================================
// copy constructor
// ============================================================================
LoKi::Hlt1::CaloHelperTool::CaloHelperTool( const LoKi::Hlt1::CaloHelperTool& right )
    : LoKi::AuxFunBase( right )
    , IInterface( right )
    , IIncidentListener( right )
    , extend_interfaces1<IIncidentListener>( right )
    , implements1<IIncidentListener>( right )
    , LoKi::Listener( right )
    , LoKi::Hlt1::HelperTool( right )
    , m_z (12300.0)
    , m_calo (NULL)
{
  setup() ;
}
// ============================================================================
// destructor
// ============================================================================
LoKi::Hlt1::CaloHelperTool::~CaloHelperTool()
{
    m_calo_clusters = nullptr;
    m_calo_digits = nullptr;
    m_calo_hypos = nullptr;
    m_calo = nullptr;
}
// ============================================================================
// common constructor setup
// ============================================================================
StatusCode LoKi::Hlt1::CaloHelperTool::setup()
{
    LoKi::ILoKiSvc* svc = lokiSvc() ;
    SmartIF<IToolSvc> tsvc ( svc ) ;
    return tsvc->retrieveTool("CaloDataProvider", "EcalReadoutTool", m_calo);
}
/*  handle the incidents:
 *  clear the involved pointers
 */
// ============================================================================
void LoKi::Hlt1::CaloHelperTool::handle( const Incident& incident )
{
    // clear all pointers
    m_calo_digits = nullptr;
    m_calo_clusters = nullptr;
    m_calo_hypos = nullptr;
    LoKi::Hlt1::HelperTool::handle(incident) ;
}
// ============================================================================
// create the stored clusters
// ============================================================================
LHCb::CaloCluster::Container* LoKi::Hlt1::CaloHelperTool::_createClusters( ) const
{
    typedef LHCb::CaloCluster::Container OBJECTS;
    return alg()->getOrCreate<OBJECTS, OBJECTS>( "Hlt/Calo/Clusters" );
}
// ============================================================================
// create the stored digits
// ============================================================================
LHCb::CaloDigits* LoKi::Hlt1::CaloHelperTool::_createDigits( ) const
{
    //typedef LHCb::CaloDigit::Container OBJECTS;
    typedef LHCb::CaloDigits OBJECTS;
    return alg()->getOrCreate<OBJECTS, OBJECTS>( "Hlt/Calo/Digits" );
}
// ============================================================================
// create the stored hypos
// ============================================================================
LHCb::CaloHypo::Container* LoKi::Hlt1::CaloHelperTool::_createHypos( ) const
{
    typedef LHCb::CaloHypo::Container OBJECTS;
    return alg()->getOrCreate<OBJECTS, OBJECTS>( "Hlt/Calo/Hypos" );
}
// ============================================================================
/*  Convert L0CaloCandidates to LHCb::Particles
 */
// ============================================================================
LHCb::Particle* LoKi::Hlt1::CaloHelperTool::caloCandidateToParticle( const LHCb::L0CaloCandidate* cand,
                                                                     LHCb::ParticleID pid ) const
{
    const Gaudi::XYZPoint pos = cand->position() ;
    double x = pos.x() ;
    double y = pos.y() ;
    double r = pos.rho() ;
    const LHCb::CaloCellID id = cand->id() ;

    // Make CaloCluster with some minimal energy/position
    LHCb::CaloCluster* cluster = newCluster() ;
    cluster->position().setZ(m_z) ;
    cluster->setSeed(id) ;
    cluster->position().parameters()(LHCb::CaloPosition::E) = cand->et()*std::sqrt((m_z*m_z)/(r*r)+1) ;
    cluster->position().parameters()(LHCb::CaloPosition::X) = x ;
    cluster->position().parameters()(LHCb::CaloPosition::Y) = y ;

    // Make CaloDigits and populate cluster entries if needed
    LHCb::CaloDigitStatus::Status used = LHCb::CaloDigitStatus::UseForEnergy ;
    LHCb::CaloCellID id1(id.calo(),id.area(),id.row()  , id.col()+1) ;
    LHCb::CaloCellID id2(id.calo(),id.area(),id.row()+1, id.col()  ) ;
    LHCb::CaloCellID id3(id.calo(),id.area(),id.row()+1, id.col()+1) ;

    LHCb::CaloDigit *d  = newDigit(id) ;
    d->setE(m_calo->digit(id,  0.)) ;
    LHCb::CaloDigit *d1  = newDigit(id1) ;
    d1->setE(m_calo->digit(id1,  0.)) ;
    LHCb::CaloDigit *d2  = newDigit(id2) ;
    d2->setE(m_calo->digit(id2,  0.)) ;
    LHCb::CaloDigit *d3  = newDigit(id3) ;
    d3->setE(m_calo->digit(id3,  0.)) ;

    LHCb::CaloCluster::Entries& entries = cluster->entries() ;
    entries.push_back(LHCb::CaloClusterEntry(d, LHCb::CaloDigitStatus::SeedCell | used )) ;
    entries.push_back(LHCb::CaloClusterEntry(d1, used)) ;
    entries.push_back(LHCb::CaloClusterEntry(d2, used)) ;
    entries.push_back(LHCb::CaloClusterEntry(d3, used)) ;

    // Make CaloHypo
    LHCb::CaloHypo *hypo = newHypo() ;
    hypo->setHypothesis( LHCb::CaloHypo::Photon ) ;
    hypo->addToClusters( cluster ) ;
    hypo->setPosition( new LHCb::CaloPosition(cluster->position()) ) ;

    // Make ProtoParticle
    LHCb::ProtoParticle* proto = newProtoParticle() ;
    proto->addToCalo( hypo ) ;

    // Make Particle
    LHCb::Particle* p = new LHCb::Particle();
    p->setParticleID( pid );
    p->setProto( proto );
    LHCb::CaloParticle calopart( p );
    calopart.updateParticle();
    return p ;

}
// ============================================================================
// The END
// ============================================================================
