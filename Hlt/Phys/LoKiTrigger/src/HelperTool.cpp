// $Id$
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
#include "LoKi/HelperTool.h"
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
 *  Implementation file for class LoKi::Hlt1::HelperTool
 *  @date 2010-10-30
 *  @author Vanya BELYAEV
 */
// ============================================================================
// get the stored tracks
// ============================================================================
LHCb::Track::Container*
LoKi::Hlt1::HelperTool::_createTracks( const std::string& location ) const
{
    typedef LHCb::Track::Container OBJECTS;
    return alg()->getOrCreate<OBJECTS, OBJECTS>( location );
}
// ============================================================================
// get the stored candidates
// ============================================================================
Hlt::Candidate::Container* LoKi::Hlt1::HelperTool::_createCandidates() const
{
    typedef Hlt::Candidate::Container OBJECTS;
    return alg()->getOrCreate<OBJECTS, OBJECTS>( Hlt::CandidateLocation::Default );
}
// ============================================================================
// get the stored stages
// ============================================================================
Hlt::Stage::Container* LoKi::Hlt1::HelperTool::_createStages() const
{
    typedef Hlt::Stage::Container OBJECTS;
    return alg()->getOrCreate<OBJECTS, OBJECTS>( Hlt::StageLocation::Default );
}
// ============================================================================
// get the stored multitracks
// ============================================================================
Hlt::MultiTrack::Container* LoKi::Hlt1::HelperTool::_createMultiTracks() const
{
    typedef Hlt::MultiTrack::Container OBJECTS;
    return alg()->getOrCreate<OBJECTS, OBJECTS>( Hlt::MultiTrackLocation::Default );
}
// ============================================================================
// get the stored recvertices
// ============================================================================
LHCb::RecVertex::Container* LoKi::Hlt1::HelperTool::_createRecVertices() const
{
    typedef LHCb::RecVertex::Container OBJECTS;
    return alg()->getOrCreate<OBJECTS, OBJECTS>( "Rec/Vertex/Hlt1_Vertices" );
}
// ============================================================================
// create the tool
// ============================================================================
LoKi::Hlt1::HelperTool::HelperTool( const int /* dummy */ )
    : LoKi::Listener()
    , m_alg{nullptr}
    //
    , m_hlt_candidates{nullptr}
    , m_hlt_stages{nullptr}
    , m_hlt_multitracks{nullptr}
    , m_hlt_tracks{nullptr}
    , m_hlt_recvertices{nullptr}
    //
    , m_myname()
{
    // get GaudiAlgorithm
    GaudiAlgorithm* alg = LoKi::Hlt1::Utils::getGaudiAlg( *this );
    Assert( alg, "GaudiAlgorithm points to NULL!" );
    // get IAlgorithm
    IAlgorithm* ialg = LoKi::Hlt1::Utils::getAlg( *this );
    Assert( alg == ialg, "GaudiAlgorithm is not *my* IAlgorithm" );
    //
    m_alg = alg;
    //
    // Incidents
    //
    subscribe( IncidentType::BeginEvent, std::numeric_limits<int>::min() );
    subscribe( IncidentType::EndEvent, std::numeric_limits<int>::min() );
}
// ============================================================================
// copy constructor
// ============================================================================
LoKi::Hlt1::HelperTool::HelperTool( const LoKi::Hlt1::HelperTool& right )
    : LoKi::AuxFunBase( right )
    , IInterface( right )
    , IIncidentListener( right )
    , extend_interfaces1<IIncidentListener>( right )
    , implements1<IIncidentListener>( right )
    , LoKi::Listener( right )
    //
    , m_alg( nullptr )
    //
    , m_hlt_candidates( nullptr )
    , m_hlt_stages( nullptr )
    , m_hlt_multitracks( nullptr )
    , m_hlt_tracks( nullptr )
    , m_hlt_recvertices( nullptr )
    //
    , m_myname()
{
    // get GaudiAlgorithm
    GaudiAlgorithm* alg = LoKi::Hlt1::Utils::getGaudiAlg( *this );
    Assert( alg, "GaudiAlgorithm points to NULL!" );
    // get IAlgorithm
    IAlgorithm* ialg = LoKi::Hlt1::Utils::getAlg( *this );
    Assert( alg == ialg, "GaudiAlgorithm is not *my* IAlgorithm" );
    //
    m_alg = alg;
}
// ============================================================================
// desctructor
// ============================================================================
LoKi::Hlt1::HelperTool::~HelperTool()
{
    m_hlt_candidates = nullptr;
    m_hlt_stages = nullptr;
    m_hlt_multitracks = nullptr;
    m_hlt_tracks = nullptr;
    m_hlt_recvertices = nullptr;
    //
    m_alg = nullptr;
}
// ============================================================================
/*  handle the incidents:
 *  clear the involved pointers
 */
// ============================================================================
void LoKi::Hlt1::HelperTool::handle( const Incident& /* incident */ )
{
    // clear all pointers
    m_hlt_candidates = nullptr;
    m_hlt_stages = nullptr;
    m_hlt_multitracks = nullptr;
    m_hlt_tracks = nullptr;
    m_hlt_recvertices = nullptr;
}

// ============================================================================
// The EN D
// ============================================================================
